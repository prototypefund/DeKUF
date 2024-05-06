# Generated by Django 4.2.11 on 2024-05-06 14:53

import django.core.serializers.json
from django.db import migrations, models
import django.db.models.deletion
import uuid


class Migration(migrations.Migration):

    initial = True

    dependencies = []

    operations = [
        migrations.CreateModel(
            name="Commissioner",
            fields=[
                (
                    "id",
                    models.UUIDField(
                        default=uuid.uuid4,
                        editable=False,
                        primary_key=True,
                        serialize=False,
                    ),
                ),
                ("name", models.CharField(max_length=200)),
            ],
        ),
        migrations.CreateModel(
            name="Query",
            fields=[
                (
                    "id",
                    models.UUIDField(
                        default=uuid.uuid4,
                        editable=False,
                        primary_key=True,
                        serialize=False,
                    ),
                ),
                ("data_key", models.CharField(max_length=100)),
                (
                    "cohorts",
                    models.JSONField(
                        default=list,
                        encoder=django.core.serializers.json.DjangoJSONEncoder,
                    ),
                ),
                ("discrete", models.BooleanField(default=True)),
                ("aggregated_results", models.JSONField(default=dict)),
            ],
        ),
        migrations.CreateModel(
            name="Survey",
            fields=[
                (
                    "id",
                    models.UUIDField(
                        default=uuid.uuid4,
                        editable=False,
                        primary_key=True,
                        serialize=False,
                    ),
                ),
                ("name", models.CharField(max_length=200)),
                (
                    "commissioner",
                    models.ForeignKey(
                        on_delete=django.db.models.deletion.CASCADE,
                        to="core.commissioner",
                    ),
                ),
            ],
        ),
        migrations.CreateModel(
            name="SurveyResponse",
            fields=[
                (
                    "id",
                    models.UUIDField(
                        default=uuid.uuid4,
                        editable=False,
                        primary_key=True,
                        serialize=False,
                    ),
                ),
                (
                    "survey",
                    models.ForeignKey(
                        on_delete=django.db.models.deletion.CASCADE, to="core.survey"
                    ),
                ),
            ],
        ),
        migrations.CreateModel(
            name="QueryResponse",
            fields=[
                (
                    "id",
                    models.UUIDField(
                        default=uuid.uuid4,
                        editable=False,
                        primary_key=True,
                        serialize=False,
                    ),
                ),
                ("data", models.JSONField(default=dict)),
                (
                    "query",
                    models.ForeignKey(
                        on_delete=django.db.models.deletion.CASCADE, to="core.query"
                    ),
                ),
                (
                    "survey_response",
                    models.ForeignKey(
                        on_delete=django.db.models.deletion.CASCADE,
                        related_name="query_responses",
                        to="core.surveyresponse",
                    ),
                ),
            ],
        ),
        migrations.AddField(
            model_name="query",
            name="survey",
            field=models.ForeignKey(
                on_delete=django.db.models.deletion.CASCADE,
                related_name="queries",
                to="core.survey",
            ),
        ),
    ]
