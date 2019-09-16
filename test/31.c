int fib(int n, int a, int b)
{
    if (n == 0)
    {
        return a;
    }
    else
    {
        return fib(n - 1, a + b, a);
    }
}
int main()
{
    if (fib(5, 1, 0) != 8)
    {
        printf("fib(5) != 8\n");
        exit(1);
    }
    else
    {
        return 0;
    }
}
