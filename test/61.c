char *boo() { return "bcd"; }
int main()
{
    char *p;
    p = boo();

    char b;
    b = p[0];

    if (b != 98)
    {
        printf("p[0] != 98 (b)");
        exit(1);
    }

    return 0;
}
