#include <QDir>
#include <QString>
#include <gpgme.h>
#include <stdexcept>
#include <unistd.h>

#include "gpgme_encryption.hpp"

// TODO: This is not just rushed, prototypical code, this is a an absolute
// _nightmare_. Probably two memory leaks for each line of code, that's not
// gonna go well in the long run. As if that wasn't enough, it's also absolute
// repetitive spaghetti that shows utter ignorance of how libgpgme actually
// works. Oh, and linting is also disabled for most of this code, too. Needs
// some major profiling and refactoring. Or an exorcist.

// NOLINTBEGIN

#define CHECK_GPGME_ERROR(error, message)                                      \
    {                                                                          \
        if (error)                                                             \
            throw std::runtime_error(                                          \
                std::string(message) + ": " + gpgme_strerror(error));          \
    }

namespace {
gpgme_error_t passphraseCallback(void* opaque, const char* uid_hint,
    const char* passphrase_info, int last_was_bad, int fd)
{
    auto passphrase = "...\n";
    auto length = strlen(passphrase);
    auto offset = 0;
    int result = 0;
    do {
        result = write(fd, &passphrase[offset], length - offset); // NOLINT
        if (result > 0)
            offset += result;
    } while (result > 0 && offset != length);
    return offset == length ? 0 : gpgme_error_from_errno(errno);
}

gpgme_ctx_t initGpgme()
{
    // TODO: Figure out if setting the locale is actually necessary.
    setlocale(LC_ALL, "");
    gpgme_check_version(nullptr);
    gpgme_set_locale(nullptr, LC_CTYPE, setlocale(LC_CTYPE, nullptr));

    gpgme_ctx_t context = nullptr;
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

    error = gpgme_set_pinentry_mode(context, GPGME_PINENTRY_MODE_LOOPBACK);
    CHECK_GPGME_ERROR(error, "Failed to set pinentry mode");
    gpgme_set_passphrase_cb(context, passphraseCallback, nullptr);

    return context;
}

QString releaseData(gpgme_data_t data)
{
    size_t length = 0;
    auto buffer = gpgme_data_release_and_get_mem(data, &length); // NOLINT
    std::string s(buffer, length);
    delete buffer; // NOLINT
    return QString::fromStdString(s);
}

QString exportKey(gpgme_ctx_t context, const QString& fingerprint, int mode)
{
    gpgme_data_t data;
    auto error = gpgme_data_new(&data);
    CHECK_GPGME_ERROR(error, "Error creating key data");

    error = gpgme_data_set_encoding(data, GPGME_DATA_ENCODING_ARMOR);
    CHECK_GPGME_ERROR(error, "Failed to set key encoding");

    error = gpgme_op_export(context, fingerprint.toLatin1(), mode, data);
    CHECK_GPGME_ERROR(error, "Error exporting key");

    return releaseData(data);
}

gpgme_key_t importAndLoadKey(gpgme_ctx_t context, const QString& key)
{
    auto keyBytes = key.toUtf8();
    gpgme_data_t keyData = nullptr;
    auto error = gpgme_data_new_from_mem(
        &keyData, keyBytes.data(), keyBytes.length(), 0);
    CHECK_GPGME_ERROR(error, "Error converting key string to data");

    error = gpgme_op_import(context, keyData);
    CHECK_GPGME_ERROR(error, "Error importing key");
    auto importResult = gpgme_op_import_result(context);
    if (importResult->not_imported)
        throw std::runtime_error("Key wasn't imported");

    gpgme_op_keylist_start(context, "", 0);
    QList<gpgme_key_t> allKeys;
    while (true) {
        gpgme_key_t currentKey = nullptr;
        error = gpgme_op_keylist_next(context, &currentKey);
        if (gpg_err_code(error) == GPG_ERR_EOF)
            break;
        CHECK_GPGME_ERROR(error, "Error loading next key");
        allKeys.append(currentKey);
    }

    for (auto currentKey : allKeys) {
        auto exported = exportKey(context, currentKey->fpr, 0);
        if (exported == key)
            return currentKey;
    }

    throw std::runtime_error("Failed to import key");
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
                Passphrase: ...
            </GnupgKeyParms>
        )";
    auto error = gpgme_op_genkey(context, params, nullptr, nullptr);
    CHECK_GPGME_ERROR(error, "Error generating key pair");

    auto result = gpgme_op_genkey_result(context);

    // TODO: Now that we've figured out the passphrase stuff, it should be
    // possible to export the private key as well, could be cleaner.
    QString fingerprint(result->fpr);
    return exportKey(context, fingerprint, 0);
}

QString GpgmeEncryption::encrypt(const QString& text, const QString& key) const
{
    auto context = initGpgme();

    auto inputBytes = text.toUtf8();
    gpgme_data_t inputData = nullptr;
    auto error = gpgme_data_new_from_mem(
        &inputData, inputBytes.data(), inputBytes.length(), 0);
    CHECK_GPGME_ERROR(error, "Error converting input string to data");

    gpgme_data_t outputData = nullptr;
    error = gpgme_data_new(&outputData);
    CHECK_GPGME_ERROR(error, "Error creating output data buffer");

    auto importedKey = importAndLoadKey(context, key);
    gpgme_key_t recipientKeys[] = { importedKey, nullptr }; // NOLINT
    error = gpgme_op_encrypt(context, recipientKeys, // NOLINT
        GPGME_ENCRYPT_ALWAYS_TRUST, inputData, outputData);
    CHECK_GPGME_ERROR(error, "Encryption failed");
    auto encryptResult = gpgme_op_encrypt_result(context);
    if (encryptResult->invalid_recipients) {
        QString message = "Invalid recipient encountered "
            + QString::fromLatin1(encryptResult->invalid_recipients->fpr);
        throw std::runtime_error(message.toStdString());
    }

    // TODO: The imported key never gets deleted, which is not great, but also
    // not a huge issue for a prototype. The problem with deleting it is, that
    // the way the tests currently work, that'd delete the secret key later
    // needed for decryption. If we change the tests to actually test
    // encryption/decryption between two separate instances, it should work
    // fine.

    return releaseData(outputData);
}

QString GpgmeEncryption::decrypt(const QString& text, const QString& key) const
{
    // TODO: The key is just being ignored entirely, since GPG automatically
    // finds a suitable key - if it has one.

    auto context = initGpgme();

    auto inputBytes = text.toLatin1();
    gpgme_data_t inputData = nullptr;
    auto error = gpgme_data_new_from_mem(
        &inputData, inputBytes.data(), inputBytes.length(), 1);
    CHECK_GPGME_ERROR(error, "Error converting input string to data");

    gpgme_data_t outputData = nullptr;
    error = gpgme_data_new(&outputData);
    CHECK_GPGME_ERROR(error, "Error creating output data buffer");

    error = gpgme_op_decrypt(context, inputData, outputData);
    CHECK_GPGME_ERROR(error, "Failed to decrypt data");

    size_t length = 0;
    auto buffer = gpgme_data_release_and_get_mem(outputData, &length);
    std::string output(buffer, length);
    delete buffer; // NOLINT
    return QString::fromStdString(output);
}
