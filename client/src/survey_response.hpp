#pragma once

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
    QList<QSharedPointer<QueryResponse>> queryResponses;

    QByteArray toJsonByteArray()
    {
        QJsonObject surveyJsonResponse;
        QJsonArray queryJsonResponses;

        for (auto queryResponse : queryResponses) {
            QJsonObject queryJsonResponse;
            queryJsonResponse["dataKey"] = queryResponse->dataKey;
            queryJsonResponse["data"] = queryResponse->data;
            queryJsonResponses.push_back(queryJsonResponse);
        }

        surveyJsonResponse["queryResponses"] = queryJsonResponses;

        QJsonDocument root;
        root.setObject(surveyJsonResponse);
        return root.toJson();
    }
};
