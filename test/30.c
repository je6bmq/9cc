int fib(int n)
{
    if (n <= 1)
    {
        return 1;
    }
    else
    {
        return fib(n - 1) + fib(n - 2);
    }
}
int main()
{
    if (fib(5) != 8)
    {
        printf("fib(5) != 8\n");
        exit(1);
    }
    else
    {
        return 0;
    }
}
