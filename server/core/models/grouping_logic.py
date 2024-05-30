import random
from typing import List

from core.models.aggregation_group import AggregationGroup
from core.models.survey import Survey
from core.models.survey_signup import SurveySignup

GROUP_COUNT = 1
GROUP_SIZE = 1
GROUP_FACTOR = GROUP_SIZE * GROUP_COUNT


def group_ungrouped_signups(
    ungrouped_signups: List[SurveySignup], survey: Survey
):
    ungrouped_signups_list = list(ungrouped_signups)
    if len(ungrouped_signups_list) < GROUP_FACTOR:
        return
    random.shuffle(ungrouped_signups_list)
    for i in range(0, len(ungrouped_signups_list), GROUP_SIZE):
        signups_group = ungrouped_signups_list[i: i + GROUP_SIZE]
        if not len(signups_group) == GROUP_SIZE:
            continue

        aggregation_group = AggregationGroup.objects.create(
            survey=survey, delegate=random.choice(signups_group)
        )
        for signup in signups_group:
            signup.group = aggregation_group
            signup.save()
