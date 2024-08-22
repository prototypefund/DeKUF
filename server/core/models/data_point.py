from enum import Enum

from django.db import models


class Types(Enum):
    INTEGER = 1
    DECIMAL = 2
    # Not yet supported
    # STRING = 3


class DataPoint(models.Model):
    name = models.CharField(max_length=200, unique=True)
    key = models.CharField(max_length=50, unique=True)
    type = models.IntegerField(choices=[(t.value, t.name) for t in Types])
    max_value = models.FloatField(null=True, blank=True)
    min_value = models.FloatField(null=True, blank=True)
    precision = models.IntegerField(null=True, blank=True)

    def __str__(self):
        return f"{self.name} ({self.get_type_display()})"

    def to_data_key(self):
        return self.key

    @classmethod
    def from_data_key(cls, data_key):
        return cls.objects.get(key=data_key)
