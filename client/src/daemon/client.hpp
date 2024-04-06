#pragma once

#include <QtCore>

#include <core/sqlite_storage.hpp>
#include <core/survey.hpp>
#include <core/survey_response.hpp>

class QNetworkAccessManager;

class Client : public QObject {
    Q_OBJECT

public:
    explicit Client(QObject* parent = 0);
    void fetchSurveys();

public slots:
    void run();

signals:
    void finished();

private:
    SqliteStorage storage;
    QNetworkAccessManager* manager;

    void handleSurveysResponse(const QByteArray& data);
    void postSurveyResponse(QSharedPointer<SurveyResponse>);
};
