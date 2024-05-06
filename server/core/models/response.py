import uuid

from django.core.serializers.json import DjangoJSONEncoder
from django.db import models

from core.models.survey import Survey, Query


class SurveyResponse(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    survey = models.ForeignKey(Survey, on_delete=models.CASCADE)


class QueryResponse(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    survey_response = models.ForeignKey(
        SurveyResponse, on_delete=models.CASCADE, related_name="query_responses"
    )
    query = models.ForeignKey(Query, on_delete=models.CASCADE)
    data_key = models.CharField(max_length=100)
    data = models.JSONField(default=dict)
