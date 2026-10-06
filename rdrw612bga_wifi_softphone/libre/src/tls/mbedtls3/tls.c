/*
 * Copyright 2026 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/x509.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>
#include <mbedtls/error.h>
#include <mbedtls/debug.h>
#include <mbedtls/oid.h>
#include <mbedtls/pem.h>
#include <mbedtls/base64.h>
#include <re_types.h>
#include <re_fmt.h>
#include <re_mem.h>
#include <re_mbuf.h>
#include <re_net.h>
#include <re_main.h>
#include <re_sa.h>
#include <re_srtp.h>
#include <re_sys.h>
#include <re_tcp.h>
#include <re_tls.h>
#include <re_list.h>
#include <re_hash.h>
#include "tls.h"


#define DEBUG_MODULE "tls"
#define DEBUG_LEVEL 7
#include <re_dbg.h>


/* NOTE: shadow struct defined in tls_*.c */
struct tls_conn {
	mbedtls_ssl_context ssl;
	struct tls *tls;
	struct tls_conn_d cd;
};


static void print_certificate_info(const mbedtls_x509_crt *crt,
	                               const unsigned char *der,
								   size_t der_len)
{
#if PRINT_CERTIFICATE_INFO
    const char *header = "-----BEGIN CERTIFICATE-----\n";
    const char *footer = "-----END CERTIFICATE-----\n";

    size_t pem_len = 0;
    int ret;

    /* First call: get required buffer size */
    ret = mbedtls_pem_write_buffer(header, footer,
                                   der, der_len,
                                   NULL, 0, &pem_len);
    if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
        DEBUG_WARNING("Unexpected error: %d\n", ret);
        return;
    }

    unsigned char *pem_buf = mem_zalloc(pem_len, NULL);
    if (!pem_buf) {
        DEBUG_WARNING("Memory allocation failed\n");
        return;
    }

    /* Encode DER → PEM */
    ret = mbedtls_pem_write_buffer(header, footer,
                                   der, der_len,
                                   pem_buf, pem_len, &pem_len);
    if (ret != 0) {
        DEBUG_WARNING("PEM write failed: %d\n", ret);
        mem_deref(pem_buf);
        return;
    }

    /* Print PEM as text */
    DEBUG_INFO("%s", pem_buf);

    mem_deref(pem_buf);

	/* Print certificate details */
	char* buf = mem_zalloc(4096, NULL);
	mbedtls_x509_crt_info(buf, 4095, "", crt);
	DEBUG_INFO("%s\n", buf);
	mem_deref(buf);
#else
	(void)crt;
	(void)der;
    (void)der_len;
#endif
}


/* Adjust if you expect very large keys (e.g., 4096-bit RSA). */
#define DER_TMP_SIZE  4096

static void print_private_key_info(const mbedtls_pk_context *pk,
                                   const unsigned char *der,
                                   size_t der_len)
{
#if PRINT_CERTIFICATE_INFO
    int ret;
    size_t pem_len = 0;

    /* We will try to rebuild a DER from pk (PKCS#1 for RSA / SEC1 for EC),
       then wrap it to PEM, like in your certificate function. */
    unsigned char *der_buf = mem_zalloc(DER_TMP_SIZE, NULL);
    if (!der_buf) {
        DEBUG_WARNING("Memory allocation failed (DER buffer)\n");
        return;
    }

    /* mbedtls_pk_write_key_der() writes from the END of the buffer and
       returns the length written on success. */
    ret = mbedtls_pk_write_key_der(pk, der_buf, DER_TMP_SIZE);
    unsigned char *key_der_ptr = NULL;
    size_t key_der_len = 0;

    if (ret > 0) {
        key_der_len = (size_t) ret;
        key_der_ptr = der_buf + DER_TMP_SIZE - key_der_len;
    } else {
        /* Fall back to the provided DER (assumed to be PKCS#8, PKCS#1, or SEC1). */
        if (!der || der_len == 0) {
            DEBUG_WARNING("No DER available for private key (ret=%d)\n", ret);
            mem_deref(der_buf);
            return;
        }
        key_der_ptr = (unsigned char *) der;
        key_der_len = der_len;
    }

    /* Choose PEM header/footer. If we got DER from pk_write_key_der(), it will
       be PKCS#1 for RSA or SEC1 for EC, so use the matching headers. Otherwise,
       for an external (unknown) DER, default to PKCS#8 headers. */
    const char *header = "-----BEGIN PRIVATE KEY-----\n";
    const char *footer = "-----END PRIVATE KEY-----\n";

    mbedtls_pk_type_t kt = mbedtls_pk_get_type(pk);
    if (ret > 0) {
        if (kt == MBEDTLS_PK_RSA) {
            header = "-----BEGIN RSA PRIVATE KEY-----\n";
            footer = "-----END RSA PRIVATE KEY-----\n";
        } else if (kt == MBEDTLS_PK_ECKEY) {
            header = "-----BEGIN EC PRIVATE KEY-----\n";
            footer = "-----END EC PRIVATE KEY-----\n";
        }
    }

    /* First call: size query for PEM buffer */
    ret = mbedtls_pem_write_buffer(header, footer,
                                   key_der_ptr, key_der_len,
                                   NULL, 0, &pem_len);
    if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
        DEBUG_WARNING("Unexpected error sizing PEM buffer: %d\n", ret);
        mem_deref(der_buf);
        return;
    }

    unsigned char *pem_buf = mem_zalloc(pem_len, NULL);
    if (!pem_buf) {
        DEBUG_WARNING("Memory allocation failed (PEM buffer)\n");
        mem_deref(der_buf);
        return;
    }

    /* DER → PEM (Base64 + headers/footers) */
    ret = mbedtls_pem_write_buffer(header, footer,
                                   key_der_ptr, key_der_len,
                                   pem_buf, pem_len, &pem_len);
    if (ret != 0) {
        DEBUG_WARNING("PEM write failed: %d\n", ret);
        mem_deref(pem_buf);
        mem_deref(der_buf);
        return;
    }

    /* Print the private key PEM */
    DEBUG_INFO("%s", pem_buf);

    /* --------- Print Private Key Info (type/size[/curve]) ---------- */
    {
        char *buf = mem_zalloc(512, NULL);
        if (!buf) {
            DEBUG_WARNING("Memory allocation failed (info buffer)\n");
            mem_deref(pem_buf);
            mem_deref(der_buf);
            return;
        }

        const char *alg = "Unknown";
        if (kt == MBEDTLS_PK_RSA)      alg = "RSA";
        else if (kt == MBEDTLS_PK_ECKEY) alg = "EC";

        size_t bits = mbedtls_pk_get_bitlen(pk);
        int off = snprintf(buf, 512, "Key type: %s\nKey size: %d bits\n", alg, bits);

        if (kt == MBEDTLS_PK_ECKEY) {
            const mbedtls_ecp_keypair *ec = mbedtls_pk_ec(*pk);
            if (ec) {
                const mbedtls_ecp_curve_info *ci =
                    mbedtls_ecp_curve_info_from_grp_id(ec->grp.id);
                if (ci && ci->name) {
                    off += snprintf(buf + off, (off < 512 ? 512 - off : 0),
                                    "Curve: %s\n", ci->name);
                }
            }
        }

        DEBUG_INFO("%s", buf);
        mem_deref(buf);
    }

    /* Cleanup */
    mem_deref(pem_buf);
    mem_deref(der_buf);
