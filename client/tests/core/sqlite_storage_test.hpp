#pragma once

#include <QObject>

class Storage;

class SqliteStorageTest : public QObject {
    Q_OBJECT

private:
    Storage* storage;

private slots:
    void init();
    void cleanup();
    void testListDataPointsInitiallyEmpty();
    void testAddAndListDataPoints();
    void testListDataPointsByName();
    void testAddAndListSurveyResponses();
    void testAddAndListSurveyResponseWithSurvey();
    void testAddAndListSurveySignups();
    void testListSurveySignupForState();
    void testListActiveDelegateSurveySignups();
    void testSaveSurveySignup();
};
