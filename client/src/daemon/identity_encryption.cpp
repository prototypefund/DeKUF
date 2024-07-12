#include <QRandomGenerator>
#include <QString>

#include "identity_encryption.hpp"

QString IdentityEncryption::generateKeyPair()
{
    return QString::number(QRandomGenerator::global()->generate());
}

QString IdentityEncryption::encrypt(
    const QString& text, const QString& key) const
{
    return text;
}

QString IdentityEncryption::decrypt(
    const QString& text, const QString& key) const
{
    return text;
}
