#include <assert.h>
#include <math.h>
#include <string.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "context.h"
#include "util/macros.h"
#include "glapi.h"
#include "matrix.h"


static const GLfloat identity[] = {
    1.0F, 0.0F, 0.0F, 0.0F,
    0.0F, 1.0F, 0.0F, 0.0F,
    0.0F, 0.0F, 1.0F, 0.0F,
    0.0F, 0.0F, 0.0F, 1.0F
};

MATRIX ctx_mx_viewport;
static MATRIX ctx_mx_modelview[MAX_MODELVIEW];
static MATRIX ctx_mx_projection[MAX_PROJECTION];
static MATRIX ctx_mx_texture[MAX_TEXTURE_UNITS][MAX_TEXTURE];
static MATRIX ctx_mx_color[MAX_COLOR];
MATRIX *ctx_mx_modelview_top;
MATRIX *ctx_mx_projection_top;
MATRIX *ctx_mx_texture_top[MAX_TEXTURE_UNITS];
static MATRIX *ctx_mx_color_top;
static MATRIX **ctx_mx_top;
GLenum ctx_mx_mode;


XFORM matrix_mul_vec4;
XFORM_BATCH matrix_mul_vec4_batch;
XFORM matrix_mul_vec3;
XFORM_BATCH matrix_mul_vec3_batch;
XFORM matrix_mul_vec_rot;


static void
matrix_mul_vec4_c (GLfloat4 dst, const GLfloat *m, const GLfloat4 src)
{
    /*
     *	a e i m    x
     *	b f j n  * y
     *	c g k o    z
     *	d h l p    w
     */
    dst[0] = m[0] * src[0] + m[4] * src[1] + m[8]  * src[2] + m[12] * src[3];
    dst[1] = m[1] * src[0] + m[5] * src[1] + m[9]  * src[2] + m[13] * src[3];
    dst[2] = m[2] * src[0] + m[6] * src[1] + m[10] * src[2] + m[14] * src[3];
    dst[3] = m[3] * src[0] + m[7] * src[1] + m[11] * src[2] + m[15] * src[3];
}


static void
matrix_mul_vec4_batch_c (GLfloat4 dst[], const GLfloat *m, const GLfloat4 src[], int n)
{
    do {
	dst[0][0] = m[0] * src[0][0] + m[4] * src[0][1] + m[8]  * src[0][2] + m[12] * src[0][3];
	dst[0][1] = m[1] * src[0][0] + m[5] * src[0][1] + m[9]  * src[0][2] + m[13] * src[0][3];
	dst[0][2] = m[2] * src[0][0] + m[6] * src[0][1] + m[10] * src[0][2] + m[14] * src[0][3];
	dst[0][3] = m[3] * src[0][0] + m[7] * src[0][1] + m[11] * src[0][2] + m[15] * src[0][3];
	src++;
	dst++;
    } while (--n);
}


static void
matrix_mul_vec3_c (GLfloat4 dst, const GLfloat *m, const GLfloat4 src)
{
    /*
     *	a e i m    x
     *	b f j n  * y
     *	c g k o    z
     *	d h l p    1
     */
    dst[0] = m[0] * src[0] + m[4] * src[1] + m[8]  * src[2] + m[12];
    dst[1] = m[1] * src[0] + m[5] * src[1] + m[9]  * src[2] + m[13];
    dst[2] = m[2] * src[0] + m[6] * src[1] + m[10] * src[2] + m[14];
    dst[3] = m[3] * src[0] + m[7] * src[1] + m[11] * src[2] + m[15];
}


static void
matrix_mul_vec3_batch_c (GLfloat4 dst[], const GLfloat *m, const GLfloat4 src[], int n)
{
    do {
	dst[0][0] = m[0] * src[0][0] + m[4] * src[0][1] + m[8]  * src[0][2] + m[12];
	dst[0][1] = m[1] * src[0][0] + m[5] * src[0][1] + m[9]  * src[0][2] + m[13];
	dst[0][2] = m[2] * src[0][0] + m[6] * src[0][1] + m[10] * src[0][2] + m[14];
	dst[0][3] = m[3] * src[0][0] + m[7] * src[0][1] + m[11] * src[0][2] + m[15];
	src++;
	dst++;
    } while (--n);
}


