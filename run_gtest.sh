#!/bin/sh

#Clear existing GTEST_FILTER
if [ -z "$1" ]
then
    unset GTEST_FILTER
else
    export GTEST_FILTER=$1
fi

cmake .
make
./runTests
# Delete object file
rm runTests
