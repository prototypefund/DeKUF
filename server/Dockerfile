FROM python:3.10

RUN pip install --upgrade pip
RUN pip install poetry

WORKDIR /usr/src/app
COPY . .
RUN poetry install
