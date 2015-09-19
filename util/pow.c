#include <math.h>
#include <stdlib.h>

#include "GL/gl.h"

#include "macros.h"
#include "list.h"
#include "pow.h"


#define MAX_LUT_ENTRIES	32


static POW_LUT *pow_list;
static GLuint pow_size;


/*
 * In Euclidean space, the dot product of two unit
 * vectors is simply the cosine of the angle between them.
 *
 * Hence, our tables are [0..1] ^ [0..128]
 */
POW_LUT *
pow_make (POW_LUT *table, GLfloat exp)
{
    int i;
    GLfloat x, dx, scale, threshold, *tableEntry;

    if (table == NULL) {
	table = malloc(sizeof(POW_LUT));
    }
    if (table == NULL) {
	return NULL;
    }

    /* Compute threshold */
    if (exp == 0.0F) {
	threshold = 0.0F;
    } else {
	threshold = POW(0.0007F, 1.0F / exp);
    }

    scale = (POW_LUT_SIZE - 1) / (1.0F - threshold);
    dx = 1.0F / scale;
    x = threshold;
    tableEntry = table->table;
    for (i = POW_LUT_SIZE; --i >= 0; ) {
	*tableEntry++ = POW(x, exp);
	x += dx;
    }
    *tableEntry = 1.0F;
    table->threshold = threshold;
    table->scale = scale;
    table->refcount = 2; /* both cache and calling code refer to table */
    table->exp = exp;
    return table;
}


int
pow_init (void)
{
    pow_size = 0;
    pow_list = malloc(2 * sizeof(void *));
    if (pow_list == NULL) {
	return -1;
    }
    list_create(pow_list);
    return 0;
}


POW_LUT *
pow_scan (GLfloat exp)
{
    POW_LUT *ptr;
    list_foreach (ptr, pow_list) {
	if (ptr->exp == exp) {
	    return ptr;
	}
    }
    if (pow_size >= MAX_LUT_ENTRIES) {
	/* too many tables already.  start reusing them */
	ptr = list_last(pow_list);
	list_remove(ptr);
	pow_size--;
	ptr = pow_make(ptr, exp);
    } else {
	/* really create a new table */
	ptr = pow_make(NULL, exp);
    }
    if (ptr != NULL) {
	/* move the table to front */
	list_prepend(pow_list, ptr);
	pow_size++;
    }
    return ptr;
}


void
pow_fini (void)
{
    POW_LUT *ptr, *tmp;
    list_foreach_s (ptr, tmp, pow_list) {
	list_remove(ptr);
	free(ptr);
    }
    if (pow_list != NULL) {
	free(pow_list);
	pow_list = NULL;
    }
    pow_size = 0;
}


#define FAST_POW(dp, exp, tab)	\
do {				\
    GLfloat f;			\
    GLuint k;			\
    if (tab == NULL) {		\
	return POW(dp, exp);	\
    }				\
    f = dp - tab->threshold;	\
    if (IS_NEGATIVE(f)) {	\
	return 0.0F;		\
    }				\
    f = f * tab->scale + 0.5F;	\
    k = (GLuint)f;		\
    if (k >= POW_LUT_SIZE) {	\
	return 1.0F;		\
    }				\
    return tab->table[k];	\
} while (0)


#define LERP_POW(dp, exp, tab)	\
do {				\
    GLfloat f;			\
    GLuint k;			\
    GLfloat v;			\
    if (tab == NULL) {		\
	return POW(dp, exp);	\
    }				\
    f = dp - tab->threshold;	\
    if (IS_NEGATIVE(f)) {	\
	return 0.0F;		\
    }				\
    f = f * tab->scale;		\
    k = (GLuint)f;		\
    if (k >= POW_LUT_SIZE) {	\
	return 1.0F;		\
    }				\
    v = tab->table[k];		\
    return v + (f - k) * (tab->table[k + 1] - v);\
} while (0)


GLfloat
pow_sf (GLfloat dp, GLfloat exp)
{
    POW_LUT *tab = pow_scan(exp);
    FAST_POW(dp, exp, tab);
}


GLfloat
pow_sl (GLfloat dp, GLfloat exp)
{
    POW_LUT *tab = pow_scan(exp);
    LERP_POW(dp, exp, tab);
}


GLfloat
pow_tf (GLfloat dp, GLfloat exp, POW_LUT *tab)
{
    FAST_POW(dp, exp, tab);
}


GLfloat
pow_tl (GLfloat dp, GLfloat exp, POW_LUT *tab)
{
    LERP_POW(dp, exp, tab);
}
