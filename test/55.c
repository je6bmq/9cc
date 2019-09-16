int main()
{
    char x[3];
    x[0] = -1;
    x[1] = 2;
    int y;
    y = 4;

    char a;
    a = x[0] + y;
    
    if (a != 3)
    {
        printf("x[0] + y != 3\n");
        exit(1);
    }

    return 0;
}
