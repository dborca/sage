#include <assert.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "main/glapi.h"
#include "main/context.h"
#include "util/list.h"
#include "tnl.h"


#define SET_ARRAY(a, sz, t, stride, ptr)		\
    do {						\
	GLuint e_size;					\
	GLenum e_type;					\
	switch (t) {					\
	    case GL_BYTE:				\
		e_type = C_BYTE;			\
		e_size = sizeof(GLbyte);		\
		break;					\
	    case GL_UNSIGNED_BYTE:			\
		e_type = C_UNSIGNED_BYTE;		\
		e_size = sizeof(GLubyte);		\
		break;					\
	    case GL_SHORT:				\
		e_type = C_SHORT;			\
		e_size = sizeof(GLshort);		\
		break;					\
	    case GL_UNSIGNED_SHORT:			\
		e_type = C_UNSIGNED_SHORT;		\
		e_size = sizeof(GLushort);		\
		break;					\
	    case GL_INT:				\
		e_type = C_INT;				\
		e_size = sizeof(GLint);			\
		break;					\
	    case GL_UNSIGNED_INT:			\
		e_type = C_UNSIGNED_INT;		\
		e_size = sizeof(GLuint);		\
		break;					\
	    case GL_FLOAT:				\
		e_type = C_FLOAT;			\
		e_size = sizeof(GLfloat);		\
		break;					\
	    case GL_DOUBLE:				\
		e_type = C_DOUBLE;			\
		e_size = sizeof(GLdouble);		\
		break;					\
	    default:					\
		gl_assert(0);				\
		return;					\
	}						\
	a->size = sz;					\
	a->type = e_type;				\
	a->stride = stride;				\
	a->ptr = ptr;					\
	if (a->stride == 0) {				\
	    a->stride = a->size * e_size;		\
	}						\
	a->size--;					\
    } while (0)


enum {
    C_BYTE,
    C_UNSIGNED_BYTE,
    C_SHORT,
    C_UNSIGNED_SHORT,
    C_INT,
    C_UNSIGNED_INT,
    C_FLOAT,
    C_DOUBLE
};


static void GLAPIENTRY
imm_Invalid1 (const void *v)
{
    gl_assert(0);
}

static void GLAPIENTRY
imm_Invalid2 (int texture, const void *v)
{
    gl_assert(0);
}


static attr_func1 ptr_Invalid1 = imm_Invalid1;
static attr_func2 ptr_Invalid2 = imm_Invalid2;

#define PTR1(x) (attr_func1 *)&ctx_imm_table.x
#define PTR2(x) (attr_func2 *)&ctx_imm_table.x
#define INVALID1 (attr_func1 *)&ptr_Invalid1
#define INVALID2 (attr_func2 *)&ptr_Invalid2

static const attr_func1 *normal_func[4][8] = {
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 },
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 },
    { PTR1(Normal3bv),
      INVALID1,
      PTR1(Normal3sv),
      INVALID1,
      PTR1(Normal3iv),
      INVALID1,
      PTR1(Normal3fv),
      PTR1(Normal3dv) },
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 }
};

static const attr_func1 *color0_func[4][8] = {
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 },
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 },
    { PTR1(Color3bv),
      PTR1(Color3ubv),
      PTR1(Color3sv),
      PTR1(Color3usv),
      PTR1(Color3iv),
      PTR1(Color3uiv),
      PTR1(Color3fv),
      PTR1(Color3dv) },
    { PTR1(Color4bv),
      PTR1(Color4ubv),
      PTR1(Color4sv),
      PTR1(Color4usv),
      PTR1(Color4iv),
      PTR1(Color4uiv),
      PTR1(Color4fv),
      PTR1(Color4dv) }
};

static const attr_func1 *color1_func[4][8] = {
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 },
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 },
    { PTR1(SecondaryColor3bv),
      PTR1(SecondaryColor3ubv),
      PTR1(SecondaryColor3sv),
      PTR1(SecondaryColor3usv),
      PTR1(SecondaryColor3iv),
      PTR1(SecondaryColor3uiv),
      PTR1(SecondaryColor3fv),
      PTR1(SecondaryColor3dv) },
    { PTR1(SecondaryColor3bv),
      PTR1(SecondaryColor3ubv),
      PTR1(SecondaryColor3sv),
      PTR1(SecondaryColor3usv),
      PTR1(SecondaryColor3iv),
      PTR1(SecondaryColor3uiv),
      PTR1(SecondaryColor3fv),
      PTR1(SecondaryColor3dv) }
};

