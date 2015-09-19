#ifndef TREE_H_included
#define TREE_H_included

typedef enum {
    N_OP,
    N_VAR
} N_TYPE;

typedef struct NODE {
    struct NODE *left, *right;
    N_TYPE type;
    union {
	int ch;
	const char *name;
    } val;
} NODE;

NODE *tree_node_op (int op);
NODE *tree_node_var (const char *var);
void tree_display (NODE *node);
void tree_destroy (NODE *node);
int tree_check_var (NODE *node, const char *var);
NODE **tree_find_var (NODE *node, const char *var);
const char *tree_string (NODE *node, int init);

#endif
