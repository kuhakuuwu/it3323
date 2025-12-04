#! /bin/bash

# Ensure parser binary exists in ../incompleted and is executable
PARSER="../completed/parser"
if [ ! -x "$PARSER" ]; then
    echo "Error: $PARSER not found or not executable. Build it first (run 'make' in ../completed)."
    exit 2
fi

# Run parser on each example
# Strip CR characters before comparing to handle Windows/Unix line ending differences
# Script designed to be run from ../incompleted directory
TESTDIR="$(dirname "$0")"
"$PARSER" "$TESTDIR/example1.kpl" | sed 's/\r$//' | diff <(sed 's/\r$//' "$TESTDIR/result1.txt") -
"$PARSER" "$TESTDIR/example2.kpl" | sed 's/\r$//' | diff <(sed 's/\r$//' "$TESTDIR/result2.txt") -
"$PARSER" "$TESTDIR/example3.kpl" | sed 's/\r$//' | diff <(sed 's/\r$//' "$TESTDIR/result3.txt") -
"$PARSER" "$TESTDIR/example4.kpl" | sed 's/\r$//' | diff <(sed 's/\r$//' "$TESTDIR/result4.txt") -
"$PARSER" "$TESTDIR/example5.kpl" | sed 's/\r$//' | diff <(sed 's/\r$//' "$TESTDIR/result5.txt") -