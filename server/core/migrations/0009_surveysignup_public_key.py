# Generated by Django 4.2.13 on 2024-07-17 12:10

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ("core", "0008_survey_group_count_survey_group_size"),
    ]

    operations = [
        migrations.AddField(
            model_name="surveysignup",
            name="public_key",
            field=models.TextField(default="", editable=False),
            preserve_default=False,
        ),
    ]