static void
matrix_mul_vec_rot_c (GLfloat4 dst, const GLfloat *m, const GLfloat4 src)
{
    /*
     *	a e i .    x
     *	b f j .  * y
     *	c g k .    z
     *	. . . .    .
     */
    dst[0] = m[0] * src[0] + m[4] * src[1] + m[8]  * src[2];
    dst[1] = m[1] * src[0] + m[5] * src[1] + m[9]  * src[2];
    dst[2] = m[2] * src[0] + m[6] * src[1] + m[10] * src[2];
}


void
matrix_init (void)
{
    int u;

    ctx_mx_modelview_top = ctx_mx_modelview;
    ctx_mx_modelview_top->type = MATRIX_IDENTITY;
    memcpy(ctx_mx_modelview_top->mat, identity, sizeof(identity));

    ctx_mx_projection_top = ctx_mx_projection;
    ctx_mx_projection_top->type = MATRIX_IDENTITY;
    memcpy(ctx_mx_projection_top->mat, identity, sizeof(identity));

    for (u = 0; u < MAX_TEXTURE_UNITS; u++) {
	ctx_mx_texture_top[u] = ctx_mx_texture[u];
	ctx_mx_texture_top[u]->type = MATRIX_IDENTITY;
	memcpy(ctx_mx_texture_top[u]->mat, identity, sizeof(identity));
    }

    ctx_mx_color_top = ctx_mx_color;
    ctx_mx_color_top->type = MATRIX_IDENTITY;
    memcpy(ctx_mx_color_top->mat, identity, sizeof(identity));

    ctx_mx_mode = GL_MODELVIEW;
    ctx_mx_top = &ctx_mx_modelview_top;

    /* initial depth range */
    ctx_mx_viewport.mat[10] = (1.0F - 0.0F) * ctx_depthmaxf / 2.0F;
    ctx_mx_viewport.mat[14] = (1.0F + 0.0F) * ctx_depthmaxf / 2.0F;

    /* entrypoints */
    matrix_mul_vec4 = matrix_mul_vec4_c;
    matrix_mul_vec4_batch = matrix_mul_vec4_batch_c;
    matrix_mul_vec3 = matrix_mul_vec3_c;
    matrix_mul_vec3_batch = matrix_mul_vec3_batch_c;
    matrix_mul_vec_rot = matrix_mul_vec_rot_c;
}


void GLAPIENTRY
imm_MatrixMode (GLenum mode)
{
    if (mode == GL_MODELVIEW) {
	ctx_mx_top = &ctx_mx_modelview_top;
    } else if (mode == GL_PROJECTION) {
	ctx_mx_top = &ctx_mx_projection_top;
    } else if (mode == GL_TEXTURE) {
	ctx_mx_top = &ctx_mx_texture_top[ctx_active_tex - ctx_texture];
    } else if (mode == GL_COLOR) {
	ctx_mx_top = &ctx_mx_color_top;
    } else {
	gl_error(NULL, GL_INVALID_VALUE, "invalid matrix mode\n");
	return;
    }
    ctx_mx_mode = mode;
}


void GLAPIENTRY
imm_LoadIdentity (void)
{
    FLUSH_VERTICES();

    memcpy((*ctx_mx_top)->mat, identity, sizeof(identity));

    (*ctx_mx_top)->type = MATRIX_IDENTITY;
}


void GLAPIENTRY
imm_LoadMatrixf (const GLfloat *m)
{
    FLUSH_VERTICES();

    memcpy((*ctx_mx_top)->mat, m, sizeof(identity));

    (*ctx_mx_top)->type |= MATRIX_GENERAL;
}


void GLAPIENTRY
imm_MultMatrixf (const GLfloat *m)
{
    GLfloat top[16];

    FLUSH_VERTICES();

    memcpy(top, (*ctx_mx_top)->mat, sizeof(identity));
    matrix_mul((*ctx_mx_top)->mat, top, m);

    (*ctx_mx_top)->type |= MATRIX_GENERAL;
}


