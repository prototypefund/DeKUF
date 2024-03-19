#pragma once

#include <QtCore>

class Commissioner {
public:
    Commissioner(const QString& name)
        : name(name)
    {
    }
    const QString name;
};