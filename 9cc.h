#ifndef _NINE_CC
#define _NINE_CC
#include <stdbool.h>

typedef enum
{
    TK_RESERVED, // reserved symbol
    TK_IDENT,    // identifier
    TK_NUM,      // integer token
    TK_EOF,      // EOF(End Of File) Token
    TK_RETURN,   // return statement
    TK_IF,       // if statement
    TK_ELSE,     // else statement
    TK_WHILE,    // while statement
    TK_FOR,      // while statement
    TK_SIZEOF,   // sizeof operator
    TK_STRING,   // string
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
    ND_GVAR,       // global variable
    ND_NUM,        // integer
    ND_RETURN,     // return
    ND_IF,         // if statement
    ND_WHILE,      // while statement
    ND_FOR,        // while statement
    ND_BLOCK,      // code block
    ND_CALL_FUNC,  // call function
    ND_ADDR,       // & (address operator)
    ND_DEREF,      //  * (dereference operator)
    ND_DECL,       // variable declaration
    ND_STRING,     // string
    ND_INIT_ARRAY, // array intiializer
    ND_INIT_LOCAL // local variable (not array ) initializer
} NodeKind;

typedef struct NodeReferenceVector NodeReferenceVector;
typedef struct TemporaryStringVector TemporaryStringVector;
typedef struct String String;

typedef struct Node Node;
typedef struct FunctionTable FunctionTable;
typedef struct Function Function;
typedef struct Variables Variables;
typedef struct Type Type;

struct FunctionTable
{
    char *name;
    int length;
    Type *return_type;
    Variables *arguments;
};

typedef struct FunctionTableLinkedList FunctionTableLinkedList;

struct FunctionTableLinkedList
{
    FunctionTableLinkedList *next;
    FunctionTable *value;
};

struct Function
{
    char *name;
    int length;
    Variables *arguments;
    Variables *local_variables;
    NodeReferenceVector *statements;
};

struct Node
{
    int id;
    char *name;
    int name_length;
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Node *other;
    Node *another;
    Node *option1;
    Node *option2;
    int val;                         // used if kind == ND_NUM
    int offset;                      // stack offset for variable(ND_LVAR)
    NodeReferenceVector *statements; // used if block code (ND_BLOCK)
    FunctionTable *function_table;   // used if ND_CALL_FUNC
    Type *type;
};

struct NodeReferenceVector
{
    int size;
    int capacity;
    Node **elements;
    int element_size;
};

struct TemporaryStringVector
{
    int size;
    int capacity;
    String **elements;
};

struct String
{
    int id;
    char *value_str;
    int value_len;
};

typedef enum
{
    INT,
    POINTER,
    CHAR,
    ARRAY,
} TypeKind;

struct Type
{
    TypeKind kind;
    struct Type *to_type;
    int array_size;
};

typedef enum
{
    BLOCK,
    LOCAL,
    GLOBAL
} Scope;

struct Variables
{ // local variable list presented by linked list
    Variables *next;
    char *name;
    int len;
    int offset;
    Type *type;
    Scope scope;
    int *initial_value_ptr;
};

typedef struct
{
    Variables *pointer; // in address operation , it cannot use multiple pointer value.
    int const_value;
} ConstValue;

void error_at(char *loc, char *fmt, ...);
void warn_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
void warn(char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
char *read_file(char *path);

/* --production rule--

    program = ("int" ident ("[" num "]") ";") | function*
    function = "int" ident ( "int" ident,*) { stmt*}
    stmt    = expr ";"
            | "{" stmt* "}"
            | "return" expr ";"
            | "int" | "char" ident ("=" expr)? ";"
    expr       = assign
    assign     = equality ("=" assign)?
    equality   = relational ("==" relational | "!=" relational)*
    relational = add ("<" add | "<=" add | ">" add | ">=" add)*
    add        = mul ("+" mul | "-" mul)*
    mul        = unary ("*" unary | "/" unary)*
    unary      = "+"? term | "-"? term | "*" unary | "&" unary | "sizeof" unary
    term       = num 
               | ident ("(" expr,* ")")?
               | "(" expr ")"
               | "\" string* \""
*/

int desired_stack_size(Type *type);
Type *expect_type();
Type *consume_type();
Variables *add_variables(Variables **variables_ptr, TypeKind element_kind, Scope scope); // return latest variable
void program();
Function *function();
Node *stmt();
Node *assign();
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *term();
Node *new_node(NodeKind kind, Node *lhs, Node *rhs, char *name, int name_length);
Node *new_node_num(int val);

Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *consume_ident();
Token *expect_ident();
Variables *find_lvar(Token *tok);
Variables *find_gvar(Token *tok);
FunctionTable *find_function(Token *tok);
void tokenize();
int is_alnum(char c);

void gen_global(Node *node);
ConstValue *get_const_expr(Node *node, int u);
void gen_global_lval(Node *node);
void gen_lval(Node *node);
void gen(Node *node);

NodeReferenceVector *new_node_vec();
void push_node(NodeReferenceVector *vector, Node *new_element);
Node *get_node(NodeReferenceVector *vector, int index);
TemporaryStringVector *new_string_vec();
void push_string(TemporaryStringVector *vector, String *new_element);
String *get_string(TemporaryStringVector *vector, int index);

/*
    global variables
 */

extern Token *token; // current token
extern char *user_input;
extern Function *functions[100];
extern Variables *locals;
extern Variables *globals;
extern FunctionTableLinkedList *function_table;
extern int current_node_id;
extern int temporary_string_id;
extern TemporaryStringVector *string_vector;
extern char *file_name;
extern NodeReferenceVector *global_expressions;

#endif
