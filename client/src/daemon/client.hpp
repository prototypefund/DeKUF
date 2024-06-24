#pragma once

#include <QtCore>

#include <core/storage.hpp>
#include <core/survey_response.hpp>

#include "core/survey.hpp"
#include "dbus_service.hpp"

class QNetworkAccessManager;
class QNetworkReply;

class Client : public QObject {
    Q_OBJECT

    friend class ClientTest;

public:
    Client(QObject* parent, QSharedPointer<Storage> storage);
    void processSurveys(std::function<void()> callback);

public slots:
    void run();

signals:
    void finished();

private:
    QSharedPointer<Storage> storage;
    QNetworkAccessManager* manager;
    DBusService dbusService;

    void handleSurveysResponse(const QByteArray& data);
    void processSignup(SurveySignup& signup, std::function<void()> callback);
    void processSignups(std::function<void()> callback);
    void processMessagesForDelegate(
        const SurveySignup& signup, std::function<void()> callback);
    void processMessagesForDelegate(std::function<void()> callback);
    QSharedPointer<SurveyResponse> createSurveyResponse(
        const Survey& survey) const;
    QSharedPointer<QueryResponse> createQueryResponse(
        const QSharedPointer<Query>& query) const;
    void signUpForSurvey(const QSharedPointer<const Survey> survey);
    void getRequest(
        const QString& url, std::function<void(QNetworkReply*)> callback);
    void postRequest(const QString& url, const QByteArray& data,
        std::function<void(QNetworkReply*)> callback);
};
