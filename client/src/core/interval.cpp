#include "interval.hpp"

Interval::Interval(const QString& interval)
{
    if (!isValidInterval(interval)) {
        throw std::invalid_argument("Invalid interval string");
    }
    lowerInclusive = interval[0] == '[';
    upperInclusive = interval[interval.length() - 1] == ']';

    QStringList bounds = interval.mid(1, interval.length() - 2).split(",");
    lowerBound = bounds[0].trimmed().toDouble();
    upperBound = bounds[1].trimmed().toDouble();
}

bool Interval::isValidInterval(const QString& interval)
{
    QRegularExpression intervalExp(
        R"(^(\[|\()(-?\d+(\.\d+)?),\s*(-?\d+(\.\d+)?)(\]|\))$)");
    return intervalExp.match(interval).hasMatch();
}

bool Interval::isInInterval(const double& value)
{
    return (lowerInclusive ? value >= lowerBound : value > lowerBound)
        && (upperInclusive ? value <= upperBound : value < upperBound);
}
