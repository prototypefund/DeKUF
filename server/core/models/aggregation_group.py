import uuid

from core.models.survey import Survey
from django.db import models
from phe import paillier


class AggregationGroup(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    survey = models.ForeignKey(Survey, on_delete=models.DO_NOTHING)
    delegate = models.ForeignKey(
        "core.SurveySignup", on_delete=models.SET_NULL, null=True
    )
    aggregation_public_key_n = models.CharField(max_length=255, editable=False)
    aggregation_public_key_g = models.CharField(max_length=255, editable=False)
    aggregation_private_key_p = models.CharField(max_length=255, editable=False)
    aggregation_private_key_q = models.CharField(max_length=255, editable=False)

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        if not self.aggregation_public_key_n:
            public_key, private_key = paillier.generate_paillier_keypair()
            self.aggregation_public_key_n = public_key.n
            self.aggregation_public_key_g = public_key.g
            self.aggregation_private_key_p = private_key.p
            self.aggregation_private_key_q = private_key.q

    objects = models.Manager()
