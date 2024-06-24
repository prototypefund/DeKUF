import random
from typing import List

from core.models.aggregation_group import AggregationGroup
from core.models.survey import Survey
from core.models.survey_signup import SurveySignup


def group_ungrouped_signups(
    ungrouped_signups: List[SurveySignup], survey: Survey
):
    ungrouped_signups_list = list(ungrouped_signups)
    if len(ungrouped_signups_list) < survey.group_factor():
        return
    random.shuffle(ungrouped_signups_list)
    for i in range(0, len(ungrouped_signups_list), survey.group_size):
        signups_group = ungrouped_signups_list[i : i + survey.group_size]
        if not len(signups_group) == survey.group_size:
            continue

        aggregation_group = AggregationGroup.objects.create(
            survey=survey, delegate=random.choice(signups_group)
        )
        for signup in signups_group:
            signup.group = aggregation_group
            signup.save()
