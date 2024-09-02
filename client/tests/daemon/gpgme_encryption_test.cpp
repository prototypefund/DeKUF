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

void GpgmeEncryptionTest::testEncrypt()
{
    GpgmeEncryption encryption;
    auto publicKey = encryption.generateKeyPair();
    auto raw = "1337";
    auto encrypted = encryption.encrypt(raw, publicKey);
    QVERIFY(encrypted != raw);
}

QTEST_MAIN(GpgmeEncryptionTest)
