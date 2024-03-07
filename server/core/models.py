from typing import Any, Dict

from django.db import models


class JsonData(models.Model):
    data: Dict[str, Any] = models.JSONField()


class Survey(models.Model):
    issuer: str = models.CharField(max_length=100)


class Query(models.Model):
    survey: Survey = models.ForeignKey(
        Survey, on_delete=models.CASCADE, related_name="queries"
    )
    dataKey: str = models.CharField()
