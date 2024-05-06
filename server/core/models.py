from django.core.serializers.json import DjangoJSONEncoder
from django.db import models


class Commissioner(models.Model):
    name = models.CharField(max_length=200)

    def __str__(self):
        return self.name


class Survey(models.Model):
    name = models.CharField(max_length=200)
    commissioners = models.ManyToManyField(Commissioner)

    def __str__(self):
        return self.name


class Query(models.Model):
    survey = models.ForeignKey(
        Survey, on_delete=models.CASCADE, related_name="queries"
    )
    data_key = models.CharField(max_length=100)
    cohorts = models.JSONField(encoder=DjangoJSONEncoder, default=list)

    @property
    def discrete(self):
        return all(isinstance(item, str) for item in self.cohorts)

    def __str__(self):
        return f"Query on {self.data_key}"


class SurveyResponse(models.Model):
    commissioners = models.ManyToManyField(Commissioner)


class QueryResponse(models.Model):
    survey_response = models.ForeignKey(
        SurveyResponse, on_delete=models.CASCADE, related_name="queryResponses"
    )
    data_key = models.CharField(max_length=100)
    data = models.JSONField()
