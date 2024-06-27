#pragma once

#include <QtCore>

#include <core/storage.hpp>
#include <core/survey_response.hpp>

#include "core/survey.hpp"
#include "dbus_service.hpp"
#include "network.hpp"

class Daemon : public QObject {
    Q_OBJECT

    friend class DaemonTest;

public:
    Daemon(QObject* parent, QSharedPointer<Storage> storage,
        QSharedPointer<Network> network);
    QFuture<void> processSurveys();

public slots:
    void run();

signals:
    void finished();

private:
    QSharedPointer<Storage> storage;
    QSharedPointer<Network> network;
    DBusService dbusService;

    void handleSurveysResponse(const QByteArray& data);
    QFuture<void> processSignup(SurveySignup& signup);
    QFuture<void> processSignups();
    QFuture<void> processMessagesForDelegate(const SurveySignup& signup);
    QFuture<void> processMessagesForDelegates();
    QFuture<void> postAggregationResult(SurveySignup& signup);
    QFuture<void> postAggregationResults();
    QSharedPointer<SurveyResponse> createSurveyResponse(
        const Survey& survey) const;
    QSharedPointer<QueryResponse> createQueryResponse(
        const QSharedPointer<Query>& query) const;
    void signUpForSurvey(const QSharedPointer<const Survey> survey);
};
