int main()
{
    int x;
    int y;
    x = 5;
    y = 7;
    *(&x - 1) = 3;

    if (y != 3)
    {
        printf("y has to be 3\n");
        exit(1);
    }
    else
    {
        return 0;
    }
}
