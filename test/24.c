int main()
{
    int hoge;
    hoge = 0;
    int i;
    for (i = 0; i <= 10; i = i + 1)
    {
        hoge = hoge + i;
        hoge = hoge + i;
    }


    if(hoge != 110) {
        printf("hoge has to be 110\n");
        exit(1);
    }

    return 0;
}
