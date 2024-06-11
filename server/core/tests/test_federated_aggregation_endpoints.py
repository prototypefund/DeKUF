import json
import uuid

from core.models.aggregation_group import AggregationGroup
from core.models.commissioner import Commissioner
from core.models.survey import Query, Survey
from core.models.survey_signup import SurveySignup
from django.test import TestCase
from django.urls import reverse


class SurveySignupTest(TestCase):
    def setUp(self):
        commissioner = Commissioner.objects.create(name="TestCommissioner")
        self.survey = Survey.objects.create(
            name="TestSurvey", commissioner=commissioner
        )

    def test_post_endpoint_201_with_correct_survey(self):
        url = reverse("survey-signup", args=[self.survey.id])

        self.assertEqual(len(SurveySignup.objects.all()), 0)

        response = self.client.post(
            url, content_type="application/x-www-form-urlencoded"
        )

        self.assertEqual(len(SurveySignup.objects.all()), 1)

        self.assertEqual(response.status_code, 201)

    def test_post_endpoint_404_with_nonexistent_survey(self):
        url = reverse("survey-signup", args=[uuid.uuid4()])

        response = self.client.post(
            url, content_type="application/x-www-form-urlencoded"
        )

        self.assertEqual(response.status_code, 404)


class GetSignupStateTest(TestCase):

    def setUp(self):
        commissioner = Commissioner.objects.create(name="TestCommissioner")
        self.survey = Survey.objects.create(
            name="TestSurvey", commissioner=commissioner
        )
        self.signup = SurveySignup.objects.create(survey=self.survey)
        self.signup2 = SurveySignup.objects.create(survey=self.survey)

    def test_201_with_correct_client_id_no_started_aggregation(self):
        url = reverse("get-signup-state", args=[self.signup.id])

        response = self.client.get(
            url, content_type="application/x-www-form-urlencoded"
        )

        self.assertJSONEqual(
            response.content.decode("utf-8"),
            {"delegate_id": "", "aggregation_started": False},
        )

        self.assertEqual(response.status_code, 200)

    def test_201_with_correct_client_id(self):
        aggregation_group = AggregationGroup.objects.create(
            survey=self.survey, delegate=self.signup
        )
        self.signup.group = aggregation_group

        self.signup.save()
        url = reverse("get-signup-state", args=[self.signup.id])

        response = self.client.get(
            url, content_type="application/x-www-form-urlencoded"
        )

        self.assertJSONEqual(
            response.content.decode("utf-8"),
            {"delegate_id": str(self.signup.id), "aggregation_started": True},
        )

        self.assertEqual(response.status_code, 200)

    def test_404_with_incorrect_client_id(self):
        url = reverse("get-signup-state", args=[uuid.uuid4()])

        response = self.client.get(
            url, content_type="application/x-www-form-urlencoded"
        )

        self.assertEqual(response.status_code, 404)


class ResultPostingTest(TestCase):
    def setUp(self):
        commissioner = Commissioner.objects.create(name="TestCommissioner")
        self.survey = Survey.objects.create(
            name="TestSurvey", commissioner=commissioner
        )
        self.query = Query.objects.create(
            survey=self.survey, data_key="testKey", cohorts=["yes", "no"]
        )
        self.survey.queries.add(self.query)
        self.signup = SurveySignup.objects.create(survey=self.survey)
        self.signup2 = SurveySignup.objects.create(survey=self.survey)
        self.aggregation_group = AggregationGroup.objects.create(
            survey=self.survey, delegate=self.signup
        )
        self.signup.group = self.aggregation_group
        self.signup.save()
        self.signup2.group = self.aggregation_group
        self.signup2.save()

    def test_correctly_posted_aggregation_result_is_aggregated(self):

        url = reverse("post-aggregation-result", args=[self.signup.id])

        result = {
            "survey_id": str(self.survey.id),
            "query_responses": [
                {"query_id": str(self.query.id), "data": {"yes": 4, "no": 6}}
            ],
        }

        response = self.client.post(
            url, data=json.dumps(result), content_type="application/json"
        )

        query = Query.objects.get(id=self.query.id)

        self.assertEqual(query.aggregated_results["yes"], 4)
        self.assertEqual(query.aggregated_results["no"], 6)
        self.assertEqual(response.status_code, 201)
