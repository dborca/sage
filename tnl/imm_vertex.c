#include <assert.h>

#include "GL/gl.h"
#include "glinternal.h"
#include "main/glapi.h"
#include "main/context.h"
#include "util/macros.h"
#include "tnl.h"


#define COLOR0_F(r, g, b, a)				\
    do {						\
	GLfloat4 *color0 = &tnl_vb.attr[TNL_COLOR0].ptr[tnl_vb.num];\
	color0[0][0] = r;				\
	color0[0][1] = g;				\
	color0[0][2] = b;				\
	color0[0][3] = a;				\
	tnl_vb.flags[tnl_vb.num] |= TNL_COLOR0_BIT;	\
	tnl_general_flags |= TNL_COLOR0_BIT;		\
    } while (0)

#define COLOR1_F(r, g, b)				\
    do {						\
	GLfloat4 *color1 = &tnl_vb.attr[TNL_COLOR1].ptr[tnl_vb.num];\
	color1[0][0] = r;				\
	color1[0][1] = g;				\
	color1[0][2] = b;				\
	tnl_vb.flags[tnl_vb.num] |= TNL_COLOR1_BIT;	\
	tnl_general_flags |= TNL_COLOR1_BIT;		\
    } while (0)

#define FOGCOORD_F(c)					\
    do {						\
	GLfloat4 *fogcoord = &tnl_vb.attr[TNL_FOGCOORD].ptr[tnl_vb.num];\
	fogcoord[0][0] = c;				\
	tnl_vb.flags[tnl_vb.num] |= TNL_FOGCOORD_BIT;	\
	tnl_general_flags |= TNL_FOGCOORD_BIT;		\
    } while (0)

#define EDGEFLAG_B(e)					\
    do {						\
	GLfloat4 *edgeflag = &tnl_vb.attr[TNL_EDGEFLAG].ptr[tnl_vb.num];\
	GLboolean *eflag = (GLboolean *)edgeflag;	\
	eflag[0] = e;					\
	tnl_vb.flags[tnl_vb.num] |= TNL_EDGEFLAG_BIT;	\
	tnl_general_flags |= TNL_EDGEFLAG_BIT;		\
    } while (0)

