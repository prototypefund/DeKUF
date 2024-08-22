#pragma once

#include <daemon/homomorphic_encryptor.hpp>

class HomomorphicEncryptorStub : public HomomorphicEncryptor {
public:
    mpz_class encrypt(const mpz_class& plaintext) { return plaintext; };

    mpz_class addEncrypted(
        const mpz_class& cipher1, const mpz_class& cipher2) const
    {
        return cipher1 + cipher2;
    }
};
