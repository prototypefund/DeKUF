#pragma once

#include <QtCore>

#include "encrypted_survey_response.hpp"
#include "result.hpp"
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

    QSharedPointer<EncryptedQueryResponse> encrypt(
        const QSharedPointer<HomomorphicEncryptor>& encryptor) const;
};

class SurveyResponse {
public:
    int number_participants;
    static Result<QSharedPointer<SurveyResponse>> fromJsonByteArray(
        const QByteArray& responseData);

    static Result<QSharedPointer<SurveyResponse>> aggregateSurveyResponses(
        QList<QSharedPointer<SurveyResponse>>);

    explicit SurveyResponse(
        const QString& surveyId, int number_participants = 1);

    SurveyResponse(const QString& surveyId,
        QList<QSharedPointer<QueryResponse>> queryResponses,
        int number_participants = 1);

    const QString surveyId;
    QList<QSharedPointer<QueryResponse>> queryResponses;

    QByteArray toJsonByteArray() const;

    QSharedPointer<EncryptedSurveyResponse> encrypt(
        const QSharedPointer<HomomorphicEncryptor>& encryptor) const;

private:
    static QSharedPointer<SurveyResponse> fromJsonObject(
        const QJsonObject& responseObject);
};
