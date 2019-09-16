int main()
{
    char *x;
    x = "abc";

    char a;
    a = x[0];

    if (a != 97)
    {
        printf("x[0] != 97 (a)");
        exit(1);
    }

    return 0;
}
