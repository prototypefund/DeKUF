import uuid

from core.models.aggregation_group import AggregationGroup
from core.models.survey import Survey
from django.db import models


class SurveySignup(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    public_key = models.TextField(editable=False)
    survey = models.ForeignKey(
        Survey, on_delete=models.DO_NOTHING, editable=False
    )
    time = models.DateTimeField(auto_now_add=True, editable=False)
    group = models.ForeignKey(
        AggregationGroup, on_delete=models.SET_NULL, null=True
    )
