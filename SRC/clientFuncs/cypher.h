/*******************************************************************************
 *
 * Software Name: CryptoDbSS Client API RPC
 * Copyright (C) 2025 Steeven J Salazar.
 * License: CryptoDbSS: Software Review and Audit License
 * 
 * https://github.com/CryptoDbSS/CryptoDbSS-Client-API-RPC
 *
 * This notice, including the copyright notice and permission notice, 
 * must be retained in all copies or substantial portions of the Software and 
 * in all derivative works.
 *
 * IMPORTANT: Before using, compiling, or doing anything with this software,
 * you must read and accept the terms of the License provided with this software.
 *
 * If you do not have a copy of the License, you can obtain it at the following link:
 * https://github.com/CryptoDbSS/CryptoDbSS-Client-API-RPC/blob/main/LICENSE.md
 *
 * By using, compiling, or modifying this software, you implicitly accept
 * the terms of the License. If you do not agree with the terms,
 * do not use, compile, or modify this software.
 * 
 * This software is provided "as is," without warranty of any kind.
 * For more details, see the LICENSE file.
 *
 ********************************************************************************/


/*********************************************************************************
 
    The CryptoDbSS, blockchain-core, consensus, protocols and misc.

    This software is a review and audit release, it should only be used for 
    development, testing, education and auditing purposes. 

    Third-party dependencies: Crypto++, OpenSSL, libcurl.

    questions, suggestions or contact : Steevenjavier@gmail.com

                                S.S

*********************************************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <stdexcept>
#include <openssl/aes.h>
#include <openssl/kdf.h>
#include <iomanip> 
#include <vector>

void handleOpenSSLError(const std::string& message) {
    unsigned long err = ERR_get_error();
    char err_str[256];
    ERR_error_string_n(err, err_str, sizeof(err_str));
    throw std::runtime_error(message + ": " + err_str);
}

std::vector<unsigned char> encryptAES(const std::vector<unsigned char>& DataToEncrypt, const std::string& password) {
    if (password.empty()) {
        throw std::runtime_error("Password cannot be empty.");
    }

    // Generate a random salt
    unsigned char salt[16]; // Increased salt size
    if (RAND_bytes(salt, sizeof(salt)) != 1) {
        handleOpenSSLError("RAND_bytes failed");
    }

    // Derive the key and IV using PBKDF2
    unsigned char key[AES_BLOCK_SIZE];
    unsigned char iv[AES_BLOCK_SIZE];
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), salt, sizeof(salt), 10000, EVP_sha256(), AES_BLOCK_SIZE, key) != 1) {
        handleOpenSSLError("PKCS5_PBKDF2_HMAC key failed");
    }
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), salt, sizeof(salt), 10000, EVP_sha256(), AES_BLOCK_SIZE, iv) != 1) {
        handleOpenSSLError("PKCS5_PBKDF2_HMAC iv failed");
    }

    // Configure the encryption context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        handleOpenSSLError("EVP_CIPHER_CTX_new failed");
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        handleOpenSSLError("EVP_EncryptInit_ex failed");
    }

    // Encrypt the plaintext
    std::vector<unsigned char> ciphertext(DataToEncrypt.size() + AES_BLOCK_SIZE);
    int ciphertextLength;
    int finalCiphertextLength;
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &ciphertextLength, DataToEncrypt.data(), DataToEncrypt.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        handleOpenSSLError("EVP_EncryptUpdate failed");
    }

    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + ciphertextLength, &finalCiphertextLength) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        handleOpenSSLError("EVP_EncryptFinal_ex failed");
    }
    ciphertext.resize(ciphertextLength + finalCiphertextLength);

    EVP_CIPHER_CTX_free(ctx);

    // Add the salt to the ciphertext
    std::vector<unsigned char> saltedCiphertext;
    saltedCiphertext.insert(saltedCiphertext.end(), salt, salt + sizeof(salt));
    saltedCiphertext.insert(saltedCiphertext.end(), ciphertext.begin(), ciphertext.end());

    return saltedCiphertext;
}

std::vector<unsigned char> decryptAES(const std::vector<unsigned char>& saltedCiphertext, const std::string& password) {
    if (password.empty()) {
        throw std::runtime_error("Password cannot be empty.");
    }

    // Extract the salt from the ciphertext
    unsigned char salt[16];
    std::copy(saltedCiphertext.begin(), saltedCiphertext.begin() + sizeof(salt), salt);

    // Derive the key and IV using PBKDF2
    unsigned char key[AES_BLOCK_SIZE];
    unsigned char iv[AES_BLOCK_SIZE];
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), salt, sizeof(salt), 10000, EVP_sha256(), AES_BLOCK_SIZE, key) != 1) {
        handleOpenSSLError("PKCS5_PBKDF2_HMAC key failed");
    }
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(), salt, sizeof(salt), 10000, EVP_sha256(), AES_BLOCK_SIZE, iv) != 1) {
        handleOpenSSLError("PKCS5_PBKDF2_HMAC iv failed");
    }

    // Configure the decryption context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        handleOpenSSLError("EVP_CIPHER_CTX_new failed");
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        handleOpenSSLError("EVP_DecryptInit_ex failed");
    }

    // Decrypt the ciphertext
    std::vector<unsigned char> DataToEncrypt(saltedCiphertext.size() - sizeof(salt));
    int DataToEncryptLength;
    int finalDataToEncryptLength;
    if (EVP_DecryptUpdate(ctx, DataToEncrypt.data(), &DataToEncryptLength, saltedCiphertext.data() + sizeof(salt), saltedCiphertext.size() - sizeof(salt)) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        handleOpenSSLError("EVP_DecryptUpdate failed");
    }

    if (EVP_DecryptFinal_ex(ctx, DataToEncrypt.data() + DataToEncryptLength, &finalDataToEncryptLength) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        handleOpenSSLError("EVP_DecryptFinal_ex failed");
    }
    DataToEncrypt.resize(DataToEncryptLength + finalDataToEncryptLength);

    EVP_CIPHER_CTX_free(ctx);

    return DataToEncrypt;
}