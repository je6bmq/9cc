char a[4] = {1, 2, 3, 4};
int main()
{

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

    if (a[2] != 3)
    {
        printf("a[2] != 3\n");
        exit(1);
    }

    if (a[3] != 4)
    {
        printf("a[3] != 4\n");
        exit(1);
    }

    return 0;
}