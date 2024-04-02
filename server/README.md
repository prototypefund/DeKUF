# DeKUF - Server

The backend for DeKUF.

It mainly presents surveys and accepts responses from [../client](../client).

## Status

Heavily work in progress, the current goal is to get it to a proof of concept
stage.

## Development

### Requirements

Just Python 3.10.

### Creating the virtual environment

    python3 -m venv venv

### Activating the virtual environment

    . venv/bin/activate

### Installing the dependencies

    pip install -r requirements.txt

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
