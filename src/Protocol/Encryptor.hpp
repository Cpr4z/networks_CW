#pragma once

#include <string>

#include <openssl/evp.h>
#include <openssl/rand.h>

class Encryptor {
public:
    explicit Encryptor(const std::string& password);
    ~Encryptor();

    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext);

private:
    EVP_CIPHER_CTX* encrypt_ctx = nullptr;
    EVP_CIPHER_CTX* decrypt_ctx = nullptr;
};