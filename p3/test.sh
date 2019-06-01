#!/bin/sh

echo "\n=============Compiling mem.c============="
make clean
make

cd tests

echo "\n==========Compiling test files==========="
make clean
make

echo "\n==============Running tests=============="
for file in `find -type f`
do
    [ -x ${file} ] && echo "Testing: $file" && ${file}
done

cd ..
