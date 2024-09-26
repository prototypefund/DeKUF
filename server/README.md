# Privact - Server

The backend for Privact.

It mainly presents surveys and accepts responses from [../client](../client).

## Status

Heavily work in progress, the current goal is to get it to a proof of concept
stage.

## Development

### Requirements

Just Python 3.10 and Poetry >= 0.12 for dependency management

### Installing poetry dependencies

    poetry install

### Activate the poetry virtual env

    poetry shell

### Migrating the database

    ./manage.py migrate

### Running the tests

    ./manage.py test

### Linting

We use linting to improve maintainability and to reduce nit picking, please run
it before committing any changes:

    ./lint.py

It can fix issues directly, should save you some time:

    ./lint.py --fix
