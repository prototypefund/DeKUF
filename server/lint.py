#!/usr/bin/env python3

import os
import sys


def call(args):
    status = os.system(" ".join(args))
    if status != 0:
        sys.exit(1)


if __name__ == "__main__":
    fix = len(sys.argv) > 1 and sys.argv[1] == "--fix"
    extra_args = [] if fix else ["--check"]
    call(["isort", "-l", "80", "--profile", "black", "."] + extra_args)
    call(["black", "-l", "80", "-C", "."] + extra_args)
    call(["mypy", ".", "--check"])
    call(["flake8", "."])
