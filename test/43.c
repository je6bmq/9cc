int main()
{
    int a[2];
    *a = 1;
    *(a + 1) = 2;
    int *p;
    p = a;

    int b;
    b = (*p + *(p + 1));

    if (b != 3)
    {
        printf("*p + *(p + 1) != 3\n");
        exit(1);
    }

    return 0;
}
