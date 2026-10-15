/*
 * Copyright 2026 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <mbedtls/ssl.h>
#include <mbedtls/ssl_cookie.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#include <mbedtls/timing.h>
#include <mbedtls/debug.h>
#include <mbedtls/net_sockets.h>

#include <re_types.h>
#include <re_fmt.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_list.h>
#include <re_hash.h>
#include <re_sa.h>
#include <re_srtp.h>
#include <re_udp.h>
#include <re_tmr.h>
#include <re_tls.h>
#include "tls.h"


#define DEBUG_MODULE "dtls"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


enum {
	MTU_DEFAULT  = 1400,
	MTU_FALLBACK = 548,
};


/* DTLS ContentType defined in RFC 5246 */
enum content_type {
	TYPE_CHANGE_CIPHER_SPEC = 20,
	TYPE_ALERT              = 21,
	TYPE_HANDSHAKE          = 22,
	TYPE_APPLICATION_DATA   = 23
};


/**
 * DTLS Socket structure
 */
struct dtls_sock {
	struct sa peer;
	struct udp_helper *uh;
	struct udp_sock *us;
	struct hash *ht;
	struct mbuf *mb;
	dtls_conn_h *connh;
	void *arg;
	size_t mtu;
	bool single_conn;
	mbedtls_ssl_cookie_ctx cookie_ctx;
	bool cookie_init;
};


/**
 * DTLS timing context for MbedTLS
 */
struct dtls_timing_context {
	uint32_t int_ms;
	uint32_t fin_ms;
	uint64_t start;
};


/**
 * DTLS BIO context for send/receive operations
 */
struct dtls_bio_ctx {
	struct tls_conn *tc;
	struct mbuf *rx_mb;
};


/* NOTE: shadow struct defined in tls.h */
struct tls_conn {
	mbedtls_ssl_context ssl;          /* inheritance */
	struct tls *tls;                  /* inheritance */
	struct tls_conn_d cd;             /* inheritance */
	struct dtls_timing_context timing;
	struct dtls_bio_ctx bio;
	struct tmr tmr;
	struct sa peer;
	struct le he;
	struct dtls_sock *sock;
	dtls_estab_h *estabh;
	dtls_recv_h *recvh;
	dtls_close_h *closeh;
	void *arg;
	bool active;
	bool up;
	char *peer_sni;
};


#if DEBUG_LEVEL >= 6
static const char *content_type_str(enum content_type content_type)
{
	switch (content_type) {

	case TYPE_CHANGE_CIPHER_SPEC: return "CHANGE_CIPHER_SPEC";
	case TYPE_ALERT:              return "ALERT";
	case TYPE_HANDSHAKE:          return "HANDSHAKE";
	case TYPE_APPLICATION_DATA:   return "APPLICATION_DATA";
	default: return "???";
	}
}
#endif


