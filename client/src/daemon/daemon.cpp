#include <QDateTime>
#include <QRegularExpression>
#include <QTextStream>

#include "core/interval.hpp"
#include "core/storage.hpp"
#include "core/survey_response.hpp"
#include "daemon.hpp"

namespace {
/**
 * Returns a future that resolves once all provided futures resolved.
 *
 * Replace with QtPromise::whenAll once we upgrade to Qt >= 6.3.
 */
QFuture<void> whenAll(const QList<QFuture<void>>& futures)
{
    auto promise = QSharedPointer<QPromise<void>>::create();
    auto pendingFutures = QSharedPointer<int>::create(0);
    for (auto future : futures)
        if (!future.isFinished())
            *pendingFutures = *pendingFutures + 1;
    if (*pendingFutures == 0) {
        promise->finish();
        return promise->future();
    }

    for (auto future : futures)
        if (!future.isFinished()) {
            future.then([=] {
                *pendingFutures = *pendingFutures - 1;
                if (*pendingFutures == 0)
                    promise->finish();
            });
        }
    return promise->future();
}

/**
 * Chains a callback that returns a subsequent future once the provided future
 * resolves, for consecutive execution.
 *
 * This is seemingly not the behaviour of QFuture::next() - though one might
 * argue it should be, or that there should be a more elegant way to do this
 * kind of thing.
 */
template <typename T>
QFuture<void> chain(QFuture<T> future, std::function<QFuture<void>(T)> next)
{
    auto promise = QSharedPointer<QPromise<void>>::create();
    future.then(
        [=](T value) { next(value).then([promise] { promise->finish(); }); });
    return promise->future();
}

// TODO: Consider just inlining this.
QFuture<void> forEachSignup(const QList<SurveySignup>& signups,
    std::function<QFuture<void>(SurveySignup&)> callback)
{
    QList<QFuture<void>> futures;
    for (auto signup : signups)
        futures.append(callback(signup));
    return whenAll(futures);
}
};

Daemon::Daemon(QObject* parent, QSharedPointer<Storage> storage,
    QSharedPointer<Network> network)
    : QObject(parent)
    , storage(storage)
    , network(network)
    , dbusService(storage)
{
    if (auto object = dynamic_cast<QObject*>(network.get()))
        object->setParent(this);
}

void Daemon::run()
{
    qDebug() << "Processing started.";

    qDebug() << "Stored datapoints:";
    for (const auto& dataPoint : storage->listDataPoints())
        qDebug() << "-" << dataPoint.value;

    qDebug() << "Survey signups:";
    for (const auto& signup : storage->listSurveySignups())
        qDebug() << "-" << signup.survey->id << "as" << signup.clientId
                 << "state:" << signup.state;

    // TODO: While these calls seem to run one after another, it seems, the way
    //       then() works, there's no actual guarantee they do. We could use
    //       chain(), but the code wouldn't look particularly pretty. We should
    //       properly test this and find a better way to ensure they run in a
    //       series.
    qDebug() << "Processing surveys ...";
    processSurveys()
        .then([&] {
            qDebug() << "Processing signups ...";
            processSignups();
        })
        .then([&] {
            qDebug() << "Processing messages for delegate ...";
            processMessagesForDelegates();
        })
        .then([&] {
            qDebug() << "Posting aggregation results ...";
            postAggregationResults();
        })
        .then([&] {
            qDebug() << "Processing finished.";
            emit finished();
        });
}

QFuture<void> Daemon::handleSurveysResponse(const QByteArray& data)
{
    using Qt::endl;

    QTextStream cout(stdout);
    const auto surveys = Survey::listFromByteArray(data);

    qDebug() << "Fetched surveys:" << surveys.count();

    QSet<QString> signedUpSurveys;
    for (const auto& signup : storage->listSurveySignups())
        signedUpSurveys.insert(signup.survey->id);

    QList<QFuture<void>> futures;
    for (const auto& survey : surveys) {
        if (signedUpSurveys.contains(survey->id))
            continue;

        // Only KDE allowed as commissioner
        QString kdeName("KDE");
        if (survey->commissioner->name != kdeName)
            continue;

        // TODO: Only sign up for surveys if we have the data points they
        //       request.
        futures.append(signUpForSurvey(survey));
    }
    return whenAll(futures);
}

QFuture<void> Daemon::processSurveys()
{
    return chain<QByteArray>(network->listSurveys(),
        [this](QByteArray data) { return handleSurveysResponse(data); });
}

