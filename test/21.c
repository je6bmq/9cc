int main()
{
    {
        int a;
        a = 5;
        a = a + a;
        if (a != 10)
        {
            printf("a has to be 10\n");
            exit(1);
        }
        return 0;
    }
}