static uint64_t get_timer_ms(void)
{
	struct timeval tv;

	if (gettimeofday(&tv, NULL) != 0)
		return 0;

	return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


/**
 * MbedTLS timing callback: set delays
 */
static void dtls_timing_set_delay(void *data, uint32_t int_ms, uint32_t fin_ms)
{
	struct dtls_timing_context *ctx = data;

	ctx->int_ms = int_ms;
	ctx->fin_ms = fin_ms;
	ctx->start = get_timer_ms();
}


/**
 * MbedTLS timing callback: get delay status
 * Returns:
 *  -1 if cancelled (fin_ms == 0)
 *   0 if none of the delays have passed
 *   1 if only the intermediate delay has passed
 *   2 if the final delay has passed
 */
static int dtls_timing_get_delay(void *data)
{
	struct dtls_timing_context *ctx = data;
	uint64_t elapsed;

	if (ctx->fin_ms == 0)
		return -1;

	elapsed = get_timer_ms() - ctx->start;

	if (elapsed >= ctx->fin_ms)
		return 2;

	if (elapsed >= ctx->int_ms)
		return 1;

	return 0;
}


/**
 * MbedTLS BIO callback: send data over UDP
 */
static int dtls_bio_send(void *ctx, const unsigned char *buf, size_t len)
{
	struct dtls_bio_ctx *bio = ctx;
	struct tls_conn *tc = bio->tc;
	struct mbuf *mb;
	enum { SPACE = 4 };
	int err;

	if (!tc || !tc->sock || !tc->sock->us)
		return MBEDTLS_ERR_NET_SEND_FAILED;

	mb = mbuf_alloc(SPACE + len);
	if (!mb)
		return MBEDTLS_ERR_NET_SEND_FAILED;

	mb->pos = SPACE;
	(void)mbuf_write_mem(mb, buf, len);
	mb->pos = SPACE;

	err = udp_send_helper(tc->sock->us, &tc->peer, mb, tc->sock->uh);

	mem_deref(mb);

	if (err)
		return MBEDTLS_ERR_NET_SEND_FAILED;

	return (int)len;
}


/**
 * MbedTLS BIO callback: receive data
 */
static int dtls_bio_recv(void *ctx, unsigned char *buf, size_t len)
{
	struct dtls_bio_ctx *bio = ctx;
	size_t avail;

	if (!bio->rx_mb)
		return MBEDTLS_ERR_SSL_WANT_READ;

	avail = mbuf_get_left(bio->rx_mb);
	if (avail == 0)
		return MBEDTLS_ERR_SSL_WANT_READ;

	if (len > avail)
		len = avail;

	(void)mbuf_read_mem(bio->rx_mb, buf, len);

	return (int)len;
}


/**
 * MbedTLS BIO callback: receive with timeout
 */
static int dtls_bio_recv_timeout(void *ctx, unsigned char *buf, size_t len,
				 uint32_t timeout)
{
	(void)timeout;
	return dtls_bio_recv(ctx, buf, len);
}


static bool first_handler(struct le *le, void *arg)
{
	(void)le;
	(void)arg;

	return true;
}


static struct le *hash_get_first(const struct hash *ht)
{
	return hash_apply(ht, first_handler, NULL);
}


static void tls_close(struct tls_conn *tc)
{
	int ret;

	ret = mbedtls_ssl_close_notify(&tc->ssl);
	if (ret < 0) {
		DEBUG_INFO("close_notify failed: -0x%04x\n", -ret);
	}

	mbedtls_ssl_free(&tc->ssl);
}


static void conn_destructor(void *arg)
{
	struct tls_conn *tc = arg;

	hash_unlink(&tc->he);
	tmr_cancel(&tc->tmr);
	tls_close(tc);

	mem_deref(tc->peer_sni);
	mem_deref(tc->sock);
}


static void conn_close(struct tls_conn *tc, int err)
{
	tmr_cancel(&tc->tmr);
	tls_close(tc);
	tc->up = false;

	if (tc->closeh)
		tc->closeh(err, tc->arg);
}


static void check_timer(struct tls_conn *tc);


static void timeout(void *arg)
{
	struct tls_conn *tc = arg;
	int ret;

	DEBUG_INFO("timeout\n");

	ret = mbedtls_ssl_handshake(&tc->ssl);

	if (ret == MBEDTLS_ERR_SSL_WANT_READ ||
	    ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
		check_timer(tc);
	}
	else if (ret == MBEDTLS_ERR_SSL_TIMEOUT) {
		conn_close(tc, ETIMEDOUT);
	}
	else if (ret < 0 && ret != MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED) {
		DEBUG_WARNING("handshake timeout error: -0x%04x\n", -ret);
		conn_close(tc, EPROTO);
	}
}


static void check_timer(struct tls_conn *tc)
{
	int delay_status;
	uint32_t delay_ms;

	delay_status = dtls_timing_get_delay(&tc->timing);

	if (delay_status < 0) {
		tmr_cancel(&tc->tmr);
		return;
	}

	if (tc->timing.fin_ms == 0) {
		tmr_cancel(&tc->tmr);
		return;
	}

	/* Calculate remaining time to final delay */
	delay_ms = tc->timing.fin_ms;
	if (delay_status == 0 || delay_status == 1) {
		uint64_t elapsed = get_timer_ms() - tc->timing.start;
		if (elapsed < tc->timing.fin_ms) {
			delay_ms = tc->timing.fin_ms - (uint32_t)elapsed;
		}
		else {
			delay_ms = 0;
		}
	}

	tmr_start(&tc->tmr, delay_ms, timeout, tc);
}


static int tls_handshake(struct tls_conn *tc)
{
	int ret;

	ret = mbedtls_ssl_handshake(&tc->ssl);

	if (ret == MBEDTLS_ERR_SSL_WANT_READ ||
	    ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
		check_timer(tc);
		return 0;
	}

	if (ret == MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED) {
		DEBUG_INFO("DTLS hello verification required\n");
		mbedtls_ssl_session_reset(&tc->ssl);
		return 0;
	}

	if (ret < 0) {
		DEBUG_WARNING("handshake failed: -0x%04x\n", -ret);
		return EPROTO;
	}

	check_timer(tc);
	return 0;
}


static void conn_recv(struct tls_conn *tc, struct mbuf *mb)
{
	int ret;
	int err;
	uint32_t nrefs;

	if (!mbedtls_ssl_is_handshake_over(&tc->ssl) || !tc->up) {

		/* Provide data to the BIO */
		tc->bio.rx_mb = mb;

		if (tc->up && tc->cd.verifyh) {
			/* Handle renegotiation verification */
		}

		err = tls_handshake(tc);

		tc->bio.rx_mb = NULL;

		if (err) {
			conn_close(tc, err);
			return;
		}

		DEBUG_INFO("%s: handshake state\n",
			   tc->active ? "client" : "server");

		if (!mbedtls_ssl_is_handshake_over(&tc->ssl))
			return;

		tc->up = true;

		if (tc->estabh) {
			mem_ref(tc);

			tc->estabh(tc->arg);

			nrefs = mem_nrefs(tc);
			mem_deref(tc);

			if (nrefs == 1)
				return;
		}
	}

	/* Provide data to the BIO for reading */
	tc->bio.rx_mb = mb;
	mbuf_set_pos(mb, 0);

	for (;;) {
		unsigned char buf[4096];

		ret = mbedtls_ssl_read(&tc->ssl, buf, sizeof(buf));

		if (ret == MBEDTLS_ERR_SSL_WANT_READ ||
		    ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
			break;
		}

		if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
			tc->bio.rx_mb = NULL;
			conn_close(tc, ECONNRESET);
			return;
		}

		if (ret < 0) {
			DEBUG_WARNING("read error: -0x%04x\n", -ret);
			tc->bio.rx_mb = NULL;
			conn_close(tc, EPROTO);
			return;
		}

		if (ret == 0)
			break;

		/* Pass received data to handler */
		if (tc->recvh && ret > 0) {
			struct mbuf *rxmb;

			rxmb = mbuf_alloc(ret);
			if (rxmb) {
				mbuf_write_mem(rxmb, buf, ret);
				mbuf_set_pos(rxmb, 0);
				tc->recvh(rxmb, tc->arg);
				mem_deref(rxmb);
			}
		}
	}

	tc->bio.rx_mb = NULL;
}


