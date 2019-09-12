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

try 0 'int main() {return 0;}' 
try 42 'int main() {return 42;}'
try 21 'int main(){return 5+20-4;}'
try 41 'int main(){return 12 + 34 - 5;} '
try 47 'int main(){return 5+6*7;}'
try 15 'int main(){return 5*(9-6);}'
try 4 'int main(){return (3+5)/2;}'
try 15 'int main(){return (-3)*(-5);}'
try 10 'int main(){return 5+(+3)-(-2);}'
try 1 'int main(){return 1+1 == 2;}'
try 1 'int main(){return ((-3)*(-5)) < 5*6;}'
try 0 'int main(){return ((-4)*(-6)) >= 5*6;}'
try 1 'int main(){return ((-10)*(-5)) <= 12*4+2;}'
try 2 'int main(){return ((2 * 5 < 9 * 3) + (2 * 2 > 1));}'
try 3 'int main(){int a; a = 3; return a;}'
try 22 'int main(){int b; b = 5 * 6 - 8; return b;}'
try 14 'int main(){int a; a = 3; int b; b = 5 * 6 - 8; return a + b /2;}'
try 3 'int main(){int foo; foo = 1; int fuga; fuga =2; return foo + fuga;}'
try 1 'int main(){if(1 < 3) return 1; else return 3;}'
try 3 'int main(){if(3 < 3) return 1; else return 3;}'
try 4 'int main(){int a; a = 3; if(a<4) a=4; return a;}'
try 5 'int main(){int hoge; hoge = 0; while(hoge<5) hoge=hoge+1; return hoge;}'
try 4 'int main(){int hoge; hoge=2; hoge=hoge+hoge; return hoge;}'
try 55 'int main(){int hoge; hoge = 0; int i; for(i=0;i<=10;i=i+1) hoge = hoge + i; return hoge;}'
try 10 'int main(){{int a; a = 5; a = a + a; return a;}}'
try 20 'int main(){int a; a = 0; while(a<20) a = a + 1; return a;}'
try 20 'int main(){int a; a = 0; while(a<20) {a = a + 1; } return a;}'
try 110 'int main(){int hoge; hoge = 0; int i; for(i=0;i<=10;i=i+1) {hoge = hoge + i;hoge = hoge + i;} return hoge;}'
try 8 'int foo(int x,int y) {return x+y;} int main(){int hoge; hoge = 0; int a; a=3; int b; b=5; hoge=foo(a,b); return hoge;}'
try 10 'int foo(int k) {if(k<10) {return 10;} else {return 1;}} int main() {return foo(1);}'
try 1 'int foo(int k) {if(k<10) {return 10;} else {return 1;}} int main() {return foo(10);}'
try 16 'int foo(int x,int y,int z) {return x+y+z;} int main(){int hoge; hoge = 0; int a; a=3; int b; b=5; hoge=foo(a,b, foo(a,b,0)); return hoge;}'
try 27 'int foo(int x,int y,int z) {return x+y+z;} int hoge(int x,int y){return x*y;}int main(){return foo(hoge(2,3),hoge(4,5),1);}'
try 8 'int fib(int n) {if(n <= 1) {return 1;} else {return fib(n-1)+fib(n-2);}} int main(){return fib(5);}'
try 8 'int fib(int n,int a,int b) {if(n == 0) {return a;} else {return fib(n-1,a+b,a);}} int main(){return fib(5,1,0);}'
try 3 'int main() {int x; int* y; x=0; y=&x; *y = 3; return x;}'
try 3 'int main() {int x; int* y; int** z; x = 0; y = &x; z = &y; **z = 3; return x;}'
try 3 'int main() {int x; int y; x = 5; y = 7; *(&x - 1)=3; return y;}'
try 7 'int main() {int x; int y; x = 5; y = 7; *&x=3; return y;}'
try 4 'int main() {int x; return sizeof(x);}'
try 8 'int main() {int *x; return sizeof(x);}'
try 4 'int main() {int *x; int y; y = 3; x = &y ; return sizeof(*x);}'
try 4 'int main() {return sizeof(3);}'
try 4 'int main() {int x; x = 0; return sizeof(x + 3);}'
try 8 'int main() {int *x; int y; y = 3; x = &y ; return sizeof(x+5);}'
try 4 'int main() {int *x; return sizeof(sizeof(x));}'
try 3 'int main() {int a[2]; *a=1; *(a+1) = 2; int *p; p = a; return *p + *(p+1);}'
try 6 'int main() {int a[3]; *a=1; *(a+1) = 2; *(a+2) = 3; int *p; p = a; return *p + *(p+1) + *(p+2);}'
try 3 'int main() {int a[2]; a[0]=1; a[1] = 2;  return a[0] + a[1];}'
try 8 'int main() {int a[3]; a[0]=1; a[1] = 2; a[2] = 3; int *p; p = a; p[0] = p[2]; return a[0] + a[1] + a[2];}'
try 10 'int a; int main() {int a; a = 10; return a;}'
try 5 'int b[3]; int main() {int b; b = 5; return b;}'
try 8 'int a; int main() {int a[3]; a[0]=1; a[1] = 2; a[2] = 3; int *p; p = a; p[0] = p[2]; return a[0] + a[1] + a[2];}'
try 5 'int a; int main() {a = 5; return a;}'
try 10 'int a; int b[20]; int main() { b[0] = 1; b[1] = b[0]; a = 8; return a + b[0] + b[1]; }'
echo "OK"