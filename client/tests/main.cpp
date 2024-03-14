#include <QTest>

#include "storage_test.hpp"
#include "survey_response_test.hpp"
#include "survey_test.hpp"

#define ADD_TEST(c) status |= QTest::qExec(new c, argc, argv)

int main(int argc, char* argv[])
{
    int status = 0;
    ADD_TEST(SurveyResponseTest);
    ADD_TEST(SurveyTest);
    ADD_TEST(StorageTest);
    return status;
}
