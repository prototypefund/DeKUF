#pragma once

#include "storage.hpp"
#include <QSqlDatabase>
#include <QtCore>

class SqliteStorage : public Storage {
public:
    explicit SqliteStorage(const QString& databasePath);
    SqliteStorage();

    QList<QString> listDataPoints(const QString& key);
    void addDataPoint(const QString& key, const QString& value);

private:
    QSqlDatabase db;
};
