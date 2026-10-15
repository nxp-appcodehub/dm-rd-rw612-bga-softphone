/*
 * Copyright 2026 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <mbedtls/ssl.h>
#include <mbedtls/error.h>
#include <mbedtls/net_sockets.h>
#include <re_types.h>
#include <re_fmt.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_net.h>
#include <re_main.h>
#include <re_sa.h>
#include <re_srtp.h>
#include <re_tcp.h>
#include <re_tls.h>
#include "tls.h"


#define DEBUG_MODULE "tls"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


/* Internal receive buffer for BIO operations */
struct tls_bio_buf {
	uint8_t *buf;
	size_t size;
	size_t pos;
	size_t end;
};


/* NOTE: shadow struct defined in tls_*.c */
struct tls_conn {
	mbedtls_ssl_context ssl;  /* inheritance */
	struct tls *tls;          /* inheritance */
	struct tls_conn_d cd;     /* inheritance */
	struct tcp_helper *th;
	struct tcp_conn *tcp;
	struct tls_bio_buf bio_in;
	bool active;
	bool up;
};


static void bio_buf_init(struct tls_bio_buf *bio, size_t size)
{
	bio->buf = mem_zalloc(size, NULL);
	bio->size = size;
	bio->pos = 0;
	bio->end = 0;
}


static void bio_buf_free(struct tls_bio_buf *bio)
{
	mem_deref(bio->buf);
	bio->buf = NULL;
	bio->size = 0;
	bio->pos = 0;
	bio->end = 0;
}


static int bio_buf_write(struct tls_bio_buf *bio, const uint8_t *data,
                         size_t len)
{
	size_t space;

	if (!bio || !bio->buf || !data)
		return EINVAL;

	/* Compact buffer if needed */
	if (bio->pos > 0 && bio->pos == bio->end) {
		bio->pos = 0;
		bio->end = 0;
	}
	else if (bio->pos > 0) {
		memmove(bio->buf, bio->buf + bio->pos, bio->end - bio->pos);
		bio->end -= bio->pos;
		bio->pos = 0;
	}

	space = bio->size - bio->end;
	if (len > space) {
		/* Resize buffer */
		size_t new_size = bio->size + len + 4096;
		uint8_t *new_buf = mem_realloc(bio->buf, new_size);
		if (!new_buf)
			return ENOMEM;
		bio->buf = new_buf;
		bio->size = new_size;
	}

	memcpy(bio->buf + bio->end, data, len);
	bio->end += len;

	return 0;
}


static int bio_buf_read(struct tls_bio_buf *bio, uint8_t *data, size_t len)
{
	size_t available;

	if (!bio || !bio->buf || !data)
		return MBEDTLS_ERR_SSL_INTERNAL_ERROR;

	available = bio->end - bio->pos;
	if (available == 0)
		return MBEDTLS_ERR_SSL_WANT_READ;

	if (len > available)
		len = available;

	memcpy(data, bio->buf + bio->pos, len);
	bio->pos += len;

	return (int)len;
}


/* MbedTLS send callback */
static int tls_send_cb(void *ctx, const unsigned char *buf, size_t len)
{
	struct tls_conn *tc = ctx;
	struct mbuf mb;
	int err;

	if (!tc || !buf)
		return MBEDTLS_ERR_SSL_INTERNAL_ERROR;

	mb.buf = (uint8_t *)buf;
	mb.pos = 0;
	mb.end = mb.size = len;

	err = tcp_send_helper(tc->tcp, &mb, tc->th);
	if (err) {
		if (err == EAGAIN || err == EWOULDBLOCK)
			return MBEDTLS_ERR_SSL_WANT_WRITE;
		return MBEDTLS_ERR_NET_SEND_FAILED;
	}

	return (int)len;
}


/* MbedTLS recv callback */
static int tls_recv_cb(void *ctx, unsigned char *buf, size_t len)
{
	struct tls_conn *tc = ctx;

	if (!tc || !buf)
		return MBEDTLS_ERR_SSL_INTERNAL_ERROR;

	return bio_buf_read(&tc->bio_in, buf, len);
}


static void destructor(void *arg)
{
	struct tls_conn *tc = arg;

	mbedtls_ssl_free(&tc->ssl);
	bio_buf_free(&tc->bio_in);

	mem_deref(tc->th);
	mem_deref(tc->tcp);
}


static int tls_connect(struct tls_conn *tc)
{
	int ret;

	ret = mbedtls_ssl_handshake(&tc->ssl);
	if (ret != 0) {
		if (ret == MBEDTLS_ERR_SSL_WANT_READ ||
		    ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
			return 0;
		}

		DEBUG_WARNING("mbedtls_ssl_handshake failed: -0x%x\n", -ret);
		return EPROTO;
	}

	return 0;
}


static int tls_accept(struct tls_conn *tc)
{
	int ret;

	ret = mbedtls_ssl_handshake(&tc->ssl);
	if (ret != 0) {
		if (ret == MBEDTLS_ERR_SSL_WANT_READ ||
		    ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
			return 0;
		}

		DEBUG_WARNING("mbedtls_ssl_handshake (accept) failed: -0x%x\n",
		              -ret);
		return EPROTO;
	}

	return 0;
}


