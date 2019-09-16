int foo(int k)
{
    if (k < 10)
    {
        return 10;
    }
    else
    {
        return 1;
    }
}
int main()
{
    if (foo(1) != 10)
    {
        printf("foo(1) != 1\n");
        exit(1);
    }

    return 0;
}
