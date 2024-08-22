#pragma once

#include <gmpxx.h>

class HomomorphicEncryptor {
public:
    virtual ~HomomorphicEncryptor() = default;

    virtual mpz_class encrypt(const mpz_class& plaintext) = 0;

    virtual mpz_class addEncrypted(
        const mpz_class& cipher1, const mpz_class& cipher2) const = 0;
};