static bool estab_handler(int *err, bool active, void *arg)
{
	struct tls_conn *tc = arg;
	int ret;

	DEBUG_INFO("tcp established (active=%u)\n", active);

	tc->active = active;

	if (active) {
		/* Client side: outgoing connection — client_conf already set in
		 * tls_start_tcp(); just initiate the handshake. */
		if (tls_get_session_reuse(tc))
			(void)tls_reuse_session(tc);

		*err = tls_connect(tc);
	}
	else {
		/* Server side: incoming connection — tls_start_tcp() set up
		 * client_conf by default; re-initialise with server_conf now
		 * that we know the direction. */
		mbedtls_ssl_free(&tc->ssl);
		mbedtls_ssl_init(&tc->ssl);

		ret = mbedtls_ssl_setup(&tc->ssl, tls_ssl_config(tc->tls));
		if (ret != 0) {
			DEBUG_WARNING("estab_handler: ssl_setup (server) failed: -0x%x\n",
			              -ret);
			*err = ENOMEM;
			return true;
		}

		mbedtls_ssl_set_bio(&tc->ssl, tc, tls_send_cb, tls_recv_cb, NULL);

		/* Accept incoming TLS handshake from client. */
		*err = tls_accept(tc);
	}

	return true;
}


static bool recv_handler(int *err, struct mbuf *mb, bool *estab, void *arg)
{
	struct tls_conn *tc = arg;
	int ret;

	/* Feed SSL data to the BIO */
	ret = bio_buf_write(&tc->bio_in, mbuf_buf(mb), mbuf_get_left(mb));
	if (ret != 0) {
		DEBUG_WARNING("recv: bio_buf_write failed: %d\n", ret);
		*err = ENOMEM;
		return true;
	}

	if (mbedtls_ssl_is_handshake_over(&tc->ssl) == 0) {

		if (tc->active) {
			*err = tls_connect(tc);
		}
		else {
			*err = tls_accept(tc);
		}

		if (*err)
			return true;

		DEBUG_INFO("state: handshake %s\n",
		           mbedtls_ssl_is_handshake_over(&tc->ssl) ?
		           "complete" : "in progress");

		/* TLS connection is established */
		if (mbedtls_ssl_is_handshake_over(&tc->ssl) == 0)
			return true;

		*estab = true;
		tc->up = true;
	}

	mbuf_set_pos(mb, 0);

	for (;;) {
		if (mbuf_get_space(mb) < 4096) {
			*err = mbuf_resize(mb, mb->size + 8192);
			if (*err)
				return true;
		}

		ret = mbedtls_ssl_read(&tc->ssl, mbuf_buf(mb),
		                       mbuf_get_space(mb));
		if (ret <= 0) {
			if (ret == MBEDTLS_ERR_SSL_WANT_READ ||
			    ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
				break;
			}

			if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY ||
			    ret == 0) {
				*err = ECONNRESET;
				return true;
			}

			DEBUG_WARNING("mbedtls_ssl_read failed: -0x%x\n", -ret);
			*err = EPROTO;
			return true;
		}

		mb->pos += ret;
	}

	mbuf_set_end(mb, mb->pos);
	mbuf_set_pos(mb, 0);

	return false;
}


static bool send_handler(int *err, struct mbuf *mb, void *arg)
{
	struct tls_conn *tc = arg;
	int ret;

	ret = mbedtls_ssl_write(&tc->ssl, mbuf_buf(mb), mbuf_get_left(mb));
	if (ret <= 0) {
		if (ret == MBEDTLS_ERR_SSL_WANT_READ ||
		    ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
			/* Would block, retry later */
			*err = 0;
			return true;
		}

		DEBUG_WARNING("mbedtls_ssl_write failed: -0x%x\n", -ret);
		*err = EPROTO;
	}

	return true;
}


/**
 * Change used certificate+key of an existing TLS connection
 *
 * @param tc      TLS connection object
 * @param certpem Certificate in PEM format
 * @param certlen Length of certificate PEM
 * @param keypem  Private key in PEM format
 * @param keylen  Length of key PEM
 *
 * @return int 0 if success, otherwise errorcode
 */
