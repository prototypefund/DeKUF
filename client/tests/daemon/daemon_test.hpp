#pragma once

#include <QObject>

class DaemonTest : public QObject {
    Q_OBJECT

private slots:
    void testCreateSurveyResponseSucceedsForRightCommissioner();
    void testCreateSurveyResponseSucceedsForIntervals();
    void testCreateSurveyResponseSucceedsForIntervalsWithInfinity();
    void testCreateSurveyResponseNullForWrongCommissioner();
};
