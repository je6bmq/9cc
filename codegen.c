#include "9cc.h"
#include <stdio.h>

Token *token;

void gen(Node *node)
{
    if (node->kind == ND_NUM)
    {
        printf("    push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case ND_SUB:
        printf("    sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("    imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("    cqo\n"); // expand rax(64bit) to rax and rdx(128bit)
        printf("    idiv rdi\n");
        break;
    case ND_EQUAL:
        printf("    cmp rax, rdi\n");
        printf("    sete al");
        printf("    movzb rax, al");
    case ND_NOT_EQUAL:
        printf("    cmp rax, rdi\n");
        printf("    setne al");
        printf("    movzb rax, al");
        break;
    case ND_LESS_THAN:
        printf("    cmp rax, rdi\n");
        printf("    setl al");
        printf("    movzb rax, al");
        break;
    case ND_LESS_EQUAL:
        printf("    cmp rax, rdi\n");
        printf("    setle al");
        printf("    movzb rax, al");
        break;
    }

    printf("    push rax\n");
}
