#ifndef _NINE_CC
#define _NINE_CC
#include <stdbool.h>

typedef enum
{
    TK_RESERVED, // reserved symbol
    TK_IDENT,    // identifier
    TK_NUM,      // integer token
    TK_EOF,      // EOF(End Of File) Token
} TokenKind;

typedef struct Token Token;

struct Token
{
    TokenKind kind; // kind(type) of token
    Token *next;    // next token pointer
    int val;        // evaluated value if kind == TK_NUM
    char *str;      // token string
    int len;        // token length
};

typedef enum
{
    ND_ADD,        // +
    ND_SUB,        // -
    ND_MUL,        // *
    ND_DIV,        // /
    ND_EQUAL,      // ==
    ND_NOT_EQUAL,  // !=
    ND_LESS_THAN,  // <
    ND_LESS_EQUAL, // <=
    ND_ASSIGN,     // =
    ND_LVAR,       // local variable
    ND_NUM,        // integer
} NodeKind;

typedef struct Node Node;

struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val; // used if kind == ND_NUM
    int offset; // stack offset for variable(ND_LVAR)
};

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();

void program();
Node *stmt();
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
Node *new_node_num(int val);

Token *new_token(TokenKind kind, Token *cur, char *str, int len);
void tokenize();

void gen(Node *node);
/*
    global variables
 */
extern Token *token; // current token
extern char *user_input;
extern Node *code[100];

#endif
