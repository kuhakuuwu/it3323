#! /bin/bash

# Ensure parser binary exists in ../incompleted and is executable
kplc="../incompleted/kplc"
if [ ! -x "$kplc" ]; then
    echo "Error: $kplc not found or not executable. Build it first (run 'make' in ../completed)."
    exit 2
fi

# Run parser on each example
# Strip CR characters before comparing to handle Windows/Unix line ending differences
# Script designed to be run from ../incompleted directory
TESTDIR="$(dirname "$0")"
"$kplc" "$TESTDIR/example1.kpl" | sed 's/\r$//' | diff <(sed 's/\r$//' "$TESTDIR/result1.txt") -
"$kplc" "$TESTDIR/example2.kpl" | sed 's/\r$//' | diff <(sed 's/\r$//' "$TESTDIR/result2.txt") -
"$kplc" "$TESTDIR/example3.kpl" | sed 's/\r$//' | diff <(sed 's/\r$//' "$TESTDIR/result3.txt") -
"$kplc" "$TESTDIR/example4.kpl" | sed 's/\r$//' | diff <(sed 's/\r$//' "$TESTDIR/result4.txt") -
"$kplc" "$TESTDIR/example5.kpl" | sed 's/\r$//' | diff <(sed 's/\r$//' "$TESTDIR/result5.txt") -
"$kplc" "$TESTDIR/example6.kpl" | sed 's/\r$//' | diff <(sed 's/\r$//' "$TESTDIR/result6.txt") -