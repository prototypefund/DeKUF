#pragma once

#include <QObject>

class EncryptedSurveyResponseTest : public QObject {
    Q_OBJECT

private slots:
    void testToByteArrayForEmptyResponse();
    void testToByteArrayForResponse();
    void testToAndFromByteArray();
    void testAggregationWithOneQuery();
    void testAggregationWithMultipleQueries();
    void testAggregationReturnsFailureWhenSurveyIdDiffers();
};
