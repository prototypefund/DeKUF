from django.core.management import call_command
from django.db import migrations


def load_fixture(apps, schema_editor):
    call_command('loaddata', 'data_points', app_label='core')


class Migration(migrations.Migration):
    dependencies = [
        ('core', '0012_datapoint_remove_query_data_key_query_data_point'),
    ]

    operations = [
        migrations.RunPython(load_fixture),
    ]