static int conn_alloc(struct tls_conn **ptc, struct tls *tls,
		      struct dtls_sock *sock, const struct sa *peer,
		      dtls_estab_h *estabh, dtls_recv_h *recvh,
		      dtls_close_h *closeh, void *arg)
{
	struct tls_conn *tc;
	int err = 0;
	int ret;

	if (sock->single_conn) {
		if (hash_get_first(sock->ht)) {
			DEBUG_WARNING("single: only one connection allowed\n");
			return EMFILE;
		}
	}

	tc = mem_zalloc(sizeof(*tc), conn_destructor);
	if (!tc)
		return ENOMEM;

	hash_append(sock->ht, sa_hash(peer, SA_ALL), &tc->he, tc);

	tc->sock   = mem_ref(sock);
	tc->peer   = *peer;
	tc->estabh = estabh;
	tc->recvh  = recvh;
	tc->closeh = closeh;
	tc->arg    = arg;
	tc->tls    = tls;

	tc->bio.tc = tc;
	tc->bio.rx_mb = NULL;

	mbedtls_ssl_init(&tc->ssl);

	ret = mbedtls_ssl_setup(&tc->ssl, tls_ssl_config(tls));
	if (ret != 0) {
		DEBUG_WARNING("ssl_setup failed: -0x%04x\n", -ret);
		err = ENOMEM;
		goto out;
	}

	/* Set BIO callbacks */
	mbedtls_ssl_set_bio(&tc->ssl, &tc->bio,
			    dtls_bio_send,
			    dtls_bio_recv,
			    dtls_bio_recv_timeout);

	/* Set timer callbacks for DTLS */
	mbedtls_ssl_set_timer_cb(&tc->ssl, &tc->timing,
				 dtls_timing_set_delay,
				 dtls_timing_get_delay);

	/* Set MTU */
	mbedtls_ssl_set_mtu(&tc->ssl, (uint16_t)sock->mtu);

out:
	if (err)
		mem_deref(tc);
	else
		*ptc = tc;

	return err;
}


