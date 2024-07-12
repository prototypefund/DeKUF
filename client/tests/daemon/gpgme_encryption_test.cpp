#include <QTest>

#include <daemon/gpgme_encryption.hpp>

#include "gpgme_encryption_test.hpp"

void GpgmeEncryptionTest::testGenerateKeyPair()
{
    GpgmeEncryption encryption;
    auto publicKey = encryption.generateKeyPair();
    QVERIFY(!publicKey.isEmpty());
}

QTEST_MAIN(GpgmeEncryptionTest)
