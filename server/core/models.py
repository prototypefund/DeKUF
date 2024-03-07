from django.db import models


class Commissioner(models.Model):
    name = models.CharField(max_length=200)


class Survey(models.Model):
    commissioners = models.ManyToManyField(Commissioner)


class Query(models.Model):
    survey = models.ForeignKey(
        Survey, on_delete=models.CASCADE, related_name="queries"
    )
    data_key = models.CharField(max_length=100)


class SurveyResponse(models.Model):
    commissioners = models.ManyToManyField(Commissioner)


class QueryResponse(models.Model):
    survey_response = models.ForeignKey(
        SurveyResponse, on_delete=models.CASCADE, related_name="queryResponses"
    )
    data_key = models.CharField(max_length=100)
    data = models.JSONField()
