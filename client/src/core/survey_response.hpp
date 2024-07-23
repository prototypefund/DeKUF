#pragma once

#include <QtCore>

#include "commissioner.hpp"
#include "survey_response.hpp"

class QueryResponse {
public:
    const QString queryId;
    const QMap<QString, int> cohortData;

    bool operator==(const QueryResponse& other) const
    {
        return queryId == other.queryId;
    }
    QueryResponse(const QString& queryId, const QMap<QString, int>& cohortData);
};

class SurveyResponse {
public:
    static QSharedPointer<SurveyResponse> fromJsonByteArray(
        const QByteArray& responseData);

    static QSharedPointer<SurveyResponse> aggregateSurveyResponses(
        QList<QSharedPointer<SurveyResponse>>);

    explicit SurveyResponse(const QString& surveyId);

    SurveyResponse(const QString& surveyId,
        QList<QSharedPointer<QueryResponse>> queryResponses);

    const QString surveyId;
    QList<QSharedPointer<QueryResponse>> queryResponses;

    QByteArray toJsonByteArray() const;
};
