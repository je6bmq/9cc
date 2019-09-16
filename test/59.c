char *replace(char *p)
{
    *p = 20;
    return p;
}
int main()
{
    char a;
    char *q;
    a = 10;
    q = replace(&a);
    
    if (a != 20)
    {
        printf("a != 20\n");
        exit(1);
    }

    return 0;
}
