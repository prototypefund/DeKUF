#pragma once

#include <QObject>

class DaemonTest : public QObject {
    Q_OBJECT

private slots:
    void testProcessSurveysIgnoresErrors();
    void testProcessSurveysSignsUpForRightCommissioner();
    void testProcessSurveyDoesNotSignUpForWrongCommissioner();
    void testProcessSignupsIgnoresErrors();
    void testProcessSignupsIgnoresNonStartedAggregations();
    void testProcessSignupsHandlesDelegateCase();
    void testProcessSignupsHandlesNonDelegateCase();
    void testProcessMessagesForDelegatesIgnoresErrors();
    void testCreateSurveyResponseSucceedsForIntervals();
    void testCreateSurveyResponseSucceedsForIntervalsWithInfinity();
};