void GLAPIENTRY
imm_PushMatrix (void)
{
    if (ctx_mx_mode == GL_MODELVIEW) {
	if (ctx_mx_modelview_top - ctx_mx_modelview >= (MAX_MODELVIEW - 1)) {
	    gl_error(NULL, GL_STACK_OVERFLOW, "modelview\n");
	    return;
	}
    } else if (ctx_mx_mode == GL_PROJECTION) {
	if (ctx_mx_projection_top - ctx_mx_projection >= (MAX_PROJECTION - 1)) {
	    gl_error(NULL, GL_STACK_OVERFLOW, "projection\n");
	    return;
	}
    } else if (ctx_mx_mode == GL_TEXTURE) {
	GLint u = ctx_active_tex - ctx_texture;
	if (ctx_mx_texture_top[u] - ctx_mx_texture[u] >= (MAX_TEXTURE - 1)) {
	    gl_error(NULL, GL_STACK_OVERFLOW, "texture\n");
	    return;
	}
    } else if (ctx_mx_mode == GL_COLOR) {
	if (ctx_mx_color_top - ctx_mx_color >= (MAX_COLOR - 1)) {
	    gl_error(NULL, GL_STACK_OVERFLOW, "color\n");
	    return;
	}
    } else {
	/*gl_assert(0);*/
    }
    memcpy((*ctx_mx_top) + 1, *ctx_mx_top, sizeof(MATRIX));
    (*ctx_mx_top)++;
}


void GLAPIENTRY
imm_PopMatrix (void)
{
    FLUSH_VERTICES();

    if (ctx_mx_mode == GL_MODELVIEW) {
	if (ctx_mx_modelview_top == ctx_mx_modelview) {
	    gl_error(NULL, GL_STACK_UNDERFLOW, "modelview\n");
	    return;
	}
    } else if (ctx_mx_mode == GL_PROJECTION) {
	if (ctx_mx_projection_top == ctx_mx_projection) {
	    gl_error(NULL, GL_STACK_UNDERFLOW, "projection\n");
	    return;
	}
    } else if (ctx_mx_mode == GL_TEXTURE) {
	GLint u = ctx_active_tex - ctx_texture;
	if (ctx_mx_texture_top[u] == ctx_mx_texture[u]) {
	    gl_error(NULL, GL_STACK_UNDERFLOW, "texture\n");
	    return;
	}
    } else if (ctx_mx_mode == GL_COLOR) {
	if (ctx_mx_color_top == ctx_mx_color) {
	    gl_error(NULL, GL_STACK_UNDERFLOW, "color\n");
	    return;
	}
    } else {
	/*gl_assert(0);*/
    }
    (*ctx_mx_top)--;
}


void GLAPIENTRY
imm_Translatef (GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat *m = (*ctx_mx_top)->mat;

    FLUSH_VERTICES();

    /*
     *		1 0 0 x
     *	M = C *	0 1 0 y
     *		0 0 1 z
     *		0 0 0 1
     */
    m[12] = m[0] * x + m[4] * y + m[8]  * z + m[12];
    m[13] = m[1] * x + m[5] * y + m[9]  * z + m[13];
    m[14] = m[2] * x + m[6] * y + m[10] * z + m[14];
    m[15] = m[3] * x + m[7] * y + m[11] * z + m[15];

    (*ctx_mx_top)->type |= MATRIX_TRANSLATE;
}


void GLAPIENTRY
imm_Scalef (GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat *m = (*ctx_mx_top)->mat;

    FLUSH_VERTICES();

    /*
     *		x 0 0 0
     *	M = C *	0 y 0 0
     *		0 0 z 0
     *		0 0 0 1
     */
    m[0] *= x; m[4] *= y; m[8]  *= z;
    m[1] *= x; m[5] *= y; m[9]  *= z;
    m[2] *= x; m[6] *= y; m[10] *= z;
    m[3] *= x; m[7] *= y; m[11] *= z;

    (*ctx_mx_top)->type |= MATRIX_SCALE;
}


