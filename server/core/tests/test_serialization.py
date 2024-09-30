from core.json_serializers import (
    CommissionerSerializer,
    SurveyResponseSerializer,
)
from core.models.commissioner import Commissioner
from core.models.data_point import DataPoint, Types
from core.models.survey import Query, Survey
from django.test import TestCase


class CommissionerSerializersTestCase(TestCase):
    def setUp(self):
        self.commissioner_data = {"name": "test commissioner"}
        self.commissioner_obj = Commissioner.objects.create(
            **self.commissioner_data
        )

    def test_commissioner_serializer(self):
        serializer = CommissionerSerializer(self.commissioner_obj)
        self.assertEqual(
            serializer.data["name"], self.commissioner_data["name"]
        )


class SurveyResponseSerializerTestCase(TestCase):
    def setUp(self):
        self.commissioner = Commissioner.objects.create(name="test")
        self.survey = Survey.objects.create(
            name="Customer Feedback", commissioner=self.commissioner
        )
        self.data_point = DataPoint.objects.create(
            name="test", key="key", type=Types.INTEGER.value
        )
        self.query = Query.objects.create(
            survey=self.survey,
            data_point=self.data_point,
            cohorts=["Yes", "No"],
        )

    def test_survey_response_serializer(self):
        survey_response_data = {
            "survey_id": self.survey.id,
            "number_participants": 1,
            "query_responses": [
                {
                    "query_id": self.query.id,
                    "data_key": "question1",
                    "data": {"Yes": 1, "No": 0},
                }
            ],
        }
        serializer = SurveyResponseSerializer(data=survey_response_data)
        if not serializer.is_valid():
            print(serializer.errors)
        self.assertTrue(serializer.is_valid())

        survey_response = serializer.save()
        self.assertIsNotNone(survey_response.id)
        self.assertEqual(survey_response.number_participants, 1)

        query_response = survey_response.query_responses.first()
        self.assertIsNotNone(query_response)
        self.assertDictEqual(query_response.data, {"No": 0, "Yes": 1})

    def test_survey_response_serializer_with_id_relationship(self):
        survey_response_data = {
            "survey_id": self.survey.id,
            "query_responses": [],
        }

        serializer = SurveyResponseSerializer(data=survey_response_data)
        if not serializer.is_valid():
            print(serializer.errors)
        self.assertTrue(serializer.is_valid())
        survey_response = serializer.save()

        self.assertEqual(survey_response.survey, self.survey)
