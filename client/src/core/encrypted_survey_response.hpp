#pragma once

#include <QtCore>

#include "../daemon/paillier_encryptor.hpp"
#include "result.hpp"
#include <gmpxx.h>

class EncryptedQueryResponse {
public:
    const QString queryId;
    const QMap<QString, mpz_class> cohortData;

    bool operator==(const EncryptedQueryResponse& other) const
    {
        return queryId == other.queryId;
    }
    EncryptedQueryResponse(
        const QString& queryId, const QMap<QString, mpz_class>& cohortData);
};

class EncryptedSurveyResponse {
public:
    const int number_participants;
    static Result<QSharedPointer<EncryptedSurveyResponse>> fromJsonByteArray(
        const QByteArray& responseData);

    static Result<QSharedPointer<EncryptedSurveyResponse>>
    aggregateEncryptedSurveyResponses(
        QList<QSharedPointer<EncryptedSurveyResponse>>,
        const QSharedPointer<HomomorphicEncryptor> encryptor);

    explicit EncryptedSurveyResponse(
        const QString& surveyId, int number_participants = 1);

    EncryptedSurveyResponse(const QString& surveyId,
        QList<QSharedPointer<EncryptedQueryResponse>> encryptedQueryResponses,
        int number_participants = 1);

    const QString surveyId;
    QList<QSharedPointer<EncryptedQueryResponse>> encryptedQueryResponses;

    QByteArray toJsonByteArray() const;

private:
    static QSharedPointer<EncryptedSurveyResponse> fromJsonObject(
        const QJsonObject& responseObject);
};
