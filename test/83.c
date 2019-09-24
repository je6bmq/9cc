int main()
{
    int a = 10;
    int b = 20;
    int c = 30;
    int* arr[] = {&a, &b, &c};

    if (*arr[0] != 10)
    {
        printf("*a[0] != 10\n");
        exit(1);
    }

    if (*arr[1] != 20)
    {
        printf("*arr[1] != 20\n");
        exit(1);
    }

    if (*arr[2] != 30)
    {
        printf("*arr[2] != 30\n");
        exit(1);
    }

    return 0;
}