#else
	(void)pk;
	(void)der;
    (void)der_len;
#endif
}

static void print_cafile_info(const mbedtls_x509_crt *ca_chain)
{
#if PRINT_CERTIFICATE_INFO
    const char *header = "-----BEGIN CERTIFICATE-----\n";
    const char *footer = "-----END CERTIFICATE-----\n";

    if (!ca_chain) {
        DEBUG_WARNING("No CA chain provided\n");
        return;
    }

    const mbedtls_x509_crt *cur = ca_chain;
    unsigned idx = 0;

    for (; cur != NULL; cur = cur->next, ++idx) {

        /* Convert this CA cert's DER -> PEM */
        size_t pem_len = 0;
        int ret = mbedtls_pem_write_buffer(header, footer,
                                           cur->raw.p, cur->raw.len,
                                           NULL, 0, &pem_len);
        if (ret != MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
            DEBUG_WARNING("CA[%u]: unexpected size-probe error: %d\n", idx, ret);
            continue;
        }

        unsigned char *pem_buf = mem_zalloc(pem_len, NULL);
        if (!pem_buf) {
            DEBUG_WARNING("CA[%u]: memory allocation failed (PEM)\n", idx);
            break;
        }

        ret = mbedtls_pem_write_buffer(header, footer,
                                       cur->raw.p, cur->raw.len,
                                       pem_buf, pem_len, &pem_len);
        if (ret != 0) {
            DEBUG_WARNING("CA[%u]: PEM write failed: %d\n", idx, ret);
            mem_deref(pem_buf);
            continue;
        }

        /* Print PEM block for this CA certificate */
        DEBUG_INFO("----- CA %u (PEM) -----\n%.*s",
                   idx, (int)pem_len, (const char *)pem_buf);
        mem_deref(pem_buf);

        /* Print human-readable info for this CA certificate */
        char *info = mem_zalloc(4096, NULL);
        if (!info) {
            DEBUG_WARNING("CA[%u]: memory allocation failed (Info)\n", idx);
            break;
        }

        mbedtls_x509_crt_info(info, 4095, "", cur);
        DEBUG_INFO("----- CA %u (Info) -----\n%s\n", idx, info);
        mem_deref(info);
    }
#else
	(void)ca_chain;
#endif
}

static void destructor(void *data)
{
	struct tls *tls = data;

	if (tls->cert_loaded)
		mbedtls_x509_crt_free(&tls->cert);
	if (tls->pkey_loaded)
		mbedtls_pk_free(&tls->pkey);
	if (tls->cacert_loaded)
		mbedtls_x509_crt_free(&tls->cacert);
	if (tls->crl_loaded)
		mbedtls_x509_crl_free(&tls->crl);

	mbedtls_ssl_config_free(&tls->conf);
	mbedtls_ssl_config_free(&tls->client_conf);
	mbedtls_ctr_drbg_free(&tls->ctr_drbg);
	mbedtls_entropy_free(&tls->entropy);

	hash_flush(tls->reuse.ht_sessions);
	mem_deref(tls->reuse.ht_sessions);
	list_flush(&tls->certs);
}

