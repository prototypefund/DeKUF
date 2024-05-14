#pragma once

#include <QObject>

class IntervalTest : public QObject {
    Q_OBJECT

private slots:
    void testOpen();
    void testClosed();
    void testMixed();
    void testValid();
    void testInvalid();
};
