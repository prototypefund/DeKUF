import uuid

from core.models.survey import Survey
from django.db import models


class AggregationGroup(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    survey = models.ForeignKey(Survey, on_delete=models.DO_NOTHING)
    delegate = models.ForeignKey(
        "core.SurveySignup", on_delete=models.DO_NOTHING
    )

    objects = models.Manager()
