int main()
{
    char a = 100;
    char b = 75;
    char c = 50;
    char* arr[] = {&a, &b, &c};

    if (*arr[0] != 100)
    {
        printf("*a[0] != 100\n");
        exit(1);
    }

    if (*arr[1] != 75)
    {
        printf("*arr[1] != 75\n");
        exit(1);
    }

    if (*arr[2] != 50)
    {
        printf("*arr[2] != 50\n");
        exit(1);
    }

    return 0;
}