static const attr_func1 *fogcoord_func[4][8] = {
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      PTR1(FogCoordfv),
      PTR1(FogCoorddv) },
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 },
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 },
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 }
};

static const attr_func1 *edgeflag_func[4][8] = {
    { INVALID1,
      PTR1(EdgeFlagv), /* XXX if GLboolean == unsigned char */
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 },
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 },
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 },
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 }
};

static const attr_func1 *texcoord_func[4][8] = {
    { INVALID1,
      INVALID1,
      PTR1(TexCoord1sv),
      INVALID1,
      PTR1(TexCoord1iv),
      INVALID1,
      PTR1(TexCoord1fv),
      PTR1(TexCoord1dv) },
    { INVALID1,
      INVALID1,
      PTR1(TexCoord2sv),
      INVALID1,
      PTR1(TexCoord2iv),
      INVALID1,
      PTR1(TexCoord2fv),
      PTR1(TexCoord2dv) },
    { INVALID1,
      INVALID1,
      PTR1(TexCoord3sv),
      INVALID1,
      PTR1(TexCoord3iv),
      INVALID1,
      PTR1(TexCoord3fv),
      PTR1(TexCoord3dv) },
    { INVALID1,
      INVALID1,
      PTR1(TexCoord4sv),
      INVALID1,
      PTR1(TexCoord4iv),
      INVALID1,
      PTR1(TexCoord4fv),
      PTR1(TexCoord4dv) }
};

static const attr_func2 *multitexcoord_func[4][8] = {
    { INVALID2,
      INVALID2,
      PTR2(MultiTexCoord1sv),
      INVALID2,
      PTR2(MultiTexCoord1iv),
      INVALID2,
      PTR2(MultiTexCoord1fv),
      PTR2(MultiTexCoord1dv) },
    { INVALID2,
      INVALID2,
      PTR2(MultiTexCoord2sv),
      INVALID2,
      PTR2(MultiTexCoord2iv),
      INVALID2,
      PTR2(MultiTexCoord2fv),
      PTR2(MultiTexCoord2dv) },
    { INVALID2,
      INVALID2,
      PTR2(MultiTexCoord3sv),
      INVALID2,
      PTR2(MultiTexCoord3iv),
      INVALID2,
      PTR2(MultiTexCoord3fv),
      PTR2(MultiTexCoord3dv) },
    { INVALID2,
      INVALID2,
      PTR2(MultiTexCoord4sv),
      INVALID2,
      PTR2(MultiTexCoord4iv),
      INVALID2,
      PTR2(MultiTexCoord4fv),
      PTR2(MultiTexCoord4dv) }
};

static const attr_func1 *vertex_func[4][8] = {
    { INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1,
      INVALID1 },
    { INVALID1,
      INVALID1,
      PTR1(Vertex2sv),
      INVALID1,
      PTR1(Vertex2iv),
      INVALID1,
      PTR1(Vertex2fv),
      PTR1(Vertex2dv) },
    { INVALID1,
      INVALID1,
      PTR1(Vertex3sv),
      INVALID1,
      PTR1(Vertex3iv),
      INVALID1,
      PTR1(Vertex3fv),
      PTR1(Vertex3dv) },
    { INVALID1,
      INVALID1,
      PTR1(Vertex4sv),
      INVALID1,
      PTR1(Vertex4iv),
      INVALID1,
      PTR1(Vertex4fv),
      PTR1(Vertex4dv) }
};


void GLAPIENTRY
imm_ArrayElement (GLint i)
{
    CLIENT_ARRAY *a;
    list_foreach(a, &tnl_client.list2) {
	GLvoid *data = (GLubyte *)a->ptr + a->stride * i;
	a->func2(a->index, data);
    }
    list_foreach(a, &tnl_client.list1) {
	GLvoid *data = (GLubyte *)a->ptr + a->stride * i;
	a->func1(data);
    }
}


