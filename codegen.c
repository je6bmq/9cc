#include "9cc.h"
#include <stdio.h>

Token *token;

void gen_lval(Node *node)
{
    if (node->kind != ND_LVAR)
    {
        error("代入の左辺値が変数ではありません");
    }

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}

void gen(Node *node)
{
    if (node->kind == ND_RETURN)
    {
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;
    }

    switch (node->kind)
    {
    case ND_NUM:
        printf("    push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return;
    case ND_IF:
        gen(node->lhs); // if condition
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .Lend%d\n", node->id);
        gen(node->rhs);
        if (node->other)
        {
            printf("    jmp  .Lend%d\n", node->id);
            printf("  .Lelse%d:\n", node->id);
            gen(node->other);
        }
        printf("  .Lend%d:\n", node->id);
        return;
    case ND_WHILE:
        printf(".Lbegin%d:\n", node->id);
        gen(node->lhs); // loop condition
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .Lend%d\n", node->id);
        gen(node->rhs);
        printf("    jmp .Lbegin%d\n", node->id);
        printf("  .Lend%d:\n", node->id);
        return;
    case ND_FOR:
        if (node->lhs != NULL)
        {
            gen(node->lhs); // initialize part
        }
        printf(".Lbegin%d:\n", node->id);
        gen(node->rhs); // condition part
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .Lend%d\n", node->id);
        gen(node->another); // statement part
        if (node->other != NULL)
        {
            gen(node->other);        // update variable part
            printf("    pop rdi\n"); // drop the result of update expression.
        }
        printf("    jmp .Lbegin%d\n", node->id);
        printf("  .Lend%d:\n", node->id);
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
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_NOT_EQUAL:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LESS_THAN:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;
    case ND_LESS_EQUAL:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;
    }

    printf("    push rax\n");
}
