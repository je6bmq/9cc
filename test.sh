#!/bin/bash

try() {
    expected="$1"
    input="$1"

    tmp_s=$(mktemp ./XXXXXX.s)
    tmp_o=$(mktemp ./XXXXXX)

    ./9cc "$input" > $tmp_s
    gcc -o $tmp_o $tmp_s
    $tmp_o

    actual="$?"
    rm -f $tmp_o $tmp_s
    
    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

try 0 0 
try 42 42
try 21 "5+20-4"
try 41 "12 + 34 - 5 "
try 47 '5+6*7'
try 15 '5*(9-6)'
try 4 '(3+5)/2'
try 15 '(-3)*(-5)'
try 10 '5+(+3)-(-2)'
try 1 '1+1 == 2'
try 1 '((-3)*(-5)) < 5*6'
try 0 '((-4)*(-6)) >= 5*6'
try 1 '((-10)*(-5)) <= 12*4+2'
try 2 '((2 * 5 < 9 * 3) + (2 * 2 > 1))'

echo "OK"