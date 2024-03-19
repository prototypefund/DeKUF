#pragma once

#include <QtCore>

class Storage {
public:
    virtual ~Storage() {};
    virtual QList<QString> listDataPoints(const QString& key) = 0;
    virtual void addDataPoint(const QString& key, const QString& value) = 0;
};
