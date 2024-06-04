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
        self.signup = SurveySignup.objects.create(survey=self.survey)
        self.signup2 = SurveySignup.objects.create(survey=self.survey)
        self.aggregation_group = AggregationGroup.objects.create(
            survey=self.survey, delegate=self.signup
        )
        self.signup.group = self.aggregation_group
        self.signup.save()
        self.signup2.group = self.aggregation_group
        self.signup2.save()

    def test_messages_to_delegate_are_stored_correctly(self):
        url = reverse(
            "message-to-delegate", args=[self.aggregation_group.delegate.id]
        )

        response = self.client.post(
            url,
            json.dumps({"testKey": "testValue"}),
            content_type="application/x-www-form-urlencoded",
        )

        self.assertEqual(len(ClientToDelegateMessage.objects.all()), 1)
        self.assertEqual(response.status_code, 201)

    def test_messages_to_delegate_fails_with_incorrect_message_types(
        self,
    ) -> None:
        url = reverse(
            "message-to-delegate", args=[self.aggregation_group.delegate.id]
        )

        response = self.client.post(
            url, 1, content_type="application/x-www-form-urlencoded"
        )

        self.assertEqual(response.status_code, 400)

        response = self.client.post(
            url,
            json.dumps([{"testKey": "testValue"}]),
            content_type="application/x-www-form-urlencoded",
        )

        self.assertEqual(response.status_code, 400)

    def test_retrieving_for_messageless_delegate_fails(self):
        url = reverse(
            "get-messages-for-delegate",
            args=[self.aggregation_group.delegate.id],
        )

        response = self.client.get(
            url, content_type="application/x-www-form-urlencoded"
        )

        self.assertEqual(response.status_code, 400)

    def test_messages_to_delegate_can_be_retrieved_correctly(self):
        post_url = reverse(
            "message-to-delegate", args=[self.aggregation_group.delegate.id]
        )

        post_response = self.client.post(
            post_url,
            json.dumps({"testKey": "testValue"}),
            content_type="application/x-www-form-urlencoded",
        )

        self.assertEqual(len(ClientToDelegateMessage.objects.all()), 1)
        self.assertEqual(post_response.status_code, 201)

        get_url = reverse(
            "get-messages-for-delegate",
            args=[self.aggregation_group.delegate.id],
        )

        get_response = self.client.get(
            get_url, content_type="application/x-www-form-urlencoded"
        )

        response_content = get_response.content.decode("utf-8")

        self.assertJSONEqual(
            response_content, '{"messages": [{"testKey": "testValue"}]}'
        )

        post_response = self.client.post(
            post_url,
            json.dumps({"testKey2": "testValue2"}),
            content_type="application/x-www-form-urlencoded",
        )

        self.assertEqual(len(ClientToDelegateMessage.objects.all()), 2)
        self.assertEqual(post_response.status_code, 201)

        get_url = reverse(
            "get-messages-for-delegate",
            args=[self.aggregation_group.delegate.id],
        )

        get_response = self.client.get(
            get_url, content_type="application/x-www-form-urlencoded"
        )

        response_content = get_response.content.decode("utf-8")

        self.assertJSONEqual(
            response_content,
            '{"messages": '
            '[{"testKey": "testValue"}, {"testKey2": "testValue2"}]}',
        )
