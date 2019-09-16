int foo(int x, int y, int z) { return x + y + z; }
int main()
{
    int hoge;
    hoge = 0;
    int a;
    a = 3;
    int b;
    b = 5;
    hoge = foo(a, b, foo(a, b, 0));

    if (hoge != 16)
    {
        printf("hoge != 16\n");
        exit(1);
    }
    return 0;
}
