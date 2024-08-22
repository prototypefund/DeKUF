import json
import uuid

from core.models.aggregation_group import AggregationGroup
from core.models.commissioner import Commissioner
from core.models.data_point import DataPoint, Types
from core.models.survey import Query, Survey
from core.models.survey_signup import SurveySignup
from django.test import TestCase
from django.urls import reverse
from phe import paillier


class SurveySignupTest(TestCase):
    def setUp(self):
        commissioner = Commissioner.objects.create(name="TestCommissioner")
        self.survey = Survey.objects.create(
            name="TestSurvey",
            commissioner=commissioner,
            group_size=1,
            group_count=1,
        )

    def test_post_endpoint_201_with_correct_survey(self):
        url = reverse("survey-signup")

        self.assertEqual(len(SurveySignup.objects.all()), 0)

        data = {"survey_id": str(self.survey.id), "public_key": "123"}

        response = self.client.post(
            url, content_type="application/json", data=json.dumps(data)
        )

        self.assertEqual(len(SurveySignup.objects.all()), 1)

        self.assertEqual(response.status_code, 201)

    def test_post_endpoint_404_with_nonexistent_survey(self):
        url = reverse("survey-signup")

        data = {"survey_id": str(uuid.uuid4()), "public_key": "123"}

        response = self.client.post(
            url, content_type="application/json", data=json.dumps(data)
        )

        self.assertEqual(response.status_code, 404)


class GetSignupStateTest(TestCase):

    def setUp(self):
        commissioner = Commissioner.objects.create(name="TestCommissioner")
        self.survey = Survey.objects.create(
            name="TestSurvey", commissioner=commissioner, group_size=1
        )
        self.signup = SurveySignup.objects.create(
            survey=self.survey, public_key="123"
        )
        self.signup2 = SurveySignup.objects.create(
            survey=self.survey, public_key="123"
        )

    def test_201_with_correct_client_id_no_started_aggregation(self):
        url = reverse("get-signup-state", args=[self.signup.id])

        response = self.client.get(
            url, content_type="application/x-www-form-urlencoded"
        )

        self.assertJSONEqual(
            response.content.decode("utf-8"),
            {"delegate_public_key": "", "aggregation_started": False},
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
            {
                "delegate_public_key": str(self.signup.public_key),
                "aggregation_started": True,
                "group_size": 1,
                "aggregation_public_key_n": str(
                    aggregation_group.aggregation_public_key_n
                ),
            },
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
        self.data_point = DataPoint.objects.create(
            name="test", key="test", type=Types.INTEGER.value
        )

        self.query = Query.objects.create(
            survey=self.survey,
            data_point=self.data_point,
            cohorts=["yes", "no"],
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

        public_key = paillier.PaillierPublicKey(
            n=self.aggregation_group.aggregation_public_key_n
        )

        result = {
            "survey_id": str(self.survey.id),
            "query_responses": [
                {
                    "query_id": str(self.query.id),
                    "data": {
                        "yes": int(public_key.encrypt(4).ciphertext()),
                        "no": int(public_key.encrypt(6).ciphertext()),
                    },
                }
            ],
        }

        response = self.client.post(
            url, data=json.dumps(result), content_type="application/json"
        )

        query = Query.objects.get(id=self.query.id)

        self.assertEqual(query.aggregated_results["yes"], 4)
        self.assertEqual(query.aggregated_results["no"], 6)
        self.assertEqual(response.status_code, 201)

    def test_correctly_posted_aggregation_result_deletes_group_and_signup(self):
        url = reverse("post-aggregation-result", args=[self.signup.id])

        public_key = paillier.PaillierPublicKey(
            n=self.aggregation_group.aggregation_public_key_n
        )

        result = {
            "survey_id": str(self.survey.id),
            "query_responses": [
                {
                    "query_id": str(self.query.id),
                    "data": {
                        "yes": public_key.encrypt(4).ciphertext(),
                        "no": public_key.encrypt(4).ciphertext(),
                    },
                }
            ],
        }

        self.client.post(
            url, data=json.dumps(result), content_type="application/json"
        )

        with self.assertRaises(SurveySignup.DoesNotExist):
            SurveySignup.objects.get(id=self.signup.id)

        with self.assertRaises(SurveySignup.DoesNotExist):
            SurveySignup.objects.get(id=self.signup2.id)

        with self.assertRaises(AggregationGroup.DoesNotExist):
            AggregationGroup.objects.get(id=self.aggregation_group.id)
