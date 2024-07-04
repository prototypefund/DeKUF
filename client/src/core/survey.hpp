#pragma once

#include "survey_response.hpp"

#include <QtCore>

#include <core/commissioner.hpp>

class Query {
public:
    const QString id;
    const QString dataKey;
    const QList<QString> cohorts;
    const bool discrete;

    explicit Query(const QString& id, const QString& dataKey,
        const QList<QString>& cohorts, const bool& discrete);
};

class Survey {
public:
    const QString id;
    const QString name;
    QSharedPointer<Commissioner> commissioner;
    QList<QSharedPointer<Query>> queries;

    Survey(const QString& id, const QString& name);

    static QList<QSharedPointer<Survey>> listFromByteArray(
        const QByteArray& data);

    static QSharedPointer<Survey> fromByteArray(const QByteArray& data);

    QByteArray toByteArray() const;
};
