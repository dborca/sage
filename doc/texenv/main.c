#include <stdio.h>

#include "tree.h"
#include "parser.h"


#ifdef TEST


int
main (void)
{
    NODE *n = parse("Cp * (1 - Cs) + Cc * Cs");
    tree_display(n);
    tree_destroy(n);
    printf("\n");
    return 0;
}


#else  /* !TEST */


#include <string.h>


#define E_REPLACE		0
#define E_MODULATE		1
#define E_DECAL			2
#define E_BLEND			3
#define E_ADD			4
#define E_MAX			5

#define F_ALPHA			0
#define F_LUMINANCE		1
#define F_LUMINANCE_ALPHA	2
#define F_INTENSITY		3
#define F_RGB			4
#define F_RGBA			5
#define F_MAX			6

#define O_COLOR			0
#define O_ALPHA			1
#define O_MAX			2


static const char *e_tab[] = {
    "REPLACE",
    "MODULATE",
    "DECAL",
    "BLEND",
    "ADD"
};


static const char *f_tab[] = {
    "ALPHA",
    "LUMINANCE",
    "LUMINANCE_ALPHA",
    "INTENSITY",
    "RGB",
    "RGBA"
};


static const char *env[E_MAX][F_MAX][O_MAX] = {
    {			/* REPLACE */
	/* ALPHA */		{ "Cp",				"As" },
	/* LUMINANCE */		{ "Cs",				"Ap" },
	/* LUMINANCE_ALPHA */	{ "Cs",				"As" },
	/* INTENSITY */		{ "Cs",				"As" },
	/* RGB */		{ "Cs",				"Ap" },
	/* RGBA */		{ "Cs",				"As" }
    },
    {			/* MODULATE */
	/* ALPHA */		{ "Cp",				"Ap * As" },
	/* LUMINANCE */		{ "Cp * Cs",			"Ap" },
	/* LUMINANCE_ALPHA */	{ "Cp * Cs",			"Ap * As" },
	/* INTENSITY */		{ "Cp * Cs",			"Ap * As" },
	/* RGB */		{ "Cp * Cs",			"Ap" },
	/* RGBA */		{ "Cp * Cs",			"Ap * As" }
    },
    {			/* DECAL */
	/* ALPHA */		{ "undefined",			"undefined" },
	/* LUMINANCE */		{ "undefined",			"undefined" },
	/* LUMINANCE_ALPHA */	{ "undefined",			"undefined" },
	/* INTENSITY */		{ "undefined",			"undefined" },
	/* RGB */		{ "Cs",				"Ap" },
	/* RGBA */		{ "Cp * (1 - As) + Cs * As",	"Ap" }
    },
    {			/* BLEND */
	/* ALPHA */		{ "Cp",				"Ap * As" },
	/* LUMINANCE */		{ "Cp * (1 - Cs) + Cc * Cs",	"Ap" },
	/* LUMINANCE_ALPHA */	{ "Cp * (1 - Cs) + Cc * Cs",	"Ap * As" },
	/* INTENSITY */		{ "Cp * (1 - Cs) + Cc * Cs",	"Ap * (1 - As) + Ac * As" },
	/* RGB */		{ "Cp * (1 - Cs) + Cc * Cs",	"Ap" },
	/* RGBA */		{ "Cp * (1 - Cs) + Cc * Cs",	"Ap * As" }
    },
    {			/* ADD */
	/* ALPHA */		{ "Cp",				"Ap * As" },
	/* LUMINANCE */		{ "Cp + Cs",			"Ap" },
	/* LUMINANCE_ALPHA */	{ "Cp + Cs",			"Ap * As" },
	/* INTENSITY */		{ "Cp + Cs",			"Ap + As" },
	/* RGB */		{ "Cp + Cs",			"Ap" },
	/* RGBA */		{ "Cp + Cs",			"Ap * As" }
    }
};


#define REPLACE(node, var, repl)		\
    if (tree_check_var(node, var)) {		\
	tree_destroy(node);			\
	node = repl;				\
    } else {					\
	do {					\
	    NODE **p = tree_find_var(node, var);\
	    if (p == NULL) {			\
		break;				\
	    }					\
	    tree_destroy(*p);			\
	    *p = repl;				\
	} while (!0);				\
    }


NODE *
texenv (int u, int e, int f, int o, NODE *cp, NODE *ap)
{
    NODE *n = parse(env[e][f][o]);
    if (u >= 0) {
	char cs[16];
	char as[16];
	sprintf(cs, "C%d", u);
	sprintf(as, "A%d", u);

	REPLACE(n, "Cs", tree_node_var(cs));
	REPLACE(n, "As", tree_node_var(as));
	REPLACE(n, "Cp", cp);
	REPLACE(n, "Ap", ap);
    }
    return n;
}


int
main (void)
{
#if 0
    int i, j, k;
    for (j = 0; j < F_MAX; j++) {
	for (k = 0; k < O_MAX; k++) {
	    for (i = 0; i < E_MAX; i++) {
		printf("\t%s", env[e][f][o]);
	    }
	    printf("\n");
	}
    }
#else
    NODE *fr = tree_node_var("f");
    int i, ii, j, jj;
    i = E_MODULATE;
    ii = E_MODULATE;
    printf("%s -> %s\n\n", e_tab[i], e_tab[ii]);
    for (j = 0; j < F_MAX; j++) {
	for (jj = 0; jj < F_MAX; jj++) {
	    /* Voodoo: T1 -> T0 */
	    NODE *c1 = texenv(1, i,  j,  O_COLOR, fr, fr);
	    NODE *a1 = texenv(1, i,  j,  O_ALPHA, fr, fr);
	    NODE *c0 = texenv(0, ii, jj, O_COLOR, c1, a1);
	    NODE *a0 = texenv(0, ii, jj, O_ALPHA, c1, a1);
	    char tmp[1024];
	    strcpy(tmp, f_tab[j]);
	    strcat(tmp, "/");
	    strcat(tmp, f_tab[jj]);
	    printf("%-32s", tmp);
	    strcpy(tmp, tree_string(c0, 1));
	    printf("%-32s", tmp);
	    tree_display(a0);
	    printf("\n");
	}
    }
#endif
    return 0;
}


#endif /* !TEST */
