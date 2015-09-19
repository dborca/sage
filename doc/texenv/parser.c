#include <assert.h>

#include "lexer.h"
#include "tree.h"
#include "util.h"


#define IS(x) (token.type == (x))


static NODE *do_expr (void);
static NODE *do_term (void);
static NODE *do_fact (void);


/* RULE
 * <expr>::= <term>
 *         | <term> + <term>
 *         | <term> - <term>
 *
 * where:
 *
 * FIRST
 *    (, +, -, ID
 */
static NODE *
do_expr (void)
{
    NODE *n = do_term();
    if (IS(T_ADD)) {
	NODE *top = tree_node_op('+');
	top->left = n;
	next_token(); /* eat `+' */
	top->right = do_term();
	n = top;
    } else if (IS(T_SUB)) {
	NODE *top = tree_node_op('-');
	top->left = n;
	next_token(); /* eat `-' */
	top->right = do_term();
	n = top;
    }
    return n;
}


/* RULE
 * <term>::= <fact>
 *         | <fact> * <fact>
 *         | <fact> / <fact>
 *
 * where:
 *
 * FIRST
 *    (, +, -, ID
 */
static NODE *
do_term (void)
{
    NODE *n = do_fact();
    if (IS(T_MUL)) {
	NODE *top = tree_node_op('*');
	top->left = n;
	next_token(); /* eat `*' */
	top->right = do_fact();
	n = top;
    } else if (IS(T_DIV)) {
	NODE *top = tree_node_op('/');
	top->left = n;
	next_token(); /* eat `/' */
	top->right = do_fact();
	n = top;
    }
    return n;
}


/* RULE
 * <fact>::= `(' <expr> ')'
 *         | `+' <fact>
 *         | `-' <fact>
 *         | ID
 *
 * where:
 *
 * FIRST
 *    (, +, -, ID
 */
static NODE *
do_fact (void)
{
    NODE *n;
    if (IS(T_OB)) {
	next_token(); /* eat `(' */
	n = do_expr();
	if (!IS(T_CB)) {
	    assert(0);
	}
	next_token(); /* eat `)' */
    } else if (IS(T_ADD)) {
	next_token(); /* eat `+' */
	n = do_fact();
    } else if (IS(T_SUB)) {
	NODE *top = tree_node_op('-');
	top->left = 0;
	next_token(); /* eat `-' */
	top->right = do_fact();
	n = top;
    } else if (IS(T_ID)) {
	n = tree_node_var(token.sym);
	next_token(); /* eat ID */
    } else {
	assert(0);
    }

    return n;
}


NODE *
parse (const char *input)
{
    set_input(input);
    next_token();
    return do_expr();
}
