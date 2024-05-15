#include "interval.hpp"

Interval::Interval(const QString& interval)
{
    if (!isValidInterval(interval)) {
        throw std::invalid_argument("Invalid interval string");
    }
    lowerInclusive = interval[0] == '[';
    upperInclusive = interval[interval.length() - 1] == ']';

    QStringList bounds = interval.mid(1, interval.length() - 2).split(",");
    auto lowerStr = bounds[0].trimmed();
    auto upperStr = bounds[1].trimmed();

    if (lowerStr == "-inf") {
        lowerBound = -std::numeric_limits<double>::infinity();
    } else {
        lowerBound = lowerStr.toDouble();
    }

    if (upperStr == "inf") {
        upperBound = std::numeric_limits<double>::infinity();
    } else {
        upperBound = upperStr.toDouble();
    }

    if ((!std::isinf(lowerBound) || !std::isinf(upperBound))
        && lowerBound >= upperBound) {
        throw std::invalid_argument(
            "Lower bound greater than or equal upper bound");
    }
}

bool Interval::isValidInterval(const QString& interval)
{
    QRegularExpression finiteInfExp(R"(^(\[)(-?\d+(\.\d+)?),\s*(inf)(\))$)");
    QRegularExpression infFiniteExp(
        R"(^(\()(-inf),\s*(-?\d+(\.\d+)?|\binf\b)(\]|\))$)");
    QRegularExpression finiteExp(
        R"(^(\[|\()(-?\d+(\.\d+)?),\s*(-?\d+(\.\d+)?)(\]|\))$)");
    return finiteExp.match(interval).hasMatch()
        || finiteInfExp.match(interval).hasMatch()
        || infFiniteExp.match(interval).hasMatch();
}

bool Interval::isInInterval(const double& value)
{
    return (lowerInclusive ? value >= lowerBound : value > lowerBound)
        && (upperInclusive ? value <= upperBound : value < upperBound);
}
