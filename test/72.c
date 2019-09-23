int a = 4;
int* b = &a;

int main() {
    *b = 5;

    if(a != 5) {
        printf("a != 5\n");
        exit(1);
    }

    return 0;
}