int main()
{
    int a;
    a = 3;
    if (a < 4)
        a = 4;

    if (a != 4)
    {
        printf("a has to be 4\n");
        exit(1);
    }
    return 0;
}
