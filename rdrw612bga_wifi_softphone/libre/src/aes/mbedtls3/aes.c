/**
 * @file mbedtls/aes.c AES (Advanced Encryption Standard) using PSA Crypto API
 *
 * Replacement for original OpenSSL aes.c
 * Copyright 2026 NXP
 */

#include <string.h>
#include <re_types.h>
#include <re_mem.h>
#include <re_fmt.h>
#include <re_aes.h>
// Include PSA Crypto API from MbedTLS3
#include <psa/crypto.h>

#define DEBUG_MODULE "aes"
#define DEBUG_LEVEL 6
#include <re_dbg.h>

#define AES_GCM_TAG_SIZE 16

#ifndef EAUTH
#define EAUTH 80  /* Authentication error */
#endif

/*
 * Internal AES representation using PSA Crypto
 */
struct aes {
    enum aes_mode mode;
    bool encr;
    bool op_started;

    psa_key_id_t key_id;

    union {
        psa_cipher_operation_t cipher_op;
        psa_aead_operation_t   aead_op;
    };

    uint8_t iv[AES_IV_LEN];
    size_t  iv_len;

    size_t key_bits;
};

#define AES_KEY_BITS 128
#define AES_KEY_BYTES (AES_KEY_BITS / 8)
#define IV_SIZE 12

void __print_buffer(uint8_t* buff, size_t len)
{
	char print_buff[(len*3)+1];
	for(size_t i = 0; i<len; i++) {
		sprintf(&print_buff[i*3],"%02x,", buff[i]);
	}
	print_buff[len*3] = '\0';
	DEBUG_INFO("%s\n", print_buff);
}

int srtp_test_decrypt(void)
{
    psa_status_t status;
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    const uint8_t key[AES_KEY_BYTES] = {
        0x8b, 0x77, 0xfc, 0xec, 0xbc, 0x2b, 0xbd, 0x7e,
        0x2e, 0x4a, 0x5c, 0x60, 0xe9, 0x73, 0xcf, 0x02
    };

    const uint8_t salt[IV_SIZE] = {0x54, 0xdc, 0xe4, 0x98,
                                   0xcf, 0xf0, 0x71, 0x93,
                                   0x45, 0x66, 0x69, 0x96};
    uint8_t iv[IV_SIZE] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    const uint8_t ciphertext[] = {0xa3, 0xe6, 0x31, 0x41, 0xa4, 0x49, 0xae, 0x04,
                                  0xab, 0x77, 0xeb, 0x00, 0x81, 0xae, 0xca, 0xf1,
                                  0x60, 0xbb, 0x86, 0x9e, 0xd1, 0xd0, 0x62, 0x80,
                                  0x0c, 0x91, 0xc2, 0x69, 0x18};
    const size_t ciphertext_len = sizeof(ciphertext);

    const uint8_t additional_data[] = {0x80, 0x60, 0x00, 0x00,
                                       0x5b, 0x6a, 0xfd, 0xb3,
                                       0x57, 0xd0, 0x28, 0x89};

    const size_t additional_data_len = sizeof(additional_data);

    uint8_t decrypted[sizeof(ciphertext)];
    size_t decrypted_len;

    // Initialize PSA Crypto
    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Failed to initialize PSA Crypto: %d\n", status);
        return 1;
    }
    DEBUG_INFO("PSA Crypto initialized\n");

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_GCM);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, AES_KEY_BITS);

    // Import the key
    status = psa_import_key(&attributes, key, AES_KEY_BYTES, &key_id);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Failed to import key: %d\n", status);
        return 1;
    }

    DEBUG_INFO("Key imported successfully\n");

    // build the IV
    iv[0] = salt[0];
    iv[1] = salt[1];
    iv[2] = salt[2]^additional_data[8];
    iv[3] = salt[3]^additional_data[9];
    iv[4] = salt[4]^additional_data[10];
    iv[5] = salt[5]^additional_data[11];
    iv[6] = salt[6];
    iv[7] = salt[7];
    iv[8] = salt[8];
    iv[9] = salt[9];
    iv[10] = salt[10]^additional_data[2];
    iv[11] = salt[11]^additional_data[3];

    // Decrypt
    status = psa_aead_decrypt(key_id, PSA_ALG_GCM,
                              iv, IV_SIZE,
                              additional_data, additional_data_len,
                              ciphertext, ciphertext_len,
                              decrypted, sizeof(decrypted), &decrypted_len);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Decryption failed: %d\n", status);
        psa_destroy_key(key_id);
        return 1;
    }

    DEBUG_INFO("Decryption successful, plaintext length: %zu\n", decrypted_len);
    // Print decrypted text
    DEBUG_INFO("Decrypted data: ");
    __print_buffer(decrypted, decrypted_len);

    // Clean up
    psa_destroy_key(key_id);
    mbedtls_psa_crypto_free();

    return 0;
}

