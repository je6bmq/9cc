int main()
{
    int a = 5;
    int b = 3;
    int c = a + b;

    int d;
    d = a + b;

    if (c != 8)
    {
        printf("c != 8\n");
        exit(1);
    }

    if (d != 8)
    {
        printf("c != 8\n");
        exit(1);
    }

    return 0;
}