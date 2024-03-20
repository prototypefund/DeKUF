#pragma once

#include "commissioner.hpp"
#include "storage.hpp"
#include "survey.hpp"
#include <QtCore>

class QueryResponse {
public:
    QueryResponse(const QString& dataKey, const QString& data)
        : dataKey(dataKey)
        , data(data)
    {
    }
    const QString dataKey;
    const QString data;
};

class SurveyResponse {
public:
    QList<QSharedPointer<Commissioner>> commissioners;
    QList<QSharedPointer<QueryResponse>> queryResponses;

    static QSharedPointer<SurveyResponse> create(
        QSharedPointer<Survey> survey, Storage& storage)
    {
        // Only KDE allowed as commissioner
        QString kdeName("KDE");
        if (!std::any_of(survey->commissioners.begin(),
                survey->commissioners.end(),
                [&](const QSharedPointer<Commissioner>& commissioner) {
                    return commissioner->name == kdeName;
                }))
            return QSharedPointer<SurveyResponse>();

        auto surveyResponse = QSharedPointer<SurveyResponse>::create();
        surveyResponse->commissioners.append(
            QSharedPointer<Commissioner>::create(kdeName));

        for (auto query : survey->queries) {
            auto dataPoints = storage.listDataPoints(query->dataKey);
            if (dataPoints.count() == 0)
                continue;
            surveyResponse->queryResponses.append(
                QSharedPointer<QueryResponse>::create(
                    query->dataKey, dataPoints.first()));
        }
        return surveyResponse;
    }

    QByteArray toJsonByteArray()
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
};