int tls_conn_change_cert_pem(struct tls_conn *tc, const char *certpem,
                             size_t certlen, const char *keypem, size_t keylen)
{
	mbedtls_x509_crt *cert;
	mbedtls_pk_context *pkey;
	int ret;

	if (!tc || !certpem || !certlen)
		return EINVAL;

	/* Allocate new certificate */
	cert = mem_zalloc(sizeof(*cert), NULL);
	if (!cert)
		return ENOMEM;

	mbedtls_x509_crt_init(cert);

	ret = mbedtls_x509_crt_parse(cert, (const unsigned char *)certpem,
	                             certlen + 1);
	if (ret < 0) {
		DEBUG_WARNING("tls_conn_change_cert: cert parse failed: -0x%x\n",
		              -ret);
		mbedtls_x509_crt_free(cert);
		mem_deref(cert);
		return ENOENT;
	}

	/* Allocate and parse private key */
	pkey = mem_zalloc(sizeof(*pkey), NULL);
	if (!pkey) {
		mbedtls_x509_crt_free(cert);
		mem_deref(cert);
		return ENOMEM;
	}

	mbedtls_pk_init(pkey);

	if (keypem && keylen) {
		ret = mbedtls_pk_parse_key(pkey, (const unsigned char *)keypem,
		                           keylen, NULL, 0
#if MBEDTLS_VERSION_NUMBER >= 0x03000000
		                           , mbedtls_ctr_drbg_random,
		                           &tc->tls->ctr_drbg
#endif
		                           );
	}
	else {
		/* Try to extract key from cert PEM */
		ret = mbedtls_pk_parse_key(pkey, (const unsigned char *)certpem,
		                           certlen, NULL, 0
#if MBEDTLS_VERSION_NUMBER >= 0x03000000
		                           , mbedtls_ctr_drbg_random,
		                           &tc->tls->ctr_drbg
#endif
		                           );
	}

	if (ret != 0) {
		DEBUG_WARNING("tls_conn_change_cert: key parse failed: -0x%x\n",
		              -ret);
		mbedtls_x509_crt_free(cert);
		mbedtls_pk_free(pkey);
		mem_deref(cert);
		mem_deref(pkey);
		return EKEYREJECTED;
	}

	/* Set the new certificate for current handshake */
	ret = mbedtls_ssl_set_hs_own_cert(&tc->ssl, cert, pkey);
	if (ret != 0) {
		DEBUG_WARNING("tls_conn_change_cert: set_hs_own_cert failed: -0x%x\n",
		              -ret);
		mbedtls_x509_crt_free(cert);
		mbedtls_pk_free(pkey);
		mem_deref(cert);
		mem_deref(pkey);
		return EINVAL;
	}

	/* Store references in connection for cleanup
	 * Note: In a full implementation, you'd want to track these
	 * in the tls_conn structure and free them in the destructor */
	// tc->cd.own_cert = cert;
	// tc->cd.own_pkey = pkey;

	return 0;
}


/**
 * Change used certificate+key of an existing TLS connection (file-based)
 * Note: Not supported in this embedded implementation
 *
 * @param tc   TLS connection object
 * @param file Cert+Key file path (not used)
 *
 * @return int ENOSYS (not supported)
 */
int tls_conn_change_cert(struct tls_conn *tc, const char *file)
{
	(void)tc;
	(void)file;

	/* File-based loading not supported on embedded systems */
	return ENOSYS;
}


/**
 * Start TLS on a TCP-connection
 *
 * @param ptc   Pointer to allocated TLS connection
 * @param tls   TLS Context
 * @param tcp   TCP Connection
 * @param layer Protocol stack layer
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_start_tcp(struct tls_conn **ptc, struct tls *tls,
                  struct tcp_conn *tcp, int layer)
{
	struct tls_conn *tc;
	int ret, err;

	if (!ptc || !tls || !tcp)
		return EINVAL;

	tc = mem_zalloc(sizeof(*tc), destructor);
	if (!tc)
		return ENOMEM;

	err = tcp_register_helper(&tc->th, tcp, layer, estab_handler,
	                          send_handler, recv_handler, tc);
	if (err)
		goto out;

	tc->tcp = mem_ref(tcp);
	tc->tls = tls;

	/* Initialize BIO buffer */
	bio_buf_init(&tc->bio_in, 8192);
	if (!tc->bio_in.buf) {
		err = ENOMEM;
		goto out;
	}

	/* Initialize SSL context */
	mbedtls_ssl_init(&tc->ssl);

	/* Use client-mode config for outgoing connections, server-mode for incoming.
	 * The active flag is set later in estab_handler, but tls_start_tcp is only
	 * called by the SIP transport layer; outgoing TCP connections always start
	 * active (client side).  We determine direction from whether there is a
	 * tcp_conn that we initiated: transp.c calls tls_start_tcp for both
	 * directions, so we always set up with client_conf here and let
	 * tls_conn_accept override with server conf for incoming connections. */
	ret = mbedtls_ssl_setup(&tc->ssl, tls_ssl_client_config(tls));
	if (ret != 0) {
		DEBUG_WARNING("mbedtls_ssl_setup (client) failed: -0x%x\n", -ret);
		err = ENOMEM;
		goto out;
	}

	/* Set BIO callbacks */
	mbedtls_ssl_set_bio(&tc->ssl, tc, tls_send_cb, tls_recv_cb, NULL);

	err = 0;

out:
	if (err)
		mem_deref(tc);
	else
		*ptc = tc;

	return err;
}


/**
 * Get tcp connection
 *
 * @param tc   TLS connection
 *
 * @return pointer to tcp connection struct
 */
const struct tcp_conn *tls_get_tcp_conn(const struct tls_conn *tc)
{
	if (!tc)
		return NULL;

	return tc->tcp;
}
