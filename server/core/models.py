from typing import Any, Dict

from django.db import models


class JsonData(models.Model):
    data: Dict[str, Any] = models.JSONField()
