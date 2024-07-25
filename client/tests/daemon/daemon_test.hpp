#pragma once

#include <QObject>

class DaemonTest : public QObject {
    Q_OBJECT

private slots:
    void testProcessSurveysIgnoresErrors();
    void testProcessSurveysSignsUpForRightCommissioner();
    void testProcessSurveyDoesNotSignUpForWrongCommissioner();
    void testProcessSurveyDoesNotSignUpForWhenDataKeyNotPresent();
    void testProcessSignupsIgnoresEmptySignupState();
    void testProcessSignupsIgnoresNonStartedAggregations();
    void testProcessSignupsHandlesDelegateCase();
    void testProcessSignupsHandlesNonDelegateCase();
    void testProcessSignupsIgnoresEmptyMessagesForDelegate();
    void testCreateSurveyResponseSucceedsForIntervals();
    void testCreateSurveyResponseSucceedsForIntervalsWithInfinity();
};
