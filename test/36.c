int main()
{
    int x;
    if (sizeof(x) != 4)
    {
        printf("sizeof(int) has to be 4\n");
        exit(1);
    }

    return 0;
}
