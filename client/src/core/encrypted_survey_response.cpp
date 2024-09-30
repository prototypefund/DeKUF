#include "encrypted_survey_response.hpp"
#include <daemon/paillier_encryptor.hpp>

#include "result.hpp"
#include <gmpxx.h>

/**
 * The Constructors are only for testing purposes. We only want to create
 * encrypted SurveyResponses and thus QueryResponses only by encrypting them
 * (see SurveyResponse.encrypt())
 */
EncryptedQueryResponse::EncryptedQueryResponse(
    const QString& queryId, const QMap<QString, mpz_class>& cohortData)
    : queryId(queryId)
    , cohortData(cohortData)
{
}

EncryptedSurveyResponse::EncryptedSurveyResponse(
    const QString& surveyId, int number_participants)
    : surveyId(surveyId)
    , number_participants(number_participants)
{
}

EncryptedSurveyResponse::EncryptedSurveyResponse(const QString& surveyId,
    QList<QSharedPointer<EncryptedQueryResponse>> queryResponses,
    int number_participants)
    : surveyId(surveyId)
    , encryptedQueryResponses(queryResponses)
    , number_participants(number_participants)
{
}

Result<QSharedPointer<EncryptedSurveyResponse>>
EncryptedSurveyResponse::fromJsonByteArray(const QByteArray& responseData)
{
    try {
        auto responseJsonObj = QJsonDocument::fromJson(responseData).object();
        const auto surveyId = responseJsonObj["survey_id"].toString();
        const auto number_participants
            = responseJsonObj["number_participants"].toInt();
        auto response
            = QSharedPointer<EncryptedSurveyResponse>::create(surveyId);

        const auto queryResponseArray
            = responseJsonObj["query_responses"].toArray();

        for (const auto& queryResponseItem : queryResponseArray) {
            const auto queryResponseObject = queryResponseItem.toObject();
            const auto queryId = queryResponseObject["query_id"].toString();

            const auto cohortJsonData = queryResponseObject["data"].toObject();
            QMap<QString, mpz_class> cohortData;

            for (auto it = cohortJsonData.constBegin();
                 it != cohortJsonData.constEnd(); ++it) {
                cohortData[it.key()]
                    = mpz_class(it.value().toString().toStdString());
            }

            response->encryptedQueryResponses.append(
                QSharedPointer<EncryptedQueryResponse>::create(
                    queryId, cohortData));
        }
        return Result(response);
    } catch (const QJsonParseError& error) {
        return Result<QSharedPointer<EncryptedSurveyResponse>>::Failure(
            error.errorString());
    }
}

Result<QSharedPointer<EncryptedSurveyResponse>>
EncryptedSurveyResponse::aggregateEncryptedSurveyResponses(
    const QList<QSharedPointer<EncryptedSurveyResponse>> surveyResponses,
    const QSharedPointer<HomomorphicEncryptor> encryptor)
{
    if (surveyResponses.isEmpty())
        return Result<QSharedPointer<EncryptedSurveyResponse>>::Failure(
            "SurveyResponses cannot be empty for aggregation");
    auto surveyId = surveyResponses.first()->surveyId;
    QMap<QString, QMap<QString, mpz_class>> aggregatedResults;

    for (const auto& surveyResponse : surveyResponses) {
        if (surveyResponse->surveyId != surveyId)
            return Result<QSharedPointer<EncryptedSurveyResponse>>::Failure(
                "EncryptedSurveyResponses need to reference same Survey");

        for (const auto& queryResponse :
            surveyResponse->encryptedQueryResponses) {
            const auto& queryId = queryResponse->queryId;
            const auto& cohortData = queryResponse->cohortData;

            auto& queryResult = aggregatedResults[queryId];
            for (auto it = cohortData.constBegin(); it != cohortData.constEnd();
                 ++it) {
                const auto& key = it.key();
                const auto& newValue = it.value();

                if (!queryResult.contains(key)) {
                    queryResult[key] = newValue;
                    continue;
                }
                const auto& existingValue = queryResult[key];
                auto addedValue
                    = encryptor->addEncrypted(newValue, existingValue);

                queryResult[key] = addedValue;
            }
        }
    }

    QList<QSharedPointer<EncryptedQueryResponse>> queryResponses;

    for (auto it = aggregatedResults.constBegin();
         it != aggregatedResults.constEnd(); ++it) {
        auto response = QSharedPointer<EncryptedQueryResponse>::create(
            it.key(), it.value());
        queryResponses.append(response);
    }

    return Result(QSharedPointer<EncryptedSurveyResponse>::create(
        surveyId, queryResponses, surveyResponses.length()));
}

QByteArray EncryptedSurveyResponse::toJsonByteArray() const
{
    QJsonObject surveyJsonResponse;
    QJsonArray queryJsonResponses;

    for (const auto& queryResponse : encryptedQueryResponses) {
        QJsonObject queryJsonResponse;

        queryJsonResponse["query_id"] = queryResponse->queryId;

        QJsonObject cohortJsonResponse;
        for (auto it = queryResponse->cohortData.constBegin();
             it != queryResponse->cohortData.constEnd(); ++it) {
            cohortJsonResponse.insert(it.key(),
                QJsonValue(QString::fromStdString(it.value().get_str())));
        }
        queryJsonResponse["data"] = cohortJsonResponse;
        queryJsonResponses.push_back(queryJsonResponse);
    }

    surveyJsonResponse["query_responses"] = queryJsonResponses;
    surveyJsonResponse["survey_id"] = surveyId;
    surveyJsonResponse["number_participants"] = number_participants;

    QJsonDocument root;
    root.setObject(surveyJsonResponse);
    return root.toJson();
}
