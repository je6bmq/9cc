int main()
{
    int x;
    int *y;
    x = 0;
    y = &x;
    *y = 3;

    if(x != 3) {
        printf("x has to be 2\n");
        exit(1);
    }

    return 0;
}
