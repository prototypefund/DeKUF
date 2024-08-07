import json

from core.models.aggregation_group import AggregationGroup
from core.models.client_to_delegate_message import ClientToDelegateMessage
from core.models.commissioner import Commissioner
from core.models.survey import Survey
from core.models.survey_signup import SurveySignup
from django.test import TestCase
from django.urls import reverse


class MessagingToDelegateTest(TestCase):
    def setUp(self):
        commissioner = Commissioner.objects.create(name="TestCommissioner")
        self.survey = Survey.objects.create(
            name="TestSurvey", commissioner=commissioner
        )
        self.fakePublicKey = "thisCouldBeAKeyForWhatIKnow"
        self.signup = SurveySignup.objects.create(
            survey=self.survey, public_key=self.fakePublicKey
        )
        self.signup2 = SurveySignup.objects.create(survey=self.survey)
        self.aggregation_group = AggregationGroup.objects.create(
            survey=self.survey, delegate=self.signup
        )
        self.signup.group = self.aggregation_group
        self.signup.save()
        self.signup2.group = self.aggregation_group
        self.signup2.save()

    def test_messages_to_delegate_are_stored_correctly(self):
        if not self.aggregation_group.delegate:
            raise AttributeError("Delegate can't be null")

        url = reverse("message-to-delegate")

        response = self.client.post(
            url,
            json.dumps(
                {
                    "public_key": self.fakePublicKey,
                    "message": "secureencryptedmessagetrustme",
                }
            ),
            content_type="application/json",
        )

        self.assertEqual(len(ClientToDelegateMessage.objects.all()), 1)
        self.assertEqual(response.status_code, 201)

    def test_messages_to_delegate_fails_with_missing_required_fields(
        self,
    ) -> None:
        if not self.aggregation_group.delegate:
            raise AttributeError("Delegate can't be null")

        url = reverse("message-to-delegate")

        response = self.client.post(
            url,
            json.dumps({"message": "secureencryptedmessagetrustme"}),
            content_type="application/json",
        )

        self.assertEqual(response.status_code, 400)

        response = self.client.post(
            url,
            json.dumps({"public_key": self.fakePublicKey}),
            content_type="application/json",
        )

        self.assertEqual(response.status_code, 400)

    def test_retrieving_for_messageless_delegate_gives_204(self):
        if not self.aggregation_group.delegate:
            raise AttributeError("Delegate can't be null")

        url = reverse(
            "get-messages-for-delegate",
            args=[self.aggregation_group.delegate.id],
        )

        response = self.client.get(
            url, content_type="application/x-www-form-urlencoded"
        )

        self.assertEqual(response.status_code, 204)

    def test_messages_to_delegate_can_be_retrieved_correctly(self):
        if not self.aggregation_group.delegate:
            raise AttributeError("Delegate can't be null")

        post_url = reverse("message-to-delegate")

        post_response = self.client.post(
            post_url,
            json.dumps(
                {
                    "public_key": self.fakePublicKey,
                    "message": "secureencryptedmessagetrustme",
                }
            ),
            content_type="application/json",
        )
        self.assertEqual(post_response.status_code, 201)
        self.assertEqual(ClientToDelegateMessage.objects.count(), 1)

        get_url = reverse("get-messages-for-delegate", args=[self.signup.id])
        get_response = self.client.get(get_url, content_type="application/json")
        self.assertEqual(get_response.status_code, 200)

        response_content = get_response.content.decode("utf-8")
        self.assertJSONEqual(
            response_content, '{"messages": ["secureencryptedmessagetrustme"]}'
        )

        post_response = self.client.post(
            post_url,
            json.dumps(
                {
                    "public_key": self.fakePublicKey,
                    "message": "secureencryptedmessagetrustmeagain",
                }
            ),
            content_type="application/json",
        )
        self.assertEqual(post_response.status_code, 201)
        self.assertEqual(ClientToDelegateMessage.objects.count(), 2)

        get_response = self.client.get(get_url, content_type="application/json")
        self.assertEqual(get_response.status_code, 200)

        response_content = get_response.content.decode("utf-8")
        expected_response = (
            '{"messages": ["secureencryptedmessagetrustme"'
            + ',"secureencryptedmessagetrustmeagain"]}'
        )
        self.assertJSONEqual(response_content, expected_response)