void GLAPIENTRY
imm_EnableClientState (GLenum cap)
{
    CLIENT_ARRAY *a;

    switch (cap) {
	case GL_VERTEX_ARRAY:
	    if (!(tnl_client.flags & TNL_VERTEX_BIT)) {
		tnl_client.flags |= TNL_VERTEX_BIT;
		a = &tnl_client.arrays[TNL_VERTEX];
		list_append(&tnl_client.list1, a);
	    }
	    break;
	case GL_NORMAL_ARRAY:
	    if (!(tnl_client.flags & TNL_NORMAL_BIT)) {
		tnl_client.flags |= TNL_NORMAL_BIT;
		a = &tnl_client.arrays[TNL_NORMAL];
		list_prepend(&tnl_client.list1, a);
	    }
	    break;
	case GL_COLOR_ARRAY:
	    if (!(tnl_client.flags & TNL_COLOR0_BIT)) {
		tnl_client.flags |= TNL_COLOR0_BIT;
		a = &tnl_client.arrays[TNL_COLOR0];
		list_prepend(&tnl_client.list1, a);
	    }
	    break;
	case GL_SECONDARY_COLOR_ARRAY:
	    if (!(tnl_client.flags & TNL_COLOR1_BIT)) {
		tnl_client.flags |= TNL_COLOR1_BIT;
		a = &tnl_client.arrays[TNL_COLOR1];
		list_prepend(&tnl_client.list1, a);
	    }
	    break;
	case GL_FOG_COORDINATE_ARRAY:
	    if (!(tnl_client.flags & TNL_FOGCOORD_BIT)) {
		tnl_client.flags |= TNL_FOGCOORD_BIT;
		a = &tnl_client.arrays[TNL_FOGCOORD];
		list_prepend(&tnl_client.list1, a);
	    }
	    break;
	case GL_EDGE_FLAG_ARRAY:
	    if (!(tnl_client.flags & TNL_EDGEFLAG_BIT)) {
		tnl_client.flags |= TNL_EDGEFLAG_BIT;
		a = &tnl_client.arrays[TNL_EDGEFLAG];
		list_prepend(&tnl_client.list1, a);
	    }
	    break;
	case GL_TEXTURE_COORD_ARRAY:
	    if (tnl_client.texunit) {
		if (!(tnl_client.flags & (TNL_TEXCOORD0_BIT << tnl_client.texunit))) {
		    tnl_client.flags |= TNL_TEXCOORD0_BIT << tnl_client.texunit;
		    a = &tnl_client.arrays[TNL_TEXCOORD0 + tnl_client.texunit];
		    list_prepend(&tnl_client.list2, a);
		    a->index = GL_TEXTURE0 + tnl_client.texunit;
		}
	    } else {
		if (!(tnl_client.flags & TNL_TEXCOORD0_BIT)) {
		    tnl_client.flags |= TNL_TEXCOORD0_BIT;
		    a = &tnl_client.arrays[TNL_TEXCOORD0];
		    list_prepend(&tnl_client.list1, a);
		}
	    }
	    break;
	default:
	    gl_assert(0);
	    return;
    }
}


void GLAPIENTRY
imm_DisableClientState (GLenum cap)
{
    CLIENT_ARRAY *a;

    switch (cap) {
	case GL_VERTEX_ARRAY:
	    if (tnl_client.flags & TNL_VERTEX_BIT) {
		tnl_client.flags &= ~TNL_VERTEX_BIT;
		a = &tnl_client.arrays[TNL_VERTEX];
		list_remove(a);
	    }
	    break;
	case GL_NORMAL_ARRAY:
	    if (tnl_client.flags & TNL_NORMAL_BIT) {
		tnl_client.flags &= ~TNL_NORMAL_BIT;
		a = &tnl_client.arrays[TNL_NORMAL];
		list_remove(a);
	    }
	    break;
	case GL_COLOR_ARRAY:
	    if (tnl_client.flags & TNL_COLOR0_BIT) {
		tnl_client.flags &= ~TNL_COLOR0_BIT;
		a = &tnl_client.arrays[TNL_COLOR0];
		list_remove(a);
	    }
	    break;
	case GL_SECONDARY_COLOR_ARRAY:
	    if (tnl_client.flags & TNL_COLOR1_BIT) {
		tnl_client.flags &= ~TNL_COLOR1_BIT;
		a = &tnl_client.arrays[TNL_COLOR1];
		list_remove(a);
	    }
	    break;
	case GL_FOG_COORDINATE_ARRAY:
	    if (tnl_client.flags & TNL_FOGCOORD_BIT) {
		tnl_client.flags &= ~TNL_FOGCOORD_BIT;
		a = &tnl_client.arrays[TNL_FOGCOORD];
		list_remove(a);
	    }
	    break;
	case GL_EDGE_FLAG_ARRAY:
	    if (tnl_client.flags & TNL_EDGEFLAG_BIT) {
		tnl_client.flags &= ~TNL_EDGEFLAG_BIT;
		a = &tnl_client.arrays[TNL_EDGEFLAG];
		list_remove(a);
	    }
	    break;
	case GL_TEXTURE_COORD_ARRAY:
	    if (tnl_client.flags & (TNL_TEXCOORD0_BIT << tnl_client.texunit)) {
		tnl_client.flags &= ~(TNL_TEXCOORD0_BIT << tnl_client.texunit);
		a = &tnl_client.arrays[TNL_TEXCOORD0 + tnl_client.texunit];
		list_remove(a);
	    }
	    break;
	case GL_INDEX_ARRAY:
	    break;
	default:
	    gl_assert(0);
	    return;
    }
}


