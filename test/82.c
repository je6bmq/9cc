int main()
{
    int a[3] = {1, 2};

    if (a[0] != 1)
    {
        printf("a[0] != 1\n");
        exit(1);
    }

    if (a[1] != 2)
    {
        printf("a[1] != 2\n");
        exit(1);
    }

    if (a[2] != 0)
    {
        printf("a[2] != 0\n");
        exit(1);
    }

    return 0;
}