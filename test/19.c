int main()
{
    int hoge;
    hoge = 2;
    hoge = hoge + hoge;


    if(hoge != 4) {
        printf("hoge has to be 4\n");
        exit(1);
    }

    return 0;
}
