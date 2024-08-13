#include <QTest>

#include <daemon/paillier_encryptor.hpp>
#include <qglobal.h>
#include <qtestcase.h>

#include "paillier_encryptor_test.hpp"

// TODO: These tests are kind of weak, maybe we should find a better way to
// test this

void PaillierEncryptorTest::testEncryptionIsNotEqual()
{
    PaillierEncryptor encryptor = createTestEncryptor();
    mpz_class plaintext(10);
    mpz_class ciphertext = encryptor.encrypt(plaintext);

    QVERIFY(ciphertext != plaintext);

    mpz_class ciphertext2 = encryptor.encrypt(plaintext);
    QVERIFY(ciphertext != ciphertext2);
}

void PaillierEncryptorTest::testHomomorphicAddition()
{
    PaillierEncryptor encryptor = createTestEncryptor();
    mpz_class a(5), b(7);
    mpz_class cipher_a = encryptor.encrypt(a);
    mpz_class cipher_b = encryptor.encrypt(b);

    mpz_class sum_cipher = encryptor.add_encrypted(cipher_a, cipher_b);
    mpz_class sum_plain = a + b;

    QVERIFY(sum_cipher != cipher_a);
    QVERIFY(sum_cipher != cipher_b);
}

void PaillierEncryptorTest::testZeroEncryption()
{
    PaillierEncryptor encryptor = createTestEncryptor();
    mpz_class zero(0);
    mpz_class cipher_zero = encryptor.encrypt(zero);

    QVERIFY(cipher_zero != zero);
}

void PaillierEncryptorTest::testLargeNumbers()
{
    PaillierEncryptor encryptor = createTestEncryptor();
    mpz_class large_num("1000000000000000000000000");
    mpz_class cipher_large = encryptor.encrypt(large_num);

    QVERIFY(!cipher_large.get_str().empty());
}

QTEST_MAIN(PaillierEncryptorTest)
