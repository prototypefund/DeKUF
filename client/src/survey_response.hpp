#pragma once

#include "commissioner.hpp"
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
