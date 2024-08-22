#pragma once

#include <QObject>
#include <daemon/paillier_encryptor.hpp>

class PaillierEncryptorTest : public QObject {
    Q_OBJECT

private slots:
private:
    PaillierEncryptor createTestEncryptor()
    {
        return PaillierEncryptor(
            QString("3141592653589793238462643383279502884197169399375105820974"
                    "9445923078164062862089986280348253421170679821480865132823"
                    "06647093844609550582231725359408128481"));
    }

private slots:
    void testEncryptionIsNotEqual();
    void testHomomorphicAddition();
    void testZeroEncryption();
    void testLargeNumbers();
};
