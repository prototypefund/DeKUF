#pragma once

#include <QtCore>

#include <core/storage.hpp>
#include <core/survey.hpp>
#include <core/survey_response.hpp>

class QNetworkAccessManager;

class Client : public QObject {
    Q_OBJECT

    friend class ClientTest;

public:
    Client(QObject* parent, QSharedPointer<Storage> storage);
    void fetchSurveys();

public slots:
    void run();

signals:
    void finished();

private:
    QSharedPointer<Storage> storage;
    QNetworkAccessManager* manager;

    void handleSurveysResponse(const QByteArray& data);
    QSharedPointer<SurveyResponse> createSurveyResponse(
        const Survey& survey) const;
    void postSurveyResponse(QSharedPointer<SurveyResponse>);
};
