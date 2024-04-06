import unittest

from django.test import TestCase

from .json_serializers import (
    CommissionerSerializer,
    QueryResponseSerializer,
    SurveyResponseSerializer,
)
from .models import Commissioner, QueryResponse, SurveyResponse


class SerializersTestCase(TestCase):
    def setUp(self):
        self.commissioner_data = {"name": "test commissioner"}
        self.commissioner_obj = Commissioner.objects.create(
            **self.commissioner_data
        )

        self.survey_response_data = {
            "commissioners": [self.commissioner_data],
            "queryResponses": [
                {"dataKey": "question1", "data": {"answer": "Yes"}}
            ],
        }

    def test_commissioner_serializer(self):
        serializer = CommissionerSerializer(self.commissioner_obj)
        self.assertEqual(
            serializer.data["name"], self.commissioner_data["name"]
        )

    def test_survey_response_serializer(self):
        serializer = SurveyResponseSerializer(data=self.survey_response_data)
        is_valid = serializer.is_valid()
        if not is_valid:
            print(serializer.errors)
        self.assertTrue(serializer.is_valid())

        survey_response = serializer.save()
        self.assertIsNotNone(survey_response.id)
        self.assertEqual(survey_response.commissioners.count(), 1)

        query_response = survey_response.queryResponses.first()
        self.assertIsNotNone(query_response)
        self.assertEqual(query_response.data_key, "question1")
        self.assertDictEqual(query_response.data, {"answer": "Yes"})
