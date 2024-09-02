#include <QTest>

#include <daemon/gpgme_encryption.hpp>

#include "gpgme_encryption_test.hpp"

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