/**
 * Allocate a new TLS context
 *
 * @param tlsp    Pointer to allocated TLS context
 * @param method  TLS method
 * @param certpem Optional certificate in PEM format
 * @param certlen Length of certificate
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_alloc(struct tls **tlsp, enum tls_method method,
			  const unsigned char *certbuf, size_t certbuf_len,
              const unsigned char *keybuf, size_t keybuf_len,
			  const unsigned char *cafilebuf, size_t cafilebuf_len,
              const char *pwd)
{
	struct tls *tls;
	int err;
	const char *pers = "re_tls";

	(void)pwd;

	if (!tlsp)
		return EINVAL;

	tls = mem_zalloc(sizeof(*tls), destructor);
	if (!tls)
		return ENOMEM;
	tls->method = method;
	tls->verify_server = true;

	/* --- Mbed TLS init --- */
	mbedtls_ssl_config_init(&tls->conf);
	mbedtls_x509_crt_init(&tls->cacert);
	mbedtls_x509_crt_init(&tls->cert);
	mbedtls_pk_init(&tls->pkey);
	mbedtls_entropy_init(&tls->entropy);
	mbedtls_ctr_drbg_init(&tls->ctr_drbg);

	/* RNG seed (you should ensure entropy sources are correct on your MCU) */
	err = mbedtls_ctr_drbg_seed(&tls->ctr_drbg, mbedtls_entropy_func,
	                            &tls->entropy,
	                            (const unsigned char *)pers,
	                            strlen(pers));
	if (err) {
		DEBUG_WARNING("mbedtls_ctr_drbg_seed failed: -0x%x\n", -err);
		err = EINVAL;
		goto out;
	}

    /* Parse certs from RAM buffers
     * NOTE: If buffers are PEM, make sure they are NUL-terminated and pass len+1.
     * If buffers are DER, pass the exact len as below.
	 * Is better to use DER.
     */
	if (certbuf && certbuf_len > 0) {
		DEBUG_INFO("tls_alloc: Loading certificate\n");
		err = mbedtls_x509_crt_parse(&tls->cert, certbuf, certbuf_len);
		if (err != 0) {
			goto out;
		}
		/* Print the certificate for debugging purposes */
 		print_certificate_info(&tls->cert, certbuf, certbuf_len);
	}
	else {
		DEBUG_WARNING("tls_alloc: Certificate buffer not provided.\n");
		goto out;
	}

	if (cafilebuf && cafilebuf_len > 0) {
		DEBUG_INFO("tls_alloc: Loading cafile\n");
		err = mbedtls_x509_crt_parse(&tls->cacert, cafilebuf, cafilebuf_len);
		if (err != 0) {
			goto out;
		}
		print_cafile_info(&tls->cacert);
	}
	else {
		DEBUG_WARNING("tls_alloc: Certificate Autority file not provided.\n");
		goto out;
	}

	if (keybuf && keybuf_len > 0) {
		DEBUG_INFO("tls_alloc: Loading private key\n");
		err = mbedtls_pk_parse_key(&tls->pkey,
					(const unsigned char *)keybuf, keybuf_len,
					(const unsigned char *)pwd, pwd ? strlen(pwd) : 0,
					mbedtls_ctr_drbg_random, &tls->ctr_drbg
				);
		if (err != 0) {
			goto out;
		}
		print_private_key_info(&tls->pkey, keybuf, keybuf_len);
	}
	else {
		DEBUG_WARNING("tls_alloc: Private Key file not provided.\n");
		goto out;
	}

	/* --- Server-mode TLS configuration (accepts incoming connections) --- */
    err = mbedtls_ssl_config_defaults(&tls->conf,
                                      MBEDTLS_SSL_IS_SERVER,
                                      MBEDTLS_SSL_TRANSPORT_STREAM,
                                      MBEDTLS_SSL_PRESET_DEFAULT);
	if(err) {
		DEBUG_WARNING("tls_alloc: mbedtls_ssl_config_defaults (server) error: %d.\n", err);
		goto out;
	}
	mbedtls_ssl_conf_rng(&tls->conf, mbedtls_ctr_drbg_random, &tls->ctr_drbg);
    mbedtls_ssl_conf_min_tls_version(&tls->conf, MBEDTLS_SSL_VERSION_TLS1_3);
#ifdef MTLS_VERIFY_CLIENT
    mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
#else
    mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_NONE);
#endif
    mbedtls_ssl_conf_ca_chain(&tls->conf, &tls->cacert, NULL);
    err = mbedtls_ssl_conf_own_cert(&tls->conf, &tls->cert, &tls->pkey);
	if(err) {
		DEBUG_WARNING("tls_alloc: mbedtls_ssl_conf_own_cert (server) error: %d.\n", err);
		goto out;
	}

	/* --- Client-mode TLS configuration (outgoing connections, e.g. SIP REGISTER) --- */
	mbedtls_ssl_config_init(&tls->client_conf);
	err = mbedtls_ssl_config_defaults(&tls->client_conf,
	                                  MBEDTLS_SSL_IS_CLIENT,
	                                  MBEDTLS_SSL_TRANSPORT_STREAM,
	                                  MBEDTLS_SSL_PRESET_DEFAULT);
	if (err) {
		DEBUG_WARNING("tls_alloc: mbedtls_ssl_config_defaults (client) error: %d.\n", err);
		goto out;
	}
	mbedtls_ssl_conf_rng(&tls->client_conf, mbedtls_ctr_drbg_random, &tls->ctr_drbg);
	mbedtls_ssl_conf_min_tls_version(&tls->client_conf, MBEDTLS_SSL_VERSION_TLS1_3);
	/* Client verifies the server certificate by default */
	mbedtls_ssl_conf_authmode(&tls->client_conf, MBEDTLS_SSL_VERIFY_REQUIRED);
	mbedtls_ssl_conf_ca_chain(&tls->client_conf, &tls->cacert, NULL);
	/* Present our own cert to the server (needed for mTLS) */
	err = mbedtls_ssl_conf_own_cert(&tls->client_conf, &tls->cert, &tls->pkey);
	if (err) {
		DEBUG_WARNING("tls_alloc: mbedtls_ssl_conf_own_cert (client) error: %d.\n", err);
		goto out;
	}

	err = hash_alloc(&tls->reuse.ht_sessions, 256);
	if (err)
		goto out;

	err = 0;
out:
	if (err)
		mem_deref(tls);
	else
		*tlsp = tls;

	return err;
}


/**
 * Add trusted CA certificates given as string
 *
 * @param tls    TLS Context
 * @param capem  Trusted CA as null-terminated string given in PEM format
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_add_capem(const struct tls *tls, const char *capem)
{
	struct tls *t = (struct tls *)tls;
	int ret;

	if (!tls || !capem)
		return EINVAL;

	if (!t->cacert_loaded) {
		mbedtls_x509_crt_init(&t->cacert);
		t->cacert_loaded = true;
	}

	ret = mbedtls_x509_crt_parse(&t->cacert,
	                             (const unsigned char *)capem,
	                             strlen(capem) + 1);
	if (ret < 0) {
		DEBUG_WARNING("mbedtls_x509_crt_parse failed: -0x%x\n", -ret);
		return EINVAL;
	}

	mbedtls_ssl_conf_ca_chain(&t->conf, &t->cacert, NULL);

	return 0;
}


/**
 * Set default locations for trusted CA certificates
 *
 * @param tls    TLS Context
 * @param cafile PEM file with CA certificates (not used, kept for API compat)
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_add_ca(struct tls *tls, const char *cafile)
{
	(void)tls;
	(void)cafile;
	/* File-based loading not supported in this implementation */
	return ENOSYS;
}


