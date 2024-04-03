#pragma once

#include "../../src/storage.hpp"
#include <QtCore>

class StorageStub : public Storage {
private:
    QList<QPair<QString, QString>> dataPoints;

public:
    QList<DataPoint> listDataPoints(const QString& key)
    {
        QList<DataPoint> matchingValues;
        for (auto dataPoint : dataPoints)
            matchingValues.push_back({ .value = dataPoint.second });
        return matchingValues;
    };
    virtual void addDataPoint(const QString& dataKey, const QString& data)
    {
        dataPoints.push_back(QPair<QString, QString>(dataKey, data));
    }
};