int srtp_test_encrypt(void)
{
    psa_status_t status;
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    const uint8_t key[AES_KEY_BYTES] = {
          0xc3, 0x41, 0x24, 0x1c, 0xb3, 0xf3, 0xe4, 0x9c, 0x30, 0x9e, 0x08, 0x94, 0x44, 0xcf, 0x32, 0x96
    };

    const uint8_t salt[IV_SIZE] = { 0xd3, 0x4d, 0x72, 0x97, 0xd4, 0xf7, 0x6a, 0x73, 0x1e, 0x0b, 0x2a, 0x26};
    uint8_t iv[IV_SIZE] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    const uint8_t plaintext[] = { 0x48, 0x0b, 0xe4, 0xc1, 0x22, 0x23, 0x61, 0xf0};

    const size_t plaintext_len = sizeof(plaintext);

    const uint8_t additional_data[] = { 0x80, 0xe0, 0x3a, 0x14, 0x00, 0x00, 0x4b, 0xa4, 0x3e, 0x58, 0xcf, 0x9e};

    const size_t additional_data_len = sizeof(additional_data);

    uint8_t chipertext[sizeof(plaintext) + 16];
    size_t encrypted_len;

    // Initialize PSA Crypto
    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Failed to initialize PSA Crypto: %d\n", status);
        return 1;
    }
    DEBUG_INFO("PSA Crypto initialized\n");

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_GCM);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, AES_KEY_BITS);

    // Import the key
    status = psa_import_key(&attributes, key, AES_KEY_BYTES, &key_id);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Failed to import key: %d\n", status);
        return 1;
    }

    DEBUG_INFO("Key imported successfully\n");

    // build the IV
    iv[0] = salt[0];
    iv[1] = salt[1];
    iv[2] = salt[2]^additional_data[8];
    iv[3] = salt[3]^additional_data[9];
    iv[4] = salt[4]^additional_data[10];
    iv[5] = salt[5]^additional_data[11];
    iv[6] = salt[6];
    iv[7] = salt[7];
    iv[8] = salt[8];
    iv[9] = salt[9];
    iv[10] = salt[10]^additional_data[2];
    iv[11] = salt[11]^additional_data[3];

    DEBUG_INFO("AES IV: ");
    __print_buffer(iv, IV_SIZE);
    // Encrypt
    status = psa_aead_encrypt(key_id, PSA_ALG_GCM,
                              iv, IV_SIZE,
                              additional_data, additional_data_len,
                              plaintext, plaintext_len,
                              chipertext, sizeof(chipertext), &encrypted_len);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Encryption failed: %d\n", status);
        psa_destroy_key(key_id);
        return 1;
    }

    DEBUG_INFO("Encryption successful, ciphertext length: %zu\n", encrypted_len);
    // Print encrypted text
    DEBUG_INFO("Encrypted data: ");
    __print_buffer(chipertext, encrypted_len);

    // Clean up
    psa_destroy_key(key_id);
    mbedtls_psa_crypto_free();

    return 0;
}

