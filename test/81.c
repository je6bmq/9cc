int main()
{
    int a[] = {1, 2, 3};

    int i;
    for (i = 0; i < 3; i = i + 1)
    {
        if (a[i] != i + 1)
        {
            printf("not expected value: %d, %d\n", a[i], i + 1);
            exit(1);
        }
    }

    return 0;
}