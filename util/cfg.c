/**
 * Read configuration files variable/value pairs into a linked list.
 * The linked list is cumulative (variables from new files are appended
 * to the list), but variables with the same name overwrite the old ones.
 * This file does not need explicit initialization.
 * Destruction is registered with atexit().
 */


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "cfg.h"


/**
 * VAR = VAL pair
 */
typedef struct ENTRY {
    struct ENTRY *next, *prev;
    char *val;		/**< value */
    char var[1];	/**< variable */
} ENTRY;


/** configuration setting list */
static ENTRY cfg_list = { &cfg_list, &cfg_list, NULL, {0} };


static int
cfg_isspace (int c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}


/**
 * Load configuration file.
 *
 * \param filename config file
 *
 * \return 0 if success
 */
int
cfg_load (const char *filename)
{
    FILE *f;
    char line[BUFSIZ];

    f = fopen(filename, "rb");
    if (f == NULL) {
	return -1;
    }

    while (fgets(line, BUFSIZ, f)) {
	char *beg, *end, *mid;
	int len;
	ENTRY *e;

	beg = line;
	while (cfg_isspace(*beg)) {
	    beg++;
	}

	end = beg + strcspn(beg, "#;");
	if (end == beg) {
	    continue;
	}
	while (cfg_isspace(*(end - 1))) {
	    end--;
	}
	*end = '\0';

	mid = strchr(beg, '=');
	if (mid == NULL) {
	    continue;		/* no `=' */
	}

	if (mid == beg) {
	    continue;		/* `=abc' */
	}

	end = mid;
	*mid++ = '\0';
	if (*mid == '\0') {
	    continue;		/* `abc=' */
	}
	while (cfg_isspace(*mid)) {
	    mid++;
	}

	while (cfg_isspace(*(end - 1))) {
	    end--;
	}
	*end = '\0';

	len = strlen(beg);
	list_foreach (e, &cfg_list) {
	    if (!strcmp(e->var, beg)) {
		break;
	    }
	}
	if (!list_at_end(&cfg_list, e)) {
	    if (!strcmp(e->val, mid)) {
		continue;
	    }
	    list_remove(e);
	    free(e);
	}
	e = malloc(offsetof(ENTRY, var) + len + 1 + strlen(mid) + 1);
	if (e != NULL) {
	    e->val = e->var + len + 1;
	    strcpy(e->var, beg);
	    strcpy(e->val, mid);
	    list_append(&cfg_list, e);
	}
    }

    fclose(f);
    atexit(cfg_kill);
    return 0;
}


/**
 * Free configuration list.  May be called multiple times.
 */
void
cfg_kill (void)
{
    ENTRY *c, *tmp;
    list_foreach_s (c, tmp, &cfg_list) {
	list_remove(c);
	free(c);
    }
}


/**
 * Get string value for given variable.
 *
 * \param var variable name
 * \param def default value if variable not found
 *
 * \return variable
 */
const char *
cfg_get (const char *var, const char *def)
{
    ENTRY *c;
    list_foreach (c, &cfg_list) {
	if (!strcmp(c->var, var)) {
	    return c->val;
	}
    }
    return def;
}


/**
 * Browse all variables.
 *
 * \param fn callback function to be called for each variable
 *
 * \return 0 if the callback returns 0 for all variables
 * otherwise the value returned by the callback (and the browsing stops)
 */
int
cfg_browse (int (*fn) (const char *var, const char *val))
{
    ENTRY *c;
    list_foreach (c, &cfg_list) {
	int rv = fn(c->var, c->val);
	if (rv) {
	    return rv;
	}
    }
    return 0;
}
