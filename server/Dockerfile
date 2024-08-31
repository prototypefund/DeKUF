FROM python:3.10

RUN pip install --upgrade pip
RUN pip install poetry

WORKDIR /usr/src/app
COPY . .
RUN poetry install
RUN rm -f db.sqlite3
RUN poetry run ./manage.py migrate
RUN <<EOF
poetry run ./manage.py shell -c '
from django.contrib.auth.models import User
User.objects.create_superuser("admin", "", "dekuf")
'
EOF
