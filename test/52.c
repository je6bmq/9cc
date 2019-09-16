int main()
{
    char x;
    
    if (sizeof(sizeof(x)) != 4)
    {
        printf("sizeof(sizeof(char)) != 4\n");
        exit(1);
    }

    return 0;
}
