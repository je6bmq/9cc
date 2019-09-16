int main()
{
    int a[2];
    a[0] = 1;
    a[1] = 2;

    int b;
    b = (a[0] + a[1]);

    if (b != 3)
    {
        printf("a[0] + a[1] != 3\n");
        exit(1);
    }

    return 0;
}
