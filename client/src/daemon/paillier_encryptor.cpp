#include "paillier_encryptor.hpp"
#include <QRandomGenerator>
#include <gmpxx.h>

PaillierEncryptor::PaillierEncryptor(const QString& n_str)
    : n(n_str.toStdString())
    , n_squared(n * n)
    , g(n + 1)
{
    rng = QSharedPointer<__gmp_randstate_struct>(
        new __gmp_randstate_struct, [](auto* p) {
            if (p) {
                gmp_randclear(p);
            }
        });
    gmp_randinit_default(rng.data());
    const unsigned long seed = QRandomGenerator::global()->generate();
    gmp_randseed_ui(rng.data(), seed);
}

Result<QSharedPointer<PaillierEncryptor>>
PaillierEncryptor::createPaillierEncryptor(const QString& n_str)
{
    try {
        auto encryptor = QSharedPointer<PaillierEncryptor>::create(n_str);
        return Result(encryptor);
    } catch (const std::invalid_argument& error) {
        return Result<QSharedPointer<PaillierEncryptor>>::Failure(
            "Data public key encryption string not valid:" + n_str);
    }
}

mpz_class PaillierEncryptor::encrypt(const mpz_class& m)
{
    mpz_class r;
    mpz_urandomm(r.get_mpz_t(), rng.data(), n.get_mpz_t());
    return (powm(g, m, n_squared) * powm(r, n, n_squared)) % n_squared;
}

mpz_class PaillierEncryptor::addEncrypted(
    const mpz_class& a, const mpz_class& b) const
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