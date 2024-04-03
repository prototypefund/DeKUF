#pragma once

#include <QtCore>

#include "commissioner.hpp"

class Query {
public:
    const QString dataKey;

    Query(const QString& dataKey);
};

class Survey {
public:
    static QList<QSharedPointer<Survey>> listFromByteArray(
        const QByteArray& data);

    const QString id;
    const QString name;
    QList<QSharedPointer<Commissioner>> commissioners;
    QList<QSharedPointer<Query>> queries;

    Survey(const QString& id, const QString& name);
};
