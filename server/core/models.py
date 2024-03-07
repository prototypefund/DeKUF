from django.db import models


class JSONData(models.Model):
    data = models.JSONField()
