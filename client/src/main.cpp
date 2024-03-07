#include <QtCore>
#include <iostream>

class Client : public QObject {
    Q_OBJECT

public:
    Client(QObject* parent = 0)
        : QObject(parent)
    {
    }

public slots:
    void run()
    {
        std::cout << "Hello, World!" << std::endl;
        emit finished();
    }

signals:
    void finished();
};

#include "main.moc"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    auto client = new Client(&app);
    QObject::connect(client, SIGNAL(finished()), &app, SLOT(quit()));
    QTimer::singleShot(1000, client, SLOT(run()));
    return app.exec();
}