int srtp_test_vector(void)
{
    psa_status_t status;
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    const uint8_t key[AES_KEY_BYTES] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    const uint8_t iv[IV_SIZE] = {
        0x51, 0x75, 0x3c, 0x65, 0x80, 0xc2, 0x72, 0x6f, 0x20, 0x71, 0x84, 0x14
    };

    const uint8_t plaintext[] = {0x47,0x61,0x6c,0x6c,  0x69,0x61,0x20,0x65,  0x73,0x74,0x20,0x6f,  0x6d,0x6e,0x69,0x73,
                                 0x20,0x64,0x69,0x76,  0x69,0x73,0x61,0x20,  0x69,0x6e,0x20,0x70,  0x61,0x72,0x74,0x65,
                                 0x73,0x20,0x74,0x72,  0x65,0x73};
    const size_t plaintext_len = sizeof(plaintext);

    const uint8_t additional_data[] = {0x80,0x40,0xf1,0x7b,  0x80,0x41,0xf8,0xd3,  0x55,0x01,0xa0,0xb2};
    const size_t additional_data_len = sizeof(additional_data);

    uint8_t ciphertext[sizeof(plaintext) + 32];
    size_t ciphertext_len;

    uint8_t decrypted[sizeof(plaintext) + 32];
    size_t decrypted_len;

    // Initialize PSA Crypto
    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Failed to initialize PSA Crypto: %d\n", status);
        return 1;
    }
    DEBUG_INFO("PSA Crypto initialized\n");

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_GCM);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, AES_KEY_BITS);

    // Import the key
    status = psa_import_key(&attributes, key, AES_KEY_BYTES, &key_id);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Failed to import key: %d\n", status);
        return 1;
    }
    DEBUG_INFO("Key imported successfully\n");

    // Encrypt
    status = psa_aead_encrypt(key_id, PSA_ALG_GCM,
                              iv, IV_SIZE,
                              additional_data, additional_data_len,
                              plaintext, plaintext_len,
                              ciphertext, sizeof(ciphertext), &ciphertext_len);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Encryption failed: %d\n", status);
        psa_destroy_key(key_id);
        return 1;
    }
    DEBUG_INFO("Encryption successful, ciphertext length: %zu\n", ciphertext_len);

    // Print ciphertext (including tag)
    DEBUG_INFO("Ciphertext + Tag: ");
    for (size_t i = 0; i < ciphertext_len; i++) {
        DEBUG_INFO("%02x", ciphertext[i]);
    }
    DEBUG_INFO("\n");

    // Decrypt
    status = psa_aead_decrypt(key_id, PSA_ALG_GCM,
                              iv, IV_SIZE,
                              additional_data, additional_data_len,
                              ciphertext, ciphertext_len,
                              decrypted, sizeof(decrypted), &decrypted_len);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Decryption failed: %d\n", status);
        psa_destroy_key(key_id);
        return 1;
    }

    DEBUG_INFO("Decryption successful, plaintext length: %zu\n", decrypted_len);

    // Print decrypted text
    DEBUG_INFO("Decrypted text: %.*s\n", (int)decrypted_len, decrypted);

    // Verify
    if (decrypted_len == plaintext_len &&
        memcmp(decrypted, plaintext, plaintext_len) == 0) {
        DEBUG_INFO("SUCCESS: Decrypted text matches original plaintext!\n");
    } else {
        DEBUG_WARNING("ERROR: Decrypted text does not match!\n");
    }

    // Clean up
    psa_destroy_key(key_id);
    mbedtls_psa_crypto_free();

    return 0;
}

