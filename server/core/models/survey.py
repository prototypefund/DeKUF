import uuid

from django.apps import apps
from django.core.serializers.json import DjangoJSONEncoder
from django.db import models

from core.models.commissioner import Commissioner



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
    cohorts can be discrete eg. ["Yes", "No", "Unclear"] or can be continuous
    by adding a "-" to indicate a range (last number is not included) eg.
    ["0-10", "10-20"] resulting in 0<=x<10 , 10<=x<20
    '''
    cohorts = models.JSONField(encoder=DjangoJSONEncoder, default=list)
    discrete = models.BooleanField(default=True)
    aggregated_results = models.JSONField(default=dict)

    def __str__(self):
        return f"Query on {self.data_key}"

    def save(self, *args, **kwargs):
        if not self.aggregated_results:
            self.aggregated_results = {str(cohort): 0 for cohort in self.cohorts}
        super().save(*args, **kwargs)

    def aggregate_query_response(self, query_response):
        if isinstance(query_response.data, dict):
            for key, value in query_response.data.items():
                self.aggregated_results[key] += value
        else:
            raise ValueError("query_response.data must be a dictionary.")

        self.save()
