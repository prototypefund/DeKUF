#pragma once

class QString;

/**
 * An asymmetric encryption system.
 */
class Encryption {
public:
    /**
     * Generates a key pair (public and private key) for assymetric enryption.
     * @remarks For the moment, only the public key is returned, whereas the
     * private key is stored in a mysterious, implementation dependent location.
     */
    virtual QString generateKeyPair() = 0;

    /**
     * Encrypts the supplied text with the supplied public key.
     */
    virtual QString encrypt(const QString& text, const QString& key) const = 0;

    /**
     * Decrypts the supplied text with the private key associated with the
     * supplied public key.
     * @remarks If private keys are no longer stored internally, we'd pass the
     * private key here, rather than the public key.
     */
    virtual QString decrypt(const QString& text, const QString& key) const = 0;
};