void GLAPIENTRY
imm_Rotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    GLfloat *m = (*ctx_mx_top)->mat;
    GLfloat c, s, omc;
    GLfloat xx, xy, xz, yy, yz, zz;
    GLfloat sx, sy, sz;
    GLfloat r[16], n[16];
    GLfloat len = x * x + y * y + z * z;

    FLUSH_VERTICES();

    if (len <= 1.0e-4) {
	return;
    }
    len = 1.0F / SQRT(len);
    x *= len;
    y *= len;
    z *= len;
    /*
     *		x
     *	u =	y
     *		z
     *
     *  u^=   x y z
     *
     *		 x	     xx xy xz
     *	u * u^ = y * x y z = yx yy yz
     *		 z	     zx zy zz
     *
     *          0  -z   y
     *	S = 	z   0  -x
     *	       -y   x   0
     *
     *			1 - xx	   -xy	   -xz
     *	I - u * u^ =	   -yx	1 - yy	   -yz	
     *			   -zx	   -zy	1 - zz
     *
     *	R = u * u^ + cos * (I - u * u^) + sin * S
     *
     *	    xx + c(1 - xx) + 0 * s   xy - c * xy - s * z	xz - c * xz + s * y
     *	R = yx - c * yx + s * z	     yy + c(1 - yy) + 0 * s	yz - c * yz - s * x
     *	    zx - c * zx - s * y	     zy - c * zy + s * x	zz + c(1 - zz) + 0*s
     *
     *	    xx + c * (1 - xx)	     xy - c * xy - s * z	xz - c * xz + s * y
     *	R = yx - c * yx + s * z	     yy + c * (1 - yy)		yz - c * yz - s * x
     *	    zx - c * zx - s * y	     zy - c * zy + s * x	zz + c * (1 - zz)
     *
     *	    xx * (1 - c) + c	     xy * (1 - c) - s * z	xz * (1 - c) + s * y
     *	R = yx * (1 - c) + s * z     yy * (1 - c) + c		yz * (1 - c) - s * x
     *	    zx * (1 - c) - s * y     zy * (1 - c) + s * x	zz * (1 - c) + c
     */
    s = SIN(angle * M_PI / 180.0F);
    xx = x * x;
    xy = x * y;
    xz = x * z;
    yy = y * y;
    yz = y * z;
    zz = z * z;
    c = COS(angle * M_PI / 180.0F);
    sx = s * x;
    sy = s * y;
    sz = s * z;
    omc = 1.0F - c;
    r[0]  = xx * omc + c;
    r[1]  = xy * omc + sz;
    r[2]  = xz * omc - sy;
    r[4]  = xy * omc - sz;
    r[5]  = yy * omc + c;
    r[6]  = yz * omc + sx;
    r[8]  = xz * omc + sy;
    r[9]  = yz * omc - sx;
    r[10] = zz * omc + c;
    memcpy(n, m, 12 * sizeof(GLfloat));
    /*
     *	   a e i m	A E I 0
     * P = b f j n  *	B F J 0
     *	   c g k o	C G K 0
     *	   d h l p	0 0 0 1
     */
    m[0]  = n[0] * r[0] + n[4] * r[1] + n[8]  * r[2];
    m[1]  = n[1] * r[0] + n[5] * r[1] + n[9]  * r[2];
    m[2]  = n[2] * r[0] + n[6] * r[1] + n[10] * r[2];
    m[3]  = n[3] * r[0] + n[7] * r[1] + n[11] * r[2];

    m[4]  = n[0] * r[4] + n[4] * r[5] + n[8]  * r[6];
    m[5]  = n[1] * r[4] + n[5] * r[5] + n[9]  * r[6];
    m[6]  = n[2] * r[4] + n[6] * r[5] + n[10] * r[6];
    m[7]  = n[3] * r[4] + n[7] * r[5] + n[11] * r[6];

    m[8]  = n[0] * r[8] + n[4] * r[9] + n[8]  * r[10];
    m[9]  = n[1] * r[8] + n[5] * r[9] + n[9]  * r[10];
    m[10] = n[2] * r[8] + n[6] * r[9] + n[10] * r[10];
    m[11] = n[3] * r[8] + n[7] * r[9] + n[11] * r[10];

    (*ctx_mx_top)->type |= MATRIX_ROTATE;
}


void GLAPIENTRY
imm_Viewport (int x, int y, GLsizei w, GLsizei h)
{
    FLUSH_VERTICES();

    /*
     *	e = M * v	(eye coord)
     *	c = P * e	(clip coord)
     *	d = c / wc	(device coord, wd -> 1.0)
     *	w = V * d	(viewport)
     *
     *	px/2	0	0	ox
     *	0	py/2	0	oy
     *	0	0	(f-n)/2	(n+f)/2
     *	0	0	0	1
     */
    ctx_mx_viewport.mat[0] = w / 2.0F;
    ctx_mx_viewport.mat[12] = x + w / 2.0F;

    ctx_mx_viewport.mat[5] = h / 2.0F;
    ctx_mx_viewport.mat[13] = y + h / 2.0F;
}


void GLAPIENTRY
imm_DepthRange (GLclampd near_val, GLclampd far_val)
{
    FLUSH_VERTICES();

    ctx_mx_viewport.mat[10] = (far_val - near_val) * ctx_depthmaxf / 2;
    ctx_mx_viewport.mat[14] = (far_val + near_val) * ctx_depthmaxf / 2;
}


