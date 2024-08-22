#pragma once

#include "homomorphic_encryptor.hpp"
#include <core/result.hpp>

#include <QtCore>
#include <gmpxx.h>

class PaillierEncryptor : public HomomorphicEncryptor {
public:
    explicit PaillierEncryptor(const QString& n_str);

    static Result<QSharedPointer<PaillierEncryptor>> createPaillierEncryptor(
        const QString& n_str);

    mpz_class encrypt(const mpz_class& m) override;
    mpz_class addEncrypted(
        const mpz_class& a, const mpz_class& b) const override;

private:
    mpz_class n;
    mpz_class n_squared;
    mpz_class g;
    QSharedPointer<__gmp_randstate_struct> rng;

    static mpz_class powm(
        const mpz_class& base, const mpz_class& exp, const mpz_class& mod);
};