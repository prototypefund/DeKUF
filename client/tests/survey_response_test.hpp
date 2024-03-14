#pragma once

#include <QObject>

class SurveyResponseTest : public QObject {
    Q_OBJECT

private slots:
    void testToByteArrayForEmptyResponse();
    void testToByteArrayForResponse();
};