/**
 * DTLS Connect
 *
 * @param ptc    Pointer to allocated DTLS connection
 * @param tls    TLS Context
 * @param sock   DTLS Socket
 * @param peer   Peer address
 * @param estabh Establish handler
 * @param recvh  Receive handler
 * @param closeh Close handler
 * @param arg    Handler argument
 *
 * @return 0 if success, otherwise errorcode
 */
int dtls_connect(struct tls_conn **ptc, struct tls *tls,
		 struct dtls_sock *sock, const struct sa *peer,
		 dtls_estab_h *estabh, dtls_recv_h *recvh,
		 dtls_close_h *closeh, void *arg)
{
	struct tls_conn *tc;
	int err;

	if (!ptc || !tls || !sock || !peer)
		return EINVAL;

	err = conn_alloc(&tc, tls, sock, peer, estabh, recvh, closeh, arg);
	if (err)
		return err;

	tc->active = true;

	/* Set client mode */
	mbedtls_ssl_set_hs_authmode(&tc->ssl, MBEDTLS_SSL_VERIFY_OPTIONAL);

	err = tls_handshake(tc);
	if (err)
		goto out;

out:
	if (err)
		mem_deref(tc);
	else
		*ptc = tc;

	return err;
}


/**
 * DTLS Accept
 *
 * @param ptc    Pointer to allocated DTLS connection
 * @param tls    TLS Context
 * @param sock   DTLS Socket
 * @param estabh Establish handler
 * @param recvh  Receive handler
 * @param closeh Close handler
 * @param arg    Handler argument
 *
 * @return 0 if success, otherwise errorcode
 */
int dtls_accept(struct tls_conn **ptc, struct tls *tls,
		struct dtls_sock *sock,
		dtls_estab_h *estabh, dtls_recv_h *recvh,
		dtls_close_h *closeh, void *arg)
{
	struct tls_conn *tc;
	int err;
	unsigned char client_ip[16];
	size_t client_ip_len;

	if (!ptc || !tls || !sock || !sock->mb)
		return EINVAL;

	err = conn_alloc(&tc, tls, sock, &sock->peer, estabh, recvh, closeh,
			 arg);
	if (err)
		return err;

	tc->active = false;

	/* Set up client ID for cookie verification */
	if (sa_af(&sock->peer) == AF_INET) {
		client_ip_len = 4;
		memcpy(client_ip, &sock->peer.u.in.sin_addr, 4);
	}
	else {
		client_ip_len = 16;
		memcpy(client_ip, &sock->peer.u.in6.sin6_addr, 16);
	}

	mbedtls_ssl_set_client_transport_id(&tc->ssl, client_ip, client_ip_len);

	/* Provide initial data */
	tc->bio.rx_mb = sock->mb;

	err = tls_handshake(tc);

	tc->bio.rx_mb = NULL;

	if (err)
		goto out;

	sock->mb = mem_deref(sock->mb);

out:
	if (err)
		mem_deref(tc);
	else
		*ptc = tc;

	return err;
}


/**
 * Send data on a DTLS connection
 *
 * @param tc DTLS connection
 * @param mb Buffer to send
 *
 * @return 0 if success, otherwise errorcode
 */