void GLAPIENTRY
imm_ClientActiveTexture (GLenum texture)
{
    tnl_client.texunit = texture - GL_TEXTURE0;
}


void GLAPIENTRY
imm_VertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
    CLIENT_ARRAY *a = &tnl_client.arrays[TNL_VERTEX];
    SET_ARRAY(a, size, type, stride, ptr);
    a->func1 = *vertex_func[a->size][a->type];
}


void GLAPIENTRY
imm_NormalPointer (GLenum type, GLsizei stride, const GLvoid *ptr)
{
    CLIENT_ARRAY *a = &tnl_client.arrays[TNL_NORMAL];
    SET_ARRAY(a, 3, type, stride, ptr);
    a->func1 = *normal_func[a->size][a->type];
}


void GLAPIENTRY
imm_ColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
    CLIENT_ARRAY *a = &tnl_client.arrays[TNL_COLOR0];
    SET_ARRAY(a, size, type, stride, ptr);
    a->func1 = *color0_func[a->size][a->type];
}


void GLAPIENTRY
imm_SecondaryColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
    CLIENT_ARRAY *a = &tnl_client.arrays[TNL_COLOR1];
    SET_ARRAY(a, size, type, stride, ptr);
    a->func1 = *color1_func[a->size][a->type];
}


void GLAPIENTRY
imm_FogCoordPointer (GLenum type, GLsizei stride, const GLvoid *ptr)
{
    CLIENT_ARRAY *a = &tnl_client.arrays[TNL_FOGCOORD];
    SET_ARRAY(a, 1, type, stride, ptr);
    a->func1 = *fogcoord_func[a->size][a->type];
}


void GLAPIENTRY
imm_EdgeFlagPointer (GLsizei stride, const GLvoid *ptr)
{
    CLIENT_ARRAY *a = &tnl_client.arrays[TNL_EDGEFLAG];
    /* XXX if GLboolean == unsigned char */
    SET_ARRAY(a, 1, GL_UNSIGNED_BYTE, stride, ptr);
    a->func1 = *edgeflag_func[a->size][a->type];
}


void GLAPIENTRY
imm_TexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
    CLIENT_ARRAY *a = &tnl_client.arrays[TNL_TEXCOORD0 + tnl_client.texunit];
    SET_ARRAY(a, size, type, stride, ptr);
    if (tnl_client.texunit) {
	a->func2 = *multitexcoord_func[a->size][a->type];
    } else {
	a->func1 = *texcoord_func[a->size][a->type];
    }
}


void APIENTRY
imm_LockArraysEXT (GLint first, GLsizei count)
{
    /* XXX tobedone */
}


void APIENTRY
imm_UnlockArraysEXT (void)
{
    /* XXX tobedone */
}


void GLAPIENTRY
imm_DrawElements (GLenum mode, GLsizei count,
                  GLenum type, const GLvoid *indices)
{
    GLsizei i;

    imm_Begin(mode);
    switch (type) {
	case GL_UNSIGNED_BYTE: {
	    GLubyte *ptr = (GLubyte *)indices;
	    for (i = 0; i < count; i++) {
		imm_ArrayElement(ptr[i]);
	    }
	    break;
	}
	case GL_UNSIGNED_SHORT: {
	    GLushort *ptr = (GLushort *)indices;
	    for (i = 0; i < count; i++) {
		imm_ArrayElement(ptr[i]);
	    }
	    break;
	}
	case GL_UNSIGNED_INT: {
	    GLuint *ptr = (GLuint *)indices;
	    for (i = 0; i < count; i++) {
		imm_ArrayElement(ptr[i]);
	    }
	    break;
	}
	default:
	    gl_assert(0);
	    break;
    }
    imm_End();
}


void GLAPIENTRY
imm_DrawArrays (GLenum mode, GLint first, GLsizei count)
{
    imm_Begin(mode);
    while (count--) {
	imm_ArrayElement(first++);
    }
    imm_End();
}


