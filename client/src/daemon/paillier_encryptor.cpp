#include "paillier_encryptor.hpp"
#include <QRandomGenerator>
#include <gmpxx.h>

PaillierEncryptor::PaillierEncryptor(const QString& n_str)
    : n(n_str.toStdString())
    , n_squared(n * n)
    , g(n + 1)
{
    gmp_randinit_default(rng);
    unsigned long seed = QRandomGenerator::global()->generate();
    gmp_randseed_ui(rng, seed);
}

mpz_class PaillierEncryptor::encrypt(const mpz_class& m)
{
    mpz_class r;
    mpz_urandomm(r.get_mpz_t(), rng, n.get_mpz_t());
    return (powm(g, m, n_squared) * powm(r, n, n_squared)) % n_squared;
}

mpz_class PaillierEncryptor::add_encrypted(
    const mpz_class& a, const mpz_class& b)
{
    return (a * b) % n_squared;
}

mpz_class PaillierEncryptor::powm(
    const mpz_class& base, const mpz_class& exp, const mpz_class& mod)
{
    mpz_class result;
    mpz_powm(
        result.get_mpz_t(), base.get_mpz_t(), exp.get_mpz_t(), mod.get_mpz_t());
    return result;
}