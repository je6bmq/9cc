int a;
int b[20];
int main()
{
    b[0] = 1;
    b[1] = b[0];
    a = 8;

    int c; 
    c = a + b[0] + b[1];

    if (c != 10)
    {
        printf("a + b[0] + b[1]!= 10\n");
        exit(1);
    }

    return 0;
}
