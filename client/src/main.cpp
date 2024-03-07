#include "client.hpp"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    auto client = new Client(&app);
    QObject::connect(client, SIGNAL(finished()), &app, SLOT(quit()));
    QTimer::singleShot(1000, client, SLOT(run()));
    return app.exec();
}
