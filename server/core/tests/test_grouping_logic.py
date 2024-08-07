from typing import Optional, Union
from uuid import UUID

from core.models.aggregation_group import AggregationGroup
from core.models.commissioner import Commissioner
from core.models.grouping_logic import group_ungrouped_signups
from core.models.survey import Survey
from core.models.survey_signup import SurveySignup
from django.test import TestCase


class UngroupedSignupsGroupingTest(TestCase):
    def setUp(self):
        self.commissioner = Commissioner.objects.create(name="TestCommissioner")
        self.survey = Survey.objects.create(
            name="TestSurvey",
            commissioner=self.commissioner,
            group_size=2,
            group_count=2,
        )

    def test_no_grouping_when_size_and_number_not_met(self):
        survey = Survey.objects.create(
            name="TestSurvey",
            commissioner=self.commissioner,
            group_size=10,
            group_count=2,
        )
        for i in range(5):
            SurveySignup.objects.create(survey=survey)

        group_ungrouped_signups(list(SurveySignup.objects.all()), survey)

        self.assertEqual(SurveySignup.objects.count(), 5)
        self.assertEqual(
            SurveySignup.objects.filter(group__isnull=True).count(), 5
        )

    def test_grouping_works_for_multiple_groups(self) -> None:
        for i in range(4):
            SurveySignup.objects.create(survey=self.survey)

            group_ungrouped_signups(
                list(SurveySignup.objects.all()), self.survey
            )

        self.assertEqual(
            SurveySignup.objects.filter(group__isnull=False).count(), 4
        )

        signup: Optional[SurveySignup] = SurveySignup.objects.first()
        self.assertIsNotNone(signup)

        if signup:
            group: Optional[Union[AggregationGroup, UUID]] = signup.group
            self.assertIsNotNone(group)
            self.assertEqual(
                SurveySignup.objects.filter(group=group).count(), 2
            )

    def test_grouping_works_for_multiple_groups_with_delegate(self) -> None:
        for i in range(4):
            SurveySignup.objects.create(survey=self.survey)

        group_ungrouped_signups(list(SurveySignup.objects.all()), self.survey)

        self.assertEqual(
            AggregationGroup.objects.filter(delegate__isnull=False).count(), 2
        )

        aggregation_group: Optional[AggregationGroup] = (
            AggregationGroup.objects.first()
        )
        self.assertIsNotNone(aggregation_group)

        if aggregation_group:
            signups_with_group = SurveySignup.objects.filter(
                group=aggregation_group
            )
            self.assertTrue(
                hasattr(aggregation_group, "delegate")
                and aggregation_group.delegate
            )
            if aggregation_group.delegate:
                self.assertIn(aggregation_group.delegate, signups_with_group)

    def test_grouping_generates_paillier_keys(self):
        for i in range(4):
            SurveySignup.objects.create(survey=self.survey)

        group_ungrouped_signups(list(SurveySignup.objects.all()), self.survey)

        aggregation_group: Optional[AggregationGroup] = (
            AggregationGroup.objects.first()
        )

        if not aggregation_group:
            self.fail()

        self.assertIsNotNone(aggregation_group.aggregation_public_key_n)
        self.assertIsNotNone(aggregation_group.aggregation_private_key_q)
        self.assertIsNotNone(aggregation_group.aggregation_private_key_p)
