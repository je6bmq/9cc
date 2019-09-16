int main()
{
    char x;
    if (sizeof(x) != 1)
    {
        printf("sizeof(char) has to be 1\n");
        exit(1);
    }

    return 0;
}
