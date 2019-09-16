int main()
{
    int x;
    x = 0;

    if (sizeof(x + 3) != 4)
    {
        printf("sizeof(int) has to be 4\n");
        exit(1);
    }

    return 0;
}
