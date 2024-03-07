#pragma once

#include <QtCore>

class Client : public QObject {
    Q_OBJECT

public:
    Client(QObject* parent = 0);

public slots:
    void run();

signals:
    void finished();
};
