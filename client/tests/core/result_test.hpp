#pragma once

#include <QObject>

class ResultTest : public QObject {
    Q_OBJECT

private slots:
    void testVoidResultSuccess();
    void testVoidResultFailure();
    void testIntResultSuccess();
    void testIntResultFailure();
    void testQStringResultSuccess();
    void testQStringResultFailure();
    void testCustomTypeResultSuccess();
    void testCustomTypeResultFailure();
};
