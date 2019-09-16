int main()
{
    int *x;

    if (sizeof(sizeof(x)) != 4)
    {
        printf("sizeof(sizeof(x)) has to be 4\n");
        exit(1);
    }

    return 0;
}
