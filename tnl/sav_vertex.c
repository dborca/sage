#include <assert.h>

#include "GL/gl.h"
#include "glinternal.h"
#include "main/glapi.h"
#include "main/context.h"
#include "util/macros.h"
#include "main/dlist.h"


#define COLOR0_F(n, r, g, b, a)				\
    do {						\
	DL_NODE *node = dlist_new_operation(OP_COLOR##n##F);\
	(node + 1)->f = r;				\
	(node + 2)->f = g;				\
	(node + 3)->f = b;				\
	if (n > 3) {					\
	    (node + 4)->f = a;				\
	}						\
    } while (0)

#define COLOR1_F(r, g, b)				\
    do {						\
	DL_NODE *node = dlist_new_operation(OP_SECONDARYCOLOR3F);\
	(node + 1)->f = r;				\
	(node + 2)->f = g;				\
	(node + 3)->f = b;				\
    } while (0)

#define FOGCOORD_F(c)					\
    do {						\
	DL_NODE *node = dlist_new_operation(OP_FOGCOORDF);\
	(node + 1)->f = c;				\
    } while (0)

#define EDGEFLAG_B(e)					\
    do {						\
	DL_NODE *node = dlist_new_operation(OP_EDGEFLAG);\
	(node + 1)->i = e;				\
    } while (0)

#define MAT_F(n, face, attr, m0, m1, m2, m3)		\
    do {						\
	if (n == 1) {					\
	    DL_NODE *node = dlist_new_operation(OP_MATERIALF);\
	    (node + 1)->i = face;			\
	    (node + 2)->i = attr;			\
	    (node + 3)->f = m0;				\
	} else {					\
	    DL_NODE *node = dlist_new_operation(OP_MATERIALFV);\
	    (node + 1)->i = face;			\
	    (node + 2)->i = attr;			\
	    (node + 3)->f = m0;				\
	    (node + 4)->f = m1;				\
	    (node + 5)->f = m2;				\
	    (node + 6)->f = m3;				\
	}						\
    } while (0)

#define NORMAL_F(x, y, z)				\
    do {						\
	DL_NODE *node = dlist_new_operation(OP_NORMAL3F);\
	(node + 1)->f = x;				\
	(node + 2)->f = y;				\
	(node + 3)->f = z;				\
    } while (0)

#define TEXCOORD_F(n, u, s, t, r, q)			\
    do {						\
	DL_NODE *node = dlist_new_operation(OP_TEXCOORD##n##F);\
	(node + 1)->f = s;				\
	if (n > 1) {					\
	    (node + 2)->f = t;				\
	}						\
	if (n > 2) {					\
	    (node + 3)->f = r;				\
	}						\
	if (n > 3) {					\
	    (node + 4)->f = q;				\
	}						\
    } while (0)

#define MULTITEXCOORD_F(n, u, s, t, r, q)		\
    do {						\
	DL_NODE *node = dlist_new_operation(OP_MULTITEXCOORD##n##F);\
	(node + 1)->i = u + GL_TEXTURE0;		\
	(node + 2)->f = s;				\
	if (n > 1) {					\
	    (node + 3)->f = t;				\
	}						\
	if (n > 2) {					\
	    (node + 4)->f = r;				\
	}						\
	if (n > 3) {					\
	    (node + 5)->f = q;				\
	}						\
    } while (0)

#define VERTEX_F(n, x, y, z, w)				\
    do {						\
	DL_NODE *node = dlist_new_operation(OP_VERTEX##n##F);\
	(node + 1)->f = x;				\
	(node + 2)->f = y;				\
	if (n > 2) {					\
	    (node + 3)->f = z;				\
	}						\
	if (n > 3) {					\
	    (node + 4)->f = w;				\
	}						\
    } while (0)


/******************************************************************************
 * COLOR0
 */
void GLAPIENTRY
sav_Color3f (GLfloat r, GLfloat g, GLfloat b)
{
    COLOR0_F(3, r, g, b, 1.0F);
}


void GLAPIENTRY
sav_Color3fv (const GLfloat *v)
{
    COLOR0_F(3, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
sav_Color4f (GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    COLOR0_F(4, r, g, b, a);
}


void GLAPIENTRY
sav_Color4fv (const GLfloat *v)
{
    COLOR0_F(4, v[0], v[1], v[2], v[3]);
}


/******************************************************************************
 * COLOR1
 */
void GLAPIENTRY
sav_SecondaryColor3f (GLfloat r, GLfloat g, GLfloat b)
{
    COLOR1_F(r, g, b);
}


void GLAPIENTRY
sav_SecondaryColor3fv (const GLfloat *v)
{
    COLOR1_F(v[0], v[1], v[2]);
}


/******************************************************************************
 * FOGCOORD
 */
void GLAPIENTRY
sav_FogCoordf (GLfloat c)
{
    FOGCOORD_F(c);
}


void GLAPIENTRY
sav_FogCoordfv (const GLfloat *v)
{
    FOGCOORD_F(v[0]);
}


/******************************************************************************
 * EDGEFLAG
 */
void GLAPIENTRY
sav_EdgeFlag (GLboolean flag)
{
    EDGEFLAG_B(flag);
}


void GLAPIENTRY
sav_EdgeFlagv (const GLboolean *flag)
{
    EDGEFLAG_B(flag[0]);
}


/******************************************************************************
 * MATERIAL
 */
void GLAPIENTRY
sav_Materialf (GLenum face, GLenum pname, GLfloat param)
{
    gl_assert(face == GL_FRONT || face == GL_BACK || face == GL_FRONT_AND_BACK);
    gl_assert(pname == GL_SHININESS);

    MAT_F(1, face, pname, param, 0.0F, 0.0F, 0.0F);
}


void GLAPIENTRY
sav_Materialfv (GLenum face, GLenum pname, const GLfloat *params)
{
    gl_assert(face == GL_FRONT || face == GL_BACK || face == GL_FRONT_AND_BACK);

    gl_assert(pname == GL_AMBIENT ||
	      pname == GL_DIFFUSE ||
	      pname == GL_SPECULAR ||
	      pname == GL_EMISSION ||
	      pname == GL_SHININESS ||
	      pname == GL_AMBIENT_AND_DIFFUSE);

    if (pname != GL_SHININESS) {
	MAT_F(4, face, pname, params[0], params[1], params[2], params[3]);
    } else {
	MAT_F(1, face, pname, params[0], 0.0F, 0.0F, 0.0F);
    }
}


/******************************************************************************
 * NORMAL
 */
void GLAPIENTRY
sav_Normal3f (GLfloat x, GLfloat y, GLfloat z)
{
    NORMAL_F(x, y, z);
}


void GLAPIENTRY
sav_Normal3fv (const GLfloat *v)
{
    NORMAL_F(v[0], v[1], v[2]);
}


/******************************************************************************
 * TEXCOORD
 */
void GLAPIENTRY
sav_TexCoord1f (GLfloat s)
{
    TEXCOORD_F(1, 0, s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord1fv (const GLfloat *v)
{
    TEXCOORD_F(1, 0, v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord2f (GLfloat s, GLfloat t)
{
    TEXCOORD_F(2, 0, s, t, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord2fv (const GLfloat *v)
{
    TEXCOORD_F(2, 0, v[0], v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord3f (GLfloat s, GLfloat t, GLfloat r)
{
    TEXCOORD_F(3, 0, s, t, r, 1.0F);
}


void GLAPIENTRY
sav_TexCoord3fv (const GLfloat *v)
{
    TEXCOORD_F(3, 0, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
sav_TexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    TEXCOORD_F(4, 0, s, t, r, q);
}


void GLAPIENTRY
sav_TexCoord4fv (const GLfloat *v)
{
    TEXCOORD_F(4, 0, v[0], v[1], v[2], v[3]);
}


void GLAPIENTRY
sav_MultiTexCoord1f (GLenum texture, GLfloat s)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(1, u, s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord1fv (GLenum texture, const GLfloat *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(1, u, v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord2f (GLenum texture, GLfloat s, GLfloat t)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(2, u, s, t, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord2fv (GLenum texture, const GLfloat *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(2, u, v[0], v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord3f (GLenum texture, GLfloat s, GLfloat t, GLfloat r)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(3, u, s, t, r, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord3fv (GLenum texture, const GLfloat *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(3, u, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord4f (GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(4, u, s, t, r, q);
}


void GLAPIENTRY
sav_MultiTexCoord4fv (GLenum texture, const GLfloat *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(4, u, v[0], v[1], v[2], v[3]);
}


/******************************************************************************
 * VERTEX
 */
void GLAPIENTRY
sav_Vertex2f (GLfloat x, GLfloat y)
{
    VERTEX_F(2, x, y, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_Vertex2fv (const GLfloat *v)
{
    VERTEX_F(2, v[0], v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
sav_Vertex3f (GLfloat x, GLfloat y, GLfloat z)
{
    VERTEX_F(3, x, y, z, 1.0F);
}


void GLAPIENTRY
sav_Vertex3fv (const GLfloat *v)
{
    VERTEX_F(3, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
sav_Vertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    VERTEX_F(4, x, y, z, w);
}


void GLAPIENTRY
sav_Vertex4fv (const GLfloat *v)
{
    VERTEX_F(4, v[0], v[1], v[2], v[3]);
}


/* variations */


void GLAPIENTRY
sav_Color3b (GLbyte r, GLbyte g, GLbyte b)
{
    COLOR0_F(3, B_TO_FLOAT(r), B_TO_FLOAT(g), B_TO_FLOAT(b), 1.0F);
}


void GLAPIENTRY
sav_Color3bv (const GLbyte *v)
{
    COLOR0_F(3, B_TO_FLOAT(v[0]), B_TO_FLOAT(v[1]), B_TO_FLOAT(v[2]), 1.0F);
}


void GLAPIENTRY
sav_Color3d (GLdouble r, GLdouble g, GLdouble b)
{
    COLOR0_F(3, r, g, b, 1.0F);
}


void GLAPIENTRY
sav_Color3dv (const GLdouble *v)
{
    COLOR0_F(3, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
sav_Color3i (GLint r, GLint g, GLint b)
{
    COLOR0_F(3, I_TO_FLOAT(r), I_TO_FLOAT(g), I_TO_FLOAT(b), 1.0F);
}


void GLAPIENTRY
sav_Color3iv (const GLint *v)
{
    COLOR0_F(3, I_TO_FLOAT(v[0]), I_TO_FLOAT(v[1]), I_TO_FLOAT(v[2]), 1.0F);
}


void GLAPIENTRY
sav_Color3s (GLshort r, GLshort g, GLshort b)
{
    COLOR0_F(3, S_TO_FLOAT(r), S_TO_FLOAT(g), S_TO_FLOAT(b), 1.0F);
}


void GLAPIENTRY
sav_Color3sv (const GLshort *v)
{
    COLOR0_F(3, S_TO_FLOAT(v[0]), S_TO_FLOAT(v[1]), S_TO_FLOAT(v[2]), 1.0F);
}


void GLAPIENTRY
sav_Color3ub (GLubyte r, GLubyte g, GLubyte b)
{
    COLOR0_F(3, UB_TO_FLOAT(r), UB_TO_FLOAT(g), UB_TO_FLOAT(b), 1.0F);
}


void GLAPIENTRY
sav_Color3ubv (const GLubyte *v)
{
    COLOR0_F(3, UB_TO_FLOAT(v[0]), UB_TO_FLOAT(v[1]), UB_TO_FLOAT(v[2]), 1.0F);
}


void GLAPIENTRY
sav_Color3ui (GLuint r, GLuint g, GLuint b)
{
    COLOR0_F(3, UI_TO_FLOAT(r), UI_TO_FLOAT(g), UI_TO_FLOAT(b), 1.0F);
}


void GLAPIENTRY
sav_Color3uiv (const GLuint *v)
{
    COLOR0_F(3, UI_TO_FLOAT(v[0]), UI_TO_FLOAT(v[1]), UI_TO_FLOAT(v[2]), 1.0F);
}


void GLAPIENTRY
sav_Color3us (GLushort r, GLushort g, GLushort b)
{
    COLOR0_F(3, US_TO_FLOAT(r), US_TO_FLOAT(g), US_TO_FLOAT(b), 1.0F);
}


void GLAPIENTRY
sav_Color3usv (const GLushort *v)
{
    COLOR0_F(3, US_TO_FLOAT(v[0]), US_TO_FLOAT(v[1]), US_TO_FLOAT(v[2]), 1.0F);
}


void GLAPIENTRY
sav_Color4b (GLbyte r, GLbyte g, GLbyte b, GLbyte a)
{
    COLOR0_F(4, B_TO_FLOAT(r), B_TO_FLOAT(g), B_TO_FLOAT(b), B_TO_FLOAT(a));
}


void GLAPIENTRY
sav_Color4bv (const GLbyte *v)
{
    COLOR0_F(4, B_TO_FLOAT(v[0]), B_TO_FLOAT(v[1]), B_TO_FLOAT(v[2]), B_TO_FLOAT(v[3]));
}


void GLAPIENTRY
sav_Color4d (GLdouble r, GLdouble g, GLdouble b, GLdouble a)
{
    COLOR0_F(4, r, g, b, a);
}


void GLAPIENTRY
sav_Color4dv (const GLdouble *v)
{
    COLOR0_F(4, v[0], v[1], v[2], v[3]);
}


void GLAPIENTRY
sav_Color4i (GLint r, GLint g, GLint b, GLint a)
{
    COLOR0_F(4, I_TO_FLOAT(r), I_TO_FLOAT(g), I_TO_FLOAT(b), I_TO_FLOAT(a));
}


void GLAPIENTRY
sav_Color4iv (const GLint *v)
{
    COLOR0_F(4, I_TO_FLOAT(v[0]), I_TO_FLOAT(v[1]), I_TO_FLOAT(v[2]), I_TO_FLOAT(v[3]));
}


void GLAPIENTRY
sav_Color4s (GLshort r, GLshort g, GLshort b, GLshort a)
{
    COLOR0_F(4, S_TO_FLOAT(r), S_TO_FLOAT(g), S_TO_FLOAT(b), S_TO_FLOAT(a));
}


void GLAPIENTRY
sav_Color4sv (const GLshort *v)
{
    COLOR0_F(4, S_TO_FLOAT(v[0]), S_TO_FLOAT(v[1]), S_TO_FLOAT(v[2]), S_TO_FLOAT(v[3]));
}


void GLAPIENTRY
sav_Color4ub (GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
    COLOR0_F(4, UB_TO_FLOAT(r), UB_TO_FLOAT(g), UB_TO_FLOAT(b), UB_TO_FLOAT(a));
}


void GLAPIENTRY
sav_Color4ubv (const GLubyte *v)
{
    COLOR0_F(4, UB_TO_FLOAT(v[0]), UB_TO_FLOAT(v[1]), UB_TO_FLOAT(v[2]), UB_TO_FLOAT(v[3]));
}


void GLAPIENTRY
sav_Color4ui (GLuint r, GLuint g, GLuint b, GLuint a)
{
    COLOR0_F(4, UI_TO_FLOAT(r), UI_TO_FLOAT(g), UI_TO_FLOAT(b), UI_TO_FLOAT(a));
}


void GLAPIENTRY
sav_Color4uiv (const GLuint *v)
{
    COLOR0_F(4, UI_TO_FLOAT(v[0]), UI_TO_FLOAT(v[1]), UI_TO_FLOAT(v[2]), UI_TO_FLOAT(v[3]));
}


void GLAPIENTRY
sav_Color4us (GLushort r, GLushort g, GLushort b, GLushort a)
{
    COLOR0_F(4, US_TO_FLOAT(r), US_TO_FLOAT(g), US_TO_FLOAT(b), US_TO_FLOAT(a));
}


void GLAPIENTRY
sav_Color4usv (const GLushort *v)
{
    COLOR0_F(4, US_TO_FLOAT(v[0]), US_TO_FLOAT(v[1]), US_TO_FLOAT(v[2]), US_TO_FLOAT(v[3]));
}


void GLAPIENTRY
sav_SecondaryColor3b (GLbyte r, GLbyte g, GLbyte b)
{
    COLOR1_F(B_TO_FLOAT(r), B_TO_FLOAT(g), B_TO_FLOAT(b));
}


void GLAPIENTRY
sav_SecondaryColor3bv (const GLbyte *v)
{
    COLOR1_F(B_TO_FLOAT(v[0]), B_TO_FLOAT(v[1]), B_TO_FLOAT(v[2]));
}


void GLAPIENTRY
sav_SecondaryColor3d (GLdouble r, GLdouble g, GLdouble b)
{
    COLOR1_F(r, g, b);
}


void GLAPIENTRY
sav_SecondaryColor3dv (const GLdouble *v)
{
    COLOR1_F(v[0], v[1], v[2]);
}


void GLAPIENTRY
sav_SecondaryColor3i (GLint r, GLint g, GLint b)
{
    COLOR1_F(I_TO_FLOAT(r), I_TO_FLOAT(g), I_TO_FLOAT(b));
}


void GLAPIENTRY
sav_SecondaryColor3iv (const GLint *v)
{
    COLOR1_F(I_TO_FLOAT(v[0]), I_TO_FLOAT(v[1]), I_TO_FLOAT(v[2]));
}


void GLAPIENTRY
sav_SecondaryColor3s (GLshort r, GLshort g, GLshort b)
{
    COLOR1_F(S_TO_FLOAT(r), S_TO_FLOAT(g), S_TO_FLOAT(b));
}


void GLAPIENTRY
sav_SecondaryColor3sv (const GLshort *v)
{
    COLOR1_F(S_TO_FLOAT(v[0]), S_TO_FLOAT(v[1]), S_TO_FLOAT(v[2]));
}


void GLAPIENTRY
sav_SecondaryColor3ub (GLubyte r, GLubyte g, GLubyte b)
{
    COLOR1_F(UB_TO_FLOAT(r), UB_TO_FLOAT(g), UB_TO_FLOAT(b));
}


void GLAPIENTRY
sav_SecondaryColor3ubv (const GLubyte *v)
{
    COLOR1_F(UB_TO_FLOAT(v[0]), UB_TO_FLOAT(v[1]), UB_TO_FLOAT(v[2]));
}


void GLAPIENTRY
sav_SecondaryColor3ui (GLuint r, GLuint g, GLuint b)
{
    COLOR1_F(UI_TO_FLOAT(r), UI_TO_FLOAT(g), UI_TO_FLOAT(b));
}


void GLAPIENTRY
sav_SecondaryColor3uiv (const GLuint *v)
{
    COLOR1_F(UI_TO_FLOAT(v[0]), UI_TO_FLOAT(v[1]), UI_TO_FLOAT(v[2]));
}


void GLAPIENTRY
sav_SecondaryColor3us (GLushort r, GLushort g, GLushort b)
{
    COLOR1_F(US_TO_FLOAT(r), US_TO_FLOAT(g), US_TO_FLOAT(b));
}


void GLAPIENTRY
sav_SecondaryColor3usv (const GLushort *v)
{
    COLOR1_F(US_TO_FLOAT(v[0]), US_TO_FLOAT(v[1]), US_TO_FLOAT(v[2]));
}


void GLAPIENTRY
sav_FogCoordd (GLdouble c)
{
    FOGCOORD_F(c);
}


void GLAPIENTRY
sav_FogCoorddv (const GLdouble *v)
{
    FOGCOORD_F(v[0]);
}


void GLAPIENTRY
sav_Materiali (GLenum face, GLenum pname, GLint param)
{
    gl_assert(face == GL_FRONT || face == GL_BACK || face == GL_FRONT_AND_BACK);
    gl_assert(pname == GL_SHININESS);

    MAT_F(1, face, pname, (GLfloat)param, 0.0F, 0.0F, 0.0F);
}


void GLAPIENTRY
sav_Materialiv (GLenum face, GLenum pname, const GLint *params)
{
    gl_assert(face == GL_FRONT || face == GL_BACK || face == GL_FRONT_AND_BACK);

    gl_assert(pname == GL_AMBIENT ||
	      pname == GL_DIFFUSE ||
	      pname == GL_SPECULAR ||
	      pname == GL_EMISSION ||
	      pname == GL_SHININESS ||
	      pname == GL_AMBIENT_AND_DIFFUSE);

    if (pname != GL_SHININESS) {
	MAT_F(4, face, pname, I_TO_FLOAT(params[0]), I_TO_FLOAT(params[1]), I_TO_FLOAT(params[2]), I_TO_FLOAT(params[3]));
    } else {
	MAT_F(1, face, pname, (GLfloat)params[0], 0.0F, 0.0F, 0.0F);
    }
}


void GLAPIENTRY
sav_Normal3b (GLbyte x, GLbyte y, GLbyte z)
{
    NORMAL_F(B_TO_FLOAT(x), B_TO_FLOAT(y), B_TO_FLOAT(z));
}


void GLAPIENTRY
sav_Normal3bv (const GLbyte *v)
{
    NORMAL_F(B_TO_FLOAT(v[0]), B_TO_FLOAT(v[1]), B_TO_FLOAT(v[2]));
}


void GLAPIENTRY
sav_Normal3d (GLdouble x, GLdouble y, GLdouble z)
{
    NORMAL_F(x, y, z);
}


void GLAPIENTRY
sav_Normal3dv (const GLdouble *v)
{
    NORMAL_F(v[0], v[1], v[2]);
}


void GLAPIENTRY
sav_Normal3i (GLint x, GLint y, GLint z)
{
    NORMAL_F(I_TO_FLOAT(x), I_TO_FLOAT(y), I_TO_FLOAT(z));
}


void GLAPIENTRY
sav_Normal3iv (const GLint *v)
{
    NORMAL_F(I_TO_FLOAT(v[0]), I_TO_FLOAT(v[1]), I_TO_FLOAT(v[2]));
}


void GLAPIENTRY
sav_Normal3s (GLshort x, GLshort y, GLshort z)
{
    NORMAL_F(S_TO_FLOAT(x), S_TO_FLOAT(y), S_TO_FLOAT(z));
}


void GLAPIENTRY
sav_Normal3sv (const GLshort *v)
{
    NORMAL_F(S_TO_FLOAT(v[0]), S_TO_FLOAT(v[1]), S_TO_FLOAT(v[2]));
}


void GLAPIENTRY
sav_TexCoord1d (GLdouble s)
{
    TEXCOORD_F(1, 0, s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord1dv (const GLdouble *v)
{
    TEXCOORD_F(1, 0, v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord1i (GLint s)
{
    TEXCOORD_F(1, 0, (GLfloat)s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord1iv (const GLint *v)
{
    TEXCOORD_F(1, 0, (GLfloat)v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord1s (GLshort s)
{
    TEXCOORD_F(1, 0, (GLfloat)s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord1sv (const GLshort *v)
{
    TEXCOORD_F(1, 0, (GLfloat)v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord2d (GLdouble s, GLdouble t)
{
    TEXCOORD_F(2, 0, s, t, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord2dv (const GLdouble *v)
{
    TEXCOORD_F(2, 0, v[0], v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord2i (GLint s, GLint t)
{
    TEXCOORD_F(2, 0, (GLfloat)s, (GLfloat)t, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord2iv (const GLint *v)
{
    TEXCOORD_F(2, 0, (GLfloat)v[0], (GLfloat)v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord2s (GLshort s, GLshort t)
{
    TEXCOORD_F(2, 0, (GLfloat)s, (GLfloat)t, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord2sv (const GLshort *v)
{
    TEXCOORD_F(2, 0, (GLfloat)v[0], (GLfloat)v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
sav_TexCoord3d (GLdouble s, GLdouble t, GLdouble r)
{
    TEXCOORD_F(3, 0, s, t, r, 1.0F);
}


void GLAPIENTRY
sav_TexCoord3dv (const GLdouble *v)
{
    TEXCOORD_F(3, 0, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
sav_TexCoord3i (GLint s, GLint t, GLint r)
{
    TEXCOORD_F(3, 0, (GLfloat)s, (GLfloat)t, (GLfloat)r, 1.0F);
}


void GLAPIENTRY
sav_TexCoord3iv (const GLint *v)
{
    TEXCOORD_F(3, 0, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], 1.0F);
}


void GLAPIENTRY
sav_TexCoord3s (GLshort s, GLshort t, GLshort r)
{
    TEXCOORD_F(3, 0, (GLfloat)s, (GLfloat)t, (GLfloat)r, 1.0F);
}


void GLAPIENTRY
sav_TexCoord3sv (const GLshort *v)
{
    TEXCOORD_F(3, 0, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], 1.0F);
}


void GLAPIENTRY
sav_TexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    TEXCOORD_F(4, 0, s, t, r, q);
}


void GLAPIENTRY
sav_TexCoord4dv (const GLdouble *v)
{
    TEXCOORD_F(4, 0, v[0], v[1], v[2], v[3]);
}


void GLAPIENTRY
sav_TexCoord4i (GLint s, GLint t, GLint r, GLint q)
{
    TEXCOORD_F(4, 0, (GLfloat)s, (GLfloat)t, (GLfloat)r, (GLfloat)q);
}


void GLAPIENTRY
sav_TexCoord4iv (const GLint *v)
{
    TEXCOORD_F(4, 0, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], (GLfloat)v[3]);
}


void GLAPIENTRY
sav_TexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q)
{
    TEXCOORD_F(4, 0, (GLfloat)s, (GLfloat)t, (GLfloat)r, (GLfloat)q);
}


void GLAPIENTRY
sav_TexCoord4sv (const GLshort *v)
{
    TEXCOORD_F(4, 0, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], (GLfloat)v[3]);
}


void GLAPIENTRY
sav_MultiTexCoord1d (GLenum texture, GLdouble s)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(1, u, s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord1dv (GLenum texture, const GLdouble *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(1, u, v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord1i (GLenum texture, GLint s)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(1, u, (GLfloat)s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord1iv (GLenum texture, const GLint *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(1, u, (GLfloat)v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord1s (GLenum texture, GLshort s)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(1, u, (GLfloat)s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord1sv (GLenum texture, const GLshort *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(1, u, (GLfloat)v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord2d (GLenum texture, GLdouble s, GLdouble t)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(2, u, s, t, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord2dv (GLenum texture, const GLdouble *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(2, u, v[0], v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord2i (GLenum texture, GLint s, GLint t)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(2, u, (GLfloat)s, (GLfloat)t, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord2iv (GLenum texture, const GLint *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(2, u, (GLfloat)v[0], (GLfloat)v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord2s (GLenum texture, GLshort s, GLshort t)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(2, u, (GLfloat)s, (GLfloat)t, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord2sv (GLenum texture, const GLshort *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(2, u, (GLfloat)v[0], (GLfloat)v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord3d (GLenum texture, GLdouble s, GLdouble t, GLdouble r)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(3, u, s, t, r, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord3dv (GLenum texture, const GLdouble *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(3, u, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord3i (GLenum texture, GLint s, GLint t, GLint r)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(3, u, (GLfloat)s, (GLfloat)t, (GLfloat)r, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord3iv (GLenum texture, const GLint *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(3, u, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord3s (GLenum texture, GLshort s, GLshort t, GLshort r)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(3, u, (GLfloat)s, (GLfloat)t, (GLfloat)r, 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord3sv (GLenum texture, const GLshort *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(3, u, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], 1.0F);
}


void GLAPIENTRY
sav_MultiTexCoord4d (GLenum texture, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(4, u, s, t, r, q);
}


void GLAPIENTRY
sav_MultiTexCoord4dv (GLenum texture, const GLdouble *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(4, u, v[0], v[1], v[2], v[3]);
}


void GLAPIENTRY
sav_MultiTexCoord4i (GLenum texture, GLint s, GLint t, GLint r, GLint q)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(4, u, (GLfloat)s, (GLfloat)t, (GLfloat)r, (GLfloat)q);
}


void GLAPIENTRY
sav_MultiTexCoord4iv (GLenum texture, const GLint *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(4, u, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], (GLfloat)v[3]);
}


void GLAPIENTRY
sav_MultiTexCoord4s (GLenum texture, GLshort s, GLshort t, GLshort r, GLshort q)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(4, u, (GLfloat)s, (GLfloat)t, (GLfloat)r, (GLfloat)q);
}


void GLAPIENTRY
sav_MultiTexCoord4sv (GLenum texture, const GLshort *v)
{
    GLuint u = texture - GL_TEXTURE0;
    MULTITEXCOORD_F(4, u, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], (GLfloat)v[3]);
}


void GLAPIENTRY
sav_Vertex2d (GLdouble x, GLdouble y)
{
    VERTEX_F(2, x, y, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_Vertex2dv (const GLdouble *v)
{
    VERTEX_F(2, v[0], v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
sav_Vertex2i (GLint x, GLint y)
{
    VERTEX_F(2, (GLfloat)x, (GLfloat)y, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_Vertex2iv (const GLint *v)
{
    VERTEX_F(2, (GLfloat)v[0], (GLfloat)v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
sav_Vertex2s (GLshort x, GLshort y)
{
    VERTEX_F(2, (GLfloat)x, (GLfloat)y, 0.0F, 1.0F);
}


void GLAPIENTRY
sav_Vertex2sv (const GLshort *v)
{
    VERTEX_F(2, (GLfloat)v[0], (GLfloat)v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
sav_Vertex3d (GLdouble x, GLdouble y, GLdouble z)
{
    VERTEX_F(3, x, y, z, 1.0F);
}


void GLAPIENTRY
sav_Vertex3dv (const GLdouble *v)
{
    VERTEX_F(3, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
sav_Vertex3i (GLint x, GLint y, GLint z)
{
    VERTEX_F(3, (GLfloat)x, (GLfloat)y, (GLfloat)z, 1.0F);
}


void GLAPIENTRY
sav_Vertex3iv (const GLint *v)
{
    VERTEX_F(3, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], 1.0F);
}


void GLAPIENTRY
sav_Vertex3s (GLshort x, GLshort y, GLshort z)
{
    VERTEX_F(3, (GLfloat)x, (GLfloat)y, (GLfloat)z, 1.0F);
}


void GLAPIENTRY
sav_Vertex3sv (const GLshort *v)
{
    VERTEX_F(3, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], 1.0F);
}


void GLAPIENTRY
sav_Vertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    VERTEX_F(4, x, y, z, w);
}


void GLAPIENTRY
sav_Vertex4dv (const GLdouble *v)
{
    VERTEX_F(4, v[0], v[1], v[2], v[3]);
}


void GLAPIENTRY
sav_Vertex4i (GLint x, GLint y, GLint z, GLint w)
{
    VERTEX_F(4, (GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)w);
}


void GLAPIENTRY
sav_Vertex4iv (const GLint *v)
{
    VERTEX_F(4, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], (GLfloat)v[3]);
}


void GLAPIENTRY
sav_Vertex4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    VERTEX_F(4, (GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)w);
}


void GLAPIENTRY
sav_Vertex4sv (const GLshort *v)
{
    VERTEX_F(4, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], (GLfloat)v[3]);
}
