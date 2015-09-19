#ifndef POW_H_included
#define POW_H_included

#define POW_LUT_SIZE	256

typedef struct POW_LUT {
    struct POW_LUT *next, *prev;
    GLint refcount;
    GLfloat threshold, scale, exp;
    GLfloat table[POW_LUT_SIZE + 1];
} POW_LUT;

int pow_init (void);
POW_LUT *pow_make (POW_LUT *table, GLfloat exp);
POW_LUT *pow_scan (GLfloat exp);
void pow_fini (void);

GLfloat pow_sf (GLfloat dp, GLfloat exp);
GLfloat pow_sl (GLfloat dp, GLfloat exp);
GLfloat pow_tf (GLfloat dp, GLfloat exp, POW_LUT *tab);
GLfloat pow_tl (GLfloat dp, GLfloat exp, POW_LUT *tab);

#endif
