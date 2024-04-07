#include <core/sqlite_storage.hpp>

#include "client.hpp"

const int initialDelay = 1000;
const int interval = 60000;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    auto storage = QSharedPointer<SqliteStorage>::create();
    Client client(&app, storage);
    QTimer timer(&app);

    QObject::connect(&timer, &QTimer::timeout, &app, [&]() {
        timer.stop();
        client.run();
    });

    QObject::connect(
        &client, &Client::finished, &app, [&]() { timer.start(interval); });

    timer.start(initialDelay);
    return app.exec();
}