/**
 * Set default file and path for trusted CA certificates
 *
 * @param tls    TLS Context
 * @param cafile PEM file with CA certificate(s) (not used)
 * @param capath Path containing CA certificates files (not used)
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_add_cafile_path(struct tls *tls, const char *cafile,
                        const char *capath)
{
	(void)tls;
	(void)cafile;
	(void)capath;
	/* File-based loading not supported in this implementation */
	return ENOSYS;
}


/**
 * Add trusted CRL certificates given as string
 *
 * @param tls  TLS Context
 * @param pem  Trusted CRL as null-terminated string given in PEM format
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_add_crlpem(const struct tls *tls, const char *pem)
{
	struct tls *t = (struct tls *)tls;
	int ret;

	if (!tls || !pem)
		return EINVAL;

	if (!t->crl_loaded) {
		mbedtls_x509_crl_init(&t->crl);
		t->crl_loaded = true;
	}

	ret = mbedtls_x509_crl_parse(&t->crl,
	                             (const unsigned char *)pem,
	                             strlen(pem) + 1);
	if (ret < 0) {
		DEBUG_WARNING("mbedtls_x509_crl_parse failed: -0x%x\n", -ret);
		return EINVAL;
	}

	mbedtls_ssl_conf_ca_chain(&t->conf, &t->cacert, &t->crl);

	return 0;
}


/**
 * Set SSL verification of the certificate purpose
 *
 * @param tls     TLS Context
 * @param purpose Certificate purpose as string
 *
 * @return int    0 if success, errorcode otherwise
 */
int tls_set_verify_purpose(struct tls *tls, const char *purpose)
{
	(void)tls;
	(void)purpose;
	/* MbedTLS doesn't have direct equivalent */
	return 0;
}


/**
 * Create a selfsigned X509 certificate using EC
 *
 * @param tls      TLS Context
 * @param cn       Common Name
 * @param curve_n  Known EC curve name
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_set_selfsigned_ec(struct tls *tls, const char *cn,
                          const char *curve_n)
{
	(void)tls;
	(void)cn;
	(void)curve_n;
	/* Self-signed certificate generation not implemented */
	return ENOSYS;
}


/**
 * Set the certificate and private key on a TLS context
 *
 * @param tls      TLS Context
 * @param cert     Certificate in PEM format
 * @param len_cert Length of certificate PEM string
 * @param key      Private key in PEM format, will be read from cert if NULL
 * @param len_key  Length of private key PEM string
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_set_certificate_pem(struct tls *tls, const char *cert,
                            size_t len_cert, const char *key,
                            size_t len_key)
{
	int ret;

	if (!tls || !cert || !len_cert)
		return EINVAL;

	if (!key) {
		key = cert;
		len_key = len_cert;
	}

	if (!tls->cert_loaded) {
		mbedtls_x509_crt_init(&tls->cert);
		tls->cert_loaded = true;
	}

	ret = mbedtls_x509_crt_parse(&tls->cert,
	                             (const unsigned char *)cert,
	                             len_cert + 1);
	if (ret < 0) {
		DEBUG_WARNING("mbedtls_x509_crt_parse failed: -0x%x\n", -ret);
		return EINVAL;
	}

	if (!tls->pkey_loaded) {
		mbedtls_pk_init(&tls->pkey);
		tls->pkey_loaded = true;
	}

	ret = mbedtls_pk_parse_key(&tls->pkey,
	                           (const unsigned char *)key,
	                           len_key + 1, NULL, 0,
					           mbedtls_ctr_drbg_random, &tls->ctr_drbg);
	if (ret != 0) {
		DEBUG_WARNING("mbedtls_pk_parse_key failed: -0x%x\n", -ret);
		return EINVAL;
	}

	ret = mbedtls_ssl_conf_own_cert(&tls->conf, &tls->cert, &tls->pkey);
	if (ret != 0) {
		DEBUG_WARNING("mbedtls_ssl_conf_own_cert failed: -0x%x\n",
		              -ret);
		return EINVAL;
	}

	return 0;
}


/**
 * Set the certificate and private key on a TLS context
 *
 * @param tls TLS Context
 * @param pem Certificate and private key in PEM format
 * @param len Length of PEM string
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_set_certificate(struct tls *tls, const char *pem, size_t len)
{
	return tls_set_certificate_pem(tls, pem, len, NULL, 0);
}


static int verify_callback(void *arg, mbedtls_x509_crt *crt,
                          int depth, uint32_t *flags)
{
	(void)arg;
	(void)crt;
	(void)depth;

#if (DEBUG_LEVEL >= 6)
	char buf[512];
	DEBUG_INFO("Verify requested for (Depth %d):\n", depth);
	mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);
	DEBUG_INFO("%s", buf);
#endif

	if (*flags) {
		DEBUG_WARNING("Certificate verification failed: 0x%x\n",
		              *flags);
	}

	return 0;
}


/**
 * Set TLS server context to request certificate from peer
 * and set trust all certificates of peer.
 *
 * @param tls    TLS Context
 */
void tls_set_verify_client_trust_all(struct tls *tls)
{
	if (!tls)
		return;

	mbedtls_ssl_conf_authmode(&tls->conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
	mbedtls_ssl_conf_verify(&tls->conf, verify_callback, NULL);
}


/**
 * Enable request certificate from peer in TLS server connection
 * Set verify handler.
 *
 * @param tc      TLS connection
 * @param depth   Max depth certificate chain accepted.
 * @param verifyh SSL verify handler. If NULL default verify handler is used.
 * @param arg     Handler argument
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_set_verify_client_handler(struct tls_conn *tc, int depth,
                                  int (*verifyh)(int ok, void *arg),
                                  void *arg)
{
	(void)tc;
	(void)depth;
	(void)verifyh;
	(void)arg;

	/* Custom verify handler not fully supported */
	return 0;
}


