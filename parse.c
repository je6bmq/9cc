#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

Token *token; // current token
char *user_input;
Function *functions[100];
Variables *locals;
Variables *globals;
FunctionTableLinkedList *function_table;
int current_node_id;

void error_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void warn_at(char *loc, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
}

/*
    error function like fprintf
 */
void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt); // obtain varargs after "fmt"
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

/*
  return whether next token is reserved symbol or not.
 */
bool consume(char *op)
{
    if ((token->kind != TK_RESERVED && token->kind != TK_RETURN && token->kind != TK_IF && token->kind != TK_ELSE && token->kind != TK_FOR && token->kind != TK_WHILE && token->kind != TK_SIZEOF) || strlen(op) != token->len || memcmp(token->str, op, token->len))
    {
        return false;
    }
    token = token->next;
    return true;
}

Token *consume_ident()
{
    if (token->kind != TK_IDENT)
    {
        return NULL;
    }
    for (int i = 0; i < token->len; i++)
    {
        char c = token->str[i];
        if (!is_alnum(c))
        {
            return NULL;
        }
    }
    Token *tok = token;  // copy of current token
    token = token->next; // step token to continuing process
    return tok;
}
Token *expect_ident()
{
    if (token->kind != TK_IDENT || token->str[0] < 'a' || token->str[0] > 'z')
    {
        error_at(token->str, "変数名ではありません");
    }
    else
    {
        Token *tok = token;  // copy of current token
        token = token->next; // step token to continuing process
        return tok;
    }
}

/*
    if next token is not reserved, abnormal exit.
 */
void expect(char *op)
{
    if ((token->kind != TK_RESERVED && token->kind != TK_RETURN && token->kind != TK_IF && token->kind != TK_ELSE && token->kind != TK_FOR && token->kind != TK_WHILE) || strlen(op) != token->len || memcmp(token->str, op, token->len))
    {
        error_at(token->str, "'%s'ではありません", op);
    }
    token = token->next;
}

/*
    if next token is not a number, abnormal exit.
 */
int expect_number()
{
    if (token->kind != TK_NUM)
    {
        error_at(token->str, "数ではありません");
    }
    int val = token->val;
    token = token->next;
    return val;
}

/*
    return whether current token is EOF or not.
 */
bool at_eof()
{
    return token->kind == TK_EOF;
}

/*
    create new token and chain current token
 */
Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

/*
    tokenize from provided string
 */
