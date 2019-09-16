int foo(int x, int y) { return x + y; }
int main()
{
    int hoge;
    hoge = 0;
    int a;
    a = 3;
    int b;
    b = 5;
    hoge = foo(a, b);
    
    if(hoge != 8) {
        printf("hoge has to be 8\n");
        exit(1);
    }

    return 0;
}
