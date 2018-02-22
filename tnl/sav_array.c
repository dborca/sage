#include <assert.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "main/glapi.h"
#include "main/context.h"
#include "tnl.h"


static void GLAPIENTRY
sav_Invalid1 (const void *v)
{
    gl_assert(0);
}

static void GLAPIENTRY
sav_Invalid2 (int texture, const void *v)
{
    gl_assert(0);
}


#define PTR1(x) (attr_func1)sav_##x
#define PTR2(x) (attr_func2)sav_##x
#define INVALID1 (attr_func1)sav_Invalid1
#define INVALID2 (attr_func2)sav_Invalid2

static const attr_func1 normal_func[4][8] = {
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

static const attr_func1 color0_func[4][8] = {
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

static const attr_func1 color1_func[4][8] = {
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

static const attr_func1 fogcoord_func[4][8] = {
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

static const attr_func1 edgeflag_func[4][8] = {
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

static const attr_func1 texcoord_func[4][8] = {
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

static const attr_func2 multitexcoord_func[4][8] = {
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

static const attr_func1 vertex_func[4][8] = {
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
sav_ArrayElement (GLint i)
{
    if (tnl_client.flags & TNL_NORMAL_BIT) {
	CLIENT_ARRAY *a = &tnl_client.arrays[TNL_NORMAL];
	GLvoid *data = (GLubyte *)a->ptr + a->stride * i;
	normal_func[a->size][a->type](data);
    }
    if (tnl_client.flags & TNL_COLOR0_BIT) {
	CLIENT_ARRAY *a = &tnl_client.arrays[TNL_COLOR0];
	GLvoid *data = (GLubyte *)a->ptr + a->stride * i;
	color0_func[a->size][a->type](data);
    }
    if (tnl_client.flags & TNL_COLOR1_BIT) {
	CLIENT_ARRAY *a = &tnl_client.arrays[TNL_COLOR1];
	GLvoid *data = (GLubyte *)a->ptr + a->stride * i;
	color1_func[a->size][a->type](data);
    }
    if (tnl_client.flags & TNL_FOGCOORD_BIT) {
	CLIENT_ARRAY *a = &tnl_client.arrays[TNL_FOGCOORD];
	GLvoid *data = (GLubyte *)a->ptr + a->stride * i;
	fogcoord_func[a->size][a->type](data);
    }
    if (tnl_client.flags & TNL_EDGEFLAG_BIT) {
	CLIENT_ARRAY *a = &tnl_client.arrays[TNL_EDGEFLAG];
	GLvoid *data = (GLubyte *)a->ptr + a->stride * i;
	edgeflag_func[a->size][a->type](data);
    }
    /* XXX use generalized multitexcoord?!? */
    if (tnl_client.flags & TNL_TEXCOORD0_BIT) {
	CLIENT_ARRAY *a = &tnl_client.arrays[TNL_TEXCOORD0];
	GLvoid *data = (GLubyte *)a->ptr + a->stride * i;
	texcoord_func[a->size][a->type](data);
    }
    if (tnl_client.flags & TNL_TEXCOORD1_BIT) {
	CLIENT_ARRAY *a = &tnl_client.arrays[TNL_TEXCOORD1];
	GLvoid *data = (GLubyte *)a->ptr + a->stride * i;
	multitexcoord_func[a->size][a->type](GL_TEXTURE1, data);
    }
    if (tnl_client.flags & TNL_VERTEX_BIT) {
	CLIENT_ARRAY *a = &tnl_client.arrays[TNL_VERTEX];
	GLvoid *data = (GLubyte *)a->ptr + a->stride * i;
	vertex_func[a->size][a->type](data);
    }
}


void GLAPIENTRY
sav_EnableClientState (GLenum cap)
{
    imm_EnableClientState(cap);
}


void GLAPIENTRY
sav_DisableClientState (GLenum cap)
{
    imm_DisableClientState(cap);
}


void GLAPIENTRY
sav_VertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
    imm_VertexPointer(size, type, stride, ptr);
}


void GLAPIENTRY
sav_NormalPointer (GLenum type, GLsizei stride, const GLvoid *ptr)
{
    imm_NormalPointer(type, stride, ptr);
}


void GLAPIENTRY
sav_ColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
    imm_ColorPointer(size, type, stride, ptr);
}


void GLAPIENTRY
sav_SecondaryColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
    imm_SecondaryColorPointer(size, type, stride, ptr);
}


void GLAPIENTRY
sav_FogCoordPointer (GLenum type, GLsizei stride, const GLvoid *ptr)
{
    imm_FogCoordPointer(type, stride, ptr);
}


void GLAPIENTRY
sav_EdgeFlagPointer (GLsizei stride, const GLvoid *ptr)
{
    imm_EdgeFlagPointer(stride, ptr);
}


void GLAPIENTRY
sav_TexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
    imm_TexCoordPointer(size, type, stride, ptr);
}


void GLAPIENTRY
sav_DrawElements (GLenum mode, GLsizei count,
                  GLenum type, const GLvoid *indices)
{
    GLsizei i;

    sav_Begin(mode);
    switch (type) {
	case GL_UNSIGNED_BYTE: {
	    GLubyte *ptr = (GLubyte *)indices;
	    for (i = 0; i < count; i++) {
		sav_ArrayElement(ptr[i]);
	    }
	    break;
	}
	case GL_UNSIGNED_SHORT: {
	    GLushort *ptr = (GLushort *)indices;
	    for (i = 0; i < count; i++) {
		sav_ArrayElement(ptr[i]);
	    }
	    break;
	}
	case GL_UNSIGNED_INT: {
	    GLuint *ptr = (GLuint *)indices;
	    for (i = 0; i < count; i++) {
		sav_ArrayElement(ptr[i]);
	    }
	    break;
	}
	default:
	    gl_assert(0);
	    break;
    }
    sav_End();
}


void GLAPIENTRY
sav_DrawArrays (GLenum mode, GLint first, GLsizei count)
{
    sav_Begin(mode);
    while (count--) {
	sav_ArrayElement(first++);
    }
    sav_End();
}


void GLAPIENTRY
sav_DrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count,
		       GLenum type, const GLvoid *indices)
{
    sav_DrawElements(mode, count, type, indices);
}


void GLAPIENTRY
sav_InterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer)
{
    imm_InterleavedArrays(format, stride, pointer);
}
