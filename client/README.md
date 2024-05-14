# DeKUF - Client

The client-side daemon for DeKUF.

It has two functionalities:

1. Continuously gathering personal data (at first only system health data).
2. Responding to federated surveys (polled from [../server](../server)).

## Status

Heavily work in progress, the current goal is to get it to a proof of concept
stage.

## Development

### Requirements

For the moment, you'll need to manually install a few dependencies:

1. A C++ toolchain (e.g. GCC, LLVM or MSVC) - note that at the time of writing,
   you will have to use Clang for the linting to work (see below).
2. [CMake](https://cmake.org/)
3. The [Qt framework](https://www.qt.io/product/qt6), version 6.2.4 (or later,
   presumably)
4. [Qt Designer](https://doc.qt.io/qt-6/qtdesigner-manual.html)
5. [clang-format](https://clang.llvm.org/docs/ClangFormat.html)

#### Debian/Ubuntu

    sudo apt install build-essential clang cmake qt6-base-dev clang-format clang-tidy designer-qt6
    echo 'export CXX=/usr/bin/clang++' >> ~/.profile
    . ~/.profile

### Building

    make

### Running the tests

    make && make test

### Linting

We use linting to improve maintainability and to reduce nit picking, please run
it before committing any changes:

    make lint

To comply with the formatting rules, run:

    make format

### Running the daemon

    make run-daemon

### Running the UI

    make run-ui

### Submitting data

The daemon receives data from suppliers via D-Bus. See
[example_supplier](example_supplier) for an example supplier you can use to
submit arbitrary data.