int dtls_send(struct tls_conn *tc, struct mbuf *mb)
{
	int ret;

	if (!tc || !mb)
		return EINVAL;

	if (!tc->up)
		return ENOTCONN;

	ret = mbedtls_ssl_write(&tc->ssl, mbuf_buf(mb),
				mbuf_get_left(mb));

	if (ret < 0) {
		if (ret == MBEDTLS_ERR_SSL_WANT_READ ||
		    ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
			return EAGAIN;
		}
		DEBUG_WARNING("write error: -0x%04x\n", -ret);
		return EPROTO;
	}

	return 0;
}


/**
 * Set handlers on a DTLS Connection
 *
 * @param tc     DTLS Connection
 * @param estabh DTLS Connection Established handler
 * @param recvh  DTLS Connection Receive data handler
 * @param closeh DTLS Connection Close handler
 * @param arg    Handler argument
 */
void dtls_set_handlers(struct tls_conn *tc, dtls_estab_h *estabh,
		       dtls_recv_h *recvh, dtls_close_h *closeh, void *arg)
{
	if (!tc)
		return;

	tc->estabh = estabh;
	tc->recvh  = recvh;
	tc->closeh = closeh;
	tc->arg    = arg;
}


/**
 * Get the remote peer of a DTLS Connection
 *
 * @param tc DTLS Connection
 *
 * @return Remote peer
 */
const struct sa *dtls_peer(const struct tls_conn *tc)
{
	return tc ? &tc->peer : NULL;
}


/**
 * Set the remote peer of a DTLS Connection
 *
 * @param tc   DTLS Connection
 * @param peer Peer address
 */
void dtls_set_peer(struct tls_conn *tc, const struct sa *peer)
{
	if (!tc || !peer)
		return;

	hash_unlink(&tc->he);
	hash_append(tc->sock->ht, sa_hash(peer, SA_ALL), &tc->he, tc);

	tc->peer = *peer;
}


static void sock_destructor(void *arg)
{
	struct dtls_sock *sock = arg;

	hash_clear(sock->ht);

	if (sock->cookie_init) {
		mbedtls_ssl_cookie_free(&sock->cookie_ctx);
	}

	mem_deref(sock->uh);
	mem_deref(sock->us);
	mem_deref(sock->ht);
	mem_deref(sock->mb);
}


static bool cmp_handler(struct le *le, void *arg)
{
	struct tls_conn *tc = le->data;

	return sa_cmp(&tc->peer, arg, SA_ALL);
}


static struct tls_conn *conn_lookup(struct dtls_sock *sock,
				    const struct sa *peer)
{
	if (sock->single_conn) {
		struct le *le = hash_get_first(sock->ht);
		return list_ledata(le);
	}

	return list_ledata(hash_lookup(sock->ht, sa_hash(peer, SA_ALL),
				       cmp_handler, (void *)peer));
}


static bool recv_handler(struct sa *src, struct mbuf *mb, void *arg)
{
	struct dtls_sock *sock = arg;
	struct tls_conn *tc;
	uint8_t b;

	if (mbuf_get_left(mb) < 1)
		return false;

	b = mb->buf[mb->pos];
	if (b < 20 || b > 63)
		return false;

	DEBUG_INFO("receive '%s' from %J\n", content_type_str(b), src);

	tc = conn_lookup(sock, src);
	if (tc) {
		conn_recv(tc, mb);
		return true;
	}

	if (sock->connh) {
		mem_deref(sock->mb);
		sock->mb   = mem_ref(mb);
		sock->peer = *src;

		sock->connh(src, sock->arg);
	}

	return true;
}


/**
 * Create DTLS Socket
 *
 * @param sockp  Pointer to returned DTLS Socket
 * @param laddr  Local listen address (optional)
 * @param us     External UDP socket (optional)
 * @param htsize Connection hash table size
 * @param layer  UDP protocol layer
 * @param connh  Connect handler
 * @param arg    Handler argument
 *
 * @return 0 if success, otherwise errorcode
 */
