#!/bin/bash -xe
cd test/

for file in $(\ls *.c)
do
    ../9cc $file > ${file%%.c}.s 2>/dev/null && gcc ${file%%.c}.s -o ${file%%.c}.o -no-pie && ./${file%%.c}.o
done
echo "OK"