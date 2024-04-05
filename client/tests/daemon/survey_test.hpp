#pragma once

#include <QObject>

class SurveyTest : public QObject {
    Q_OBJECT

private slots:
    void testListFromByteArrayForEmptyArray();
    void testListFromByteArrayForSingleSurvey();
    void testListFromByteArrayForSingleSurveyWithQuery();
};