int dtls_listen(struct dtls_sock **sockp, const struct sa *laddr,
		struct udp_sock *us, uint32_t htsize, int layer,
		dtls_conn_h *connh, void *arg)
{
	struct dtls_sock *sock;
	int err;

	if (!sockp)
		return EINVAL;

	sock = mem_zalloc(sizeof(*sock), sock_destructor);
	if (!sock)
		return ENOMEM;

	if (us) {
		sock->us = mem_ref(us);
	}
	else {
		err = udp_listen(&sock->us, laddr, NULL, NULL);
		if (err)
			goto out;
	}

	err = udp_register_helper(&sock->uh, sock->us, layer,
				  NULL, recv_handler, sock);
	if (err)
		goto out;

	err = hash_alloc(&sock->ht, hash_valid_size(htsize));
	if (err)
		goto out;

	/* Initialize DTLS cookie context */
	mbedtls_ssl_cookie_init(&sock->cookie_ctx);
	sock->cookie_init = true;

	sock->mtu   = MTU_DEFAULT;
	sock->connh = connh;
	sock->arg   = arg;

out:
	if (err)
		mem_deref(sock);
	else
		*sockp = sock;

	return err;
}


/**
 * Get the underlying UDP socket of a DTLS Socket
 *
 * @param sock DTLS Socket
 *
 * @return UDP Socket
 */
struct udp_sock *dtls_udp_sock(struct dtls_sock *sock)
{
	return sock ? sock->us : NULL;
}


/**
 * Set MTU on a DTLS Socket
 *
 * @param sock DTLS Socket
 * @param mtu  MTU value
 */
void dtls_set_mtu(struct dtls_sock *sock, size_t mtu)
{
	if (!sock)
		return;

	sock->mtu = mtu;
}


/**
 * Receive a DTLS packet
 *
 * @param sock DTLS Socket
 * @param src  Source network address
 * @param mb   Mbuffer with DTLS packet
 */
void dtls_recv_packet(struct dtls_sock *sock, const struct sa *src,
		      struct mbuf *mb)
{
	struct sa addr;

	if (!sock || !src || !mb)
		return;

	addr = *src;

	recv_handler(&addr, mb, sock);
}


/**
 * Set single connection mode
 *
 * @param sock   DTLS Socket
 * @param single True to enable, False to disable
 */
void dtls_set_single(struct dtls_sock *sock, bool single)
{
	if (!sock)
		return;

	sock->single_conn = single;
}


/**
 * Set SNI hostname for DTLS client connection
 *
 * @param tc       DTLS Connection
 * @param hostname SNI hostname
 *
 * @return 0 if success, otherwise errorcode
 */
int dtls_set_sni(struct tls_conn *tc, const char *hostname)
{
	int ret;

	if (!tc || !hostname)
		return EINVAL;

	tc->peer_sni = mem_deref(tc->peer_sni);

	ret = mbedtls_ssl_set_hostname(&tc->ssl, hostname);
	if (ret != 0) {
		DEBUG_WARNING("set_hostname failed: -0x%04x\n", -ret);
		return EINVAL;
	}

	return str_dup(&tc->peer_sni, hostname);
}


/**
 * Get the TLS connection context
 *
 * @param tc DTLS Connection
 *
 * @return MbedTLS SSL context
 */
mbedtls_ssl_context *tls_conn_ssl(const struct tls_conn *tc)
{
	return tc ? (mbedtls_ssl_context *)&tc->ssl : NULL;
}


/**
 * Check if DTLS connection is established
 *
 * @param tc DTLS Connection
 *
 * @return true if established, false otherwise
 */
bool dtls_is_established(const struct tls_conn *tc)
{
	if (!tc)
		return false;

	return tc->up && mbedtls_ssl_is_handshake_over((mbedtls_ssl_context*)&tc->ssl);
}


/**
 * Get the DTLS connection verification status
 *
 * @param tc DTLS Connection
 *
 * @return Verification flags (0 = success)
 */
uint32_t dtls_peer_verify(const struct tls_conn *tc)
{
	if (!tc)
		return (uint32_t)-1;

	return mbedtls_ssl_get_verify_result(&tc->ssl);
}


/**
 * Get the negotiated DTLS cipher suite
 *
 * @param tc DTLS Connection
 *
 * @return Cipher suite name
 */
const char *dtls_cipher(const struct tls_conn *tc)
{
	if (!tc)
		return NULL;

	return mbedtls_ssl_get_ciphersuite(&tc->ssl);
}
