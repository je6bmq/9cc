#!/bin/bash

try() {
    expected="$1"
    input="$2"

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

try 0 'return 0;' 
try 42 'return 42;'
try 21 'return 5+20-4;'
try 41 'return 12 + 34 - 5 ;'
try 47 'return 5+6*7;'
try 15 'return 5*(9-6);'
try 4 'return (3+5)/2;'
try 15 'return (-3)*(-5);'
try 10 'return 5+(+3)-(-2);'
try 1 'return 1+1 == 2;'
try 1 'return ((-3)*(-5)) < 5*6;'
try 0 'return ((-4)*(-6)) >= 5*6;'
try 1 'return ((-10)*(-5)) <= 12*4+2;'
try 2 'return ((2 * 5 < 9 * 3) + (2 * 2 > 1));'
try 3 'a = 3; return a;'
try 22 'b = 5 * 6 - 8; return b;'
try 14 'a = 3; b = 5 * 6 - 8; return a + b /2;'
try 3 'foo = 1; fuga=2; return foo + fuga;'
try 1 'if(1 < 3) return 1; else return 3;'
try 4 'a = 3; if(a<4) a=4; return a;'
try 5 'hoge = 0; while(hoge<5) hoge=hoge+1; return hoge;'
try 4 'hoge=2; hoge=hoge+hoge; return hoge;'
try 55 'hoge = 0; for(i=0;i<=10;i=i+1) hoge = hoge + i; return hoge;'
try 10 '{a = 5; a = a + a; return a;}'
try 20 'a = 0; while(a<20) a = a + 1; return a;'
try 20 'a = 0; while(a<20) {a = a + 1; } return a;'
try 110 'hoge = 0; for(i=0;i<=10;i=i+1) {hoge = hoge + i;hoge = hoge + i;} return hoge;'
echo "OK"