import uuid

from core.models.check_intervals import check_intervals
from core.models.commissioner import Commissioner
from django.core.serializers.json import DjangoJSONEncoder
from django.db import models


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
    cohorts = models.JSONField(encoder=DjangoJSONEncoder, default=list)
    discrete = models.BooleanField(default=True)
    number_participants = models.IntegerField(default=0, editable=False)
    aggregated_results = models.JSONField(default=dict, editable=False)

    def __str__(self):
        return f"Query on {self.data_key}"

    def save(self, *args, **kwargs):
        if not self.discrete:
            check_intervals(self.cohorts)
        # TODO: Improve comparison
        if not set(self.aggregated_results.keys()) == set(self.cohorts):
            self.aggregated_results = {
                str(cohort): 0 for cohort in self.cohorts
            }
        super().save(*args, **kwargs)

    def aggregate_query_response(self, query_response):
        if isinstance(query_response.data, dict):
            for key, value in query_response.data.items():
                if isinstance(value, str):
                    raise ValueError("The value should not be a string")
                self.aggregated_results[key] += value
        else:
            raise ValueError("query_response.data must be a dictionary.")
        self.number_participants += 1
        self.save()
