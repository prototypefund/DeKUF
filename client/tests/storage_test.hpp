#pragma once

#include <QObject>
#include <qtmetamacros.h>

class StorageTest : public QObject {
    Q_OBJECT

private slots:
    void init();
    void testListDataPointsInitiallyEmpty();
    void testAddAndListDataPoints();
};
