char a[] = "abc";
int main()
{
    int i;

    for (i = 0; i < 3; i = i + 1)
    {
        if (a[i] != 97 + i)
        {
            printf("not expected character");
            exit(1);
        }
    }
    return 0;
}