#pragma once

#include <QtCore>

class Storage;

namespace detail {
class Data : public QObject {
    Q_OBJECT

public:
    Data(QSharedPointer<Storage> storage);

public slots:
    QString submit_data_point(const QString& key, const QString& value);

private:
    QSharedPointer<Storage> storage;
};
};

class DBusService {
public:
    DBusService(QSharedPointer<Storage> storage);

private:
    detail::Data data;
};
