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

try 0 'main() {return 0;}' 
try 42 'main() {return 42;}'
try 21 'main(){return 5+20-4;}'
try 41 'main(){return 12 + 34 - 5;} '
try 47 'main(){return 5+6*7;}'
try 15 'main(){return 5*(9-6);}'
try 4 'main(){return (3+5)/2;}'
try 15 'main(){return (-3)*(-5);}'
try 10 'main(){return 5+(+3)-(-2);}'
try 1 'main(){return 1+1 == 2;}'
try 1 'main(){return ((-3)*(-5)) < 5*6;}'
try 0 'main(){return ((-4)*(-6)) >= 5*6;}'
try 1 'main(){return ((-10)*(-5)) <= 12*4+2;}'
try 2 'main(){return ((2 * 5 < 9 * 3) + (2 * 2 > 1));}'
try 3 'main(){a = 3; return a;}'
try 22 'main(){b = 5 * 6 - 8; return b;}'
try 14 'main(){a = 3; b = 5 * 6 - 8; return a + b /2;}'
try 3 'main(){foo = 1; fuga=2; return foo + fuga;}'
try 1 'main(){if(1 < 3) return 1; else return 3;}'
try 4 'main(){a = 3; if(a<4) a=4; return a;}'
try 5 'main(){hoge = 0; while(hoge<5) hoge=hoge+1; return hoge;}'
try 4 'main(){hoge=2; hoge=hoge+hoge; return hoge;}'
try 55 'main(){hoge = 0; for(i=0;i<=10;i=i+1) hoge = hoge + i; return hoge;}'
try 10 'main(){{a = 5; a = a + a; return a;}}'
try 20 'main(){a = 0; while(a<20) a = a + 1; return a;}'
try 20 'main(){a = 0; while(a<20) {a = a + 1; } return a;}'
try 110 'main(){hoge = 0; for(i=0;i<=10;i=i+1) {hoge = hoge + i;hoge = hoge + i;} return hoge;}'
try 8 'foo(x,y) {return x+y;} main(){hoge = 0; a=3; b=5; hoge=foo(a,b); return hoge;}'
echo "OK"