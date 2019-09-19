char a = 1 + 2 * 3 + 4 / 4;
char b = (a / 4) / 2;
char c = a + b;
int main() {
    if(c != 9) {
        printf("c != 9\n");
        exit(1);
    }
    return 0;
}