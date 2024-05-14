#include "survey_response.hpp"

QueryResponse::QueryResponse(
    const QString& queryId, const QMap<QString, int>& cohortData)
    : queryId(queryId)
    , cohortData(cohortData)
{
}

SurveyResponse::SurveyResponse(const QString& surveyId)
    : surveyId(surveyId)
{
}

QSharedPointer<SurveyResponse> SurveyResponse::fromJsonByteArray(
    const QByteArray& responseData)
{

    const auto surveyResponseObject
        = QJsonDocument::fromJson(responseData).object();

    const auto surveyId = surveyResponseObject["survey_id"].toString();
    auto response = QSharedPointer<SurveyResponse>::create(surveyId);

    const auto queryResponseArray
        = surveyResponseObject["query_responses"].toArray();

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

    return response;
}

QByteArray SurveyResponse::toJsonByteArray() const
{
    QJsonObject surveyJsonResponse;
    QJsonArray queryJsonResponses;

    qDebug() << "sjdbfjhsbdhjbsdjhbfj";

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

    QJsonDocument root;
    root.setObject(surveyJsonResponse);
    return root.toJson();
}
