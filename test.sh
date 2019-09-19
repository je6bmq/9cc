#!/bin/bash -xe
cd test/

TEST_LOG="../test.log"
rm -f $TEST_LOG
touch $TEST_LOG

for file in $(\ls *.c)
do
    echo "file: $file" >>  $TEST_LOG
    
    set +x 
    tmp=$(mktemp ./XXXXXX.tmp)
    
    set -x 
    ../9cc $file > ${file%%.c}.s 2>$tmp && cat $tmp >> $TEST_LOG  
    set +x

    rm -f $tmp
    
    set -x  
    gcc ${file%%.c}.s -o ${file%%.c}.o -no-pie >> $TEST_LOG 2>&1 && ./${file%%.c}.o >> $TEST_LOG 2>&1
    set +x
    
    echo "" >>  $TEST_LOG
done

echo "OK"