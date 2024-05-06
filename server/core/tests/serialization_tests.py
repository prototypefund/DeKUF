from django.test import TestCase

from core.json_serializers import CommissionerSerializer, SurveyResponseSerializer
from core.models import Commissioner, Survey, Query


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
        self.survey = Survey.objects.create(name="Customer Feedback",
                                            commissioner=self.commissioner)
        self.query = Query.objects.create(survey=self.survey, data_key="test",
                                          cohorts=["Yes", "No"])

    def test_survey_response_serializer(self):
        survey_response_data = {
            "surveyId": self.survey.id,
            "queryResponses": [
                {"queryId": self.query.id, "dataKey": "question1", "data": {"Yes": 1, "No": 0}}
            ],
        }
        serializer = SurveyResponseSerializer(data=survey_response_data)
        if not serializer.is_valid():
            print(serializer.errors)
        self.assertTrue(serializer.is_valid())

        survey_response = serializer.save()
        self.assertIsNotNone(survey_response.id)

        query_response = survey_response.query_responses.first()
        self.assertIsNotNone(query_response)
        self.assertEqual(query_response.data_key, "question1")
        self.assertEqual(query_response.id, self.query.id)
        self.assertDictEqual(query_response.data, {'No': 0, 'Yes': 1})

    def test_survey_response_serializer_with_id_relationship(self):
        survey_response_data = {
            "surveyId": self.survey.id,
            "queryResponses": []
        }

        serializer = SurveyResponseSerializer(data=survey_response_data)
        if not serializer.is_valid():
            print(serializer.errors)
        self.assertTrue(serializer.is_valid())
        survey_response = serializer.save()

        self.assertEqual(survey_response.survey, self.survey)
