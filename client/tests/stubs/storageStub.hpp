#pragma once

#include "../../src/storage.hpp"
#include <QtCore>

class StorageStub : public Storage {
private:
    QList<QPair<QString, QString>> dataPoints;

public:
    QList<QString> listDataPoints(const QString& key)
    {
        QList<QString> matchingValues;
        for (auto dataPoint : dataPoints)
            matchingValues.push_back(dataPoint.second);
        return matchingValues;
    };
    virtual void addDataPoint(const QString& dataKey, const QString& data)
    {
        dataPoints.push_back(QPair<QString, QString>(dataKey, data));
    }
};
