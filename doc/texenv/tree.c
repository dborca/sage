#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "tree.h"


#define PRECEDENCE(f, s) \
    ( \
     (((f)->val.ch == '*' || (f)->val.ch == '/') && \
      (s)->type == N_OP && ((s)->val.ch == '+' || (s)->val.ch == '-')) \
    || \
     ((f)->val.ch == '-' && \
      (s)->type == N_OP && (s)->val.ch != '*' && (s)->val.ch != '/') \
    )


NODE *
tree_node_op (int op)
{
    NODE *n = malloc(sizeof(NODE));
    assert(n);
    n->type = N_OP;
    n->val.ch = op;
    return n;
}


NODE *
tree_node_var (const char *var)
{
    NODE *n = malloc(sizeof(NODE));
    assert(n);
    n->type = N_VAR;
    n->val.name = str_dup(var);
    assert(n->val.name);
    return n;
}


void
tree_display (NODE *node)
{
    if (node->type == N_OP) {
	if (node->left) {
	    if (PRECEDENCE(node, node->left)) {
		printf("(");
	    }
	    tree_display(node->left);
	    if (PRECEDENCE(node, node->left)) {
		printf(")");
	    }
	    printf(" %c ", node->val.ch);
	} else {
	    printf("%c", node->val.ch);
	}
	if (PRECEDENCE(node, node->right)) {
	    printf("(");
	}
	tree_display(node->right);
	if (PRECEDENCE(node, node->right)) {
	    printf(")");
	}
    } else {
	printf("%s", node->val.name);
    }
}


void
tree_destroy (NODE *node)
{
    if (node->type == N_OP) {
	if (node->left) {
	    tree_destroy(node->left);
	}
	tree_destroy(node->right);
    } else {
	free((char *)node->val.name);
    }
    free(node);
}


int
tree_check_var (NODE *node, const char *var)
{
    return (node->type == N_VAR && !strcmp(node->val.name, var));
}


NODE **
tree_find_var (NODE *node, const char *var)
{
    NODE **rv;
    if (node->type == N_OP) {
	if (node->left != NULL) {
	    if (tree_check_var(node->left, var)) {
		return &node->left;
	    }
	    rv = tree_find_var(node->left, var);
	    if (rv != NULL) {
		return rv;
	    }
	}
	if (tree_check_var(node->right, var)) {
	    return &node->right;
	}
	rv = tree_find_var(node->right, var);
	if (rv != NULL) {
	    return rv;
	}
    }
    return NULL;
}


const char *
tree_string (NODE *node, int init)
{
    static char buf[1024];
    static int len;

    if (init) {
	len = 0;
    }

    if (node->type == N_OP) {
	if (node->left) {
	    if (PRECEDENCE(node, node->left)) {
		len += sprintf(&buf[len], "(");
	    }
	    tree_string(node->left, 0);
	    if (PRECEDENCE(node, node->left)) {
		len += sprintf(&buf[len], ")");
	    }
	    len += sprintf(&buf[len], " %c ", node->val.ch);
	} else {
	    len += sprintf(&buf[len], "%c", node->val.ch);
	}
	if (PRECEDENCE(node, node->right)) {
	    len += sprintf(&buf[len], "(");
	}
	tree_string(node->right, 0);
	if (PRECEDENCE(node, node->right)) {
	    len += sprintf(&buf[len], ")");
	}
    } else {
	len += sprintf(&buf[len], "%s", node->val.name);
    }
    return buf;
}
