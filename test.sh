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
    ../9cc $file > ${file%%.c}.s 2>$tmp  
    set +x

    set -x  
    gcc ${file%%.c}.s -o ${file%%.c}.o -no-pie >> $tmp 2>&1 && ./${file%%.c}.o >> $tmp 2>&1
    set +x

    if [ $? -ne 0 ]; then
        cat $tmp
        exit 1
    fi

    cat $tmp >> $TEST_LOG
    rm -f $tmp    
    echo "" >>  $TEST_LOG
done

echo "OK"