#define MAT_F(face, a, m0, m1, m2, m3)			\
    do {						\
	GLfloat4 *mat;					\
	if (face != GL_BACK) { /* front */		\
	    mat = &tnl_vb.attr[a##_f].ptr[tnl_vb.num];	\
	    mat[0][0] = m0;				\
	    mat[0][1] = m1;				\
	    mat[0][2] = m2;				\
	    mat[0][3] = m3;				\
	    tnl_vb.flags[tnl_vb.num] |= a##_f_BIT;	\
	    tnl_general_flags |= a##_f_BIT;		\
	}						\
	if (face != GL_FRONT) { /* back */		\
	    mat = &tnl_vb.attr[a##_b].ptr[tnl_vb.num];	\
	    mat[0][0] = m0;				\
	    mat[0][1] = m1;				\
	    mat[0][2] = m2;				\
	    mat[0][3] = m3;				\
	    tnl_vb.flags[tnl_vb.num] |= a##_b_BIT;	\
	    tnl_general_flags |= a##_b_BIT;		\
	}						\
    } while (0)

#define NORMAL_F(x, y, z)				\
    do {						\
	GLfloat4 *normal = &tnl_vb.attr[TNL_NORMAL].ptr[tnl_vb.num];\
	normal[0][0] = x;				\
	normal[0][1] = y;				\
	normal[0][2] = z;				\
	tnl_vb.flags[tnl_vb.num] |= TNL_NORMAL_BIT;	\
	tnl_general_flags |= TNL_NORMAL_BIT;		\
    } while (0)

#define TEXCOORD_F(n, u, s, t, r, q)			\
    do {						\
	GLfloat4 *texcoord = &tnl_vb.attr[TNL_TEXCOORD0 + u].ptr[tnl_vb.num];\
	texcoord[0][0] = s;				\
	texcoord[0][1] = t;				\
	texcoord[0][2] = r;				\
	texcoord[0][3] = q;				\
	tnl_vb.flags[tnl_vb.num] |= TNL_TEXCOORD0_BIT << u;\
	if (n < 4) {					\
	    tnl_general_flags |= TNL_TEXCOORD0_BIT << u;\
	} else {					\
	    tnl_general_flags |= (TNL_TEXCOORD0_BIT|TNL_TEXCOORD0W_BIT) << u;\
	}						\
    } while (0)

#define VERTEX_F(n, x, y, z, w)				\
    do {						\
	GLfloat4 *vertex = &tnl_vb.attr[TNL_VERTEX].ptr[tnl_vb.num++];\
	vertex[0][0] = x;				\
	vertex[0][1] = y;				\
	vertex[0][2] = z;				\
	vertex[0][3] = w;				\
	if (n == 4) {					\
	    tnl_general_flags |= TNL_VERTEXW_BIT;	\
	}						\
	if (tnl_vb.num == tnl_vb.max) {			\
	    tnl_flush();				\
	}						\
    } while (0)


/******************************************************************************
 * COLOR0
 */
void GLAPIENTRY
imm_Color3f (GLfloat r, GLfloat g, GLfloat b)
{
    COLOR0_F(r, g, b, 1.0F);
}


void GLAPIENTRY
imm_Color3fv (const GLfloat *v)
{
    COLOR0_F(v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
imm_Color4f (GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
    COLOR0_F(r, g, b, a);
}


void GLAPIENTRY
imm_Color4fv (const GLfloat *v)
{
    COLOR0_F(v[0], v[1], v[2], v[3]);
}


/******************************************************************************
 * COLOR1
 */
void GLAPIENTRY
imm_SecondaryColor3f (GLfloat r, GLfloat g, GLfloat b)
{
    COLOR1_F(r, g, b);
}


void GLAPIENTRY
imm_SecondaryColor3fv (const GLfloat *v)
{
    COLOR1_F(v[0], v[1], v[2]);
}


/******************************************************************************
 * FOGCOORD
 */
void GLAPIENTRY
imm_FogCoordf (GLfloat c)
{
    FOGCOORD_F(c);
}


void GLAPIENTRY
imm_FogCoordfv (const GLfloat *v)
{
    FOGCOORD_F(v[0]);
}


/******************************************************************************
 * EDGEFLAG
 */
void GLAPIENTRY
imm_EdgeFlag (GLboolean flag)
{
    EDGEFLAG_B(flag);
}


void GLAPIENTRY
imm_EdgeFlagv (const GLboolean *flag)
{
    EDGEFLAG_B(flag[0]);
}


/******************************************************************************
 * MATERIAL
 */
void GLAPIENTRY
imm_Materialf (GLenum face, GLenum pname, GLfloat param)
{
    switch (face) {
    case GL_FRONT:
    case GL_BACK:
    case GL_FRONT_AND_BACK:
	break;
    default:
	gl_assert(0);
	return;
    }

    switch (pname) {
	case GL_SHININESS:
	    MAT_F(face, TNL_SHININESS, param, 0.0F, 0.0F, 0.0F);
	    break;
	default:
	    gl_assert(0);
	    return;
    }
}


void GLAPIENTRY
imm_Materialfv (GLenum face, GLenum pname, const GLfloat *params)
{
    switch (face) {
    case GL_FRONT:
    case GL_BACK:
    case GL_FRONT_AND_BACK:
	break;
    default:
	gl_assert(0);
	return;
    }

    switch (pname) {
	case GL_AMBIENT:
	    MAT_F(face, TNL_AMBIENT, params[0], params[1], params[2], params[3]);
	    break;
	case GL_DIFFUSE:
	    MAT_F(face, TNL_DIFFUSE, params[0], params[1], params[2], params[3]);
	    break;
	case GL_SPECULAR:
	    MAT_F(face, TNL_SPECULAR, params[0], params[1], params[2], params[3]);
	    break;
	case GL_EMISSION:
	    MAT_F(face, TNL_EMISSION, params[0], params[1], params[2], params[3]);
	    break;
	case GL_SHININESS:
	    MAT_F(face, TNL_SHININESS, params[0], 0.0F, 0.0F, 0.0F);
	    break;
	case GL_AMBIENT_AND_DIFFUSE:
	    MAT_F(face, TNL_AMBIENT, params[0], params[1], params[2], params[3]);
	    MAT_F(face, TNL_DIFFUSE, params[0], params[1], params[2], params[3]);
	    break;
	default:
	    gl_assert(0);
	    return;
    }
}


/******************************************************************************
 * NORMAL
 */
void GLAPIENTRY
imm_Normal3f (GLfloat x, GLfloat y, GLfloat z)
{
    NORMAL_F(x, y, z);
}


void GLAPIENTRY
imm_Normal3fv (const GLfloat *v)
{
    NORMAL_F(v[0], v[1], v[2]);
}


/******************************************************************************
 * TEXCOORD
 */
void GLAPIENTRY
imm_TexCoord1f (GLfloat s)
{
    TEXCOORD_F(1, 0, s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord1fv (const GLfloat *v)
{
    TEXCOORD_F(1, 0, v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord2f (GLfloat s, GLfloat t)
{
    TEXCOORD_F(2, 0, s, t, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord2fv (const GLfloat *v)
{
    TEXCOORD_F(2, 0, v[0], v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord3f (GLfloat s, GLfloat t, GLfloat r)
{
    TEXCOORD_F(3, 0, s, t, r, 1.0F);
}


void GLAPIENTRY
imm_TexCoord3fv (const GLfloat *v)
{
    TEXCOORD_F(3, 0, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
imm_TexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    TEXCOORD_F(4, 0, s, t, r, q);
}


void GLAPIENTRY
imm_TexCoord4fv (const GLfloat *v)
{
    TEXCOORD_F(4, 0, v[0], v[1], v[2], v[3]);
}


void GLAPIENTRY
imm_MultiTexCoord1f (GLenum texture, GLfloat s)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(1, u, s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord1fv (GLenum texture, const GLfloat *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(1, u, v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord2f (GLenum texture, GLfloat s, GLfloat t)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(2, u, s, t, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord2fv (GLenum texture, const GLfloat *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(2, u, v[0], v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord3f (GLenum texture, GLfloat s, GLfloat t, GLfloat r)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(3, u, s, t, r, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord3fv (GLenum texture, const GLfloat *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(3, u, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord4f (GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(4, u, s, t, r, q);
}


void GLAPIENTRY
imm_MultiTexCoord4fv (GLenum texture, const GLfloat *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(4, u, v[0], v[1], v[2], v[3]);
}


/******************************************************************************
 * VERTEX
 */
void GLAPIENTRY
imm_Vertex2f (GLfloat x, GLfloat y)
{
    VERTEX_F(2, x, y, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_Vertex2fv (const GLfloat *v)
{
    VERTEX_F(2, v[0], v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
imm_Vertex3f (GLfloat x, GLfloat y, GLfloat z)
{
    VERTEX_F(3, x, y, z, 1.0F);
}


void GLAPIENTRY
imm_Vertex3fv (const GLfloat *v)
{
    VERTEX_F(3, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
imm_Vertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    VERTEX_F(4, x, y, z, w);
}


void GLAPIENTRY
imm_Vertex4fv (const GLfloat *v)
{
    VERTEX_F(4, v[0], v[1], v[2], v[3]);
}


/* variations */


void GLAPIENTRY
imm_Color3b (GLbyte r, GLbyte g, GLbyte b)
{
    COLOR0_F(B_TO_FLOAT(r), B_TO_FLOAT(g), B_TO_FLOAT(b), 1.0F);
}


void GLAPIENTRY
imm_Color3bv (const GLbyte *v)
{
    COLOR0_F(B_TO_FLOAT(v[0]), B_TO_FLOAT(v[1]), B_TO_FLOAT(v[2]), 1.0F);
}


void GLAPIENTRY
imm_Color3d (GLdouble r, GLdouble g, GLdouble b)
{
    COLOR0_F(r, g, b, 1.0F);
}


void GLAPIENTRY
imm_Color3dv (const GLdouble *v)
{
    COLOR0_F(v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
imm_Color3i (GLint r, GLint g, GLint b)
{
    COLOR0_F(I_TO_FLOAT(r), I_TO_FLOAT(g), I_TO_FLOAT(b), 1.0F);
}


void GLAPIENTRY
imm_Color3iv (const GLint *v)
{
    COLOR0_F(I_TO_FLOAT(v[0]), I_TO_FLOAT(v[1]), I_TO_FLOAT(v[2]), 1.0F);
}


void GLAPIENTRY
imm_Color3s (GLshort r, GLshort g, GLshort b)
{
    COLOR0_F(S_TO_FLOAT(r), S_TO_FLOAT(g), S_TO_FLOAT(b), 1.0F);
}


void GLAPIENTRY
imm_Color3sv (const GLshort *v)
{
    COLOR0_F(S_TO_FLOAT(v[0]), S_TO_FLOAT(v[1]), S_TO_FLOAT(v[2]), 1.0F);
}


void GLAPIENTRY
imm_Color3ub (GLubyte r, GLubyte g, GLubyte b)
{
    COLOR0_F(UB_TO_FLOAT(r), UB_TO_FLOAT(g), UB_TO_FLOAT(b), 1.0F);
}


void GLAPIENTRY
imm_Color3ubv (const GLubyte *v)
{
    COLOR0_F(UB_TO_FLOAT(v[0]), UB_TO_FLOAT(v[1]), UB_TO_FLOAT(v[2]), 1.0F);
}


void GLAPIENTRY
imm_Color3ui (GLuint r, GLuint g, GLuint b)
{
    COLOR0_F(UI_TO_FLOAT(r), UI_TO_FLOAT(g), UI_TO_FLOAT(b), 1.0F);
}


void GLAPIENTRY
imm_Color3uiv (const GLuint *v)
{
    COLOR0_F(UI_TO_FLOAT(v[0]), UI_TO_FLOAT(v[1]), UI_TO_FLOAT(v[2]), 1.0F);
}


void GLAPIENTRY
imm_Color3us (GLushort r, GLushort g, GLushort b)
{
    COLOR0_F(US_TO_FLOAT(r), US_TO_FLOAT(g), US_TO_FLOAT(b), 1.0F);
}


void GLAPIENTRY
imm_Color3usv (const GLushort *v)
{
    COLOR0_F(US_TO_FLOAT(v[0]), US_TO_FLOAT(v[1]), US_TO_FLOAT(v[2]), 1.0F);
}


void GLAPIENTRY
imm_Color4b (GLbyte r, GLbyte g, GLbyte b, GLbyte a)
{
    COLOR0_F(B_TO_FLOAT(r), B_TO_FLOAT(g), B_TO_FLOAT(b), B_TO_FLOAT(a));
}


void GLAPIENTRY
imm_Color4bv (const GLbyte *v)
{
    COLOR0_F(B_TO_FLOAT(v[0]), B_TO_FLOAT(v[1]), B_TO_FLOAT(v[2]), B_TO_FLOAT(v[3]));
}


void GLAPIENTRY
imm_Color4d (GLdouble r, GLdouble g, GLdouble b, GLdouble a)
{
    COLOR0_F(r, g, b, a);
}


void GLAPIENTRY
imm_Color4dv (const GLdouble *v)
{
    COLOR0_F(v[0], v[1], v[2], v[3]);
}


void GLAPIENTRY
imm_Color4i (GLint r, GLint g, GLint b, GLint a)
{
    COLOR0_F(I_TO_FLOAT(r), I_TO_FLOAT(g), I_TO_FLOAT(b), I_TO_FLOAT(a));
}


void GLAPIENTRY
imm_Color4iv (const GLint *v)
{
    COLOR0_F(I_TO_FLOAT(v[0]), I_TO_FLOAT(v[1]), I_TO_FLOAT(v[2]), I_TO_FLOAT(v[3]));
}


void GLAPIENTRY
imm_Color4s (GLshort r, GLshort g, GLshort b, GLshort a)
{
    COLOR0_F(S_TO_FLOAT(r), S_TO_FLOAT(g), S_TO_FLOAT(b), S_TO_FLOAT(a));
}


void GLAPIENTRY
imm_Color4sv (const GLshort *v)
{
    COLOR0_F(S_TO_FLOAT(v[0]), S_TO_FLOAT(v[1]), S_TO_FLOAT(v[2]), S_TO_FLOAT(v[3]));
}


void GLAPIENTRY
imm_Color4ub (GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
    COLOR0_F(UB_TO_FLOAT(r), UB_TO_FLOAT(g), UB_TO_FLOAT(b), UB_TO_FLOAT(a));
}


void GLAPIENTRY
imm_Color4ubv (const GLubyte *v)
{
    COLOR0_F(UB_TO_FLOAT(v[0]), UB_TO_FLOAT(v[1]), UB_TO_FLOAT(v[2]), UB_TO_FLOAT(v[3]));
}


void GLAPIENTRY
imm_Color4ui (GLuint r, GLuint g, GLuint b, GLuint a)
{
    COLOR0_F(UI_TO_FLOAT(r), UI_TO_FLOAT(g), UI_TO_FLOAT(b), UI_TO_FLOAT(a));
}


void GLAPIENTRY
imm_Color4uiv (const GLuint *v)
{
    COLOR0_F(UI_TO_FLOAT(v[0]), UI_TO_FLOAT(v[1]), UI_TO_FLOAT(v[2]), UI_TO_FLOAT(v[3]));
}


void GLAPIENTRY
imm_Color4us (GLushort r, GLushort g, GLushort b, GLushort a)
{
    COLOR0_F(US_TO_FLOAT(r), US_TO_FLOAT(g), US_TO_FLOAT(b), US_TO_FLOAT(a));
}


void GLAPIENTRY
imm_Color4usv (const GLushort *v)
{
    COLOR0_F(US_TO_FLOAT(v[0]), US_TO_FLOAT(v[1]), US_TO_FLOAT(v[2]), US_TO_FLOAT(v[3]));
}


void GLAPIENTRY
imm_SecondaryColor3b (GLbyte r, GLbyte g, GLbyte b)
{
    COLOR1_F(B_TO_FLOAT(r), B_TO_FLOAT(g), B_TO_FLOAT(b));
}


void GLAPIENTRY
imm_SecondaryColor3bv (const GLbyte *v)
{
    COLOR1_F(B_TO_FLOAT(v[0]), B_TO_FLOAT(v[1]), B_TO_FLOAT(v[2]));
}


void GLAPIENTRY
imm_SecondaryColor3d (GLdouble r, GLdouble g, GLdouble b)
{
    COLOR1_F(r, g, b);
}


void GLAPIENTRY
imm_SecondaryColor3dv (const GLdouble *v)
{
    COLOR1_F(v[0], v[1], v[2]);
}


void GLAPIENTRY
imm_SecondaryColor3i (GLint r, GLint g, GLint b)
{
    COLOR1_F(I_TO_FLOAT(r), I_TO_FLOAT(g), I_TO_FLOAT(b));
}


void GLAPIENTRY
imm_SecondaryColor3iv (const GLint *v)
{
    COLOR1_F(I_TO_FLOAT(v[0]), I_TO_FLOAT(v[1]), I_TO_FLOAT(v[2]));
}


void GLAPIENTRY
imm_SecondaryColor3s (GLshort r, GLshort g, GLshort b)
{
    COLOR1_F(S_TO_FLOAT(r), S_TO_FLOAT(g), S_TO_FLOAT(b));
}


void GLAPIENTRY
imm_SecondaryColor3sv (const GLshort *v)
{
    COLOR1_F(S_TO_FLOAT(v[0]), S_TO_FLOAT(v[1]), S_TO_FLOAT(v[2]));
}


void GLAPIENTRY
imm_SecondaryColor3ub (GLubyte r, GLubyte g, GLubyte b)
{
    COLOR1_F(UB_TO_FLOAT(r), UB_TO_FLOAT(g), UB_TO_FLOAT(b));
}


void GLAPIENTRY
imm_SecondaryColor3ubv (const GLubyte *v)
{
    COLOR1_F(UB_TO_FLOAT(v[0]), UB_TO_FLOAT(v[1]), UB_TO_FLOAT(v[2]));
}


void GLAPIENTRY
imm_SecondaryColor3ui (GLuint r, GLuint g, GLuint b)
{
    COLOR1_F(UI_TO_FLOAT(r), UI_TO_FLOAT(g), UI_TO_FLOAT(b));
}


void GLAPIENTRY
imm_SecondaryColor3uiv (const GLuint *v)
{
    COLOR1_F(UI_TO_FLOAT(v[0]), UI_TO_FLOAT(v[1]), UI_TO_FLOAT(v[2]));
}


void GLAPIENTRY
imm_SecondaryColor3us (GLushort r, GLushort g, GLushort b)
{
    COLOR1_F(US_TO_FLOAT(r), US_TO_FLOAT(g), US_TO_FLOAT(b));
}


void GLAPIENTRY
imm_SecondaryColor3usv (const GLushort *v)
{
    COLOR1_F(US_TO_FLOAT(v[0]), US_TO_FLOAT(v[1]), US_TO_FLOAT(v[2]));
}


void GLAPIENTRY
imm_FogCoordd (GLdouble c)
{
    FOGCOORD_F(c);
}


void GLAPIENTRY
imm_FogCoorddv (const GLdouble *v)
{
    FOGCOORD_F(v[0]);
}


void GLAPIENTRY
imm_Materiali (GLenum face, GLenum pname, GLint param)
{
    switch (face) {
    case GL_FRONT:
    case GL_BACK:
    case GL_FRONT_AND_BACK:
	break;
    default:
	gl_assert(0);
	return;
    }

    switch (pname) {
	case GL_SHININESS:
	    MAT_F(face, TNL_SHININESS, (GLfloat)param, 0.0F, 0.0F, 0.0F);
	    break;
	default:
	    gl_assert(0);
	    return;
    }
}


void GLAPIENTRY
imm_Materialiv (GLenum face, GLenum pname, const GLint *params)
{
    switch (face) {
    case GL_FRONT:
    case GL_BACK:
    case GL_FRONT_AND_BACK:
	break;
    default:
	gl_assert(0);
	return;
    }

    switch (pname) {
	case GL_AMBIENT:
	    MAT_F(face, TNL_AMBIENT, I_TO_FLOAT(params[0]), I_TO_FLOAT(params[1]), I_TO_FLOAT(params[2]), I_TO_FLOAT(params[3]));
	    break;
	case GL_DIFFUSE:
	    MAT_F(face, TNL_DIFFUSE, I_TO_FLOAT(params[0]), I_TO_FLOAT(params[1]), I_TO_FLOAT(params[2]), I_TO_FLOAT(params[3]));
	    break;
	case GL_SPECULAR:
	    MAT_F(face, TNL_SPECULAR, I_TO_FLOAT(params[0]), I_TO_FLOAT(params[1]), I_TO_FLOAT(params[2]), I_TO_FLOAT(params[3]));
	    break;
	case GL_EMISSION:
	    MAT_F(face, TNL_EMISSION, I_TO_FLOAT(params[0]), I_TO_FLOAT(params[1]), I_TO_FLOAT(params[2]), I_TO_FLOAT(params[3]));
	    break;
	case GL_SHININESS:
	    MAT_F(face, TNL_SHININESS, (GLfloat)params[0], 0.0F, 0.0F, 0.0F);
	    break;
	case GL_AMBIENT_AND_DIFFUSE:
	    MAT_F(face, TNL_AMBIENT, I_TO_FLOAT(params[0]), I_TO_FLOAT(params[1]), I_TO_FLOAT(params[2]), I_TO_FLOAT(params[3]));
	    MAT_F(face, TNL_DIFFUSE, I_TO_FLOAT(params[0]), I_TO_FLOAT(params[1]), I_TO_FLOAT(params[2]), I_TO_FLOAT(params[3]));
	    break;
	default:
	    gl_assert(0);
	    return;
    }
}


void GLAPIENTRY
imm_Normal3b (GLbyte x, GLbyte y, GLbyte z)
{
    NORMAL_F(B_TO_FLOAT(x), B_TO_FLOAT(y), B_TO_FLOAT(z));
}


void GLAPIENTRY
imm_Normal3bv (const GLbyte *v)
{
    NORMAL_F(B_TO_FLOAT(v[0]), B_TO_FLOAT(v[1]), B_TO_FLOAT(v[2]));
}


void GLAPIENTRY
imm_Normal3d (GLdouble x, GLdouble y, GLdouble z)
{
    NORMAL_F(x, y, z);
}


void GLAPIENTRY
imm_Normal3dv (const GLdouble *v)
{
    NORMAL_F(v[0], v[1], v[2]);
}


void GLAPIENTRY
imm_Normal3i (GLint x, GLint y, GLint z)
{
    NORMAL_F(I_TO_FLOAT(x), I_TO_FLOAT(y), I_TO_FLOAT(z));
}


void GLAPIENTRY
imm_Normal3iv (const GLint *v)
{
    NORMAL_F(I_TO_FLOAT(v[0]), I_TO_FLOAT(v[1]), I_TO_FLOAT(v[2]));
}


void GLAPIENTRY
imm_Normal3s (GLshort x, GLshort y, GLshort z)
{
    NORMAL_F(S_TO_FLOAT(x), S_TO_FLOAT(y), S_TO_FLOAT(z));
}


void GLAPIENTRY
imm_Normal3sv (const GLshort *v)
{
    NORMAL_F(S_TO_FLOAT(v[0]), S_TO_FLOAT(v[1]), S_TO_FLOAT(v[2]));
}


void GLAPIENTRY
imm_TexCoord1d (GLdouble s)
{
    TEXCOORD_F(1, 0, s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord1dv (const GLdouble *v)
{
    TEXCOORD_F(1, 0, v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord1i (GLint s)
{
    TEXCOORD_F(1, 0, (GLfloat)s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord1iv (const GLint *v)
{
    TEXCOORD_F(1, 0, (GLfloat)v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord1s (GLshort s)
{
    TEXCOORD_F(1, 0, (GLfloat)s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord1sv (const GLshort *v)
{
    TEXCOORD_F(1, 0, (GLfloat)v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord2d (GLdouble s, GLdouble t)
{
    TEXCOORD_F(2, 0, s, t, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord2dv (const GLdouble *v)
{
    TEXCOORD_F(2, 0, v[0], v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord2i (GLint s, GLint t)
{
    TEXCOORD_F(2, 0, (GLfloat)s, (GLfloat)t, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord2iv (const GLint *v)
{
    TEXCOORD_F(2, 0, (GLfloat)v[0], (GLfloat)v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord2s (GLshort s, GLshort t)
{
    TEXCOORD_F(2, 0, (GLfloat)s, (GLfloat)t, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord2sv (const GLshort *v)
{
    TEXCOORD_F(2, 0, (GLfloat)v[0], (GLfloat)v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
imm_TexCoord3d (GLdouble s, GLdouble t, GLdouble r)
{
    TEXCOORD_F(3, 0, s, t, r, 1.0F);
}


void GLAPIENTRY
imm_TexCoord3dv (const GLdouble *v)
{
    TEXCOORD_F(3, 0, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
imm_TexCoord3i (GLint s, GLint t, GLint r)
{
    TEXCOORD_F(3, 0, (GLfloat)s, (GLfloat)t, (GLfloat)r, 1.0F);
}


void GLAPIENTRY
imm_TexCoord3iv (const GLint *v)
{
    TEXCOORD_F(3, 0, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], 1.0F);
}


void GLAPIENTRY
imm_TexCoord3s (GLshort s, GLshort t, GLshort r)
{
    TEXCOORD_F(3, 0, (GLfloat)s, (GLfloat)t, (GLfloat)r, 1.0F);
}


void GLAPIENTRY
imm_TexCoord3sv (const GLshort *v)
{
    TEXCOORD_F(3, 0, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], 1.0F);
}


void GLAPIENTRY
imm_TexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    TEXCOORD_F(4, 0, s, t, r, q);
}


void GLAPIENTRY
imm_TexCoord4dv (const GLdouble *v)
{
    TEXCOORD_F(4, 0, v[0], v[1], v[2], v[3]);
}


void GLAPIENTRY
imm_TexCoord4i (GLint s, GLint t, GLint r, GLint q)
{
    TEXCOORD_F(4, 0, (GLfloat)s, (GLfloat)t, (GLfloat)r, (GLfloat)q);
}


void GLAPIENTRY
imm_TexCoord4iv (const GLint *v)
{
    TEXCOORD_F(4, 0, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], (GLfloat)v[3]);
}


void GLAPIENTRY
imm_TexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q)
{
    TEXCOORD_F(4, 0, (GLfloat)s, (GLfloat)t, (GLfloat)r, (GLfloat)q);
}


void GLAPIENTRY
imm_TexCoord4sv (const GLshort *v)
{
    TEXCOORD_F(4, 0, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], (GLfloat)v[3]);
}


void GLAPIENTRY
imm_MultiTexCoord1d (GLenum texture, GLdouble s)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(1, u, s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord1dv (GLenum texture, const GLdouble *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(1, u, v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord1i (GLenum texture, GLint s)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(1, u, (GLfloat)s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord1iv (GLenum texture, const GLint *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(1, u, (GLfloat)v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord1s (GLenum texture, GLshort s)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(1, u, (GLfloat)s, 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord1sv (GLenum texture, const GLshort *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(1, u, (GLfloat)v[0], 0.0F, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord2d (GLenum texture, GLdouble s, GLdouble t)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(2, u, s, t, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord2dv (GLenum texture, const GLdouble *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(2, u, v[0], v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord2i (GLenum texture, GLint s, GLint t)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(2, u, (GLfloat)s, (GLfloat)t, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord2iv (GLenum texture, const GLint *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(2, u, (GLfloat)v[0], (GLfloat)v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord2s (GLenum texture, GLshort s, GLshort t)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(2, u, (GLfloat)s, (GLfloat)t, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord2sv (GLenum texture, const GLshort *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(2, u, (GLfloat)v[0], (GLfloat)v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord3d (GLenum texture, GLdouble s, GLdouble t, GLdouble r)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(3, u, s, t, r, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord3dv (GLenum texture, const GLdouble *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(3, u, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord3i (GLenum texture, GLint s, GLint t, GLint r)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(3, u, (GLfloat)s, (GLfloat)t, (GLfloat)r, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord3iv (GLenum texture, const GLint *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(3, u, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord3s (GLenum texture, GLshort s, GLshort t, GLshort r)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(3, u, (GLfloat)s, (GLfloat)t, (GLfloat)r, 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord3sv (GLenum texture, const GLshort *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(3, u, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], 1.0F);
}


void GLAPIENTRY
imm_MultiTexCoord4d (GLenum texture, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(4, u, s, t, r, q);
}


void GLAPIENTRY
imm_MultiTexCoord4dv (GLenum texture, const GLdouble *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(4, u, v[0], v[1], v[2], v[3]);
}


void GLAPIENTRY
imm_MultiTexCoord4i (GLenum texture, GLint s, GLint t, GLint r, GLint q)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(4, u, (GLfloat)s, (GLfloat)t, (GLfloat)r, (GLfloat)q);
}


void GLAPIENTRY
imm_MultiTexCoord4iv (GLenum texture, const GLint *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(4, u, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], (GLfloat)v[3]);
}


void GLAPIENTRY
imm_MultiTexCoord4s (GLenum texture, GLshort s, GLshort t, GLshort r, GLshort q)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(4, u, (GLfloat)s, (GLfloat)t, (GLfloat)r, (GLfloat)q);
}


void GLAPIENTRY
imm_MultiTexCoord4sv (GLenum texture, const GLshort *v)
{
    GLuint u = texture - GL_TEXTURE0;
    TEXCOORD_F(4, u, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], (GLfloat)v[3]);
}


void GLAPIENTRY
imm_Vertex2d (GLdouble x, GLdouble y)
{
    VERTEX_F(2, x, y, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_Vertex2dv (const GLdouble *v)
{
    VERTEX_F(2, v[0], v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
imm_Vertex2i (GLint x, GLint y)
{
    VERTEX_F(2, (GLfloat)x, (GLfloat)y, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_Vertex2iv (const GLint *v)
{
    VERTEX_F(2, (GLfloat)v[0], (GLfloat)v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
imm_Vertex2s (GLshort x, GLshort y)
{
    VERTEX_F(2, (GLfloat)x, (GLfloat)y, 0.0F, 1.0F);
}


void GLAPIENTRY
imm_Vertex2sv (const GLshort *v)
{
    VERTEX_F(2, (GLfloat)v[0], (GLfloat)v[1], 0.0F, 1.0F);
}


void GLAPIENTRY
imm_Vertex3d (GLdouble x, GLdouble y, GLdouble z)
{
    VERTEX_F(3, x, y, z, 1.0F);
}


void GLAPIENTRY
imm_Vertex3dv (const GLdouble *v)
{
    VERTEX_F(3, v[0], v[1], v[2], 1.0F);
}


void GLAPIENTRY
imm_Vertex3i (GLint x, GLint y, GLint z)
{
    VERTEX_F(3, (GLfloat)x, (GLfloat)y, (GLfloat)z, 1.0F);
}


void GLAPIENTRY
imm_Vertex3iv (const GLint *v)
{
    VERTEX_F(3, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], 1.0F);
}


void GLAPIENTRY
imm_Vertex3s (GLshort x, GLshort y, GLshort z)
{
    VERTEX_F(3, (GLfloat)x, (GLfloat)y, (GLfloat)z, 1.0F);
}


void GLAPIENTRY
imm_Vertex3sv (const GLshort *v)
{
    VERTEX_F(3, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], 1.0F);
}


void GLAPIENTRY
imm_Vertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    VERTEX_F(4, x, y, z, w);
}


void GLAPIENTRY
imm_Vertex4dv (const GLdouble *v)
{
    VERTEX_F(4, v[0], v[1], v[2], v[3]);
}


void GLAPIENTRY
imm_Vertex4i (GLint x, GLint y, GLint z, GLint w)
{
    VERTEX_F(4, (GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)w);
}


void GLAPIENTRY
imm_Vertex4iv (const GLint *v)
{
    VERTEX_F(4, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], (GLfloat)v[3]);
}


void GLAPIENTRY
imm_Vertex4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    VERTEX_F(4, (GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)w);
}


void GLAPIENTRY
imm_Vertex4sv (const GLshort *v)
{
    VERTEX_F(4, (GLfloat)v[0], (GLfloat)v[1], (GLfloat)v[2], (GLfloat)v[3]);
}
