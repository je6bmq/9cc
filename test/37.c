int main()
{
    int *x;

    if (sizeof(x) != 8)
    {
        printf("sizeof(int*) has to be 8\n");
        exit(1);
    }

    return 0;
}
