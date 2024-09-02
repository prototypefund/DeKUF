import random
from typing import List

from core.models.aggregation_group import AggregationGroup
from core.models.survey import Survey
from core.models.survey_signup import SurveySignup
from django.db import transaction


def group_ungrouped_signups(
    ungrouped_signups: List[SurveySignup], survey: Survey
):
    with transaction.atomic():
        ungrouped_signups = list(
            SurveySignup.objects.select_for_update().filter(
                id__in=[signup.id for signup in ungrouped_signups],
                group__isnull=True,
            )
        )

        if len(ungrouped_signups) < survey.group_factor():
            return

        random.shuffle(ungrouped_signups)

        for i in range(0, len(ungrouped_signups), survey.group_size):
            signups_group = ungrouped_signups[i : i + survey.group_size]
            if len(signups_group) != survey.group_size:
                continue

            delegate = random.choice(signups_group)
            aggregation_group = AggregationGroup.objects.create(
                survey=survey, delegate=delegate
            )

            SurveySignup.objects.filter(
                id__in=[signup.id for signup in signups_group]
            ).update(group=aggregation_group)
