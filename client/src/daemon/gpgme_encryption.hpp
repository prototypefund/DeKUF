#pragma once

#include "encryption.hpp"

class GpgmeEncryption : public Encryption {
public:
    QString generateKeyPair();
    QString encrypt(const QString& text, const QString& key) const;
    QString decrypt(const QString& text, const QString& key) const;
};
