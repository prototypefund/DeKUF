#pragma once

#include <QtCore>

class Commisioner;
class Survey;
class SurveyResponse;

class QueryResponse {
public:
    const QString dataKey;
    const QString data;

    QueryResponse(const QString& dataKey, const QString& data);
};

class SurveyResponse {
public:
    QList<QSharedPointer<Commissioner>> commissioners;
    QList<QSharedPointer<QueryResponse>> queryResponses;

    static QSharedPointer<SurveyResponse> create(
        QSharedPointer<Survey> survey, Storage& storage);

    QByteArray toJsonByteArray();
};