/**
 * Set SRTP suites on TLS context
 *
 * @param tls    TLS Context
 * @param suites Secure-RTP Profiles
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_set_srtp(struct tls *tls, const char *suites)
{
	(void)tls;
	(void)suites;

#ifdef MBEDTLS_SSL_DTLS_SRTP
	/* Parse suites string and configure DTLS-SRTP */
	return 0;
#else
	return ENOSYS;
#endif
}


/**
 * Get fingerprint of local certificate
 *
 * @param tls  TLS Context
 * @param type Digest type
 * @param md   Buffer for fingerprint digest
 * @param size Buffer size
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_fingerprint(const struct tls *tls, enum tls_fingerprint type,
                    uint8_t *md, size_t size)
{
	mbedtls_md_type_t md_type;
	int ret;

	if (!tls || !md || !tls->cert_loaded)
		return EINVAL;

	switch (type) {
	case TLS_FINGERPRINT_SHA256:
		if (size < 32)
			return EOVERFLOW;
		md_type = MBEDTLS_MD_SHA256;
		break;
	default:
		return ENOSYS;
	}

	ret = mbedtls_md(mbedtls_md_info_from_type(md_type),
	                 tls->cert.raw.p, tls->cert.raw.len, md);
	if (ret != 0) {
		DEBUG_WARNING("mbedtls_md failed: -0x%x\n", -ret);
		return EINVAL;
	}

	return 0;
}


/**
 * Get fingerprint of peer certificate of a TLS connection
 *
 * @param tc   TLS Connection
 * @param type Digest type
 * @param md   Buffer for fingerprint digest
 * @param size Buffer size
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_peer_fingerprint(const struct tls_conn *tc,
                        enum tls_fingerprint type,
                        uint8_t *md, size_t size)
{
	const mbedtls_x509_crt *peer_cert;
	mbedtls_md_type_t md_type;
	int ret;

	if (!tc || !md)
		return EINVAL;

	peer_cert = mbedtls_ssl_get_peer_cert(&tc->ssl);
	if (!peer_cert)
		return ENOENT;

	switch (type) {
	case TLS_FINGERPRINT_SHA256:
		if (size < 32)
			return EOVERFLOW;
		md_type = MBEDTLS_MD_SHA256;
		break;
	default:
		return ENOSYS;
	}

	ret = mbedtls_md(mbedtls_md_info_from_type(md_type),
	                 peer_cert->raw.p, peer_cert->raw.len, md);
	if (ret != 0) {
		DEBUG_WARNING("mbedtls_md failed: -0x%x\n", -ret);
		return EINVAL;
	}

	return 0;
}


/**
 * Get common name of peer certificate of a TLS connection
 *
 * @param tc   TLS Connection
 * @param cn   Returned common name
 * @param size Size of common name
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_peer_common_name(const struct tls_conn *tc, char *cn, size_t size)
{
	const mbedtls_x509_crt *peer_cert;
	const mbedtls_x509_name *name;

	if (!tc || !cn || !size)
		return EINVAL;

	peer_cert = mbedtls_ssl_get_peer_cert(&tc->ssl);
	if (!peer_cert)
		return ENOENT;

	name = &peer_cert->subject;
	while (name) {
		if (MBEDTLS_OID_CMP(MBEDTLS_OID_AT_CN, &name->oid) == 0) {
			size_t len = name->val.len;
			if (len >= size)
				len = size - 1;
			memcpy(cn, name->val.p, len);
			cn[len] = '\0';
			return 0;
		}
		name = name->next;
	}

	return ENOENT;
}


/**
 * Verify peer certificate of a TLS connection
 *
 * @param tc TLS Connection
 *
 * @return 0 if verified, otherwise errorcode
 */
int tls_peer_verify(const struct tls_conn *tc)
{
	uint32_t flags;

	if (!tc)
		return EINVAL;

	flags = mbedtls_ssl_get_verify_result(&tc->ssl);
	if (flags != 0) {
		DEBUG_WARNING("Certificate verification failed: 0x%x\n", flags);
		return EAUTH;
	}

	return 0;
}


/**
 * Get SRTP suite and keying material of a TLS connection
 *
 * @param tc           TLS Connection
 * @param suite        Returned SRTP suite
 * @param cli_key      Client key
 * @param cli_key_size Client key size
 * @param srv_key      Server key
 * @param srv_key_size Server key size
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_srtp_keyinfo(const struct tls_conn *tc, enum srtp_suite *suite,
                     uint8_t *cli_key, size_t cli_key_size,
                     uint8_t *srv_key, size_t srv_key_size)
{
	(void)tc;
	(void)suite;
	(void)cli_key;
	(void)cli_key_size;
	(void)srv_key;
	(void)srv_key_size;

#ifdef MBEDTLS_SSL_DTLS_SRTP
	/* Implement DTLS-SRTP key extraction */
	return ENOSYS;
#else
	return ENOSYS;
#endif
}


/**
 * Get cipher name of a TLS connection
 *
 * @param tc TLS Connection
 *
 * @return name of cipher actually used.
 */
const char *tls_cipher_name(const struct tls_conn *tc)
{
	if (!tc)
		return NULL;

	return mbedtls_ssl_get_ciphersuite(&tc->ssl);
}


