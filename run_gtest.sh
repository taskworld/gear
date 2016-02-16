#!/bin/sh
cmake .
make
./runTests
# Delete object file
rm runTests