QFuture<void> Daemon::signUpForSurvey(const QSharedPointer<const Survey> survey)
{
    return network->surveySignup(survey->id).then([&, survey](QByteArray data) {
        const auto responseObject = QJsonDocument::fromJson(data).object();
        const auto clientId = responseObject["client_id"].toString();
        storage->addSurveySignup(*survey, "initial", clientId, "");
    });
}

QFuture<void> Daemon::processSignup(SurveySignup& signup)
{
    return network->getSignupState(signup.clientId)
        .then([&, signup](QByteArray data) mutable {
            const auto responseDocument = QJsonDocument::fromJson(data);
            const auto responseObject = responseDocument.object();
            if (!responseObject["aggregation_started"].toBool()) {
                return;
            }

            signup.delegateId = responseObject["delegate_id"].toString();

            if (signup.clientId == signup.delegateId) {
                signup.state = "processing";
                signup.groupSize = responseObject["group_size"].toInt();
                // TODO: Either send data to itself here, or implement some
                // other
                //       logic to deal with the delegate's own data - also for
                //       the group_size = 1 case.
            } else {
                // TODO: Send data to delegate.
                signup.state = "done";
            }

            storage->saveSurveySignup(signup);
        });
}

QFuture<void> Daemon::processSignups()
{
    auto signups = storage->listSurveySignupsForState("initial");
    return forEachSignup(
        signups, [&](SurveySignup& signup) { return processSignup(signup); });
}

QFuture<void> Daemon::processMessagesForDelegate(const SurveySignup& signup)
{
    return network->getMessagesForDelegate(signup.delegateId)
        .then([](QByteArray) {
            // TODO: Store responses from other clients.
        });
}

QFuture<void> Daemon::processMessagesForDelegates()
{
    auto signups = storage->listActiveDelegateSurveySignups();
    return forEachSignup(signups, [&](const SurveySignup& signup) {
        return processMessagesForDelegate(signup);
    });
}

QFuture<void> Daemon::postAggregationResult(SurveySignup& signup)
{
    // TODO: Once messages are actually stored, ensure that the amount equals
    //       group size - 1.
    assert(signup.groupSize == 1);

    const auto& survey = *signup.survey;
    auto delegateResponse = createSurveyResponse(survey);
    // TODO: If there is more than one message, aggregate the results into a
    //       single response.

    auto data = delegateResponse->toJsonByteArray();
    return network->postAggregationResult(signup.delegateId, data)
        .then([=]() mutable {
            // Note that we are saving the response of the delegate itself here,
            // just how non-delegate clients would store their response to the
            // delegate. We should not store the aggregated response here.
            storage->addSurveyResponse(*delegateResponse, survey);
            signup.state = "done";
            storage->saveSurveySignup(signup);
        });
}

QFuture<void> Daemon::postAggregationResults()
{
    auto signups = storage->listActiveDelegateSurveySignups();
    return forEachSignup(signups,
        [&](SurveySignup& signup) { return postAggregationResult(signup); });
}

QSharedPointer<SurveyResponse> Daemon::createSurveyResponse(
    const Survey& survey) const
{
    auto surveyResponse = QSharedPointer<SurveyResponse>::create(survey.id);

    for (const auto& query : survey.queries) {
        auto queryResponse = createQueryResponse(query);
        if (queryResponse == nullptr)
            continue;
        surveyResponse->queryResponses.append(queryResponse);
    }
    return surveyResponse;
}

QSharedPointer<QueryResponse> Daemon::createQueryResponse(
    const QSharedPointer<Query>& query) const
{
    const auto dataPoints = storage->listDataPoints(query->dataKey);

    qDebug() << "Datakey" << query->dataKey;

    if (dataPoints.count() == 0)
        return nullptr;

    qDebug() << "Found dataPoints:" << dataPoints.count();

    QMap<QString, int> cohortData;

    for (const QString& cohort : query->cohorts) {
        cohortData[cohort] = 0;

        for (const DataPoint& dataPoint : dataPoints) {
            if (query->discrete) {
                if (cohort == dataPoint.value) {
                    cohortData[cohort]++;
                }
                continue;
            }
            try {
                auto interval = Interval(cohort);

                if (interval.isInInterval(dataPoint.value.toDouble())) {
                    cohortData[cohort]++;
                }
            } catch (std::invalid_argument) {
                // Error handling here
            }
        }
    }

    return QSharedPointer<QueryResponse>::create(query->id, cohortData);
}
