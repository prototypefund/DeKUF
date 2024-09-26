# Example supplier for the Privact client

## Requirements

- Python 3.10
- Poetry >= 0.12
- Qt (see [../README.md](../README.md))

## Installing poetry dependencies

    poetry install

## Running the example supplier

First, make sure the daemon is running (see [../README.md](../README.md), then:

    poetry run ./submit_data_point.py KEY VALUE

Where `KEY` and `VALUE` are the actual key and value of the data point you want
to submit.

## Formatting the code

    poetry run black .