void tokenize()
{
    Token head;
    head.next = (NULL);
    Token *cur = &head;
    char *p = user_input;

    while (*p)
    {
        if (isspace(*p))
        {
            // skip space
            p++;
            continue;
        }

        if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2]))
        {
            cur = new_token(TK_IF, cur, p, 2);
            p += 2;
            continue;
        }
        if (strncmp(p, "while", 2) == 0 && !is_alnum(p[5]))
        {
            cur = new_token(TK_WHILE, cur, p, 5);
            p += 5;
            continue;
        }

        if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4]))
        {
            cur = new_token(TK_ELSE, cur, p, 4);
            p += 4;
            continue;
        }

        if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3]))
        {
            cur = new_token(TK_FOR, cur, p, 3);
            p += 3;
            continue;
        }

        if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6]))
        {
            cur = new_token(TK_RETURN, cur, p, 6);
            p += 6;
            continue;
        }

        if (memcmp(p, "<=", 2) == 0 || memcmp(p, ">=", 2) == 0 || memcmp(p, "==", 2) == 0 || memcmp(p, "!=", 2) == 0)
        {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (memcmp(p, "+", 1) == 0 || memcmp(p, "-", 1) == 0 || memcmp(p, "*", 1) == 0 || memcmp(p, "/", 1) == 0)
        {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (memcmp(p, ";", 1) == 0 || memcmp(p, "(", 1) == 0 || memcmp(p, ")", 1) == 0 || memcmp(p, "{", 1) == 0 || memcmp(p, "}", 1) == 0 || memcmp(p, ",", 1) == 0 || memcmp(p, "[", 1) == 0 || memcmp(p, "]", 1) == 0)
        {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (memcmp(p, "=", 1) == 0)
        {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (memcmp(p, "<", 1) == 0 || memcmp(p, ">", 1) == 0)
        {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (memcmp(p, "&", 1) == 0)
        {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (memcmp(p, "int", 3) == 0 && (!is_alnum(p[3]) || p[3] == '*'))
        {
            cur = new_token(TK_RESERVED, cur, p, 3);
            p += 3;
            continue;
        }

        if (memcmp(p, "char", 4) == 0 && (!is_alnum(p[4]) || p[4] == '*'))
        {
            cur = new_token(TK_RESERVED, cur, p, 4);
            p += 4;
            continue;
        }

        if (memcmp(p, "sizeof", 6) == 0 && !is_alnum(p[6]))
        {
            cur = new_token(TK_SIZEOF, cur, p, 6);
            p += 6;
            continue;
        }

        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p, 1);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        if (is_alnum(*p))
        {
            int name_count = 0;
            while (is_alnum(*(p + name_count)))
            {
                name_count++;
            }
            cur = new_token(TK_IDENT, cur, p, name_count);
            p += name_count;
            continue;
        }

        error_at(token->str, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 1);
    token = head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs, char *name, int name_length)
{
    Node *node = calloc(1, sizeof(Node));
    node->id = current_node_id++;
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;

    node->name = name;
    node->name_length = name_length;

    return node;
}

Node *new_node_num(int val)
{
    Node *node = calloc(1, sizeof(Node));
    node->id = current_node_id++;
    node->kind = ND_NUM;
    node->val = val;
    node->type = (Type *)calloc(1, sizeof(Type));
    node->type->kind = INT;
    node->type->to_type = NULL;
    return node;
}

int desired_stack_size(Type *type)
{
    if (type == NULL)
    {
        error_at(token->str, "型情報がありません．");
    }
    switch (type->kind)
    {
    case INT:
        return 4;
    case POINTER:
        return 8;
    case CHAR:
        return 1;
    case ARRAY:
    {
        int size = type->array_size;
        Type *value_type = type->to_type;
        if (size <= 0)
        {
            error_at(token->str, "配列の要素数は正の数である必要があります．");
        }
        else if (value_type == NULL)
        {
            error_at(token->str, "要素の型情報がありません．");
        }
        return size * desired_stack_size(value_type);
    }
    }
}

Type *expect_type()
{
    Type *type = (Type *)calloc(1, sizeof(Type));
    type->to_type = NULL;
    type->array_size = 0;

    if (consume("int"))
    {
        type->kind = INT;
    }
    else if (consume("char"))
    {
        type->kind = CHAR;
    }
    else
    {
        error_at(token->str, "返り値の型を明記してください．");
    }
    return type;
}

Type *consume_type()
{
    Type *type = (Type *)calloc(1, sizeof(Type));
    type->to_type = NULL;
    type->array_size = 0;

    if (consume("int"))
    {
        type->kind = INT;
    }
    else if (consume("char"))
    {
        type->kind = CHAR;
    }
    else
    {
        type = NULL;
    }
    return type;
}

Function *function()
{
    Function *func = (Function *)calloc(1, sizeof(Function));

    Type *type = expect_type();

    func->statements = new_vec();
    if (token->kind != TK_IDENT)
    {
        fprintf(stderr, "関数名ではありません．");
        for (int i = 0; i < token->len; i++)
        {
            fprintf(stderr, "%c", token->str[i]);
        }
        fprintf(stderr, "\n");
        exit(1);
    }
    FunctionTableLinkedList *list = (FunctionTableLinkedList *)calloc(1, sizeof(FunctionTableLinkedList));

    FunctionTable *func_table = (FunctionTable *)calloc(1, sizeof(FunctionTable));
    func_table->name = token->str;
    func_table->length = token->len;
    list->value = func_table;
    if (function_table == NULL)
    {

        function_table = list;
    }
    else
    {
        FunctionTableLinkedList *table_list;
        for (FunctionTableLinkedList *table = function_table; table; table = table->next)
        {
            table_list = table;
        }
        table_list->next = list;
    }
    func->name = token->str;
    func->length = token->len;
    token = token->next;
    expect("(");
    Variables *arguments = NULL;

    if (!consume(")"))
    {
        while (true)
        {
            Type *type = expect_type();

            while (consume("*"))
            {
                Type *tmp_type = type;
                type = (Type *)calloc(1, sizeof(Type));
                type->kind = POINTER;
                type->to_type = tmp_type;
            }
            Variables *arg = (Variables *)calloc(1, sizeof(Variables));
            if (token->kind != TK_IDENT)
            {
                error_at(token->str, "引数の変数名が不正です．");
            }
            arg->name = token->str;
            arg->len = token->len;
            arg->type = type;
            arg->scope = LOCAL;

            int current_offset = 0;
            for (Variables *var = arguments; var; var = var->next)
            {
                current_offset = var->offset;
            }
            arg->offset = current_offset + desired_stack_size(type);

            if (arguments == NULL)
            {
                arguments = arg;
                arguments->next = NULL;
            }
            else
            {
                Variables *last_arg;
                for (Variables *var = arguments; var; var = var->next)
                {
                    last_arg = var;
                }
                last_arg->next = arg;
            }

            token = token->next;
            if (consume(","))
            {
                continue;
            }
            else if (consume(")"))
            {
                break;
            }
            else
            {
                fprintf(stderr, "不正なトークンです．\n");
                exit(1);
            }
        }
    }
    locals = (Variables *)calloc(1, sizeof(Variables));
    locals->scope = LOCAL;
    func_table->arguments = arguments; // "arguments" indicates pointer, so this statement has to be placed after parsing arguments.
    func->arguments = arguments;
    Variables *arg_last = locals;
    for (Variables *arg = arguments; arg; arg = arg->next)
    {
        arg_last->next = arg;
        arg_last = arg_last->next;
    } // arguments are also local variable.
    expect("{");
    while (!consume("}"))
    {
        push(func->statements, stmt());
    }
    func->local_variables = locals; // set local variables in current function.
    return func;
}

void add_variables(Variables **variables_ptr, TypeKind element_kind, Scope scope)
{
    Type *type = (Type *)calloc(1, sizeof(Type));
    type->kind = element_kind;
    type->to_type = NULL;

    while (consume("*"))
    {
        Type *tmp_type = type;
        type = (Type *)calloc(1, sizeof(Type));
        type->kind = POINTER;
        type->to_type = tmp_type;
        type->array_size = 0;
    }

    Token *tok = expect_ident();
    if (consume("["))
    {
        Type *tmp_type = type;
        type = (Type *)calloc(1, sizeof(Type));
        type->to_type = tmp_type;
        type->kind = ARRAY;
        type->array_size = expect_number();
        expect("]");
    }

    // Variables *lvar = (Variables *)calloc(1, sizeof(Variables));
    Variables *lvar = NULL;
    Variables *lvar_next = (Variables *)calloc(1, sizeof(Variables));
    for (Variables *var = (*variables_ptr); var; var = var->next)
    {
        lvar = var;
    }
    lvar_next->name = tok->str;
    lvar_next->len = tok->len;
    lvar_next->offset = (scope == GLOBAL ? 0 : (lvar ? lvar->offset : 0)) + desired_stack_size(type);
    lvar_next->type = type;
    lvar_next->scope = scope;

    if (lvar != NULL)
    {
        lvar->next = lvar_next;
    }
    else
    {
        if (*variables_ptr != NULL)
        {
            (*variables_ptr)->next = lvar_next;
        }
        else
        {
            (*variables_ptr) = lvar_next;
        }
    }
}

void program()
{
    int i = 0;
    while (!at_eof())
    {
        Token *tmp_token = token;
        bool is_global_variable = false;
        if (memcmp(tmp_token->str, "int", 3) == 0)
        {
            tmp_token = tmp_token->next;
            while (memcmp(tmp_token->str, "*", 1) == 0)
            {
                tmp_token = tmp_token->next;
            }
            if (tmp_token->kind != TK_IDENT)
            {
                error_at(tmp_token->str, "変数または関数名ではありません．");
            }
            tmp_token = tmp_token->next;
            is_global_variable = memcmp(tmp_token->str, "(", 1) != 0;
        }

        if (is_global_variable)
        {
            Type *type = expect_type();
            add_variables(&globals, type->kind, GLOBAL);
            expect(";");
        }
        else
        {
            functions[i++] = function();
        }
    }
    functions[i] = NULL;
}

Node *stmt()
{
    Node *node;

    if (consume("if"))
    {
        expect("(");
        Node *condition = expr();
        expect(")");
        Node *if_true_statement = stmt();
        node = new_node(ND_IF, condition, if_true_statement, NULL, 0);
        if (consume("else"))
        {
            node->other = stmt();
        }
        else
        {
            node->other = NULL;
        }
        return node;
    }

    if (consume("while"))
    {
        expect("(");
        Node *condition = expr();
        expect(")");
        Node *process = stmt();
        node = new_node(ND_WHILE, condition, process, NULL, 0);
        return node;
    }

    if (consume("for"))
    {
        expect("(");
        Node *initial;
        if (consume(";"))
        {
            initial = NULL;
        }
        else
        {
            initial = expr();
            expect(";");
        }
        Node *condition;
        if (consume(";"))
        {
            condition = new_node(ND_LESS_THAN, new_node_num(0), new_node_num(1), NULL, 0);
        }
        else
        {
            condition = expr();
            expect(";");
        }
        Node *updater;
        if (consume(")"))
        {
            updater = NULL;
        }
        else
        {
            updater = expr();
            expect(")");
        }

        node = new_node(ND_FOR, initial, condition, NULL, 0);

        Node *statement;
        statement = stmt();
        node->other = updater;
        node->another = statement;

        return node;
    }

    if (consume("{"))
    {
        node = new_node(ND_BLOCK, NULL, NULL, NULL, 0);
        NodeReferenceVector *vector = new_vec();
        while (!consume("}"))
        {
            Node *statement = stmt();
            push(vector, statement);
        }
        node->statements = vector;
        return node;
    }

    if (consume("return"))
    {
        node = calloc(1, sizeof(Node));
        current_node_id++;
        node->kind = ND_RETURN;
        node->lhs = expr();
    }
    else
    {
        Type *type = consume_type();
        if (type != NULL)
        {
            add_variables(&locals, type->kind, LOCAL);
            node = new_node(ND_DECL, NULL, NULL, NULL, 0);
        }
        else
        {
            node = expr();
        }
    }

    if (!consume(";"))
    {
        error_at(token->str, "';'ではないトークンです．");
    }
    return node;
}

Node *expr()
{
    return assign();
}

Node *assign()
{
    Node *node = equality();
    if (consume("="))
    {
        node = new_node(ND_ASSIGN, node, assign(), NULL, 0);
    }
    return node;
}

Node *equality()
{
    Node *node = relational();
    for (;;)
    {
        if (consume("=="))
        {
            node = new_node(ND_EQUAL, node, relational(), NULL, 0);
        }
        else if (consume("!="))
        {
            node = new_node(ND_NOT_EQUAL, node, relational(), NULL, 0);
        }
        else
        {
            return node;
        }
    }
}

Node *relational()
{
    Node *node = add();
    for (;;)
    {
        if (consume("<"))
        {
            node = new_node(ND_LESS_THAN, node, add(), NULL, 0);
        }
        else if (consume("<="))
        {
            node = new_node(ND_LESS_EQUAL, node, add(), NULL, 0);
        }
        else if (consume(">"))
        {
            node = new_node(ND_LESS_THAN, add(), node, NULL, 0);
        }
        else if (consume(">="))
        {
            node = new_node(ND_LESS_EQUAL, add(), node, NULL, 0);
        }
        else
        {
            return node;
        }
    }
}

Node *add()
{
    Node *node = mul();

    for (;;)
    {
        Type *type = (Type *)calloc(1, sizeof(Type));
        if (node->type != NULL)
        {
            type->kind = node->type->kind;
            type->to_type = node->type->to_type;
        }
        else
        {
            warn_at(token->str, "型情報が不定です．");
            type = NULL;
        }

        if (consume("+"))
        {
            node = new_node(ND_ADD, node, mul(), NULL, 0);
            node->type = type;
        }
        else if (consume("-"))
        {
            node = new_node(ND_SUB, node, mul(), NULL, 0);
            node->type = type;
        }
        else
        {
            return node;
        }
    }
}

Node *mul()
{
    Node *node = unary();

    for (;;)
    {
        Type *type = (Type *)calloc(1, sizeof(Type));
        if (node->type != NULL)
        {
            type->kind = node->type->kind;
            type->to_type = node->type->to_type;
        }
        else
        {
            warn_at(token->str, "型情報が不定です．");
            type = NULL;
        }
        if (consume("*"))
        {
            node = new_node(ND_MUL, node, unary(), NULL, 0);
            node->type = type;
            if (node->type != NULL && node->type->kind == POINTER)
            {
                warn_at(token->str, "ポインタ型に対する乗算，除算は危険です．");
            }
        }
        else if (consume("/"))
        {
            node = new_node(ND_DIV, node, unary(), NULL, 0);
            node->type = type;
            if (node->type != NULL && node->type->kind == POINTER)
            {
                warn_at(token->str, "ポインタ型に対する乗算，除算は危険です．");
            }
        }
        else
        {
            return node;
        }
    }
}

Node *unary()
{
    if (consume("*"))
    {
        Node *lhs = unary();
        Node *node = new_node(ND_DEREF, lhs, NULL, NULL, 0);
        if (lhs->type->to_type == NULL)
        {
            error_at(token->str, "参照外し可能な変数ではありません．");
        }
        node->type = lhs->type->to_type;
        return node;
    }
    if (consume("&"))
    {
        Node *lhs = unary();
        Type *type = (Type *)calloc(1, sizeof(Type));
        type->kind = POINTER;
        type->to_type = lhs->type;

        Node *node = new_node(ND_ADDR, lhs, NULL, NULL, 0);
        node->type = type;
        return node;
    }
    if (consume("+"))
    {
        return term();
    }
    if (consume("-"))
    {
        return new_node(ND_SUB, new_node_num(0), term(), NULL, 0);
    }
    if (consume("sizeof"))
    {
        bool is_parentheses = consume("(");
        Node *node = expr();
        if (node->type == NULL)
        {
            error_at(token->str, "型が未定義です");
        }

        if (is_parentheses)
        {
            expect(")");
        }
        if (node->type->kind == POINTER)
        {
            return new_node_num(8);
        }
        else if (node->type->kind == INT)
        {
            return new_node_num(4);
        }
        else if (node->type->kind = CHAR)
        {
            return new_node_num(1);
        }
    }

    return term();
}

Node *term()
{
    if (consume("("))
    {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok)
    {
        Node *node = (Node *)calloc(1, sizeof(Node));

        if (consume("("))
        {
            FunctionTable *func = find_function(tok);

            node->kind = ND_CALL_FUNC;
            node->lhs = (Node *)calloc(1, sizeof(Node));
            node->rhs = (Node *)calloc(1, sizeof(Node));
            node->other = (Node *)calloc(1, sizeof(Node));
            node->another = (Node *)calloc(1, sizeof(Node));
            node->option1 = (Node *)calloc(1, sizeof(Node));
            node->option2 = (Node *)calloc(1, sizeof(Node));

            bool is_declared_function = false;

            Node **arguments[6] = {&(node->lhs), &(node->rhs), &(node->other), &(node->another), &(node->option1), &(node->option2)};
            if (func)
            {
                is_declared_function = true;
            }
            else
            {
                // un-declared or in other file function
                is_declared_function = false;
                func = (FunctionTable *)calloc(1, sizeof(FunctionTable));
                func->name = tok->str;
                func->length = tok->len;

                FunctionTableLinkedList *list = (FunctionTableLinkedList *)calloc(1, sizeof(FunctionTableLinkedList));
                list->next = function_table;
                list->value = func;
                function_table = list;
            }

            node->function_table = func;

            int arg_index = 0;

            if (!consume(")"))
            {
                while (true)
                {
                    *(arguments[arg_index++]) = expr();

                    if (arg_index > 6)
                    {
                        error_at(token->str, "引数が多すぎます．");
                    }
                    if (consume(")"))
                    {
                        break;
                    }
                    else
                    {
                        expect(",");
                    }
                }
            }

            if (is_declared_function)
            {
                Variables *arguments = func->arguments;
                int count = 0;
                for (Variables *arg = arguments; arg; arg = arg->next)
                {
                    count++;
                }

                if (arg_index != count)
                {
                    error_at(token->str, "引数の数が一致しません．\n");
                }
            }
            else
            {
                // un-declared or in other file function
                Variables *argument_vars;
                char *tmp_arg_names[6] = {"a", "b", "c", "d", "e", "f"};
                if (arg_index == 0)
                {
                    argument_vars = NULL;
                }
                else
                {
                    argument_vars = (Variables *)calloc(1, sizeof(Variables));
                    argument_vars->len = 1;

                    argument_vars->name = tmp_arg_names[arg_index - 1];
                    Variables *args = argument_vars;
                    for (int i = arg_index - 1; i >= 0; i--)
                    {
                        Variables *arg = (Variables *)calloc(1, sizeof(Variables));
                        arg->len = 1;
                        arg->name = tmp_arg_names[i];
                        arg->scope = LOCAL;
                        args->next = arg;
                        args = arg;
                    }
                    argument_vars = args;
                }
                func->arguments = argument_vars;
            }

            for (int i = arg_index; i < 6; i++)
            {
                free(*arguments[i]);
                *arguments[i] = NULL;
            }
        }
        else
        {
            Variables *lvar = find_lvar(tok);

            if (lvar == NULL)
            {
                lvar = find_gvar(tok);
                node->kind = ND_GVAR;
            }
            else
            {
                node->kind = ND_LVAR;
            }

            if (lvar)
            {
                // found
                node->offset = lvar->offset;
                node->type = lvar->type;
                node->name = tok->str;
                node->name_length = tok->len;
                if (consume("["))
                { // array indexing
                    Node *index = expr();
                    Node *array = node;
                    Node *indexed_pointer = new_node(ND_ADD, array, index, NULL, 0);
                    indexed_pointer->type = array->type;
                    node = new_node(ND_DEREF, indexed_pointer, NULL, NULL, 0);
                    if (indexed_pointer->type->to_type == NULL || (array->type->kind != ARRAY && array->type->kind != POINTER))
                    {
                        error_at(token->str, "添字アクセスをサポートしていません．");
                    }
                    if (index->kind == ND_NUM)
                    {
                        int i = index->val;
                        if (i >= array->type->array_size)
                        {
                            warn_at(token->str, "配列外参照の可能性があります．");
                        }
                    }
                    node->type = indexed_pointer->type->to_type;
                    expect("]");
                }
            }
            else
            {
                error_at(token->str, "未定義の変数を使用しています．\n");
            }
        }
        return node;
    }
    Node *node = new_node_num(expect_number());
    return node;
}

FunctionTable *find_function(Token *tok)
{
    for (FunctionTableLinkedList *list = function_table; list; list = list->next)
    {
        if (list->value->length == tok->len && !memcmp(tok->str, list->value->name, tok->len))
        {
            return list->value;
        }
    }
    return NULL;
}

Variables *find_lvar(Token *tok)
{
    for (Variables *var = locals; var; var = var->next)
    {
        if (var->scope != LOCAL)
        {
            error("ローカル変数が正しく検出されていません．%d", var->scope);
        }
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
        {
            return var;
        }
    }
    return NULL;
}

Variables *find_gvar(Token *tok)
{
    for (Variables *var = globals; var; var = var->next)
    {
        if (var->scope != GLOBAL)
        {
            error("ローカル変数が正しく検出されていません．");
        }
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len))
        {
            return var;
        }
    }
    return NULL;
}

int is_alnum(char c)
{
    return ('a' <= c && c <= 'z') ||
           ('A' <= c && c <= 'Z') ||
           ('0' <= c && c <= '9') ||
           (c == '_');
}