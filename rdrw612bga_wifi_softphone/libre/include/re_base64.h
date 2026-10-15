/**
 * @file re_base64.h  Interface to Base64 encoding/decoding functions
 *
 * Copyright (C) 2010 Creytiv.com
 */


int re_base64_encode(const uint8_t *in, size_t ilen, char *out, size_t *olen);
int base64url_encode(const uint8_t *in, size_t ilen, char *out, size_t *olen);
int base64_print(struct re_printf *pf, const uint8_t *ptr, size_t len);
int re_base64_decode(const char *in, size_t ilen, uint8_t *out, size_t *olen);
