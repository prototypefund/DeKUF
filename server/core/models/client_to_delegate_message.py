import uuid

from core.models.aggregation_group import AggregationGroup
from django.db import models


class ClientToDelegateMessage(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    delegate_id = models.UUIDField(editable=False)
    group = models.ForeignKey(
        AggregationGroup, on_delete=models.DO_NOTHING, editable=False
    )
    content = models.TextField(editable=False)

    objects = models.Manager()
