from core.models.commissioner import Commissioner
from core.models.data_point import DataPoint, Types
from core.models.response import QueryResponse, SurveyResponse
from core.models.survey import Query, Survey
from django.test import TestCase
from phe import paillier


class QueryResponseAggregationTestCase(TestCase):
    def setUp(self):
        self.commissioner = Commissioner.objects.create(name="test")
        self.survey = Survey.objects.create(
            name="Customer Feedback", commissioner=self.commissioner
        )
        self.data_point = DataPoint.objects.create(
            name="test", key="test", type=Types.INTEGER.value
        )
        self.public_key, self.private_key = paillier.generate_paillier_keypair()

    def test_correct_aggregation_with_one_query_response(self):
        query = Query.objects.create(
            survey=self.survey,
            data_point=self.data_point,
            cohorts=["Yes", "No"],
        )
        response_data = {
            "Yes": self.public_key.encrypt(1).ciphertext(),
            "No": self.public_key.encrypt(0).ciphertext(),
        }

        survey_response = SurveyResponse.objects.create(survey=self.survey)
        query_response = QueryResponse.objects.create(
            survey_response=survey_response, query=query, data=response_data
        )
        query.aggregate_query_response(
            query_response, self.public_key, self.private_key
        )

        self.assertEqual(query.aggregated_results, {"Yes": 1, "No": 0})

    def test_correct_aggregation_with_multiple_query_responses(self):
        query = Query.objects.create(
            survey=self.survey,
            data_point=self.data_point,
            cohorts=["Yes", "No"],
        )
        response_data_1 = {
            "Yes": int(self.public_key.encrypt(1).ciphertext()),
            "No": int(self.public_key.encrypt(0).ciphertext()),
        }

        survey_response_1 = SurveyResponse.objects.create(survey=self.survey)
        query_response_1 = QueryResponse.objects.create(
            survey_response=survey_response_1, query=query, data=response_data_1
        )
        query.aggregate_query_response(
            query_response_1, self.public_key, self.private_key
        )

        response_data_1 = {
            "Yes": int(self.public_key.encrypt(11).ciphertext()),
            "No": int(self.public_key.encrypt(2).ciphertext()),
        }

        survey_response_1 = SurveyResponse.objects.create(survey=self.survey)
        query_response_1 = QueryResponse.objects.create(
            survey_response=survey_response_1, query=query, data=response_data_1
        )
        query.aggregate_query_response(
            query_response_1, self.public_key, self.private_key
        )

        self.assertEqual(query.aggregated_results, {"Yes": 12, "No": 2})

    def test_wrong_response_leads_to_value_or_key_error(self):
        query = Query.objects.create(
            survey=self.survey,
            data_point=self.data_point,
            cohorts=["Yes", "No"],
        )
        response_data_wrong_cohort_name = {"Yesaaa": 1, "No": 0}
        response_data_no_number = {"Yes": "a", "No": 0}
        response_no_dict = "aa"

        survey_response = SurveyResponse.objects.create(survey=self.survey)
        query_response = QueryResponse.objects.create(
            survey_response=survey_response,
            query=query,
            data=response_data_wrong_cohort_name,
        )

        with self.assertRaises(KeyError):
            query.aggregate_query_response(
                query_response, self.public_key, self.private_key
            )

        query_response = QueryResponse.objects.create(
            survey_response=survey_response,
            query=query,
            data=response_data_no_number,
        )

        with self.assertRaises(ValueError):
            query.aggregate_query_response(
                query_response, self.public_key, self.private_key
            )

        query_response = QueryResponse.objects.create(
            survey_response=survey_response, query=query, data=response_no_dict
        )

        with self.assertRaises(ValueError):
            query.aggregate_query_response(
                query_response, self.public_key, self.private_key
            )
