#pragma once

#include <QtCore>

// We're making synchronous network requests here - not particularly future
// proof. We've considered two alternatives, which would both bleed quite
// heavily into any code using this:
//
// 1. Using futures and promises: Unfortunately, the Qt version we use at the
//    time of writing - 6.2 - doesn't really provide the primitives we need to
//    write proper asynchronous code that's not error-prone and hard to read
//    (with home grown workardounds). The kind of things we'd need seem to be in
//    place since Qt 6.4, so upgrading to that version would probably make this
//    approach feasible, but since that version isn't in the package
//    repositories of popular distributions, it seems a bit too bleeding edge
//    for solid KDE compatibility.
//
// 2. Use signals and slots for a more traditional Qt approach. QStateMachine
//    seems like it would be quite helpful, but at the end of the day, this
//    rewrite would add quite a bit of boilerplate and make the logic more
//    tedious to change. It could be a good option once we've got the logic
//    nailed down.
//
// For the time being, we've decided to stick to synchronous requests. Firstly,
// our logic is - at the time of writing - strictly serial and sequential
// anyway. Secondly, this makes it quite a bit easier to prototype the logic we
// need quickly, and it also makes it easier to understand. By the time anyone
// other than the original authors reads this, we'd probably be happy to accept
// patches and/or guidance to make this properly asynchronous.

class Network {
public:
    virtual ~Network() = default;

    virtual QFuture<QByteArray> listSurveys() const = 0;
    virtual QFuture<QByteArray> surveySignup(
        const QString& surveyId, const QString& publicKey)
        = 0;
    virtual QByteArray getSignupState(const QString& clientId) const = 0;
    virtual bool postMessageToDelegate(
        const QString& delegatePublicKey, const QString& message) const = 0;
    virtual QByteArray getMessagesForDelegate(
        const QString& delegateId) const = 0;
    virtual bool postAggregationResult(
        const QString& delegateId, const QByteArray& data)
        = 0;
};
