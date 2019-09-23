int a = 4;
int* b = &a + 1 * 7 - 7;

int main() {
    *b = 5;

    if(a != 5) {
        printf("a != 5\n");
        exit(1);
    }

    return 0;
}