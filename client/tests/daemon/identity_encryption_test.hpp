#pragma once

#include <QObject>

class IdentityEncryptionTest : public QObject {
    Q_OBJECT

private slots:
    void testGenerateKeyPair();
    void testEncrypt();
    void testDecrypt();
};
