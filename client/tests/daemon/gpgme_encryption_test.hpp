#pragma once

#include <QObject>

class GpgmeEncryptionTest : public QObject {
    Q_OBJECT

private slots:
    void testGenerateKeyPair();
    void testEncrypt();
};
