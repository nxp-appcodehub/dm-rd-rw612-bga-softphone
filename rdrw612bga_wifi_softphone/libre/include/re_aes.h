/**
 * @file re_aes.h Interface to AES (Advanced Encryption Standard)
 *
 * Copyright (C) 2010 Creytiv.com
 */


#ifndef AES_BLOCK_SIZE
#define AES_BLOCK_SIZE 16
#endif

#ifndef AES_IV_LEN
#define AES_IV_LEN     16
#endif
#ifndef AES_GCM_IV_LEN
#define AES_GCM_IV_LEN 12
#endif

/** AES mode */
enum aes_mode {
	AES_MODE_CTR,  /**< AES Counter mode (CTR) */
	AES_MODE_GCM,  /**< AES Galois Counter Mode (GCM) */
};

struct aes;

int  aes_alloc(struct aes **stp, enum aes_mode mode,
	       const uint8_t *key, size_t key_bits,
	       const uint8_t *iv);
void aes_set_iv(struct aes *aes, const uint8_t *iv, size_t iv_len);
int  aes_encr(struct aes *aes, uint8_t *out, const uint8_t *in, size_t len);
int  aes_decr(struct aes *aes, uint8_t *out, const uint8_t *in, size_t len);
// AES GCM encryption with associated data
int  aes_gcm_encr(struct aes *aes,
	              uint8_t *ct_out, size_t ct_buff_len, size_t *ct_len,
	              const uint8_t *pt_in, size_t pt_len,
				  const uint8_t *ad_in, size_t ad_len);
// AES GCM decryption with associated data
int  aes_gcm_decr(struct aes *aes,
	              uint8_t *pt_out, size_t pt_buff_len, size_t *pt_len,
	              const uint8_t *ct_in, size_t ct_len,
				  const uint8_t *ad_in, size_t ad_len);
int psa_test(void);
int srtp_test_vector(void);
int srtp_test_decrypt(void);
int srtp_test_encrypt(void);