void
matrix_mul (GLfloat *dst, const GLfloat *a, const GLfloat *b)
{
    /*
     *	a0  a4  a8  a12		b0  b4  b8  b12
     *	a1  a5  a9  a13	   *	b1  b5  b9  b13
     *	a2  a6  a10 a14		b2  b6  b10 b14
     *	a3  a7  a11 a15		b3  b7  b11 b15
     */
    dst[0]  = a[0] * b[0]  + a[4] * b[1]  + a[8]  * b[2]  + a[12] * b[3];
    dst[1]  = a[1] * b[0]  + a[5] * b[1]  + a[9]  * b[2]  + a[13] * b[3];
    dst[2]  = a[2] * b[0]  + a[6] * b[1]  + a[10] * b[2]  + a[14] * b[3];
    dst[3]  = a[3] * b[0]  + a[7] * b[1]  + a[11] * b[2]  + a[15] * b[3];

    dst[4]  = a[0] * b[4]  + a[4] * b[5]  + a[8]  * b[6]  + a[12] * b[7];
    dst[5]  = a[1] * b[4]  + a[5] * b[5]  + a[9]  * b[6]  + a[13] * b[7];
    dst[6]  = a[2] * b[4]  + a[6] * b[5]  + a[10] * b[6]  + a[14] * b[7];
    dst[7]  = a[3] * b[4]  + a[7] * b[5]  + a[11] * b[6]  + a[15] * b[7];

    dst[8]  = a[0] * b[8]  + a[4] * b[9]  + a[8]  * b[10] + a[12] * b[11];
    dst[9]  = a[1] * b[8]  + a[5] * b[9]  + a[9]  * b[10] + a[13] * b[11];
    dst[10] = a[2] * b[8]  + a[6] * b[9]  + a[10] * b[10] + a[14] * b[11];
    dst[11] = a[3] * b[8]  + a[7] * b[9]  + a[11] * b[10] + a[15] * b[11];

    dst[12] = a[0] * b[12] + a[4] * b[13] + a[8]  * b[14] + a[12] * b[15];
    dst[13] = a[1] * b[12] + a[5] * b[13] + a[9]  * b[14] + a[13] * b[15];
    dst[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
    dst[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
}


void GLAPIENTRY
imm_Ortho (GLdouble left, GLdouble right,
           GLdouble bottom, GLdouble top,
           GLdouble near_val, GLdouble far_val)
{
    GLfloat n[16];
    GLfloat o[16];
    GLfloat *m = (*ctx_mx_top)->mat;

    FLUSH_VERTICES();

    memcpy(n, m, sizeof(identity));

    o[0]  = 2.0 / (right - left);
    o[5]  = 2.0 / (top - bottom);
    o[10] = 2.0 / (near_val - far_val);
    o[12] = (right + left) / (left - right);
    o[13] = (top + bottom) / (bottom - top);
    o[14] = (far_val + near_val) / (near_val - far_val);
    /*
     *	a0  a4  a8  a12		b0  0   0   b12
     *	a1  a5  a9  a13	   *	0   b5  0   b13
     *	a2  a6  a10 a14		0   0   b10 b14
     *	a3  a7  a11 a15		0   0   0   1
     */
    m[0]  = n[0]  * o[0];
    m[1]  = n[1]  * o[0];
    m[2]  = n[2]  * o[0];
    m[3]  = n[3]  * o[0];

    m[4]  = n[4]  * o[5];
    m[5]  = n[5]  * o[5];
    m[6]  = n[6]  * o[5];
    m[7]  = n[7]  * o[5];

    m[8]  = n[8]  * o[10];
    m[9]  = n[9]  * o[10];
    m[10] = n[10] * o[10];
    m[11] = n[11] * o[10];

    m[12] = n[0]  * o[12] + n[4] * o[13] + n[8]  * o[14] + n[12];
    m[13] = n[1]  * o[12] + n[5] * o[13] + n[9]  * o[14] + n[13];
    m[14] = n[2]  * o[12] + n[6] * o[13] + n[10] * o[14] + n[14];
    m[15] = n[3]  * o[12] + n[7] * o[13] + n[11] * o[14] + n[15];

    (*ctx_mx_top)->type |= MATRIX_SCALE | MATRIX_TRANSLATE;
}


void GLAPIENTRY
imm_Frustum (GLdouble left, GLdouble right,
             GLdouble bottom, GLdouble top,
             GLdouble near_val, GLdouble far_val)
{
    GLfloat n[16];
    GLfloat f[16];
    GLfloat *m = (*ctx_mx_top)->mat;

    FLUSH_VERTICES();

    memcpy(n, m, sizeof(identity));

    f[0]  = 2.0 * near_val / (right - left);
    f[5]  = 2.0 * near_val / (top - bottom);
    f[8]  = (right + left) / (right - left);
    f[9]  = (top + bottom) / (top - bottom);
    f[10] = (far_val + near_val) / (near_val - far_val);
    f[14] = 2.0 * far_val * near_val / (near_val - far_val);
    /*
     *	a0  a4  a8  a12		b0  0   b8  0
     *	a1  a5  a9  a13	   *	0   b5  b9  0
     *	a2  a6  a10 a14		0   0   b10 b14
     *	a3  a7  a11 a15		0   0   -1  0
     */
    m[0]  = n[0]  * f[0];
    m[1]  = n[1]  * f[0];
    m[2]  = n[2]  * f[0];
    m[3]  = n[3]  * f[0];

    m[4]  = n[4]  * f[5];
    m[5]  = n[5]  * f[5];
    m[6]  = n[6]  * f[5];
    m[7]  = n[7]  * f[5];

    m[8]  = n[0]  * f[8] + n[4] * f[9] + n[8]  * f[10] - n[12];
    m[9]  = n[1]  * f[8] + n[5] * f[9] + n[9]  * f[10] - n[13];
    m[10] = n[2]  * f[8] + n[6] * f[9] + n[10] * f[10] - n[14];
    m[11] = n[3]  * f[8] + n[7] * f[9] + n[11] * f[10] - n[15];

    m[12] = n[8]  * f[14];
    m[13] = n[9]  * f[14];
    m[14] = n[10] * f[14];
    m[15] = n[11] * f[14];

    (*ctx_mx_top)->type |= MATRIX_GENERAL; /* XXX true? */
}


void
matrix_mul_vec2 (GLfloat4 dst, const GLfloat *m, const GLfloat4 src)
{
    /*
     *	a e i m    x
     *	b f j n  * y
     *	c g k o    0
     *	d h l p    1
     */
    dst[0] = m[0] * src[0] + m[4] * src[1] + m[12];
    dst[1] = m[1] * src[0] + m[5] * src[1] + m[13];
    dst[2] = m[2] * src[0] + m[6] * src[1] + m[14];
    dst[3] = m[3] * src[0] + m[7] * src[1] + m[15];
}


void
matrix_transpose (GLfloat *transpose, const GLfloat *src)
{
    transpose[0]  = src[0];
    transpose[1]  = src[4];
    transpose[2]  = src[8];
    transpose[3]  = src[12];
    transpose[4]  = src[1];
    transpose[5]  = src[5];
    transpose[6]  = src[9];
    transpose[7]  = src[13];
    transpose[8]  = src[2];
    transpose[9]  = src[6];
    transpose[10] = src[10];
    transpose[11] = src[14];
    transpose[12] = src[3];
    transpose[13] = src[7];
    transpose[14] = src[11];
    transpose[15] = src[15];
}


static GLboolean
matrix_invt_rst (GLfloat *inv, const GLfloat *src)
{
    GLfloat x00, x01, x02;
    GLfloat x10, x11, x12;
    GLfloat x20, x21, x22;
    GLfloat rcp;

    /*
     * [          ] -t    [   -1      -1   ]
     * [  Q    0  ]       [  Q      -Q  T  ]
     * [          ]    =  [                ]
     * [  T    1  ]       [  0        1    ]
     * [          ]       [                ]
     */
    GLfloat z00, z01, z02;
    GLfloat z10, z11, z12;
    GLfloat z20, z21, z22;

    /* read 3x3 matrix into registers */
    x00 = src[0];
    x01 = src[1];
    x02 = src[2];
    x10 = src[4];
    x11 = src[5];
    x12 = src[6];
    x20 = src[8];
    x21 = src[9];
    x22 = src[10];

    /* compute first three 2x2 cofactors */
    z20 = x01 * x12 - x11 * x02;
    z10 = x21 * x02 - x01 * x22;
    z00 = x11 * x22 - x12 * x21;

    /* compute 3x3 determinant & its reciprocal */
    rcp = x20 * z20 + x10 * z10 + x00 * z00;
    if (rcp == 0.0F)
	return GL_FALSE;
    rcp = 1.0F / rcp;

    /* compute other six 2x2 cofactors */
    z01 = x20 * x12 - x10 * x22;
    z02 = x10 * x21 - x20 * x11;
    z11 = x00 * x22 - x20 * x02;
    z12 = x20 * x01 - x00 * x21;
    z21 = x10 * x02 - x00 * x12;
    z22 = x00 * x11 - x10 * x01;

    /* multiply all cofactors by reciprocal */
    inv[0]  = z00 * rcp;
    inv[1]  = z01 * rcp;
    inv[2]  = z02 * rcp;
    inv[4]  = z10 * rcp;
    inv[5]  = z11 * rcp;
    inv[6]  = z12 * rcp;
    inv[8]  = z20 * rcp;
    inv[9]  = z21 * rcp;
    inv[10] = z22 * rcp;

    /* read translation vector & negate */
    x00 = -src[12];
    x01 = -src[13];
    x02 = -src[14];

    /* store bottom row of inverse transpose */
    inv[12] = 0;
    inv[13] = 0;
    inv[14] = 0;
    inv[15] = 1;

    /* finish by tranforming translation vector */
    inv[3]  = inv[0] * x00 + inv[1] * x01 + inv[2]  * x02;
    inv[7]  = inv[4] * x00 + inv[5] * x01 + inv[6]  * x02;
    inv[11] = inv[8] * x00 + inv[9] * x01 + inv[10] * x02;

    return GL_TRUE;
}


static GLboolean
matrix_invt_gen (GLfloat *inv, const GLfloat *src)
{
    GLfloat x00, x01, x02;
    GLfloat x10, x11, x12;
    GLfloat x20, x21, x22;
    GLfloat rcp;

    GLfloat x30, x31, x32;
    GLfloat y01, y02, y03, y12, y13, y23;
    GLfloat z02, z03, z12, z13, z22, z23, z32, z33;

#define x03 x01
#define x13 x11
#define x23 x21
#define x33 x31
#define z00 x02
#define z10 x12
#define z20 x22
#define z30 x32
#define z01 x03
#define z11 x13
#define z21 x23
#define z31 x33

    /* read 1st two rows of matrix into registers */
    x00 = src[0];
    x01 = src[1];
    x10 = src[4];
    x11 = src[5];
    x20 = src[8];
    x21 = src[9];
    x30 = src[12];
    x31 = src[13];

    /* compute all six 2x2 determinants of 1st two rows */
    y01 = x00 * x11 - x10 * x01;
    y02 = x00 * x21 - x20 * x01;
    y03 = x00 * x31 - x30 * x01;
    y12 = x10 * x21 - x20 * x11;
    y13 = x10 * x31 - x30 * x11;
    y23 = x20 * x31 - x30 * x21;

    /* read 2nd two rows of matrix into registers */
    x02 = src[2];
    x03 = src[3];
    x12 = src[6];
    x13 = src[7];
    x22 = src[10];
    x23 = src[11];
    x32 = src[14];
    x33 = src[15];

    /* compute all 3x3 cofactors for 2nd two rows */
    z33 = x02 * y12 - x12 * y02 + x22 * y01;
    z23 = x12 * y03 - x32 * y01 - x02 * y13;
    z13 = x02 * y23 - x22 * y03 + x32 * y02;
    z03 = x22 * y13 - x32 * y12 - x12 * y23;
    z32 = x13 * y02 - x23 * y01 - x03 * y12;
    z22 = x03 * y13 - x13 * y03 + x33 * y01;
    z12 = x23 * y03 - x33 * y02 - x03 * y23;
    z02 = x13 * y23 - x23 * y13 + x33 * y12;

    /* compute all six 2x2 determinants of 2nd two rows */
    y01 = x02 * x13 - x12 * x03;
    y02 = x02 * x23 - x22 * x03;
    y03 = x02 * x33 - x32 * x03;
    y12 = x12 * x23 - x22 * x13;
    y13 = x12 * x33 - x32 * x13;
    y23 = x22 * x33 - x32 * x23;

    /* read 1st two rows of matrix into registers */
    x00 = src[0];
    x01 = src[1];
    x10 = src[4];
    x11 = src[5];
    x20 = src[8];
    x21 = src[9];
    x30 = src[12];
    x31 = src[13];

    /* compute all 3x3 cofactors for 1st row */
    z30 = x11 * y02 - x21 * y01 - x01 * y12;
    z20 = x01 * y13 - x11 * y03 + x31 * y01;
    z10 = x21 * y03 - x31 * y02 - x01 * y23;
    z00 = x11 * y23 - x21 * y13 + x31 * y12;

    /* compute 4x4 determinant & its reciprocal */
    rcp = x30 * z30 + x20 * z20 + x10 * z10 + x00 * z00;
    if (rcp == 0.0F)
	return GL_FALSE;
    rcp = 1.0F / rcp;

    /* compute all 3x3 cofactors for 2nd row */
    z31 = x00 * y12 - x10 * y02 + x20 * y01;
    z21 = x10 * y03 - x30 * y01 - x00 * y13;
    z11 = x00 * y23 - x20 * y03 + x30 * y02;
    z01 = x20 * y13 - x30 * y12 - x10 * y23;

    /* multiply all 3x3 cofactors by reciprocal */
    inv[0]  = z00 * rcp;
    inv[1]  = z01 * rcp;
    inv[4]  = z10 * rcp;
    inv[2]  = z02 * rcp;
    inv[8]  = z20 * rcp;
    inv[3]  = z03 * rcp;
    inv[12] = z30 * rcp;
    inv[5]  = z11 * rcp;
    inv[6]  = z12 * rcp;
    inv[9]  = z21 * rcp;
    inv[7]  = z13 * rcp;
    inv[13] = z31 * rcp;
    inv[10] = z22 * rcp;
    inv[11] = z23 * rcp;
    inv[14] = z32 * rcp;
    inv[15] = z33 * rcp;

    return GL_TRUE;
}


GLboolean
matrix_invt (GLfloat *inv, const MATRIX *src)
{
    if (MAT_IDENTITY(src)) {
	memcpy(inv, src->mat, sizeof(identity));
	return GL_TRUE;
    } else if (!MAT_GENERAL(src)) {
	return matrix_invt_rst(inv, src->mat);
    } else {
	return matrix_invt_gen(inv, src->mat);
    }
}


#if 0
static void
matrix_print (const GLfloat *m)
{
    printf("%6.2f, %6.2f, %6.2f, %6.2f\n", m[0], m[4], m[8], m[12]);
    printf("%6.2f, %6.2f, %6.2f, %6.2f\n", m[1], m[5], m[9], m[13]);
    printf("%6.2f, %6.2f, %6.2f, %6.2f\n", m[2], m[6], m[10], m[14]);
    printf("%6.2f, %6.2f, %6.2f, %6.2f\n", m[3], m[7], m[11], m[15]);
    printf("\n");
}
#endif


const GLfloat *
get_mvp (void)
{
    static GLfloat mvp[16];
    matrix_mul(mvp, ctx_mx_projection_top->mat, ctx_mx_modelview_top->mat);
    return mvp;
}


const GLfloat *
get_imv (void)
{
    static GLfloat imv[16];
    matrix_invt(imv, ctx_mx_modelview_top);
    return imv;
}


/* variations */


void GLAPIENTRY
imm_LoadMatrixd (const GLdouble *m)
{
    GLfloat tmp[16];
    tmp[0]  = m[0];
    tmp[1]  = m[1];
    tmp[2]  = m[2];
    tmp[3]  = m[3];
    tmp[4]  = m[4];
    tmp[5]  = m[5];
    tmp[6]  = m[6];
    tmp[7]  = m[7];
    tmp[8]  = m[8];
    tmp[9]  = m[9];
    tmp[10] = m[10];
    tmp[11] = m[11];
    tmp[12] = m[12];
    tmp[13] = m[13];
    tmp[14] = m[14];
    tmp[15] = m[15];
    imm_LoadMatrixf(tmp);
}


void GLAPIENTRY
imm_MultMatrixd (const GLdouble *m)
{
    GLfloat tmp[16];
    tmp[0]  = m[0];
    tmp[1]  = m[1];
    tmp[2]  = m[2];
    tmp[3]  = m[3];
    tmp[4]  = m[4];
    tmp[5]  = m[5];
    tmp[6]  = m[6];
    tmp[7]  = m[7];
    tmp[8]  = m[8];
    tmp[9]  = m[9];
    tmp[10] = m[10];
    tmp[11] = m[11];
    tmp[12] = m[12];
    tmp[13] = m[13];
    tmp[14] = m[14];
    tmp[15] = m[15];
    imm_MultMatrixf(tmp);
}


void GLAPIENTRY
imm_Translated (GLdouble x, GLdouble y, GLdouble z)
{
    imm_Translatef(x, y, z);
}


void GLAPIENTRY
imm_Scaled (GLdouble x, GLdouble y, GLdouble z)
{
    imm_Scalef(x, y, z);
}


void GLAPIENTRY
imm_Rotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
    imm_Rotatef(angle, x, y, z);
}
