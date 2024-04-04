#pragma once

#include <QtCore>

class Commissioner {
public:
    const QString name;

    explicit Commissioner(const QString& name)
        : name(name)
    {
    }

    bool operator==(const Commissioner& other) const
    {
        return name == other.name;
    }
};
