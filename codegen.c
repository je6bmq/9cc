#include "9cc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Token *token;
Variables *globals;

ConstValue *get_const_expr(Node *node, int u)
{
    switch (node->kind)
    {
    case ND_NUM:
    {
        ConstValue *value = (ConstValue *)calloc(1, sizeof(ConstValue *));
        value->pointer = NULL;

        value->const_value = node->val;

        return value;
    }
    case ND_ADDR:
    {
        Node *lvar_node = node->lhs;
        Variables *variable = NULL;
        for (Variables *var = globals; var; var = var->next)
        {
            if (lvar_node->name_length == var->len && strncmp(lvar_node->name, var->name, lvar_node->name_length) == 0)
            {
                variable = var;
            }
        }

        if (variable == NULL)
        {
            error("予期せぬエラーが発生しました");
        }

        if (variable->initial_value_ptr == NULL)
        {
            error("グローバル変数が初期化されていません．");
        }

        ConstValue *value = (ConstValue *)calloc(1, sizeof(ConstValue *));
        value->pointer = variable;
        value->const_value = u;

        return value;
    }
    case ND_GVAR:
    {
        Variables *variable = NULL;
        for (Variables *var = globals; var; var = var->next)
        {
            if (node->name_length == var->len && strncmp(node->name, var->name, node->name_length) == 0)
            {
                variable = var;
            }
        }

        if (variable == NULL)
        {
            error("予期せぬエラーが発生しました");
        }

        if (variable->initial_value_ptr == NULL)
        {
            error("グローバル変数が初期化されていません．");
        }

        ConstValue *value = (ConstValue *)calloc(1, sizeof(ConstValue *));
        value->pointer = NULL;

        value->const_value = *(variable->initial_value_ptr);
        return value;
    }
    default:
        break;
    }

    if (node->lhs == NULL)
    {
        error("式が構成されていません．");
    }
    if (node->rhs == NULL)
    {
        ConstValue *value = (ConstValue *)calloc(1, sizeof(ConstValue *));
        value->pointer = NULL;

        value->const_value = u;

        return value;
    }

    int left_unit;
    switch (node->lhs->kind)
    {
    case ND_ADD:
    case ND_SUB:
        left_unit = 0;
        break;
    case ND_MUL:
    case ND_DIV:
        left_unit = 1;
        break;
    case ND_NUM:
    case ND_GVAR:
    case ND_ADDR:
        left_unit = 0;
        break;
    default:
        error("左辺は静的に計算できる式ではありません．%d", node->lhs->kind);
    }
    int right_unit;
    switch (node->rhs->kind)
    {
    case ND_ADD:
    case ND_SUB:
        right_unit = 0;
        break;
    case ND_MUL:
    case ND_DIV:
        right_unit = 1;
        break;
    case ND_NUM:
    case ND_GVAR:
    case ND_ADDR:
        right_unit = 0;
        break;
    default:
        error("右辺は計算できる式ではありません．%d", node->rhs->kind);
    }

    ConstValue *lhs = get_const_expr(node->lhs, left_unit);
    ConstValue *rhs = get_const_expr(node->rhs, right_unit);

    if (lhs->pointer != NULL && rhs->pointer != NULL)
    {
        error("アドレス同士の演算はできません．");
    }
    ConstValue *value = (ConstValue *)calloc(1, sizeof(ConstValue *));
    value->pointer = NULL;

    if (lhs->pointer != NULL)
    {
        value->pointer = lhs->pointer;
    }
    else if (rhs->pointer != NULL)
    {
        value->pointer = rhs->pointer;
    }

    switch (node->kind)
    {
    case ND_ADD:
        value->const_value = lhs->const_value + rhs->const_value;
        break;
    case ND_SUB:
        value->const_value = lhs->const_value - rhs->const_value;
        break;
    case ND_MUL:
        value->const_value = lhs->const_value * rhs->const_value;
        break;
    case ND_DIV:
        value->const_value = lhs->const_value / rhs->const_value;
        break;
    }
    return value;
}

