#include <QTest>

#include <daemon/gpgme_encryption.hpp>

#include "gpgme_encryption_test.hpp"

// TODO: Key pair generation can take a while, so these tests take ridiculously
// long compared to all the others. We should figure out how to speed that up.

void GpgmeEncryptionTest::testGenerateKeyPair()
{
    GpgmeEncryption encryption;
    auto publicKey = encryption.generateKeyPair();
    QVERIFY(!publicKey.isEmpty());
}

// TODO: The encryption/decryption tests aren't great, since they rely on
// encrypting data in the exact same instance, so stuff like key importing is
// not actually tested. Ideally, these tests would cover encryption and
// decryption in complete isolation.

void GpgmeEncryptionTest::testEncrypt()
{
    GpgmeEncryption encryption;
    auto publicKey = encryption.generateKeyPair();
    auto raw = "1337";
    auto encrypted = encryption.encrypt(raw, publicKey);
    QVERIFY(encrypted != raw);
}

void GpgmeEncryptionTest::testDecrypt()
{
    GpgmeEncryption encryption;
    auto publicKey = encryption.generateKeyPair();
    auto raw = "1337";
    auto encrypted = encryption.encrypt(raw, publicKey);
    auto decrypted = encryption.decrypt(encrypted, publicKey);
    QVERIFY(decrypted == raw);
}

QTEST_MAIN(GpgmeEncryptionTest)
