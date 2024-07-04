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

public slots:
    void run();

signals:
    void finished();

private:
    QSharedPointer<Storage> storage;
    QSharedPointer<Network> network;
    DBusService dbusService;

    QFuture<void> handleSurveysResponse(const QByteArray& data);
    QFuture<void> processSurveys();
    QFuture<void> processInitialSignup(SurveyRecord& signup);
    QFuture<void> processSignups();
    QFuture<void> processMessagesForDelegate(SurveyRecord& signup);
    QFuture<void> postAggregationResult(SurveyRecord& signup);
    QSharedPointer<SurveyResponse> createSurveyResponse(
        const Survey& survey) const;
    QSharedPointer<QueryResponse> createQueryResponse(
        const QSharedPointer<Query>& query) const;
    QFuture<void> signUpForSurvey(const QSharedPointer<const Survey> survey);
};
