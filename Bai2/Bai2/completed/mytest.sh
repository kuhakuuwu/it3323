#! /bin/bash

# Ensure local scanner binary exists and is executable
if [ ! -x ./scanner ]; then
	echo "Error: ./scanner not found or not executable. Build it first (run 'make' in this directory)."
	exit 2
fi

./scanner ../test/example1.kpl | diff ../test/result1.txt -
./scanner ../test/example2.kpl | diff ../test/result2.txt -
./scanner ../test/example3.kpl | diff ../test/result3.txt -
./scanner ../test/example4.kpl | diff ../test/result4.txt -
./scanner ../test/example5.kpl | diff ../test/result5.txt -
./scanner ../test/example6.kpl | diff ../test/result6.txt -
./scanner ../test/example7.kpl | diff ../test/result7.txt -
#Test moi them
./scanner ../test/example8.kpl | diff ../test/result8.txt -
