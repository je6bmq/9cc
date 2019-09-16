int main()
{
    int *x;
    int y;
    y = 3;
    x = &y;

    if (sizeof(*x) != 4)
    {
        printf("sizeof(int) has to be 4\n");
        exit(1);
    }

    return 0;
}