void gen_global(Node *node)
{
    switch (node->kind)
    {
    case ND_INIT_ARRAY:
    {
        for (int i = 0; i < node->name_length; i++)
        {
            printf("%c", node->name[i]);
        }
        printf(":\n");
        if (node->type->kind == ARRAY && node->type->to_type->kind == CHAR && node->lhs != NULL && node->lhs->kind == ND_STRING)
        {
            printf("    .string \"");
            for (int i = 0; i < node->lhs->name_length; i++)
            {
                printf("%c", node->lhs->name[i]);
            }
            printf("\"\n");
            return;
        }
        for (int i = 0; i < node->statements->size; i++)
        {
            Node *element = get_node(node->statements, i);
            ConstValue *value = get_const_expr(element, 0);

            switch (node->type->to_type->kind)
            {
            case INT:
                printf("    .long %d\n", value->const_value);
                break;
            case CHAR:
                printf("    .byte %d\n", value->const_value);
                break;
            case POINTER:
                printf("    .quad ");
                for (int j = 0; j < value->pointer->len; j++)
                {
                    printf("%c", value->pointer->name[j]);
                }
                printf("\n");
                break;
            default:
                error("未実装の型です．");
            }
        }
        int type_size = desired_stack_size(node->type->to_type);

        if (node->type->array_size - node->statements->size > 0)
        {
            printf("    .zero %d\n", type_size * (node->type->array_size - node->statements->size));
        }
    }
    break;
    case ND_DECL:
    {
        Node *lvar = node->lhs;
        for (Variables *vars = globals; vars; vars = vars->next)
        {
            if (lvar->name_length == vars->len && strncmp(lvar->name, vars->name, lvar->name_length) == 0)
            {
                printf(".comm   ");
                if (vars->scope != GLOBAL)
                {
                    error("グローバル変数の読み込みに失敗しました．");
                }
                for (int i = 0; i < vars->len; i++)
                {
                    printf("%c", vars->name[i]);
                }
                printf(", %d\n", vars->offset);
                break;
            }
        }
    }
    break;
    case ND_ASSIGN:
    {
        Node *lvar = node->lhs;
        for (int i = 0; i < lvar->name_length; i++)
        {
            printf("%c", lvar->name[i]);
        }
        printf(":\n");
        switch (lvar->type->kind)
        {
        case INT:
            printf("    .long ");
            break;
        case CHAR:
            printf("    .byte ");
            break;
        case POINTER:
            printf("    .quad ");
            break;
        default:
            error("未対応の変数の型です．\n");
        }
        if (lvar->type->kind == INT || lvar->type->kind == CHAR)
        {
            ConstValue *value = get_const_expr(node->rhs, 0);
            Variables *variable = NULL;
            for (Variables *var = globals; var; var = var->next)
            {
                if (lvar->name_length == var->len && strncmp(lvar->name, var->name, lvar->name_length) == 0)
                {
                    variable = var;
                }
            }
            if (variable->initial_value_ptr == NULL)
            {
                variable->initial_value_ptr = (int *)calloc(1, sizeof(int));
            }
            *(variable->initial_value_ptr) = value->const_value;
            printf("%d\n", value->const_value);
        }
        else if (lvar->type->kind == POINTER)
        {
            if (node->rhs->kind == ND_STRING)
            {
                int string_id = node->rhs->lhs->val;
                printf(".LC%d\n", node->lhs->val);
            }
            else
            {
                ConstValue *value = get_const_expr(node->rhs, 0);

                for (int i = 0; i < value->pointer->len; i++)
                {
                    printf("%c", value->pointer->name[i]);
                }
                if (value->const_value != 0)
                {
                    printf("%+d\n", value->const_value);
                }
                else
                {
                    printf("\n");
                }
            }
        }
    }
    break;
    default:
        error("グローバル変数でサポートされていない構文です．%d", node->kind);
    }
}

