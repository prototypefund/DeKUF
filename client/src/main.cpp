#include "client.hpp"

const int initialDelay = 1000;
const int interval = 60000;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    Client client(&app);
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
