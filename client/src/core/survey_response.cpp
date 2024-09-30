#include "survey_response.hpp"

#include "result.hpp"

QueryResponse::QueryResponse(
    const QString& queryId, const QMap<QString, int>& cohortData)
    : queryId(queryId)
    , cohortData(cohortData)
{
}

QSharedPointer<EncryptedQueryResponse> QueryResponse::encrypt(
    const QSharedPointer<HomomorphicEncryptor>& encryptor) const
{
    QMap<QString, mpz_class> encryptedCohortData;
    for (auto it = cohortData.constBegin(); it != cohortData.constEnd(); ++it) {
        if (!encryptedCohortData.contains(it.key()))
            encryptedCohortData.insert(it.key(), {});
        encryptedCohortData[it.key()] += encryptor->encrypt(it.value());
    }
    return QSharedPointer<EncryptedQueryResponse>::create(
        queryId, encryptedCohortData);
}

SurveyResponse::SurveyResponse(const QString& surveyId, int number_participants)
    : surveyId(surveyId)
    , number_participants(number_participants)
{
}

SurveyResponse::SurveyResponse(const QString& surveyId,
    QList<QSharedPointer<QueryResponse>> queryResponses,
    int number_participants)
    : surveyId(surveyId)
    , queryResponses(queryResponses)
    , number_participants(number_participants)
{
}

Result<QSharedPointer<SurveyResponse>> SurveyResponse::fromJsonByteArray(
    const QByteArray& responseData)
{
    try {
        auto responseJsonObj = QJsonDocument::fromJson(responseData).object();
        const auto surveyId = responseJsonObj["survey_id"].toString();
        const auto number_participants
            = responseJsonObj["number_participants"].toInt();
        auto response = QSharedPointer<SurveyResponse>::create(
            surveyId, number_participants);

        const auto queryResponseArray
            = responseJsonObj["query_responses"].toArray();

        for (const auto& queryResponseItem : queryResponseArray) {
            const auto queryResponseObject = queryResponseItem.toObject();
            const auto queryId = queryResponseObject["query_id"].toString();

            const auto cohortJsonData = queryResponseObject["data"].toObject();
            QMap<QString, int> cohortData;

            for (auto it = cohortJsonData.constBegin();
                 it != cohortJsonData.constEnd(); ++it) {
                cohortData[it.key()] = it.value().toInt();
            }

            response->queryResponses.append(
                QSharedPointer<QueryResponse>::create(queryId, cohortData));
        }
        return Result(response);
    } catch (const QJsonParseError& error) {
        return Result<QSharedPointer<SurveyResponse>>::Failure(
            error.errorString());
    }
}

// TODO: We just use this for testing, join it with the encrypted versions
// aggregate method
Result<QSharedPointer<SurveyResponse>> SurveyResponse::aggregateSurveyResponses(
    const QList<QSharedPointer<SurveyResponse>> surveyResponses)
{
    if (surveyResponses.isEmpty())
        return Result<QSharedPointer<SurveyResponse>>::Failure(
            "SurveyResponses cannot be empty for aggregation");
    auto surveyId = surveyResponses.first()->surveyId;
    QMap<QString, QMap<QString, int>> aggregatedResults;

    for (const auto& surveyResponse : surveyResponses) {
        if (surveyResponse->surveyId != surveyId)
            return Result<QSharedPointer<SurveyResponse>>::Failure(
                "SurveyResponses need to reference same Survey");

        for (const auto& queryResponse : surveyResponse->queryResponses) {
            const auto& queryId = queryResponse->queryId;
            const auto& cohortData = queryResponse->cohortData;

            auto& queryResult = aggregatedResults[queryId];
            for (auto it = cohortData.constBegin(); it != cohortData.constEnd();
                 ++it) {
                queryResult[it.key()] += it.value();
            }
        }
    }

    QList<QSharedPointer<QueryResponse>> queryResponses;

    for (auto it = aggregatedResults.constBegin();
         it != aggregatedResults.constEnd(); ++it) {
        queryResponses.append(
            QSharedPointer<QueryResponse>::create(it.key(), it.value()));
    }

    return Result(
        QSharedPointer<SurveyResponse>::create(surveyId, queryResponses));
}

QByteArray SurveyResponse::toJsonByteArray() const
{
    QJsonObject surveyJsonResponse;
    QJsonArray queryJsonResponses;

    for (const auto& queryResponse : queryResponses) {
        QJsonObject queryJsonResponse;

        queryJsonResponse["query_id"] = queryResponse->queryId;

        QJsonObject cohortJsonResponse;
        for (auto it = queryResponse->cohortData.constBegin();
             it != queryResponse->cohortData.constEnd(); ++it) {
            cohortJsonResponse.insert(it.key(), QJsonValue(it.value()));
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

QSharedPointer<EncryptedSurveyResponse> SurveyResponse::encrypt(
    const QSharedPointer<HomomorphicEncryptor>& encryptor) const
{
    QList<QSharedPointer<EncryptedQueryResponse>> encryptedQueryResponses;
    for (auto const& queryResponse : queryResponses) {
        encryptedQueryResponses.push_back(queryResponse->encrypt(encryptor));
    }
    return QSharedPointer<EncryptedSurveyResponse>::create(
        surveyId, encryptedQueryResponses);
}
