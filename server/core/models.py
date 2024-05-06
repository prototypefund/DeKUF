import uuid

from django.core.serializers.json import DjangoJSONEncoder
from django.db import models


class Commissioner(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    name = models.CharField(max_length=200)

    def __str__(self):
        return self.name


class Survey(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    name = models.CharField(max_length=200)
    commissioner = models.ForeignKey(Commissioner, on_delete=models.CASCADE)

    def __str__(self):
        return self.name


class Query(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    survey = models.ForeignKey(
        Survey, on_delete=models.CASCADE, related_name="queries"
    )
    data_key = models.CharField(max_length=100)
    '''
    cohorts can be discrete eg. ["Yes", "No", "Unclear"] resulting in an answer
    that has length 3 [1, 0 ,0] for yes
    or can be continuous [0, 10, 20] expecting an answer that has length 2 for
    0<=x<10 and 10<=x<20
    '''
    cohorts = models.JSONField(encoder=DjangoJSONEncoder, default=list)
    discrete = models.BooleanField(default=True)

    def __str__(self):
        return f"Query on {self.data_key}"


class SurveyResponse(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    survey = models.ForeignKey(Survey, on_delete=models.CASCADE)


class QueryResponse(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    survey_response = models.ForeignKey(
        SurveyResponse, on_delete=models.CASCADE, related_name="query_responses"
    )
    data_key = models.CharField(max_length=100)
    data = models.JSONField()
