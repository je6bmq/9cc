int replace(char *p)
{
    *p = 20;
    return 0;
}
int main()
{
    char a;
    char *q;
    a = 10;
    q = &a;
    int r;
    r = replace(q);
    
    if (a != 20)
    {
        printf("a != 20\n");
        exit(1);
    }

    return 0;
}
