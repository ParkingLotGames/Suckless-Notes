#!/bin/sh

# Check if the operating system is MacOS
if [[ "$OSTYPE" == "darwin"* ]]; then
    # MacOS uses clang instead of gcc
    CC=clang
else
    # For Linux/Unix, use gcc
    CC=gcc
fi

$CC -o notes main.c -Wall -Wextra -pedantic