int psa_test(void)
{
    psa_status_t status;
    psa_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    // Sample data
    const uint8_t key[AES_KEY_BYTES] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    const uint8_t iv[IV_SIZE] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b
    };

    const uint8_t plaintext[] = "Hello, PSA Crypto!";
    const size_t plaintext_len = sizeof(plaintext) - 1;

    const uint8_t additional_data[] = "Additional Data";
    const size_t additional_data_len = sizeof(additional_data) - 1;

    uint8_t ciphertext[64];
    size_t ciphertext_len;

    uint8_t decrypted[64];
    size_t decrypted_len;

    // Initialize PSA Crypto
    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Failed to initialize PSA Crypto: %d\n", status);
        return 1;
    }
    DEBUG_INFO("PSA Crypto initialized\n");

    // Set up key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_GCM);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, AES_KEY_BITS);

    // Import the key
    status = psa_import_key(&attributes, key, AES_KEY_BYTES, &key_id);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Failed to import key: %d\n", status);
        return 1;
    }
    DEBUG_INFO("Key imported successfully\n");

    // Encrypt
    status = psa_aead_encrypt(key_id, PSA_ALG_GCM,
                              iv, IV_SIZE,
                              additional_data, additional_data_len,
                              plaintext, plaintext_len,
                              ciphertext, sizeof(ciphertext), &ciphertext_len);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Encryption failed: %d\n", status);
        psa_destroy_key(key_id);
        return 1;
    }
    DEBUG_INFO("Encryption successful, ciphertext length: %zu\n", ciphertext_len);

    // Print ciphertext (including tag)
    DEBUG_INFO("Ciphertext + Tag: ");
    for (size_t i = 0; i < ciphertext_len; i++) {
        DEBUG_INFO("%02x", ciphertext[i]);
    }
    DEBUG_INFO("\n");

    // Decrypt
    status = psa_aead_decrypt(key_id, PSA_ALG_GCM,
                              iv, IV_SIZE,
                              additional_data, additional_data_len,
                              ciphertext, ciphertext_len,
                              decrypted, sizeof(decrypted), &decrypted_len);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("Decryption failed: %d\n", status);
        psa_destroy_key(key_id);
        return 1;
    }
    DEBUG_INFO("Decryption successful, plaintext length: %zu\n", decrypted_len);

    // Print decrypted text
    DEBUG_INFO("Decrypted text: %.*s\n", (int)decrypted_len, decrypted);

    // Verify
    if (decrypted_len == plaintext_len &&
        memcmp(decrypted, plaintext, plaintext_len) == 0) {
        DEBUG_INFO("SUCCESS: Decrypted text matches original plaintext!\n");
    } else {
        DEBUG_WARNING("ERROR: Decrypted text does not match!\n");
    }

    // Clean up
    psa_destroy_key(key_id);
    mbedtls_psa_crypto_free();

    return 0;
}

/* ---------- Helper: Initialize PSA Crypto ---------- */

static psa_status_t ensure_psa_initialized(void)
{
    static bool initialized = false;
    psa_status_t status;

    if (!initialized) {
        status = psa_crypto_init();
        if (status != PSA_SUCCESS) {
            DEBUG_WARNING("psa_crypto_init failed: %d\n", (int)status);
            return status;
        }
        initialized = true;
    }

    return PSA_SUCCESS;
}

/* ---------- Helper: Get PSA Algorithm ---------- */

static psa_algorithm_t get_psa_algorithm(enum aes_mode mode)
{
    switch (mode) {
    case AES_MODE_CTR:
        return PSA_ALG_CTR;
    case AES_MODE_GCM:
        return PSA_ALG_GCM;
    default:
        return PSA_ALG_NONE;
    }
}

/* ---------- Constructor / Destructor ---------- */

static void destructor(void *arg)
{
    struct aes *st = arg;

    /* Abort any ongoing operation */
    if (st->op_started) {
        if (st->mode == AES_MODE_CTR) {
            psa_cipher_abort(&st->cipher_op);
        }
        else if (st->mode == AES_MODE_GCM) {
            psa_aead_abort(&st->aead_op);
        }
    }

    /* Destroy the key */
    if (st->key_id != 0) {
        psa_destroy_key(st->key_id);
    }
}

