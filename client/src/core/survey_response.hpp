#pragma once

#include <QtCore>

#include "commissioner.hpp"
#include "survey_response.hpp"

class QueryResponse {
public:
    const QString queryId;
    const QMap<QString, int> cohortData;

    QueryResponse(const QString& queryId, const QMap<QString, int>& cohortData);

    bool operator==(const QueryResponse& other) const
    {
        return queryId == other.queryId;
    }
};

class SurveyResponse {
public:
    static QSharedPointer<SurveyResponse> fromJsonByteArray(
        const QByteArray& responseData);

    QSharedPointer<Commissioner> commissioner;
    QList<QSharedPointer<QueryResponse>> queryResponses;

    QByteArray toJsonByteArray() const;
};
