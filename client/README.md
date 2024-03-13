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

1. A C++ toolchain (e.g. GCC, LLVM or MSVC)
2. [CMake](https://cmake.org/)
3. The [Qt framework](https://www.qt.io/product/qt6), version 6.2.4 (or later,
   presumably)
4. [clang-format](https://clang.llvm.org/docs/ClangFormat.html)

#### Debian/Ubuntu

    sudo apt install build-essential cmake qt6-base-dev clang-format

### Building

    mkdir build
    cd build
    cmake ..
    make

### Running the tests

    cd build
    make && make test

### Linting

We use linting to improve maintainability and to reduce nit picking, please run
it before committing any changes:

    ./lint

It can fix issues directly, should save you some time:

    ./lint --fix
