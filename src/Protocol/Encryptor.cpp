#include "Encryptor.hpp"

#include <vector>

Encryptor::Encryptor(const std::string &password) {
    unsigned char key[32] = {0};
    unsigned char iv[16] = {0};

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(mdctx, password.c_str(), password.length());
    EVP_DigestFinal_ex(mdctx, key, NULL);
    EVP_MD_CTX_free(mdctx);

    for (size_t i = 0; i < sizeof(iv); ++i) {
        iv[i] = key[i] ^ (i * 13);
    }

    encrypt_ctx = EVP_CIPHER_CTX_new();
    decrypt_ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit_ex(encrypt_ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_DecryptInit_ex(decrypt_ctx, EVP_aes_256_cbc(), NULL, key, iv);
}

Encryptor::~Encryptor() {
    EVP_CIPHER_CTX_free(encrypt_ctx);
    EVP_CIPHER_CTX_free(decrypt_ctx);
}

std::vector<uint8_t> Encryptor::encrypt(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> ciphertext(data.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len;
    int ciphertext_len = 0;

    EVP_EncryptUpdate(encrypt_ctx, ciphertext.data(), &len, data.data(), data.size());
    ciphertext_len = len;

    EVP_EncryptFinal_ex(encrypt_ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;

    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

std::vector<uint8_t> Encryptor::decrypt(const std::vector<uint8_t>& ciphertext) {
    std::vector<uint8_t> plaintext(ciphertext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len;
    int plaintext_len = 0;

    EVP_DecryptUpdate(decrypt_ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size());
    plaintext_len = len;

    EVP_DecryptFinal_ex(decrypt_ctx, plaintext.data() + len, &len);
    plaintext_len += len;

    plaintext.resize(plaintext_len);
    return plaintext;
}