/**
 * Set the ciphers to use for this TLS context
 *
 * @param tls      TLS Context
 * @param cipherv  Vector of cipher names, in order of priority
 * @param count    Number of cipher names in the vector
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_set_ciphers(struct tls *tls, const char *cipherv[], size_t count)
{
	static int ciphersuites[32];
	size_t i;

	if (!tls || !cipherv || !count || count >= ARRAY_SIZE(ciphersuites))
		return EINVAL;

	for (i = 0; i < count; i++) {
		const mbedtls_ssl_ciphersuite_t *info;
		info = mbedtls_ssl_ciphersuite_from_string(cipherv[i]);
		if (!info) {
			DEBUG_WARNING("Unknown cipher: %s\n", cipherv[i]);
			return EINVAL;
		}
		ciphersuites[i] = mbedtls_ssl_ciphersuite_get_id(info);
	}
	ciphersuites[count] = 0;

	mbedtls_ssl_conf_ciphersuites(&tls->conf, ciphersuites);

	return 0;
}


/**
 * Enable verification of server certificate and hostname (SNI)
 *
 * @param tc   TLS Connection
 * @param host Server hostname
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_set_verify_server(struct tls_conn *tc, const char *host)
{
	int ret;

	if (!tc || !host)
		return EINVAL;

	if (!tc->tls->verify_server)
		return 0;

	ret = mbedtls_ssl_set_hostname(&tc->ssl, host);
	if (ret != 0) {
		DEBUG_WARNING("mbedtls_ssl_set_hostname failed: -0x%x\n", -ret);
		return EINVAL;
	}

	mbedtls_ssl_conf_authmode(&tc->tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);

	return 0;
}


/**
 * Enable verification of client certificate
 *
 * @param tc   TLS Connection
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_verify_client(struct tls_conn *tc)
{
	if (!tc)
		return EINVAL;

	if (!tc->tls->verify_client)
		return 0;

#ifdef MTLS_VERIFY_CLIENT
	mbedtls_ssl_conf_authmode(&tc->tls->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
#endif

	return 0;
}


void tls_flush_error(void)
{
	/* MbedTLS doesn't have a global error queue like OpenSSL */
}


/**
 * Get the issuers fields of a certificate chain
 *
 * @param tls  TLS Object
 * @param mb   Memory Buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_get_issuer(struct tls *tls, struct mbuf *mb)
{
	char buf[512];
	int ret;

	if (!tls || !mb || !tls->cert_loaded)
		return EINVAL;

	ret = mbedtls_x509_dn_gets(buf, sizeof(buf), &tls->cert.issuer);
	if (ret < 0) {
		DEBUG_WARNING("mbedtls_x509_dn_gets failed: -0x%x\n", -ret);
		return EINVAL;
	}

	return mbuf_write_str(mb, buf);
}


/**
 * Get the subject fields of a certificate chain
 *
 * @param tls  TLS Object
 * @param mb   Memory Buffer
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_get_subject(struct tls *tls, struct mbuf *mb)
{
	char buf[512];
	int ret;

	if (!tls || !mb || !tls->cert_loaded)
		return EINVAL;

	ret = mbedtls_x509_dn_gets(buf, sizeof(buf), &tls->cert.subject);
	if (ret < 0) {
		DEBUG_WARNING("mbedtls_x509_dn_gets failed: -0x%x\n", -ret);
		return EINVAL;
	}

	return mbuf_write_str(mb, buf);
}


/**
 * Disables SIP TLS server verifications for following requests
 *
 * @param tls     TLS Object
 */
void tls_disable_verify_server(struct tls *tls)
{
	if (!tls)
		return;

	tls->verify_server = false;

	/* Also relax authmode on the client config so outgoing connections
	 * do not fail certificate verification when verify_server is off. */
	mbedtls_ssl_conf_authmode(&tls->client_conf, MBEDTLS_SSL_VERIFY_NONE);
}


/**
 * Enables SIP TLS client verifications for following requests
 *
 * @param tls     TLS Object
 * @param enable  true to enable client verification, false to disable
 */
void tls_enable_verify_client(struct tls *tls, bool enable)
{
	if (!tls)
		return;

	tls->verify_client = enable;
}


/**
 * Set minimum TLS version
 *
 * @param tls     TLS Object
 * @param version Minimum version
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_set_min_proto_version(struct tls *tls, int version)
{
	if (!tls)
		return EINVAL;

	mbedtls_ssl_conf_min_version(&tls->conf, MBEDTLS_SSL_MAJOR_VERSION_3,
	                             version);

	return 0;
}

/**
 * Set maximum TLS version
 *
 * @param tls     TLS Object
 * @param version Maximum version
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_set_max_proto_version(struct tls *tls, int version)
{
	if (!tls)
		return EINVAL;

	mbedtls_ssl_conf_max_version(&tls->conf, MBEDTLS_SSL_MAJOR_VERSION_3,
	                             version);

	return 0;
}


struct session_entry {
	struct le le;
	struct sa peer;
	mbedtls_ssl_session sess;
};


static void session_destructor(void *arg)
{
	struct session_entry *e = arg;

	hash_unlink(&e->le);
	mbedtls_ssl_session_free(&e->sess);
}


static bool session_cmp_handler(struct le *le, void *arg)
{
	const struct session_entry *s = le->data;
	if (!s)
		return false;

	return sa_cmp(&s->peer, arg, SA_ALL);
}


/**
 * Enable/disable TLS session cache.
 *
 * @param tls     TLS Object
 * @param enabled enabled or disable session cache. Default: disabled
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_set_session_reuse(struct tls *tls, int enabled)
{
	if (!tls)
		return EINVAL;

	tls->reuse.enabled = enabled;

	return 0;
}


/**
 * Check if session was reused
 *
 * @param tc  tlc connection object
 *
 * @return 1 reused, 0 otherwise
 */
bool tls_session_reused(const struct tls_conn *tc)
{
	(void)tc;
	/* MbedTLS session resumption check */
	return false;
}


/**
 * getter for session reuse enabled
 *
 * @param tc  tlc connection object
 *
 * @return 1 enabled, 0 disabled
 */
bool tls_get_session_reuse(const struct tls_conn *tc)
{
	if (!tc)
		return false;

	return tc->tls->reuse.enabled;
}


