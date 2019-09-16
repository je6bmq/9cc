char add(char x, char y) { return x + y; }
int main()
{
    char a;
    char b;
    a = 10;
    b = 10;

    if (add(a, b) != 20)
    {
        printf("add(a, b) != 20\n");
        exit(1);
    }

    return 0;
}
