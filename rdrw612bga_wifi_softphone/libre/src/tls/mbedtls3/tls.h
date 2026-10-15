/*
 * Copyright 2026 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef TLS_MBEDTLS_H
#define TLS_MBEDTLS_H

#include <mbedtls/ssl.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <re_list.h>

struct session_reuse {
	bool enabled;
	struct hash *ht_sessions;
};

struct tls {
	mbedtls_ssl_config conf;        /**< Server-mode TLS config (accepts connections) */
	mbedtls_ssl_config client_conf; /**< Client-mode TLS config (outgoing connections) */
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_x509_crt cacert;
	mbedtls_x509_crt cert;
	mbedtls_pk_context pkey;
	mbedtls_x509_crl crl;
	enum tls_method method;
	bool verify_server;
	bool verify_client;
	struct session_reuse reuse;
	struct list certs;
	bool cert_loaded;
	bool pkey_loaded;
	bool cacert_loaded;
	bool crl_loaded;
};

struct tls_cert {
	struct le le;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt cert;
	mbedtls_pk_context pkey;
	char *host;
	bool cert_loaded;
	bool pkey_loaded;
};

void tls_flush_error(void);
mbedtls_ssl_config *tls_ssl_config(const struct tls *tls);
mbedtls_ssl_config *tls_ssl_client_config(const struct tls *tls);
mbedtls_x509_crt *tls_cert_x509(struct tls_cert *hc);
mbedtls_ssl_config *tls_cert_config(struct tls_cert *hc);

const char *tls_cert_host(struct tls_cert *hc);
const struct list *tls_certs(const struct tls *tls);

struct tls_cert *tls_cert_for_sni(const struct tls *tls, const char *sni);
void tls_enable_sni(struct tls *tls);

#endif /* TLS_MBEDTLS_H */
