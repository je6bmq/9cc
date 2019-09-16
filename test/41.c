int main()
{
    int *x;
    int y;
    y = 3;
    x = &y;

    if (sizeof(x + 5) != 8)
    {
        printf("sizeof(int*) has to be 8\n");
        exit(1);
    }

    return 0;
}
