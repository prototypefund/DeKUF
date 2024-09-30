import uuid

from core.models.check_intervals import check_intervals
from core.models.commissioner import Commissioner
from core.models.data_point import DataPoint
from django.core.serializers.json import DjangoJSONEncoder
from django.db import models
from phe import paillier


class Survey(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    name = models.CharField(max_length=200)
    commissioner = models.ForeignKey(Commissioner, on_delete=models.CASCADE)
    group_size = models.IntegerField(default=2)
    group_count = models.IntegerField(default=2)
    number_participants = models.IntegerField(default=0)

    def __str__(self):
        return self.name

    def group_factor(self):
        return self.group_size * self.group_count


class Query(models.Model):
    id = models.UUIDField(primary_key=True, default=uuid.uuid4, editable=False)
    survey = models.ForeignKey(
        Survey, on_delete=models.CASCADE, related_name="queries"
    )
    data_point = models.ForeignKey(
        DataPoint, on_delete=models.deletion.DO_NOTHING
    )
    cohorts = models.JSONField(encoder=DjangoJSONEncoder, default=list)
    discrete = models.BooleanField(default=True)
    aggregated_results = models.JSONField(default=dict, editable=False)

    def __str__(self):
        return f"Query on {self.data_point.name}"

    def save(self, *args, **kwargs):
        if not self.discrete:
            check_intervals(
                self.cohorts,
                self.data_point.min_value,
                self.data_point.max_value,
            )
        # TODO: Improve comparison
        if not set(self.aggregated_results.keys()) == set(self.cohorts):
            self.aggregated_results = {
                str(cohort): 0 for cohort in self.cohorts
            }
        super().save(*args, **kwargs)

    def aggregate_query_response(
        self,
        query_response,
        public_key: paillier.PaillierPublicKey,
        private_key: paillier.PaillierPrivateKey,
    ):
        if not isinstance(query_response.data, dict):
            raise ValueError("query_response.data must be a dictionary.")
        for key, value in query_response.data.items():
            encrypted_number = paillier.EncryptedNumber(public_key, int(value))
            decrypted_number = private_key.decrypt(encrypted_number)
            self.aggregated_results[key] += decrypted_number

        self.save()
