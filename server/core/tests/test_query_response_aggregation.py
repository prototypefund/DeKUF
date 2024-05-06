from core.models.commissioner import Commissioner
from core.models.response import QueryResponse, SurveyResponse
from core.models.survey import Query, Survey
from django.test import TestCase


class QueryResponseAggregationTestCase(TestCase):
    def setUp(self):
        self.commissioner = Commissioner.objects.create(name="test")
        self.survey = Survey.objects.create(
            name="Customer Feedback", commissioner=self.commissioner
        )

    def test_correct_aggregation_with_one_query_response(self):
        query = Query.objects.create(
            survey=self.survey, data_key="test", cohorts=["Yes", "No"]
        )
        response_data = {"Yes": 1, "No": 0}

        survey_response = SurveyResponse.objects.create(survey=self.survey)
        query_response = QueryResponse.objects.create(
            survey_response=survey_response, query=query, data=response_data
        )
        query.aggregate_query_response(query_response)

        self.assertEqual(query.aggregated_results, response_data)
        self.assertEqual(query.number_participants, 1)

    def test_correct_aggregation_with_multiple_query_responses(self):
        query = Query.objects.create(
            survey=self.survey, data_key="test", cohorts=["Yes", "No"]
        )
        response_data_1 = {"Yes": 1, "No": 0}

        survey_response_1 = SurveyResponse.objects.create(survey=self.survey)
        query_response_1 = QueryResponse.objects.create(
            survey_response=survey_response_1, query=query, data=response_data_1
        )
        query.aggregate_query_response(query_response_1)

        response_data_1 = {"Yes": 11, "No": 2}

        survey_response_1 = SurveyResponse.objects.create(survey=self.survey)
        query_response_1 = QueryResponse.objects.create(
            survey_response=survey_response_1, query=query, data=response_data_1
        )
        query.aggregate_query_response(query_response_1)

        self.assertEqual(query.aggregated_results, {"Yes": 12, "No": 2})
        self.assertEqual(query.number_participants, 2)

    def test_wrong_response_leads_to_value_or_key_error(self):
        query = Query.objects.create(
            survey=self.survey, data_key="test", cohorts=["Yes", "No"]
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
            query.aggregate_query_response(query_response)

        query_response = QueryResponse.objects.create(
            survey_response=survey_response,
            query=query,
            data=response_data_no_number,
        )

        with self.assertRaises(ValueError):
            query.aggregate_query_response(query_response)

        query_response = QueryResponse.objects.create(
            survey_response=survey_response, query=query, data=response_no_dict
        )

        with self.assertRaises(ValueError):
            query.aggregate_query_response(query_response)
