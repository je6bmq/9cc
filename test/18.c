int main()
{
    int hoge;
    hoge = 0;
    while (hoge < 5)
        hoge = hoge + 1;
    
    if(hoge != 5) {
        printf("hoge has to be 5\n");
        exit(1);
    }

    return 0;
}
