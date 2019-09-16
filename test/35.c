int main()
{
    int x;
    int y;
    x = 5;
    y = 7;
    *&x = 3;

    if (y != 7)
    {
        printf("y has to be 7\n");
        exit(1);
    }
    
    return 0;
}
