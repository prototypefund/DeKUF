#pragma once

#include <QtCore>

class Interval {
public:
    Interval(const QString& interval);

    bool isInInterval(const double& value);

private:
    bool isValidInterval(const QString& interval);

    double lowerBound, upperBound;
    bool lowerInclusive, upperInclusive;
};
