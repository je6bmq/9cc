int main()
{
    int x;
    int *y;
    int **z;
    x = 0;
    y = &x;
    z = &y;
    **z = 3;

    if(x != 3) {
        printf("x has to be 3\n");
        exit(1);
    }

    return 0;
}
