#pragma once

#include <QObject>

class ClientTest : public QObject {
    Q_OBJECT

private slots:
    void testCreateSurveyResponseSucceedsForRightCommissioner();
    void testCreateSurveyResponseSucceedsForIntervals();
    void testCreateSurveyResponseNullForWrongCommissioner();
};
