#pragma once

#include <QObject>
#include <daemon/paillier_encryptor.hpp>

class PaillierEncryptorTest : public QObject {
    Q_OBJECT

private slots:
private:
    PaillierEncryptor createTestEncryptor()
    {
        return PaillierEncryptor("143"); // 11 * 13
    }

private slots:
    void testEncryptionIsNotEqual();
    void testHomomorphicAddition();
    void testZeroEncryption();
    void testLargeNumbers();
};
