#include <core/sqlite_storage.hpp>

#include "daemon.hpp"
#include "server_network.hpp"

const int initialDelay = 1000;
const int interval = 60000;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    auto storage = QSharedPointer<SqliteStorage>::create();
    auto network = QSharedPointer<ServerNetwork>::create();
    Daemon daemon(&app, storage, network);
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
