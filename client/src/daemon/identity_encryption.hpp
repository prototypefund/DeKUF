#pragma once

#include "encryption.hpp"

// TODO: Once we get another Encryption implementation working reliably, we
// won't need this (or its tests) any longer, presumably.

/**
 * A fake encryption system that generates unique (yet fake) keys and returns
 * the input verbatim for both encryption and decryption.
 */
class IdentityEncryption : public Encryption {
public:
    QString generateKeyPair();
    QString encrypt(const QString& text, const QString& key) const;
    QString decrypt(const QString& text, const QString& key) const;
};
