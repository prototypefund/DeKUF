#include <QDir>
#include <QString>
#include <gpgme.h>
#include <stdexcept>

#include "gpgme_encryption.hpp"

// TODO: Get rid of all these memory leaks.

// TODO: Actually fix the linter issues.
// NOLINTBEGIN

#define CHECK_GPGME_ERROR(error, message)                                      \
    {                                                                          \
        if (error)                                                             \
            throw std::runtime_error(                                          \
                std::string(message) + ": " + gpgme_strerror(error));          \
    }

namespace {
gpgme_ctx_t initGpgme()
{
    // TODO: Figure out if setting the locale is actually necessary.
    setlocale(LC_ALL, "");
    gpgme_check_version(nullptr);
    gpgme_set_locale(nullptr, LC_CTYPE, setlocale(LC_CTYPE, nullptr));

    gpgme_ctx_t context;
    gpgme_error_t error = gpgme_new(&context);
    CHECK_GPGME_ERROR(error, "Error initializing GPGME");

    // TODO: For the purpose of the tests, this needs to be in another location
    // and get removed after each test.
    // TODO: Don't hard code .dekuf here, also used in SqliteStorage.
    auto gpgPath = QDir::homePath() + QDir::separator() + ".dekuf"
        + QDir::separator() + "gpg";
    QDir(gpgPath).mkpath(".");

    error = gpgme_ctx_set_engine_info(
        context, GPGME_PROTOCOL_OpenPGP, nullptr, gpgPath.toUtf8());
    CHECK_GPGME_ERROR(error, "Error setting homedir");

    gpgme_set_armor(context, 1);

    // Keep the system's GPG agent from printing the user for a password.  It
    // seems like this won't do the trick for exporting private keys. For the
    // moment, we don't necessarily need to, but it's a foot gun.
    error = gpgme_set_pinentry_mode(context, GPGME_PINENTRY_MODE_CANCEL);
    CHECK_GPGME_ERROR(error, "Failed to set pinentry mode");

    return context;
}

std::string exportKey(
    gpgme_ctx_t context, const std::string& fingerprint, int mode)
{
    gpgme_data_t data;
    auto error = gpgme_data_new(&data);
    CHECK_GPGME_ERROR(error, "Error creationg key data");

    error = gpgme_data_set_encoding(data, GPGME_DATA_ENCODING_ARMOR);
    CHECK_GPGME_ERROR(error, "Failed to set key encoding");

    error = gpgme_op_export(context, fingerprint.c_str(), mode, data);
    CHECK_GPGME_ERROR(error, "Error exporting key");

    size_t length;
    auto buffer = gpgme_data_release_and_get_mem(data, &length);
    std::string key(buffer, length);
    delete buffer;
    return key;
}
}
// NOLINTEND

QString GpgmeEncryption::generateKeyPair()
{
    auto context = initGpgme();

    // TODO: Put the survey ID or something into Name-Comment.
    auto params = R"(
            <GnupgKeyParms format="internal">
                Key-Type: RSA
                Key-Length: 2048
                Subkey-Type: RSA
                Subkey-Length: 2048
                Name-Comment: TODO
                Expire-Date: 0
                Passphrase: ""
            </GnupgKeyParms>
        )";
    auto error = gpgme_op_genkey(context, params, nullptr, nullptr);
    CHECK_GPGME_ERROR(error, "Error generating key pair");

    auto result = gpgme_op_genkey_result(context);

    std::string fingerprint(result->fpr);
    auto publicKey = exportKey(context, fingerprint, 0);
    return QString::fromStdString(publicKey);
}

QString GpgmeEncryption::encrypt(const QString& text, const QString& key) const
{
    // TODO: Actually implement this.
    throw std::runtime_error("Not implemented");
}

QString GpgmeEncryption::decrypt(const QString& text, const QString& key) const
{
    // TODO: Actually implement this.
    throw std::runtime_error("Not implemented");
}
