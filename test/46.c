int main()
{
    int a[3];
    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    int *p;
    p = a;
    p[0] = p[2];

    int b; 
    b = a[0] + a[1] + a[2];

    if (b != 8)
    {
        printf("a[0] + a[1] + a[2] != 8\n");
        exit(1);
    }

    return 0;
}
