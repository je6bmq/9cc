int main()
{
    char x[3];
    x[0] = -1;
    x[1] = 2;
    x[2] = 4;
    char *p;
    p = x;
    p[0] = p[2];

    char a;
    a = x[0] + x[1] + x[2];
    
    if (a != 10)
    {
        printf("x[0] + x[1] + x[2] != 10\n");
        exit(1);
    }

    return 0;
}
