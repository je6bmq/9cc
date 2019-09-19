int a = 1 + 2 * 3 + 4 / 4;
int b = (a / 4) / 2;
int main() {
    if(b != 1) {
        printf("b != 1\n");
        exit(1);
    }
    return 0;
}