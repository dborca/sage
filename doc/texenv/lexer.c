#include <assert.h>
#include <ctype.h>
#include <stdlib.h>

#include "lexer.h"


#define FALSE 0
#define TRUE !FALSE

typedef enum {
    S_INIT,
    S_ID,
    S_OP,	/* operator: +, -, *, / */
    S_OB,	/* openbrace */
    S_CB,	/* closebrace */
    S_ERROR,
    S_RESET,
    S_QUIT
} S_STATE;


static int ch;
static const char *input;
static int pos;
static S_STATE state;
TOKEN token;


void
set_input (const char *i)
{
    input = i;
    pos = 0;
    state = S_INIT;
}


static int
get_char (void)
{
    ch = input[pos];
    if (ch) {
	pos++;
    }
    return ch;
}


static int
get_nextchar (void)
{
    return input[pos];
}


static int
tok_append (void)
{
    if (token.len == token.max) {
	char *p = realloc(token.sym, token.max + 256 + 1);
	if (p == NULL) {
	    return FALSE;
	}
	token.sym = p;
	token.max += 256;
    }
    token.sym[token.len++] = ch;
    return TRUE;
}


static void
cleanup (void)
{
    if (token.sym != NULL) {
	free(token.sym);
	token.sym = NULL;
	token.max = 0;
    }
    token.type = T_EOI;
    state = S_QUIT;
}


static void
fix_token (void)
{
    token.sym[token.len] = '\0';
    if (state == S_OP) {
	switch (token.sym[0]) {
	    case '+':
		token.type = T_ADD;
		break;
	    case '-':
		token.type = T_SUB;
		break;
	    case '*':
		token.type = T_MUL;
		break;
	    case '/':
		token.type = T_DIV;
		break;
	}
    } else if (state == S_OB) {
	token.type = T_OB;
    } else if (state == S_CB) {
	token.type = T_CB;
    } else if (state == S_ID) {
	token.type = T_ID;
    } else {
	assert(0);
    }
}


static S_STATE
next_state (int nch, S_STATE current)
{
    static const S_STATE scantab[][5] = {
	/*INIT  ID      OP      OB      CB*/
/*Nul*/{S_QUIT,	S_QUIT,	S_QUIT,	S_QUIT,	S_QUIT },
/*   */{S_INIT,	S_RESET,S_RESET,S_RESET,S_RESET},
/* A */{S_ID,	S_ID,	S_RESET,S_RESET,S_RESET},
/* + */{S_OP,	S_RESET,S_RESET,S_RESET,S_RESET},
/* - */{S_OP,	S_RESET,S_RESET,S_RESET,S_RESET},
/* * */{S_OP,	S_RESET,S_RESET,S_RESET,S_RESET},
/* / */{S_OP,	S_RESET,S_RESET,S_RESET,S_RESET},
/* ( */{S_OB,	S_RESET,S_RESET,S_RESET,S_RESET},
/* ) */{S_CB,	S_RESET,S_RESET,S_RESET,S_RESET},
/*Oth*/{S_ERROR,S_ERROR,S_ERROR,S_ERROR,S_ERROR}
    };

    if (isspace(nch)) {
	nch = 1;
    } else if (isalnum(nch)) {
	nch = 2;
    } else if (nch == '+') {
	nch = 3;
    } else if (nch == '-') {
	nch = 4;
    } else if (nch == '*') {
	nch = 5;
    } else if (nch == '/') {
	nch = 6;
    } else if (nch == '(') {
	nch = 7;
    } else if (nch == ')') {
	nch = 8;
    } else if (nch != '\0') {
	nch = 9;
    }
    return scantab[nch][current];
}


void
get_token (void)
{
    token.len = 0;

    if (state == S_QUIT) {
	cleanup();
	return;
    }

    do {
	int nch = get_nextchar(); /* the new character */
	S_STATE nstate = next_state(nch, state); /* new state */

	if (nstate == S_RESET) {
	    fix_token();
	    state = S_INIT;
	    return;
	} else if (nstate == S_ERROR) {
	    assert(0);
	} else if (nstate == S_QUIT) {
	    /* return the token here */
	    if (token.len) {
		fix_token();
		state = S_QUIT;
		return;
	    } else {
		cleanup();
		return;
	    }
	} else {
	    ch = get_char();
	    if (!isspace(ch)) {
		if (!tok_append()) {
		    assert(0);
		}
	    }
	}

	if (nstate == S_INIT) {
	    /* return the token here */
	    if (token.len) {
		fix_token();
		state = S_INIT;
		return;
	    }
	}
	state = nstate;
    } while (TRUE);
}
