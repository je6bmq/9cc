#include "9cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

Token *token; // current token
char *user_input;
Node *code[100];
Function *functions[100];
LVar *locals;
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
    if ((token->kind != TK_RESERVED && token->kind != TK_RETURN && token->kind != TK_IF && token->kind != TK_ELSE && token->kind != TK_FOR && token->kind != TK_WHILE) || strlen(op) != token->len || memcmp(token->str, op, token->len))
    {
        return false;
    }
    token = token->next;
    return true;
}

Token *consume_ident()
{
    if (token->kind != TK_IDENT || token->str[0] < 'a' || token->str[0] > 'z')
    {
        return NULL;
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

        if (memcmp(p, ";", 1) == 0 || memcmp(p, "(", 1) == 0 || memcmp(p, ")", 1) == 0 || memcmp(p, "{", 1) == 0 || memcmp(p, "}", 1) == 0 || memcmp(p, ",", 1) == 0)
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

        if (isdigit(*p))
        {
            cur = new_token(TK_NUM, cur, p, 1);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        if ('a' <= *p && *p <= 'z')
        {
            int name_count = 0;
            while ('a' <= *(p + name_count) && *(p + name_count) <= 'z')
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

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = calloc(1, sizeof(Node));
    node->id = current_node_id++;
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;

    return node;
}

Node *new_node_num(int val)
{
    Node *node = calloc(1, sizeof(Node));
    node->id = current_node_id++;
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

Function *function()
{
    Function *func = (Function *)calloc(1, sizeof(Function));

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

    if (function_table == NULL)
    {
        FunctionTableLinkedList *list = (FunctionTableLinkedList *)calloc(1, sizeof(FunctionTableLinkedList));

        FunctionTable *func_table = (FunctionTable *)calloc(1, sizeof(FunctionTable));
        func_table->name = token->str;
        func_table->length = token->len;
        list->value = func_table;
        function_table = list;
    }
    func->name = token->str;
    func->length = token->len;
    token = token->next;
    expect("(");
    // LVar *arguments = (LVar *)calloc(1, sizeof(LVar));
    // arguments->offset = 0;
    LVar *arguments = NULL;
    if (!consume(")"))
    {
        while (true)
        {
            LVar *arg = (LVar *)calloc(1, sizeof(LVar));
            if (token->kind != TK_IDENT)
            {
                fprintf(stderr, "変数名が不正です．\n");
                exit(1);
            }
            arg->name = token->str;
            arg->len = token->len;

            int current_offset = 0;
            for (LVar *var = arguments; var; var = var->next)
            {
                current_offset = var->offset;
            }
            arg->offset = current_offset + 0x10;

            if (arguments == NULL)
            {
                arguments = arg;
                arguments->next = NULL;
            }
            else
            {
                LVar *last_arg;
                for (LVar *var = arguments; var; var = var->next)
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

    locals = arguments; // limit local variables in current scope
    expect("{");
    while (!consume("}"))
    {
        push(func->statements, stmt());
    }
    func->arguments = locals; // set "changed" local variables in current function.
    return func;
}

void program()
{
    int i = 0;
    while (!at_eof())
    {
        // code[i++] = stmt();

        functions[i++] = function();
    }
    // code[i] = NULL;
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
        node = new_node(ND_IF, condition, if_true_statement);
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
        node = new_node(ND_WHILE, condition, process);
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
            condition = new_node(ND_LESS_THAN, new_node_num(0), new_node_num(1));
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

        node = new_node(ND_FOR, initial, condition);

        Node *statement;
        statement = stmt();
        node->other = updater;
        node->another = statement;

        return node;
    }

    if (consume("{"))
    {
        node = new_node(ND_BLOCK, NULL, NULL);
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
        node->kind = ND_RETURN;
        node->lhs = expr();
    }
    else
    {
        node = expr();
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
        node = new_node(ND_ASSIGN, node, assign());
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
            node = new_node(ND_EQUAL, node, relational());
        }
        else if (consume("!="))
        {
            node = new_node(ND_NOT_EQUAL, node, relational());
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
            node = new_node(ND_LESS_THAN, node, add());
        }
        else if (consume("<="))
        {
            node = new_node(ND_LESS_EQUAL, node, add());
        }
        else if (consume(">"))
        {
            node = new_node(ND_LESS_THAN, add(), node);
        }
        else if (consume(">="))
        {
            node = new_node(ND_LESS_EQUAL, add(), node);
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
        if (consume("+"))
        {
            node = new_node(ND_ADD, node, mul());
        }
        else if (consume("-"))
        {
            node = new_node(ND_SUB, node, mul());
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
        if (consume("*"))
        {
            node = new_node(ND_MUL, node, unary());
        }
        else if (consume("/"))
        {
            node = new_node(ND_DIV, node, unary());
        }
        else
        {
            return node;
        }
    }
}

Node *unary()
{
    if (consume("+"))
    {
        return term();
    }
    if (consume("-"))
    {
        return new_node(ND_SUB, new_node_num(0), term());
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
        Node *node = calloc(1, sizeof(Node));

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

            Node **arguments[6] = {&(node->lhs), &(node->rhs), &(node->other), &(node->another), &(node->option1), &(node->option2)};
            if (function)
            {
            }
            else
            {
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
                    *arguments[arg_index++] = expr();
                    if (arg_index >= 6)
                    {
                        fprintf(stderr, "引数過多です．\n");
                        exit(1);
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

            for (int i = arg_index; i < 6; i++)
            {
                free(*arguments[i]);
                *arguments[i] = NULL;
            }
            // expect(")");
        }
        else
        {
            LVar *lvar = find_lvar(tok);
            node->kind = ND_LVAR;
            if (lvar)
            {
                // found
                node->offset = lvar->offset;
            }
            else
            {
                LVar *lvar_next = calloc(1, sizeof(LVar));
                for (LVar *var = locals; var; var = var->next)
                {
                    lvar = var;
                }
                lvar_next->name = tok->str;
                lvar_next->len = tok->len;
                lvar_next->offset = (lvar ? lvar->offset : 0) + 0x10;
                node->offset = lvar_next->offset;

                if (lvar)
                {
                    lvar->next = lvar_next;
                }
                else
                {
                    if (locals)
                    {
                        locals->next = lvar_next;
                    }
                    else
                    {
                        locals = lvar_next;
                    }
                }
            }
        }

        return node;
    }

    return new_node_num(expect_number());
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

LVar *find_lvar(Token *tok)
{
    for (LVar *var = locals; var; var = var->next)
    {
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