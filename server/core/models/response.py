import uuid

from core.models.survey import Query, Survey
from django.db import models


class SurveyResponse(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    survey = models.ForeignKey(Survey, on_delete=models.CASCADE)


class QueryResponse(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    survey_response = models.ForeignKey(
        SurveyResponse, on_delete=models.CASCADE, related_name="query_responses"
    )
    query = models.ForeignKey(Query, on_delete=models.CASCADE)
    data = models.JSONField(default=dict)
