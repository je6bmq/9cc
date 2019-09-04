#include "9cc.h"
#include <stdlib.h>
#include <stdio.h>

// Node *code[100];
Function *functions[100];
LVar *locals;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引数の個数が正しくありません．");
        return 1;
    }

    user_input = argv[1];

    tokenize();
    program();

    printf(".intel_syntax noprefix\n");
    // printf(".global main\n");
    // printf("main:\n");

    // printf("    push rbp\n");
    // printf("    mov rbp, rsp\n");
    // printf("    sub rsp, %d\n", locals ? locals->offset : 0); // alphabet 26 chars * 8 bytes

    // for (int i = 0; code[i]; i++)
    // {
    //     gen(code[i]);
    //     if (code[i]->kind != ND_RETURN)
    //     {
    //         printf("    pop rax\n");
    //     }
    // }

    printf(".global ");
    for (int i = 0; functions[i]; i++)
    {
        for (int j = 0; j < functions[i]->length; j++)
        {
            printf("%c", functions[i]->name[j]);
        }
        if (functions[i + 1] != NULL)
        {
            printf(",");
        }
    }
    printf("\n\n");
    char *registers[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

    for (int i = 0; functions[i]; i++)
    {
        for (int j = 0; j < functions[i]->length; j++)
        {
            printf("%c", functions[i]->name[j]);
        }

        int offset = 0;
        for (LVar *var = functions[i]->local_variables; var; var = var->next)
        {
            offset = var->offset;
        }
        printf(":\n");
        printf("    push rbp\n");
        printf("    mov rbp, rsp\n");
        printf("    sub rsp, %d\n", offset);

        int arg_index = 0;
        for (LVar *var = functions[i]->arguments; var; var = var->next)
        {
            if (arg_index > 5)
            {
                fprintf(stderr, "引数過多です．\n");
                exit(1);
            }

            printf("    mov rax, rbp\n");
            printf("    sub rax, %d\n", var->offset);
            printf("    mov [rax], %s\n", registers[arg_index++]);
        }

        for (int j = 0; j < functions[i]->statements->size; j++)
        {
            Node *statement = get(functions[i]->statements, j);
            gen(statement);
            if (statement->kind != ND_RETURN)
            {
                printf("    pop rax\n");
            }
        }
    }

    // printf("    mov rsp, rbp\n");
    // printf("    pop rbp\n");
    // printf("    ret\n");
    return 0;
}