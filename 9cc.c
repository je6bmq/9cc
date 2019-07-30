#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引数の個数が正しくありません．");
        return 1;
    }

    char* p = argv[1];

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("    mov rax, %ld\n", strtol(p, &p, 10));

    while(*p) {
        switch(*p) {
            case '+':
                p++;
                printf("    add rax, %ld\n",strtol(p, &p, 10));
                break;
            case '-':
                p++;
                printf("    sub rax, %ld\n",strtol(p, &p, 10));
                break;
            default:
                fprintf(stderr, "予期しない文字です．");
                return 1;
        }
    }

    printf("    ret\n");
}