void gen_lval(Node *node)
{
    if (node->kind != ND_LVAR && node->kind != ND_GVAR)
    {
        error("代入の左辺値が変数ではありません");
    }

    if (node->kind == ND_LVAR)
    {
        printf("    mov rax, rbp\n");
        printf("    sub rax, %d\n", node->offset);
    }
    else if (node->kind == ND_GVAR)
    {
        printf("    lea rax, ");
        for (int i = 0; i < node->name_length; i++)
        {
            printf("%c", node->name[i]);
        }
        printf("[rip]\n");
    }
    else
    {
        error("未実装です．");
    }
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
    case ND_GVAR:
        gen_lval(node);
        printf("    pop rax\n");
        if (node->type->kind == POINTER)
        {
            printf("    mov rax, QWORD PTR [rax]\n");
        }
        else if (node->type->kind == INT)
        {
            printf("    mov eax, DWORD PTR [rax]\n");
        }
        else if (node->type->kind == CHAR)
        {
            printf("    movsx eax, BYTE PTR [rax]\n");
        }
        else if (node->type->kind == ARRAY)
        {
        }
        else
        {
            error("型情報が不明です．");
        }

        printf("    push rax\n");
        return;
    case ND_ASSIGN:
        if (node->lhs->kind == ND_DEREF)
        {
            Node *lhs = node->lhs;
            while (lhs->kind == ND_DEREF)
            {
                gen(node->lhs->lhs);
                lhs = lhs->lhs;
            }
        }
        else
        {
            gen_lval(node->lhs);
        }
        gen(node->rhs);

        printf("    pop rdi\n");
        printf("    pop rax\n");

        if (node->lhs->type->kind == POINTER)
        {
            printf("    mov QWORD PTR [rax], rdi\n");
        }
        else if (node->lhs->type->kind == INT)
        {
            printf("    mov DWORD PTR [rax], edi\n");
        }
        else if (node->lhs->type->kind == CHAR)
        {
            printf("    mov BYTE PTR [rax], dil\n");
        }
        else
        {
            error("型情報が不明です．");
        }

        printf("    push rdi\n");
        return;
    case ND_ADDR:
        gen_lval(node->lhs);
        return;
    case ND_DEREF:
        gen(node->lhs);
        printf("    pop rax\n");
        if (node->lhs->type->to_type == NULL)
        {
            error("参照外しできません．");
        }
        else
        {
            TypeKind kind = node->lhs->type->to_type->kind;
            switch (kind)
            {
            case POINTER:
            case ARRAY:
                printf("    mov rax, QWORD ");
                break;
            case INT:
                printf("    mov eax, DWORD ");
                break;
            case CHAR:
                printf("    movsx eax, BYTE ");
                break;
            default:
                error("未実装の型です．");
            }
            printf("PTR [rax]\n");
        }
        // printf("    mov rax, QWORD PTR [rax]\n");
        printf("    push rax\n");
        return;
    case ND_IF:
        gen(node->lhs); // if condition
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        if (node->other)
        {
            printf("    je  .Lelse%d\n", node->id);
        }
        else
        {
            printf("    je  .Lend%d\n", node->id);
        }

        gen(node->rhs);
        printf("    pop rax\n");
        if (node->other)
        {
            printf("    jmp  .Lend%d\n", node->id);
            printf("  .Lelse%d:\n", node->id);
            gen(node->other);
            printf("    pop rax\n");
        }
        printf("  .Lend%d:\n", node->id);
        printf("    push rax\n");
        return;
    case ND_WHILE:
        printf(".Lbegin%d:\n", node->id);
        gen(node->lhs); // loop condition
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .Lend%d\n", node->id);
        gen(node->rhs);
        printf("    pop rax\n");
        printf("    jmp .Lbegin%d\n", node->id);
        printf("  .Lend%d:\n", node->id);
        printf("    push rax\n");
        return;
    case ND_FOR:
        if (node->lhs != NULL)
        {
            gen(node->lhs); // initialize part
            printf("    pop rax\n");
        }
        printf(".Lbegin%d:\n", node->id);
        gen(node->rhs); // condition part
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je  .Lend%d\n", node->id);
        gen(node->another); // statement part
        printf("    pop rax\n");
        printf("    push rax\n");
        if (node->other != NULL)
        {
            gen(node->other);        // update variable part
            printf("    pop rdi\n"); // drop the result of update expression.
        }
        printf("    jmp .Lbegin%d\n", node->id);
        printf("  .Lend%d:\n", node->id);
        printf("    pop rax\n");
        return;
    case ND_BLOCK:
        for (int i = 0; i < node->statements->size; i++)
        {
            gen(get_node(node->statements, i));
            printf("    pop rax\n");
        }
        printf("    push rax\n");
        return;
    case ND_CALL_FUNC:
    {
        Node **arguments[6] = {&(node->lhs), &(node->rhs), &(node->other), &(node->another), &(node->option1), &(node->option2)};
        char *registers[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
        int num_arg = 0;

        for (int i = 0; *(arguments[i]) != NULL; i++)
        {
            gen(*(arguments[i]));
            printf("    pop rax\n");
            printf("    push rax\n");
            num_arg++;
        }
        for (int i = num_arg - 1; i >= 0; i--)
        {
            printf("    pop %s\n", registers[i]);
        }
        printf("    call ");
        for (int i = 0; i < node->function_table->length; i++)
        {
            printf("%c", node->function_table->name[i]);
        }
        printf("\n");
        printf("    push rax\n");
    }
        return;
    case ND_DECL:
        printf("    push rax\n");
        return;
    case ND_INIT_LOCAL:
        gen(node->lhs);
        gen(node->rhs);
        return;
    case ND_STRING:
        printf("    mov rax, OFFSET FLAT:.LC%d\n", node->lhs->val);
        printf("    push rax\n");
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    if (node->lhs->type != NULL && (node->kind == ND_ADD || node->kind == ND_SUB))
    {
        if (node->lhs->type->kind == POINTER || node->lhs->type->kind == ARRAY)
        {
            if (node->lhs->type->to_type->kind == POINTER)
            {
                printf("    imul rdi, %d\n", 8);
            }
            else if (node->lhs->type->to_type->kind == INT)
            {
                printf("    imul rdi, %d\n", 4);
            }
        }
    }

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
