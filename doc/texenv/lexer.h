#ifndef LEXER_H_included
#define LEXER_H_included

typedef enum {
    T_ID,
    T_ADD,
    T_SUB,
    T_MUL,
    T_DIV,
    T_OB,
    T_CB,
    T_EOI
} TOKTYPE;

typedef struct {
    TOKTYPE type;
    int len, max;
    char *sym;
} TOKEN;

extern TOKEN token;

void set_input (const char *i);
void get_token (void);

#endif