/**
 * Reuse session if possible
 *
 * @param tc  tlc connection object
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_reuse_session(const struct tls_conn *tc)
{
	struct sa peer;
	struct session_entry *sess = NULL;
	int err;

	if (!tc || !tc->tls)
		return EINVAL;

	err = tcp_conn_peer_get(tls_get_tcp_conn(tc), &peer);
	if (err) {
		DEBUG_WARNING("%s: tcp_conn_peer_get failed: (%m).\n",
		              __func__, err);
		return 0;
	}

	sess = list_ledata(hash_lookup(tc->tls->reuse.ht_sessions,
	                               sa_hash(&peer, SA_ALL),
	                               session_cmp_handler, &peer));

	if (sess) {
		err = mbedtls_ssl_set_session((mbedtls_ssl_context *)&tc->ssl,
		                              &sess->sess);
		if (err != 0) {
			DEBUG_WARNING("%s: mbedtls_ssl_set_session failed: -0x%x\n",
			              __func__, -err);
			return EFAULT;
		}
	}

	return 0;
}


/**
 * update session cache manually
 *
 * @param tc  tlc connection object
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_update_sessions(const struct tls_conn *tc)
{
	struct session_entry *e;
	struct sa peer;
	int err;

	if (!tc || !tc->tls)
		return EINVAL;

	err = tcp_conn_peer_get(tls_get_tcp_conn(tc), &peer);
	if (err) {
		DEBUG_WARNING("%s: tcp_conn_peer_get failed: (%m).\n",
		              __func__, err);
		return ENODATA;
	}

	/* Remove existing session for this peer */
	e = list_ledata(hash_lookup(tc->tls->reuse.ht_sessions,
	                            sa_hash(&peer, SA_ALL),
	                            session_cmp_handler, &peer));
	mem_deref(e);

	/* Create new session entry */
	e = mem_zalloc(sizeof(struct session_entry), session_destructor);
	if (!e)
		return ENOMEM;

	mbedtls_ssl_session_init(&e->sess);

	err = mbedtls_ssl_get_session(&tc->ssl, &e->sess);
	if (err != 0) {
		DEBUG_WARNING("%s: mbedtls_ssl_get_session failed: -0x%x\n",
		              __func__, -err);
		mem_deref(e);
		return EINVAL;
	}

	sa_cpy(&e->peer, &peer);

	hash_append(tc->tls->reuse.ht_sessions, sa_hash(&e->peer, SA_ALL),
	            &e->le, e);

	return 0;
}


/**
 * Get SSL config (server mode)
 *
 * @param tls  tls object
 *
 * @return mbedtls_ssl_config* if set or NULL otherwise
 */
mbedtls_ssl_config *tls_ssl_config(const struct tls *tls)
{
	if (!tls)
		return NULL;

	return (mbedtls_ssl_config *)&tls->conf;
}


/**
 * Get SSL config for outgoing (client-mode) connections
 *
 * @param tls  tls object
 *
 * @return mbedtls_ssl_config* for client mode, or NULL
 */
mbedtls_ssl_config *tls_ssl_client_config(const struct tls *tls)
{
	if (!tls)
		return NULL;

	return (mbedtls_ssl_config *)&tls->client_conf;
}


static void tls_cert_destructor(void *arg)
{
	struct tls_cert *uc = arg;

	mem_deref(uc->host);
	if (uc->cert_loaded)
		mbedtls_x509_crt_free(&uc->cert);
	if (uc->pkey_loaded)
		mbedtls_pk_free(&uc->pkey);
	mbedtls_ssl_config_free(&uc->conf);
}


/**
 * Add a certificate for SNI-based selection from PEM data
 *
 * @param tls      TLS context
 * @param certpem  Certificate in PEM format
 * @param certlen  Length of certificate PEM
 * @param keypem   Private key in PEM format
 * @param keylen   Length of key PEM
 * @param host     Hostname that should match the SNI from client hello
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_add_cert_pem(struct tls *tls, const char *certpem, size_t certlen,
                     const char *keypem, size_t keylen, const char *host)
{
	struct tls_cert *uc;
	int err = 0;
	int ret;

	if (!tls || !certpem || !certlen)
		return EINVAL;

	uc = mem_zalloc(sizeof(*uc), tls_cert_destructor);
	if (!uc)
		return ENOMEM;

	if (str_isset(host)) {
		err = str_dup(&uc->host, host);
		if (err)
			goto out;
	}

	/* Initialize config */
	mbedtls_ssl_config_init(&uc->conf);
	ret = mbedtls_ssl_config_defaults(&uc->conf,
	                                  MBEDTLS_SSL_IS_SERVER,
	                                  MBEDTLS_SSL_TRANSPORT_STREAM,
	                                  MBEDTLS_SSL_PRESET_DEFAULT);
	if (ret != 0) {
		err = EINVAL;
		goto out;
	}

	/* Parse certificate */
	mbedtls_x509_crt_init(&uc->cert);
	uc->cert_loaded = true;
	ret = mbedtls_x509_crt_parse(&uc->cert,
	                             (const unsigned char *)certpem,
	                             certlen);
	if (ret < 0) {
		DEBUG_WARNING("mbedtls_x509_crt_parse failed: -0x%x\n", -ret);
		err = EINVAL;
		goto out;
	}

	/* Parse private key */
	if (keypem && keylen) {
		mbedtls_pk_init(&uc->pkey);
		uc->pkey_loaded = true;

		ret = mbedtls_pk_parse_key(&uc->pkey,
		                           (const unsigned char *)keypem,
		                           keylen, NULL, 0,
								    mbedtls_ctr_drbg_random, &tls->ctr_drbg);
		if (ret != 0) {
			DEBUG_WARNING("mbedtls_pk_parse_key failed: -0x%x\n",
			              -ret);
			err = EINVAL;
			goto out;
		}

		ret = mbedtls_ssl_conf_own_cert(&uc->conf, &uc->cert, &uc->pkey);
		if (ret != 0) {
			err = EINVAL;
			goto out;
		}
	}

	/* Copy CA chain from main TLS context */
	if (tls->cacert_loaded) {
		mbedtls_ssl_conf_ca_chain(&uc->conf, &tls->cacert,
		                          tls->crl_loaded ? &tls->crl : NULL);
	}

	mbedtls_ssl_conf_rng(&uc->conf, mbedtls_ctr_drbg_random,
	                     &tls->ctr_drbg);

	list_append(&tls->certs, &uc->le, uc);
	if (list_count(&tls->certs) == 1)
		tls_enable_sni(tls);

	return 0;

out:
	mem_deref(uc);
	return err;
}


