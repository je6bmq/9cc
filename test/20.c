int main()
{
    int hoge;
    hoge = 0;
    int i;
    for (i = 0; i <= 10; i = i + 1)
        hoge = hoge + i;

    if (hoge != 55)
    {
        printf("hoge has to be 55\n");
        exit(1);
    }

    return 0;
}
