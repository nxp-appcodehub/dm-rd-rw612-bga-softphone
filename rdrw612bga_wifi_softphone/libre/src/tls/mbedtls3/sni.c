/*
 * Copyright 2026 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <mbedtls/ssl.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/oid.h>
#include <re_types.h>
#include <re_fmt.h>
#include <re_mem.h>
#include <re_sa.h>
#include <re_list.h>
#include <re_srtp.h>
#include <re_tls.h>
#include "tls.h"


#define DEBUG_MODULE "tls/sni"
#define DEBUG_LEVEL 5
#include <re_dbg.h>


struct tls_conn;


/**
 * Check if SNI matches certificate's Subject Alternative Name
 */
static int x509_match_alt_name(mbedtls_x509_crt *cert, const char *sni,
                               bool *match)
{
	const mbedtls_x509_sequence *cur;
	mbedtls_x509_subject_alternative_name san;
	int ret;

	*match = false;

	if (!cert || !sni)
		return EINVAL;

	/* Check Subject Alternative Names */
	cur = &cert->subject_alt_names;
	while (cur != NULL) {
		ret = mbedtls_x509_parse_subject_alt_name(&cur->buf, &san);
		if (ret == 0) {
			if (san.type == MBEDTLS_X509_SAN_DNS_NAME) {
				size_t len = san.san.unstructured_name.len;
				const char *name = (const char *)
					san.san.unstructured_name.p;

				if (len == strlen(sni) &&
				    strncasecmp(name, sni, len) == 0) {
					*match = true;
					mbedtls_x509_free_subject_alt_name(&san);
					return 0;
				}
			}
			mbedtls_x509_free_subject_alt_name(&san);
		}
		cur = cur->next;
	}

	return 0;
}


/**
 * Get Common Name from certificate
 */
static int get_common_name(mbedtls_x509_crt *cert, char *cn, size_t size)
{
	const mbedtls_x509_name *name;

	if (!cert || !cn || !size)
		return EINVAL;

	name = &cert->subject;
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
 * Finds a TLS certificate that matches a given Server Name Indication (SNI)
 *
 * @param tls TLS Context
 * @param sni Server Name Indication
 *
 * @return TLS certificate or NULL if not found
 */
struct tls_cert *tls_cert_for_sni(const struct tls *tls, const char *sni)
{
	struct tls_cert *tls_cert = NULL;
	struct le *le;
	const struct list *certs = tls_certs(tls);

	if (!list_head(certs))
		return NULL;

	if (!str_isset(sni))
		return list_head(certs)->data;

	LIST_FOREACH(certs, le) {
		char cn[256] = "";
		mbedtls_x509_crt *x509;
		bool match = false;
		int err;

		tls_cert = le->data;
		x509 = tls_cert_x509(tls_cert);
		if (!x509) {
			tls_cert = NULL;
			continue;
		}

		/* Check Common Name */
		err = get_common_name(x509, cn, sizeof(cn));
		if (err == 0 && strcasecmp(sni, cn) == 0)
			break;

		/* Check Subject Alternative Names */
		err = x509_match_alt_name(x509, sni, &match);
		if (err) {
			tls_cert = NULL;
			break;
		}

		if (match)
			break;

		tls_cert = NULL;
	}

	return tls_cert;
}


/**
 * SNI callback for MbedTLS
 */
static int sni_callback(void *arg, mbedtls_ssl_context *ssl,
                        const unsigned char *name, size_t name_len)
{
	struct tls *tls = arg;
	struct tls_cert *cert;
	char sni[256];
	int ret;

	if (!tls || !ssl || !name || name_len == 0)
		return 0;

	if (name_len >= sizeof(sni))
		return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;

	memcpy(sni, name, name_len);
	sni[name_len] = '\0';

	DEBUG_INFO("SNI callback for: %s\n", sni);

	cert = tls_cert_for_sni(tls, sni);
	if (!cert) {
		DEBUG_WARNING("No certificate found for SNI: %s\n", sni);
		return MBEDTLS_ERR_SSL_UNRECOGNIZED_NAME;
	}

	DEBUG_INFO("Found certificate for SNI: %s\n", sni);

	/* Set the certificate and key for this connection */
	ret = mbedtls_ssl_set_hs_own_cert(ssl, tls_cert_x509(cert),
	                                  &cert->pkey);
	if (ret != 0) {
		DEBUG_WARNING("mbedtls_ssl_set_hs_own_cert failed: -0x%x\n",
		              -ret);
		return ret;
	}

	return 0;
}


/**
 * Enables SNI handling on the given TLS context for incoming TLS connections
 *
 * @param tls TLS Context
 */
void tls_enable_sni(struct tls *tls)
{
	if (!tls)
		return;

	mbedtls_ssl_conf_sni(tls_ssl_config(tls), sni_callback, tls);
}