/**
 * Adds a certificate for Server Name Indication (SNI) based certificate
 * selection (file-based - not supported in this implementation)
 *
 * @param tls   TLS context
 * @param certf Filename of the certificate (not used)
 * @param host  Hostname that should match the SNI from client hello
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_add_certf(struct tls *tls, const char *certf, const char *host)
{
	(void)tls;
	(void)certf;
	(void)host;
	/* File-based loading not supported */
	return ENOSYS;
}


/**
 * Returns the X509 of the TLS certificate
 *
 * @param hc  TLS certificate
 *
 * @return The MbedTLS X509
 */
mbedtls_x509_crt *tls_cert_x509(struct tls_cert *hc)
{
	return hc ? &hc->cert : NULL;
}


/**
 * Returns the SSL config of the TLS certificate
 *
 * @param hc  TLS certificate
 *
 * @return The SSL config
 */
mbedtls_ssl_config *tls_cert_config(struct tls_cert *hc)
{
	return hc ? &hc->conf : NULL;
}


/**
 * Returns the host name of the TLS certificate
 *
 * @param hc  TLS certificate
 *
 * @return The host name
 */
const char *tls_cert_host(struct tls_cert *hc)
{
	return hc ? hc->host : NULL;
}


/**
 * Returns the list of TLS certificates
 *
 * @param tls TLS context
 *
 * @return The list
 */
const struct list *tls_certs(const struct tls *tls)
{
	return tls ? &tls->certs : NULL;
}


/**
 * Enable/disable posthandshake
 * Only on client side for TLSv1.3
 *
 * @param tls   tls object
 * @param value posthandshake auth value. 1 enabled, Default: 0
 *
 */
void tls_set_posthandshake_auth(struct tls *tls, int value)
{
	(void)tls;
	(void)value;
	/* Not supported in MbedTLS */
}


/**
 * Request client certificate using post handshake
 * Only on client side for TLSv1.3
 *
 * @param tc  tls connection
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_verify_client_post_handshake(struct tls_conn *tc)
{
	(void)tc;
	/* Not supported in MbedTLS */
	return ENOSYS;
}


/**
 * Set TLS session resumption mode
 *
 * @param tls  TLS Object
 * @param mode TLS session resumption mode
 *
 * @return 0 if success, otherwise errorcode
 */
int tls_set_resumption(struct tls *tls, enum tls_resume_mode mode)
{
	if (!tls)
		return EINVAL;

	if (mode & TLS_RESUMPTION_IDS) {
		tls->reuse.enabled = true;
	}
	else {
		tls->reuse.enabled = false;
	}

	if (mode & TLS_RESUMPTION_TICKETS) {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
		mbedtls_ssl_conf_session_tickets(&tls->conf,
		                                 MBEDTLS_SSL_SESSION_TICKETS_ENABLED);
#endif
	}
	else {
#ifdef MBEDTLS_SSL_SESSION_TICKETS
		mbedtls_ssl_conf_session_tickets(&tls->conf,
		                                 MBEDTLS_SSL_SESSION_TICKETS_DISABLED);
#endif
	}

	return 0;
}


/**
 * Change used certificate+key of an existing TLS context from PEM string
 *
 * @param tls       TLS Object
 * @param chain     Cert (chain) + Key in PEM format
 * @param len_chain Length of certificate + key PEM string
 *
 * @return int 0 if success, otherwise errorcode
 */
int tls_set_certificate_chain_pem(struct tls *tls, const char *chain,
                                  size_t len_chain)
{
	int ret;
	const char *key_start;
	size_t cert_len, key_len;

	if (!tls || !chain || !len_chain)
		return EINVAL;

	/* Find the private key in the chain */
	key_start = strstr(chain, "-----BEGIN");
	if (!key_start)
		return EINVAL;

	/* Skip certificates to find private key */
	while (key_start) {
		if (strstr(key_start, "-----BEGIN PRIVATE KEY-----") ||
		    strstr(key_start, "-----BEGIN RSA PRIVATE KEY-----") ||
		    strstr(key_start, "-----BEGIN EC PRIVATE KEY-----")) {
			break;
		}
		key_start = strstr(key_start + 1, "-----BEGIN");
	}

	if (!key_start)
		return EINVAL;

	cert_len = key_start - chain;
	key_len = len_chain - cert_len;

	/* Free existing certificate and key */
	if (tls->cert_loaded) {
		mbedtls_x509_crt_free(&tls->cert);
		tls->cert_loaded = false;
	}
	if (tls->pkey_loaded) {
		mbedtls_pk_free(&tls->pkey);
		tls->pkey_loaded = false;
	}

	/* Parse certificate chain */
	mbedtls_x509_crt_init(&tls->cert);
	tls->cert_loaded = true;

	ret = mbedtls_x509_crt_parse(&tls->cert,
	                             (const unsigned char *)chain,
	                             cert_len + 1);
	if (ret < 0) {
		DEBUG_WARNING("mbedtls_x509_crt_parse failed: -0x%x\n", -ret);
		return EINVAL;
	}

	/* Parse private key */
	mbedtls_pk_init(&tls->pkey);
	tls->pkey_loaded = true;

	ret = mbedtls_pk_parse_key(&tls->pkey,
	                           (const unsigned char *)key_start,
	                           key_len + 1, NULL, 0, mbedtls_ctr_drbg_random, &tls->ctr_drbg);
	if (ret != 0) {
		DEBUG_WARNING("mbedtls_pk_parse_key failed: -0x%x\n", -ret);
		return EINVAL;
	}

	ret = mbedtls_ssl_conf_own_cert(&tls->conf, &tls->cert, &tls->pkey);
	if (ret != 0) {
		DEBUG_WARNING("mbedtls_ssl_conf_own_cert failed: -0x%x\n", -ret);
		return EINVAL;
	}

	return 0;
}


/**
 * Change used certificate+key of an existing TLS context from file
 * (not supported in this implementation)
 *
 * @param tls  TLS Object
 * @param path Path to Cert (chain) + Key file (PEM format)
 *
 * @return int 0 if success, otherwise errorcode
 */
int tls_set_certificate_chain(struct tls *tls, const char *path)
{
	(void)tls;
	(void)path;
	/* File-based loading not supported */
	return ENOSYS;
}
