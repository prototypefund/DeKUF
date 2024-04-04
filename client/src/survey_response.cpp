#include "survey_response.hpp"

QueryResponse::QueryResponse(const QString& dataKey, const QString& data)
    : dataKey(dataKey)
    , data(data)
{
}

QSharedPointer<SurveyResponse> SurveyResponse::fromJsonByteArray(
    QByteArray& responseData)
{
    auto response = QSharedPointer<SurveyResponse>::create();
    auto surveyResponseObject = QJsonDocument::fromJson(responseData).object();

    auto commissionerArray = surveyResponseObject["commissioners"].toArray();
    for (auto commissionerItem : commissionerArray) {
        auto commissionerObject = commissionerItem.toObject();
        auto name = commissionerObject["name"].toString();
        response->commissioners.append(
            QSharedPointer<Commissioner>::create(name));
    }

    auto queryResponseArray = surveyResponseObject["queryResponses"].toArray();
    for (auto queryResponseItem : queryResponseArray) {
        auto queryResponseObject = queryResponseItem.toObject();
        auto dataKey = queryResponseObject["dataKey"].toString();
        auto data = queryResponseObject["data"].toString();
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

    for (auto query : survey->queries) {
        auto dataPoints = storage.listDataPoints(query->dataKey);
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

    for (auto queryResponse : queryResponses) {
        QJsonObject queryJsonResponse;
        queryJsonResponse["dataKey"] = queryResponse->dataKey;
        queryJsonResponse["data"] = queryResponse->data;
        queryJsonResponses.push_back(queryJsonResponse);
    }

    for (auto commissioner : commissioners) {
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
