int main()
{
    int a;
    a = 0;
    while (a < 20)
    {
        a = a + 1;
    }

    if (a != 20)
    {
        printf("a has to be 20\n");
        exit(1);
    }
    return 0;
}
