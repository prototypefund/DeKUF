#pragma once

#include <QObject>
#include <qtmetamacros.h>

class Storage;

class StorageTest : public QObject {
    Q_OBJECT

private:
    Storage* storage;

private slots:
    void init();
    void cleanup();
    void testListDataPointsInitiallyEmpty();
    void testAddAndListDataPoints();
};