/* ---------- Helper: Start CTR Operation ---------- */

static int start_ctr_operation(struct aes *aes)
{
    psa_status_t status;

    aes->cipher_op = psa_cipher_operation_init();

    if (aes->encr) {
        status = psa_cipher_encrypt_setup(&aes->cipher_op,
                                          aes->key_id,
                                          PSA_ALG_CTR);
    }
    else {
        status = psa_cipher_decrypt_setup(&aes->cipher_op,
                                          aes->key_id,
                                          PSA_ALG_CTR);
    }

    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("psa_cipher_setup failed: %d\n", (int)status);
        return EPROTO;
    }

    status = psa_cipher_set_iv(&aes->cipher_op, aes->iv, aes->iv_len);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("psa_cipher_set_iv failed: %d\n", (int)status);
        psa_cipher_abort(&aes->cipher_op);
        return EPROTO;
    }

    aes->op_started = true;
    return 0;
}

/* ---------- AES Allocation ---------- */

int aes_alloc(struct aes **aesp,
              enum aes_mode mode,
              const uint8_t *key,
              size_t key_bits,
              const uint8_t *iv)
{
    struct aes *st;
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_algorithm_t alg;

    if (!aesp || !key)
        return EINVAL;

    if (key_bits != 128 && key_bits != 256)
        return ENOTSUP;

    alg = get_psa_algorithm(mode);
    if (alg == PSA_ALG_NONE)
        return ENOTSUP;

    /* Ensure PSA Crypto is initialized */
    status = ensure_psa_initialized();
    if (status != PSA_SUCCESS)
        return EPROTO;

    st = mem_zalloc(sizeof(*st), destructor);
    if (!st)
        return ENOMEM;

    st->mode     = mode;
    st->encr     = true;
    st->key_bits = key_bits;
    st->key_id   = 0;
    st->op_started = false;

    /* Set up key attributes */
    psa_set_key_usage_flags(&attributes,
                            PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, key_bits);

    /* Import the key */
    status = psa_import_key(&attributes, key, key_bits / 8, &st->key_id);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("psa_import_key failed: %d\n", (int)status);
        mem_deref(st);
        return EPROTO;
    }

    /* Store IV if provided */
    if (iv) {
        memcpy(st->iv, iv, AES_IV_LEN);
        st->iv_len = AES_IV_LEN;
    }

    *aesp = st;
    return 0;
}

/* ---------- AES Set IV ---------- */

void aes_set_iv(struct aes *aes, const uint8_t *iv, size_t iv_len)
{
    if (!aes || !iv)
        return;
    if(iv_len > AES_IV_LEN) iv_len = AES_IV_LEN;

    /* If an operation is in progress, abort it */
    if (aes->op_started) {
        if (aes->mode == AES_MODE_CTR) {
            psa_cipher_abort(&aes->cipher_op);
        }
        else if (aes->mode == AES_MODE_GCM) {
            psa_aead_abort(&aes->aead_op);
        }
        aes->op_started = false;
    }

    memcpy(aes->iv, iv, iv_len);
    aes->iv_len = iv_len;
}

/* ---------- AES-CTR Encrypt/Decrypt ---------- */

static int aes_ctr_crypt(struct aes *aes,
                         uint8_t *out,
                         const uint8_t *in,
                         size_t len)
{
    psa_status_t status;
    size_t out_len;
    int err;

    /* Start operation if not already started */
    if (!aes->op_started) {
        err = start_ctr_operation(aes);
        if (err)
            return err;
    }

    status = psa_cipher_update(&aes->cipher_op,
                               in, len,
                               out, len,
                               &out_len);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("psa_cipher_update failed: %d\n", (int)status);
        psa_cipher_abort(&aes->cipher_op);
        aes->op_started = false;
        return EPROTO;
    }

    return 0;
}


