#ifndef MATRIX_H_included
#define MATRIX_H_included

#define MATRIX_IDENTITY    0
#define MATRIX_ROTATE      (1<<0)
#define MATRIX_SCALE       (1<<1)
#define MATRIX_TRANSLATE   (1<<2)
#define MATRIX_GENERAL     (1<<3)


#define MAT_IDENTITY(M) ((M)->type == MATRIX_IDENTITY)
#define MAT_GENERAL(M)  ((M)->type & MATRIX_GENERAL)


#define MAX_MODELVIEW	32
#define MAX_PROJECTION	8
#define MAX_TEXTURE	8
#define MAX_COLOR	2


typedef struct {
    GLfloat mat[16];
    GLbitfield type;
} MATRIX;


extern MATRIX ctx_mx_viewport;
extern MATRIX *ctx_mx_modelview_top;
extern MATRIX *ctx_mx_projection_top;
extern MATRIX *ctx_mx_texture_top[MAX_TEXTURE_UNITS];
extern GLenum ctx_mx_mode;


void matrix_init (void);

void matrix_mul (GLfloat *dst, const GLfloat *a, const GLfloat *b);


void matrix_mul_vec2 (GLfloat4 dst, const GLfloat *m, const GLfloat4 src);

void matrix_transpose (GLfloat *transpose, const GLfloat *src);
GLboolean matrix_invt (GLfloat *inv, const MATRIX *src);


typedef void (*XFORM) (GLfloat4 dst, const GLfloat *m, const GLfloat4 src);
typedef void (*XFORM_BATCH) (GLfloat4 dst[], const GLfloat *m, const GLfloat4 src[], int n);

extern XFORM matrix_mul_vec4;
extern XFORM_BATCH matrix_mul_vec4_batch;
extern XFORM matrix_mul_vec3;
extern XFORM_BATCH matrix_mul_vec3_batch;
extern XFORM matrix_mul_vec_rot;


const GLfloat *get_mvp (void);
const GLfloat *get_imv (void);


#endif
