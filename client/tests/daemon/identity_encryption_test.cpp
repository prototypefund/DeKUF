#include <QTest>

#include <daemon/identity_encryption.hpp>

#include "identity_encryption_test.hpp"

void IdentityEncryptionTest::testGenerateKeyPair()
{
    IdentityEncryption encryption;
    auto firstKey = encryption.generateKeyPair();
    QVERIFY(!firstKey.isEmpty());
    auto secondKey = encryption.generateKeyPair();
    QVERIFY(firstKey != secondKey);
}

void IdentityEncryptionTest::testEncrypt()
{
    IdentityEncryption encryption;
    auto encrypted = encryption.encrypt("Foo", "Bar");
    QCOMPARE(encrypted, "Foo");
}

void IdentityEncryptionTest::testDecrypt()
{
    IdentityEncryption encryption;
    auto decrypted = encryption.decrypt("Foo", "Bar");
    QCOMPARE(decrypted, "Foo");
}

QTEST_MAIN(IdentityEncryptionTest)