/* ---------- Public API: Encryption ---------- */

int aes_encr(struct aes *aes,
             uint8_t *out,
             const uint8_t *in,
             size_t len)
{
    if (!aes || !in)
        return EINVAL;

    aes->encr = true;

    switch (aes->mode) {

    case AES_MODE_CTR:
        if(!out) return EINVAL;
        return aes_ctr_crypt(aes, out, in, len);
/*
    case AES_MODE_GCM:
        // If out is NULL it may be because the caller only wants to process the tag for AAD
        // this is the case of the Header and Index
        if(!out) {
            out = dummy_out;
        }
        return aes_gcm_encrypt(aes, out, in, len);
*/
    default:
        return ENOTSUP;
    }
}

int  aes_gcm_encr(struct aes *aes,
	              uint8_t *ct_out, size_t ct_buff_len, size_t *ct_len,
	              const uint8_t *pt_in, size_t pt_len,
				  const uint8_t *ad_in, size_t ad_len)
{
    psa_status_t status;

    if (!aes || !ct_out || !ct_len)
        return EINVAL;

    if (!pt_in && pt_len > 0)
        return EINVAL;

    /* Ensure output buffer is large enough for ciphertext + tag */
    if (ct_buff_len < pt_len + AES_GCM_TAG_SIZE)
        return EOVERFLOW;

    aes->encr = true;
    status = psa_aead_encrypt(aes->key_id, PSA_ALG_GCM,
                              aes->iv, aes->iv_len,
                              ad_in, ad_len,
                              pt_in, pt_len,
                              ct_out, ct_buff_len, ct_len);
    if (status != PSA_SUCCESS) {
        DEBUG_WARNING("aes_gcm_encr: psa_aead_encrypt failed: %d\n", status);
        return EPROTO;
    }

    return 0;
}


/* ---------- Public API: Decryption ---------- */

int aes_decr(struct aes *aes,
             uint8_t *out,
             const uint8_t *in,
             size_t len)
{
    if (!aes || !in)
        return EINVAL;

    aes->encr = false;

    switch (aes->mode) {

    case AES_MODE_CTR:
        if(!out) return EINVAL;
        return aes_ctr_crypt(aes, out, in, len);
/*
    case AES_MODE_GCM:
        // If out is NULL it may be because the caller only wants to process the tag for AAD
        // this is the case of the Header and Index
        if(!out) {
            out = dummy_out;
        }
        return aes_gcm_decrypt(aes, out, in, len);
*/
    default:
        return ENOTSUP;
    }
}

int  aes_gcm_decr(struct aes *aes,
	              uint8_t *pt_out, size_t pt_buff_len, size_t *pt_len,
	              const uint8_t *ct_in, size_t ct_len,
				  const uint8_t *ad_in, size_t ad_len)
{
    psa_status_t status;

    if (!aes || !pt_out || !pt_len)
        return EINVAL;

    if (!ct_in && ct_len > 0)
        return EINVAL;

    /* Ciphertext must be at least as large as the tag */
    if (ct_len < AES_GCM_TAG_SIZE)
        return EINVAL;

    /* Ensure output buffer is large enough for plaintext (ciphertext - tag) */
    if (pt_buff_len < ct_len - AES_GCM_TAG_SIZE)
        return EOVERFLOW;

    aes->encr = false;
    status = psa_aead_decrypt(aes->key_id, PSA_ALG_GCM,
                              aes->iv, aes->iv_len,
                              ad_in, ad_len,
                              ct_in, ct_len,
                              pt_out, pt_buff_len, pt_len);

    if (status != PSA_SUCCESS) {
        /* Distinguish authentication failure from other errors */
        if (status == PSA_ERROR_INVALID_SIGNATURE)
            return EAUTH;

        return EPROTO;
    }

    return 0;
}