void GLAPIENTRY
imm_DrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count,
		       GLenum type, const GLvoid *indices)
{
    imm_DrawElements(mode, count, type, indices);
}


typedef struct {
    GLboolean et, ec, en;
    GLint st, sc, sv;
    GLenum tc;
    GLuint pc, pn, pv;
    GLsizei s;
} IL_STATE;


#define _C (4 * sizeof(GLubyte))
#define _F sizeof(GLfloat)
static const IL_STATE il_table[] = {
    { GL_FALSE, GL_FALSE, GL_FALSE, 0, 0, 2, GL_NONE,          0,    0,         0,         2*_F },
    { GL_FALSE, GL_FALSE, GL_FALSE, 0, 0, 3, GL_NONE,          0,    0,         0,         3*_F },
    { GL_FALSE, GL_TRUE,  GL_FALSE, 0, 4, 2, GL_UNSIGNED_BYTE, 0,    0,    _C,        _C + 2*_F },
    { GL_FALSE, GL_TRUE,  GL_FALSE, 0, 4, 3, GL_UNSIGNED_BYTE, 0,    0,    _C,        _C + 3*_F },
    { GL_FALSE, GL_TRUE,  GL_FALSE, 0, 3, 3, GL_FLOAT,         0,    0,         3*_F,      6*_F },
    { GL_FALSE, GL_FALSE, GL_TRUE,  0, 0, 3, GL_NONE,          0,    0,         3*_F,      6*_F },
    { GL_FALSE, GL_TRUE,  GL_TRUE,  0, 4, 3, GL_FLOAT,         0,    4*_F,      7*_F,     10*_F },
    { GL_TRUE,  GL_FALSE, GL_FALSE, 2, 0, 3, GL_NONE,          0,    0,         2*_F,      5*_F },
    { GL_TRUE,  GL_FALSE, GL_FALSE, 4, 0, 4, GL_NONE,          0,    0,         4*_F,      8*_F },
    { GL_TRUE,  GL_TRUE,  GL_FALSE, 2, 4, 3, GL_UNSIGNED_BYTE, 2*_F, 0,    _C + 2*_F, _C + 5*_F },
    { GL_TRUE,  GL_TRUE,  GL_FALSE, 2, 3, 3, GL_FLOAT,         2*_F, 0,         5*_F,      8*_F },
    { GL_TRUE,  GL_FALSE, GL_TRUE,  2, 0, 3, GL_NONE,          0,    2*_F,      5*_F,      8*_F },
    { GL_TRUE,  GL_TRUE,  GL_TRUE,  2, 4, 3, GL_FLOAT,         2*_F, 6*_F,      9*_F,     12*_F },
    { GL_TRUE,  GL_TRUE,  GL_TRUE,  4, 4, 4, GL_FLOAT,         4*_F, 8*_F,     11*_F,     15*_F }
};
#undef _F
#undef _C


void GLAPIENTRY
imm_InterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer)
{
    GLsizei str;
    const IL_STATE *il;
    format -= GL_V2F;
    il = &il_table[format];
    str = stride;
    if (!str) {
	str = il->s;
    }
    imm_DisableClientState(GL_EDGE_FLAG_ARRAY);
    imm_DisableClientState(GL_INDEX_ARRAY);
    imm_DisableClientState(GL_SECONDARY_COLOR_ARRAY);
    imm_DisableClientState(GL_FOG_COORDINATE_ARRAY);
    /* XXX what about OTHER texture units?!? */
    if (il->et) {
	imm_EnableClientState(GL_TEXTURE_COORD_ARRAY);
	imm_TexCoordPointer(il->st, GL_FLOAT, str, pointer);
    } else {
	imm_DisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
    if (il->ec) {
	imm_EnableClientState(GL_COLOR_ARRAY);
	imm_ColorPointer(il->sc, il->tc, str, (GLubyte *)pointer + il->pc);
    } else {
	imm_DisableClientState(GL_COLOR_ARRAY);
    }
    if (il->en) {
	imm_EnableClientState(GL_NORMAL_ARRAY);
	imm_NormalPointer(GL_FLOAT, str, (GLubyte *)pointer + il->pn);
    } else {
	imm_DisableClientState(GL_NORMAL_ARRAY);
    }
    imm_EnableClientState(GL_VERTEX_ARRAY);
    imm_VertexPointer(il->sv, GL_FLOAT, str, (GLubyte *)pointer + il->pv);
}
