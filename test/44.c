int main()
{
    int a[3];
    *a = 1;
    *(a + 1) = 2;
    *(a + 2) = 3;
    int *p;
    p = a;

    int b; 
    b = (*p + *(p + 1) + *(p + 2));
    
    if (b != 6)
    {
        printf("*p + *(p + 1) + *(p + 2) != 6\n");
        exit(1);
    }

    return 0;
}
