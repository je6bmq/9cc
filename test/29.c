int foo(int x, int y, int z) { return x + y + z; }
int hoge(int x, int y) { return x * y; }
int main()
{
    if (foo(hoge(2, 3), hoge(4, 5), 1) != 27)
    {
        printf(" foo(hoge(2, 3), hoge(4, 5), 1) != 27\n");
        exit(1);
    }

    return 0;
}
