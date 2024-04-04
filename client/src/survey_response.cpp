#include "survey_response.hpp"

QueryResponse::QueryResponse(const QString& dataKey, const QString& data)
    : dataKey(dataKey)
    , data(data)
{
}

QSharedPointer<SurveyResponse> SurveyResponse::fromJsonByteArray(
    const QByteArray& responseData)
{
    auto response = QSharedPointer<SurveyResponse>::create();
    const auto surveyResponseObject
        = QJsonDocument::fromJson(responseData).object();

    const auto commissionerArray
        = surveyResponseObject["commissioners"].toArray();
    for (const auto& commissionerItem : commissionerArray) {
        const auto commissionerObject = commissionerItem.toObject();
        const auto name = commissionerObject["name"].toString();
        response->commissioners.append(
            QSharedPointer<Commissioner>::create(name));
    }

    const auto queryResponseArray
        = surveyResponseObject["queryResponses"].toArray();
    for (const auto& queryResponseItem : queryResponseArray) {
        const auto queryResponseObject = queryResponseItem.toObject();
        const auto dataKey = queryResponseObject["dataKey"].toString();
        const auto data = queryResponseObject["data"].toString();
        response->queryResponses.append(
            QSharedPointer<QueryResponse>::create(dataKey, data));
    }

    return response;
}

QSharedPointer<SurveyResponse> SurveyResponse::create(
    QSharedPointer<Survey> survey, Storage& storage)
{
    // Only KDE allowed as commissioner
    QString kdeName("KDE");
    if (!std::any_of(survey->commissioners.begin(), survey->commissioners.end(),
            [&](const QSharedPointer<Commissioner>& commissioner) {
                return commissioner->name == kdeName;
            }))
        return {};

    auto surveyResponse = QSharedPointer<SurveyResponse>::create();
    surveyResponse->commissioners.append(
        QSharedPointer<Commissioner>::create(kdeName));

    for (const auto& query : survey->queries) {
        const auto dataPoints = storage.listDataPoints(query->dataKey);
        if (dataPoints.count() == 0)
            continue;
        surveyResponse->queryResponses.append(
            QSharedPointer<QueryResponse>::create(
                query->dataKey, dataPoints.first().value));
    }
    return surveyResponse;
}

QByteArray SurveyResponse::toJsonByteArray() const
{
    QJsonObject surveyJsonResponse;
    QJsonArray queryJsonResponses;
    QJsonArray commissionersJson;

    for (const auto& queryResponse : queryResponses) {
        QJsonObject queryJsonResponse;
        queryJsonResponse["dataKey"] = queryResponse->dataKey;
        queryJsonResponse["data"] = queryResponse->data;
        queryJsonResponses.push_back(queryJsonResponse);
    }

    for (const auto& commissioner : commissioners) {
        QJsonObject commissionerJson;
        commissionerJson["name"] = commissioner->name;
        commissionersJson.push_back(commissionerJson);
    }

    surveyJsonResponse["queryResponses"] = queryJsonResponses;
    surveyJsonResponse["commissioners"] = commissionersJson;

    QJsonDocument root;
    root.setObject(surveyJsonResponse);
    return root.toJson();
}
