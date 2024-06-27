#pragma once

#include <QObject>

class DaemonTest : public QObject {
    Q_OBJECT

private slots:
    void testProcessSurveysIgnoresErrors();
    void testProcessSurveysSignsUpForRightCommissioner();
    void testProcessSurveyDoesNotSignUpForWrongCommissioner();
    void testCreateSurveyResponseSucceedsForIntervals();
    void testCreateSurveyResponseSucceedsForIntervalsWithInfinity();
};
