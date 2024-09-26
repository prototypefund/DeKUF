#include <core/sqlite_storage.hpp>

#include "daemon.hpp"
#include "encryption.hpp"
#include "gpgme_encryption.hpp"
#include "identity_encryption.hpp"
#include "server_network.hpp"

const int initialDelay = 1000;
const int interval = 60000;

QSharedPointer<Encryption> createEncryption()
{
    // End to end encryption is pretty experimental, and disabled by default.
    if (qgetenv("PRIVACT_CLIENT_ENABLE_E2E") == "1") {
        qDebug() << "GPG end to end encryption active";
        return QSharedPointer<GpgmeEncryption>::create();
    }
    return QSharedPointer<IdentityEncryption>::create();
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    auto storage = QSharedPointer<SqliteStorage>::create();
    auto network = QSharedPointer<ServerNetwork>::create();
    auto encryption = createEncryption();
    Daemon daemon(&app, storage, network, encryption);
    QTimer timer(&app);

    QObject::connect(&timer, &QTimer::timeout, &app, [&]() {
        timer.stop();
        daemon.run();
    });

    QObject::connect(
        &daemon, &Daemon::finished, &app, [&]() { timer.start(interval); });

    timer.start(initialDelay);
    return app.exec();
}
