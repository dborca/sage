#include <assert.h>
#include <stdlib.h>

#include "GL/gl.h"
#include "glinternal.h"
#include "context.h"
#include "tnl/tnl.h"
#include "glapi.h"
#include "util/list.h"
#include "dlist.h"


/*
 * The display list is allocated in blocks of DLIST_BLOCK_SIZE.  Each block
 * has the following layout:
 * OPCODE, operand1, ... operandN, OPCODE, operand1, ... operandM
 * N, M is the number of operands required by each node
 *
 * For example:
 * OP_CONTINUE is followed by 1 node, containing the address of the next block
 * OP_VERTEX2F is followed by 2 nodes, containing vertex' X & Y
 *
 * Note:
 * OP_VERTEX2FV is kept as OP_VERTEX2F, and so on...
 */
#define DLIST_BLOCK_SIZE 256


DLIST ctx_dlist_list;

static GLuint ctx_dlist_hinumber;
GLenum ctx_dlist_mode;
GLuint ctx_dlist_base;

static DLIST *ctx_dlist_current_list;
static DL_NODE *ctx_dlist_current_block;
static int ctx_dlist_current_size;

static const int opsize[] = {
    0,     /* OP_CONTINUE */

    1 + 1, /* OP_BEGIN */
    1 + 0, /* OP_END */
    1 + 2, /* OP_VERTEX2F */
    1 + 3, /* OP_VERTEX3F */
    1 + 4, /* OP_VERTEX4F */
    1 + 3, /* OP_COLOR3F */
    1 + 4, /* OP_COLOR4F */
    1 + 3, /* OP_SECONDARYCOLOR3F */
    1 + 3, /* OP_NORMAL3F */
    1 + 1, /* OP_TEXCOORD1F */
    1 + 2, /* OP_TEXCOORD2F */
    1 + 3, /* OP_TEXCOORD3F */
    1 + 4, /* OP_TEXCOORD4F */
    1 + 2, /* OP_MULTITEXCOORD1F */
    1 + 3, /* OP_MULTITEXCOORD2F */
    1 + 4, /* OP_MULTITEXCOORD3F */
    1 + 5, /* OP_MULTITEXCOORD4F */
    1 + 3, /* OP_MATERIALF */
    1 + 6, /* OP_MATERIALFV */
    1 + 1, /* OP_FOGCOORDF */
    1 + 1, /* OP_EDGEFLAG */
    1 + 4, /* OP_RECTF */
    1 + 0, /* OP_PUSHMATRIX */
    1 + 0, /* OP_POPMATRIX */
    1 + 3, /* OP_TRANSLATEF */
    1 + 4, /* OP_ROTATEF */
    1 + 3, /* OP_SCALEF */
    1 + 0, /* OP_LOADIDENTITY */
    1 + 1, /* OP_MATRIXMODE */
    1 + 1, /* OP_ENABLE */
    1 + 1, /* OP_DISABLE */
    1 + 1, /* OP_FRONTFACE */
    1 + 1, /* OP_SHADEMODEL */
    1 + 1, /* OP_CULLFACE */
    1 + 4, /* OP_BLENDFUNC */
    1 + 1, /* OP_DEPTHMASK */
    1 + 1, /* OP_DEPTHFUNC */
    1 + 2, /* OP_ALPHAFUNC */
    1 + 2, /* OP_BINDTEX */
    1 + 1, /* OP_ACTIVETEX */
    1 + 6, /* OP_TEXENV */
    1 + 6, /* OP_TEXGEN */
    1 + 6, /* OP_TEXPARAM */
    1 + 1, /* OP_CALLLIST */

    0      /* OP_EOL */
};


/******************************************************************************
 * display list management
 */


DL_NODE *
dlist_new_operation (DL_OPCODE op)
{
    int index;
    int size = opsize[op];
    if (ctx_dlist_current_size + size + 2 > DLIST_BLOCK_SIZE) {
	/* need a new block */
	DL_NODE *block = malloc(DLIST_BLOCK_SIZE * sizeof(DL_NODE));
	gl_assert(block != NULL);
	/* link the old block to current one */
	ctx_dlist_current_block[ctx_dlist_current_size].op = OP_CONTINUE;
	ctx_dlist_current_block[ctx_dlist_current_size + 1].next = block;
	/* and reset */
	ctx_dlist_current_size = 0;
	ctx_dlist_current_block = block;
    }
    index = ctx_dlist_current_size;
    ctx_dlist_current_size += size;
    ctx_dlist_current_block[index].op = op;
    return &ctx_dlist_current_block[index];
}


static DLIST *
dlist_get_list (GLuint real_id)
{
    DLIST *obj;
    list_foreach (obj, &ctx_dlist_list) {
	if (obj->name == real_id) {
	    return obj;
	}
    }
    return NULL;
}


static DL_NODE *
dlist_get_block (GLuint real_id)
{
    DLIST *obj;
    list_foreach (obj, &ctx_dlist_list) {
	if (obj->name == real_id) {
	    return obj->block;
	}
    }
    return NULL;
}


static void GLAPIENTRY
dlist_playback (GLuint real_id)
{
    DL_NODE *node = dlist_get_block(real_id);
    if (node == NULL) {
	return;
    }
    while (node->op != OP_EOL) {
	if (node->op == OP_CONTINUE) {
	    node = (node + 1)->next;
	    continue;
	}
	switch (node->op) {
	    case OP_BEGIN:
		imm_Begin((node + 1)->i);
		break;
	    case OP_END:
		imm_End();
		break;
	    case OP_VERTEX2F:
		imm_Vertex2fv((const GLfloat *)(node + 1));
		break;
	    case OP_VERTEX3F:
		imm_Vertex3fv((const GLfloat *)(node + 1));
		break;
	    case OP_VERTEX4F:
		imm_Vertex4fv((const GLfloat *)(node + 1));
		break;
	    case OP_COLOR3F:
		imm_Color3fv((const GLfloat *)(node + 1));
		break;
	    case OP_COLOR4F:
		imm_Color4fv((const GLfloat *)(node + 1));
		break;
	    case OP_SECONDARYCOLOR3F:
		imm_SecondaryColor3fv((const GLfloat *)(node + 1));
		break;
	    case OP_NORMAL3F:
		imm_Normal3fv((const GLfloat *)(node + 1));
		break;
	    case OP_TEXCOORD1F:
		imm_TexCoord1fv((const GLfloat *)(node + 1));
		break;
	    case OP_TEXCOORD2F:
		imm_TexCoord2fv((const GLfloat *)(node + 1));
		break;
	    case OP_TEXCOORD3F:
		imm_TexCoord3fv((const GLfloat *)(node + 1));
		break;
	    case OP_TEXCOORD4F:
		imm_TexCoord4fv((const GLfloat *)(node + 1));
		break;
	    case OP_MULTITEXCOORD1F:
		imm_MultiTexCoord1fv((node + 1)->i, (const GLfloat *)(node + 2));
		break;
	    case OP_MULTITEXCOORD2F:
		imm_MultiTexCoord2fv((node + 1)->i, (const GLfloat *)(node + 2));
		break;
	    case OP_MULTITEXCOORD3F:
		imm_MultiTexCoord3fv((node + 1)->i, (const GLfloat *)(node + 2));
		break;
	    case OP_MULTITEXCOORD4F:
		imm_MultiTexCoord4fv((node + 1)->i, (const GLfloat *)(node + 2));
		break;
	    case OP_MATERIALF:
		imm_Materialf((node + 1)->i, (node + 2)->i, (node + 3)->f);
		break;
	    case OP_MATERIALFV:
		imm_Materialfv((node + 1)->i, (node + 2)->i, (const GLfloat *)(node + 3));
		break;
	    case OP_FOGCOORDF:
		imm_FogCoordf((node + 1)->f);
		break;
	    case OP_EDGEFLAG:
		imm_EdgeFlag((node + 1)->i);
		break;
	    case OP_RECTF:
		imm_Rectf((node + 1)->f, (node + 2)->f, (node + 3)->f, (node + 4)->f);
		break;
	    case OP_PUSHMATRIX:
		imm_PushMatrix();
		break;
	    case OP_POPMATRIX:
		imm_PopMatrix();
		break;
	    case OP_TRANSLATEF:
		imm_Translatef((node + 1)->f, (node + 2)->f, (node + 3)->f);
		break;
	    case OP_ROTATEF:
		imm_Rotatef((node + 1)->f, (node + 2)->f, (node + 3)->f, (node + 4)->f);
		break;
	    case OP_SCALEF:
		imm_Scalef((node + 1)->f, (node + 2)->f, (node + 3)->f);
		break;
	    case OP_LOADIDENTITY:
		imm_LoadIdentity();
		break;
	    case OP_MATRIXMODE:
		imm_MatrixMode((node + 1)->i);
		break;
	    case OP_ENABLE:
		imm_Enable((node + 1)->i);
		break;
	    case OP_DISABLE:
		imm_Disable((node + 1)->i);
		break;
	    case OP_FRONTFACE:
		imm_FrontFace((node + 1)->i);
		break;
	    case OP_SHADEMODEL:
		imm_ShadeModel((node + 1)->i);
		break;
	    case OP_CULLFACE:
		imm_CullFace((node + 1)->i);
		break;
	    case OP_BINDTEX:
		imm_BindTexture((node + 1)->i, (node + 2)->i);
		break;
	    case OP_ACTIVETEX:
		imm_ActiveTexture((node + 1)->i);
		break;
	    case OP_BLENDFUNC:
		imm_BlendFuncSeparate((node + 1)->i, (node + 2)->i, (node + 3)->i, (node + 4)->i);
		break;
	    case OP_DEPTHMASK:
		imm_DepthMask((node + 1)->i);
		break;
	    case OP_DEPTHFUNC:
		imm_DepthFunc((node + 1)->i);
		break;
	    case OP_ALPHAFUNC:
		imm_AlphaFunc((node + 1)->i, (node + 2)->f);
		break;
	    case OP_TEXENV:
		imm_TexEnvfv((node + 1)->i, (node + 2)->i, (const GLfloat *)(node + 3));
		break;
	    case OP_TEXGEN:
		imm_TexGenfv((node + 1)->i, (node + 2)->i, (const GLfloat *)(node + 3));
		break;
	    case OP_TEXPARAM:
		imm_TexParameterfv((node + 1)->i, (node + 2)->i, (const GLfloat *)(node + 3));
		break;
	    case OP_CALLLIST:
		imm_CallList((node + 1)->i);
		break;
	    default:
		gl_assert(0);
		return;
	}
	node += opsize[node->op];
    }
}


static DLIST *
dlist_destroy (GLuint real_id)
{
    void *block;
    DLIST *obj;
    DL_NODE *node;

    obj = dlist_get_list(real_id);
    if (obj == NULL) {
	return NULL;
    }
    block = node = obj->block;
    if (block == NULL) {
	return obj;
    }
    while (node->op != OP_EOL) {
	switch (node->op) {
	    case OP_CONTINUE:
		node = (node + 1)->next;
		free(block);
		block = node;
		break;
	    case OP_BEGIN:
	    case OP_END:
	    case OP_VERTEX2F:
	    case OP_VERTEX3F:
	    case OP_VERTEX4F:
	    case OP_COLOR3F:
	    case OP_COLOR4F:
	    case OP_SECONDARYCOLOR3F:
	    case OP_NORMAL3F:
	    case OP_TEXCOORD1F:
	    case OP_TEXCOORD2F:
	    case OP_TEXCOORD3F:
	    case OP_TEXCOORD4F:
	    case OP_MULTITEXCOORD1F:
	    case OP_MULTITEXCOORD2F:
	    case OP_MULTITEXCOORD3F:
	    case OP_MULTITEXCOORD4F:
	    case OP_MATERIALF:
	    case OP_MATERIALFV:
	    case OP_FOGCOORDF:
	    case OP_EDGEFLAG:
	    case OP_RECTF:
	    case OP_PUSHMATRIX:
	    case OP_POPMATRIX:
	    case OP_TRANSLATEF:
	    case OP_ROTATEF:
	    case OP_SCALEF:
	    case OP_LOADIDENTITY:
	    case OP_MATRIXMODE:
	    case OP_ENABLE:
	    case OP_DISABLE:
	    case OP_FRONTFACE:
	    case OP_SHADEMODEL:
	    case OP_CULLFACE:
	    case OP_BINDTEX:
	    case OP_ACTIVETEX:
	    case OP_BLENDFUNC:
	    case OP_DEPTHMASK:
	    case OP_DEPTHFUNC:
	    case OP_ALPHAFUNC:
	    case OP_TEXENV:
	    case OP_TEXGEN:
	    case OP_TEXPARAM:
	    case OP_CALLLIST:
		node += opsize[node->op];
		break;
	    case OP_EOL:
	    case OP_INVALID:
		gl_assert(0);
		return NULL;
	}
    }
    free(block);
    return obj;
}


static void
dlist_optimizer (void)
{
}


static void
dlist_compiler (void)
{
}


/******************************************************************************
 * the alternate entrypoints, for when building the display list
 */
#define NYI() gl_error(NULL, GL_INVALID_ENUM, "%s:\n", __FUNCTION__)
void GLAPIENTRY
sav_ActiveTexture (GLenum texture)
{
    DL_NODE *node = dlist_new_operation(OP_ACTIVETEX);
    (node + 1)->i = texture;
}


void GLAPIENTRY
sav_AlphaFunc (GLenum func, GLclampf ref)
{
    DL_NODE *node = dlist_new_operation(OP_ALPHAFUNC);
    (node + 1)->i = func;
    (node + 2)->f = ref;
}


void GLAPIENTRY
sav_BindTexture (GLenum target, GLuint texture)
{
    DL_NODE *node = dlist_new_operation(OP_BINDTEX);
    (node + 1)->i = target;
    (node + 2)->i = texture;
}


void GLAPIENTRY
sav_BlendFunc (GLenum sfactor, GLenum dfactor)
{
    sav_BlendFuncSeparate(sfactor, dfactor, sfactor, dfactor);
}


void GLAPIENTRY
sav_BlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    DL_NODE *node = dlist_new_operation(OP_BLENDFUNC);
    (node + 1)->i = sfactorRGB;
    (node + 2)->i = dfactorRGB;
    (node + 3)->i = sfactorAlpha;
    (node + 4)->i = dfactorAlpha;
}


void GLAPIENTRY
sav_CallList (GLuint list)
{
    DL_NODE *node = dlist_new_operation(OP_CALLLIST);
    (node + 1)->i = list;
}


void GLAPIENTRY
sav_CallLists (GLsizei n, GLenum type, const GLvoid *lists)
{
    NYI();
}


void GLAPIENTRY
sav_Clear (GLbitfield mask)
{
    NYI();
}


void GLAPIENTRY
sav_ClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    NYI();
}


void GLAPIENTRY
sav_ClearDepth (GLclampd depth)
{
    NYI();
}


void GLAPIENTRY
sav_ClearStencil (GLint s)
{
    NYI();
}


void GLAPIENTRY
sav_ClientActiveTexture (GLenum texture)
{
    NYI();
}


void GLAPIENTRY
sav_ClipPlane (GLenum plane, const GLdouble *equation)
{
    NYI();
}


void GLAPIENTRY
sav_ColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    NYI();
}


void GLAPIENTRY
sav_ColorMaterial (GLenum face, GLenum mode)
{
    NYI();
}


void GLAPIENTRY
sav_CompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{
    NYI();
}


void GLAPIENTRY
sav_CompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    NYI();
}


void GLAPIENTRY
sav_CopyTexSubImage2D (GLenum target, GLint level,
		       GLint xoffset, GLint yoffset,
		       GLint x, GLint y,
		       GLsizei width, GLsizei height)
{
    NYI();
}


void GLAPIENTRY
sav_CullFace (GLenum mode)
{
    DL_NODE *node = dlist_new_operation(OP_CULLFACE);
    (node + 1)->i = mode;
}


void GLAPIENTRY
sav_DeleteLists (GLuint list, GLsizei range)
{
    NYI();
}


void GLAPIENTRY
sav_DeleteTextures (GLsizei n, const GLuint *textures)
{
    NYI();
}


void GLAPIENTRY
sav_DepthFunc (GLenum func)
{
    DL_NODE *node = dlist_new_operation(OP_DEPTHFUNC);
    (node + 1)->i = func;
}


void GLAPIENTRY
sav_DepthMask (GLboolean flag)
{
    DL_NODE *node = dlist_new_operation(OP_DEPTHMASK);
    (node + 1)->i = flag;
}


void GLAPIENTRY
sav_DepthRange (GLclampd near_val, GLclampd far_val)
{
    NYI();
}


void GLAPIENTRY
sav_Disable (GLenum cap)
{
    DL_NODE *node = dlist_new_operation(OP_DISABLE);
    (node + 1)->i = cap;
}


void GLAPIENTRY
sav_DrawBuffer (GLenum mode)
{
    NYI();
}


void GLAPIENTRY
sav_Enable (GLenum cap)
{
    DL_NODE *node = dlist_new_operation(OP_ENABLE);
    (node + 1)->i = cap;
}


void GLAPIENTRY
sav_EndList (void)
{
    DL_NODE *node = dlist_new_operation(OP_EOL);

    /* switch to immediate mode */
    gl_switch_imm();

    /* flush the list */
    dlist_compiler();
    if (ctx_dlist_mode == GL_COMPILE_AND_EXECUTE) {
	dlist_playback(ctx_dlist_current_list->name);
    }
}


void GLAPIENTRY
sav_Fogf (GLenum pname, GLfloat param)
{
    NYI();
}


void GLAPIENTRY
sav_Fogfv (GLenum pname, const GLfloat *params)
{
    NYI();
}


void GLAPIENTRY
sav_Fogi (GLenum pname, GLint param)
{
    NYI();
}


void GLAPIENTRY
sav_Fogiv (GLenum pname, const GLint *params)
{
    NYI();
}


void GLAPIENTRY
sav_FrontFace (GLenum mode)
{
    DL_NODE *node = dlist_new_operation(OP_FRONTFACE);
    (node + 1)->i = mode;
}


void GLAPIENTRY
sav_Frustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    NYI();
}


GLuint GLAPIENTRY
sav_GenLists (GLsizei range)
{
    NYI();
    return 0;
}


void GLAPIENTRY
sav_GenTextures (GLsizei n, GLuint *textures)
{
    NYI();
}


void GLAPIENTRY
sav_GetBooleanv (GLenum pname, GLboolean *params)
{
    NYI();
}


void GLAPIENTRY
sav_GetCompressedTexImage (GLenum target, GLint lod, GLvoid *img)
{
    NYI();
}


void GLAPIENTRY
sav_GetDoublev (GLenum pname, GLdouble *params)
{
    NYI();
}


GLenum GLAPIENTRY
sav_GetError (void)
{
    NYI();
    return GL_INVALID_ENUM;
}


void GLAPIENTRY
sav_GetFloatv (GLenum pname, GLfloat *params)
{
    NYI();
}


void GLAPIENTRY
sav_GetIntegerv (GLenum pname, GLint *params)
{
    NYI();
}


const GLubyte * GLAPIENTRY
sav_GetString (GLenum name)
{
    NYI();
    return NULL;
}


void GLAPIENTRY
sav_GetTexImage (GLenum target, GLint level,
		 GLenum format, GLenum type,
		 GLvoid *pixels)
{
    NYI();
}


void GLAPIENTRY
sav_GetTexLevelParameterfv (GLenum target, GLint level,
			    GLenum pname, GLfloat *params)
{
    NYI();
}


void GLAPIENTRY
sav_GetTexLevelParameteriv (GLenum target, GLint level,
			    GLenum pname, GLint *params)
{
    NYI();
}


void GLAPIENTRY
sav_Hint (GLenum target, GLenum mode)
{
    NYI();
}


GLboolean GLAPIENTRY
sav_IsEnabled (GLenum cap)
{
    NYI();
    return GL_FALSE;
}


GLboolean GLAPIENTRY
sav_IsList (GLuint list)
{
    NYI();
    return GL_FALSE;
}


GLboolean GLAPIENTRY
sav_IsTexture (GLuint texture)
{
    NYI();
    return GL_FALSE;
}


void GLAPIENTRY
sav_LightModelf (GLenum pname, GLfloat param)
{
    NYI();
}


void GLAPIENTRY
sav_LightModelfv (GLenum pname, const GLfloat *params)
{
    NYI();
}


void GLAPIENTRY
sav_LightModeli (GLenum pname, GLint param)
{
    NYI();
}


void GLAPIENTRY
sav_LightModeliv (GLenum pname, const GLint *params)
{
    NYI();
}


void GLAPIENTRY
sav_Lightf (GLenum light, GLenum pname, GLfloat param)
{
    NYI();
}


void GLAPIENTRY
sav_Lightfv (GLenum light, GLenum pname, const GLfloat *params)
{
    NYI();
}


void GLAPIENTRY
sav_Lightiv (GLenum light, GLenum pname, const GLint *params)
{
    NYI();
}


void GLAPIENTRY
sav_Lighti (GLenum light, GLenum pname, GLint param)
{
    NYI();
}


void GLAPIENTRY
sav_ListBase (GLuint base)
{
    NYI();
}


void GLAPIENTRY
sav_LoadIdentity (void)
{
    DL_NODE *node = dlist_new_operation(OP_LOADIDENTITY);
}


void GLAPIENTRY
sav_LoadMatrixd (const GLdouble *m)
{
    NYI();
}


void GLAPIENTRY
sav_LoadMatrixf (const GLfloat *m)
{
    NYI();
}


void APIENTRY
sav_LockArraysEXT (GLint first, GLsizei count)
{
    NYI();
}


void GLAPIENTRY
sav_MatrixMode (GLenum mode)
{
    DL_NODE *node = dlist_new_operation(OP_MATRIXMODE);
    (node + 1)->i = mode;
}


void GLAPIENTRY
sav_MultMatrixd (const GLdouble *m)
{
    NYI();
}


void GLAPIENTRY
sav_MultMatrixf (const GLfloat *m)
{
    NYI();
}


void GLAPIENTRY
sav_NewList (GLuint list, GLenum mode)
{
    NYI();
}


void GLAPIENTRY
sav_Ortho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val)
{
    NYI();
}


void GLAPIENTRY
sav_PixelStoref (GLenum pname, GLfloat param)
{
    NYI();
}


void GLAPIENTRY
sav_PixelStorei (GLenum pname, GLint param)
{
    NYI();
}


void GLAPIENTRY
sav_PolygonMode (GLenum face, GLenum mode)
{
    NYI();
}


void GLAPIENTRY
sav_PolygonOffset (GLfloat factor, GLfloat units)
{
    NYI();
}


void GLAPIENTRY
sav_PolygonOffsetEXT (GLfloat factor, GLfloat bias)
{
    NYI();
}


void GLAPIENTRY
sav_PopAttrib (void)
{
    NYI();
}


void GLAPIENTRY
sav_PopClientAttrib (void)
{
    NYI();
}


void GLAPIENTRY
sav_PopMatrix (void)
{
    DL_NODE *node = dlist_new_operation(OP_POPMATRIX);
}


void GLAPIENTRY
sav_PushAttrib (GLbitfield mask)
{
    NYI();
}


void GLAPIENTRY
sav_PushClientAttrib (GLbitfield mask)
{
    NYI();
}


void GLAPIENTRY
sav_PushMatrix (void)
{
    DL_NODE *node = dlist_new_operation(OP_PUSHMATRIX);
}


void GLAPIENTRY
sav_ReadBuffer (GLenum mode)
{
    NYI();
}


void GLAPIENTRY
sav_ReadPixels (GLint x, GLint y,
                GLsizei width, GLsizei height,
                GLenum format, GLenum type,
                GLvoid *pixels)
{
    NYI();
}


void GLAPIENTRY
sav_Rotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
    sav_Rotatef(angle, x, y, z);
}


void GLAPIENTRY
sav_Rotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    DL_NODE *node = dlist_new_operation(OP_ROTATEF);
    (node + 1)->f = angle;
    (node + 2)->f = x;
    (node + 3)->f = y;
    (node + 4)->f = z;
}


void GLAPIENTRY
sav_Scaled (GLdouble x, GLdouble y, GLdouble z)
{
    sav_Scalef(x, y, z);
}


void GLAPIENTRY
sav_Scalef (GLfloat x, GLfloat y, GLfloat z)
{
    DL_NODE *node = dlist_new_operation(OP_SCALEF);
    (node + 1)->f = x;
    (node + 2)->f = y;
    (node + 3)->f = z;
}


void GLAPIENTRY
sav_Scissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
    NYI();
}


void GLAPIENTRY
sav_ShadeModel (GLenum mode)
{
    DL_NODE *node = dlist_new_operation(OP_SHADEMODEL);
    (node + 1)->i = mode;
}


void GLAPIENTRY
sav_StencilFunc (GLenum func, GLint ref, GLuint mask)
{
    NYI();
}


void GLAPIENTRY
sav_StencilMask (GLuint mask)
{
    NYI();
}


void GLAPIENTRY
sav_StencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
    NYI();
}


void GLAPIENTRY
sav_TexEnvf (GLenum target, GLenum pname, GLfloat param)
{
    sav_TexEnvfv(target, pname, &param);
}


void GLAPIENTRY
sav_TexEnvfv (GLenum target, GLenum pname, const GLfloat *params)
{
    DL_NODE *node = dlist_new_operation(OP_TEXENV);
    (node + 1)->i = target;
    (node + 2)->i = pname;
    (node + 3)->f = params[0];
    if (pname == GL_TEXTURE_ENV_COLOR) {
	(node + 4)->f = params[1];
	(node + 5)->f = params[2];
	(node + 6)->f = params[3];
    }
}


void GLAPIENTRY
sav_TexEnvi (GLenum target, GLenum pname, GLint param)
{
    sav_TexEnviv(target, pname, &param);
}


void GLAPIENTRY
sav_TexEnviv (GLenum target, GLenum pname, const GLint *params)
{
    GLfloat tmp[4];
    tmp[0] = params[0];
    if (pname == GL_TEXTURE_ENV_COLOR) {
	tmp[1] = params[1];
	tmp[2] = params[2];
	tmp[3] = params[3];
    }
    sav_TexEnvfv(target, pname, tmp);
}


void GLAPIENTRY
sav_TexGend (GLenum coord, GLenum pname, GLdouble param)
{
    sav_TexGendv(coord, pname, &param);
}


void GLAPIENTRY
sav_TexGendv (GLenum coord, GLenum pname, const GLdouble *params)
{
    GLfloat tmp[4];
    tmp[0] = params[0];
    if (pname != GL_TEXTURE_GEN_MODE) {
	tmp[1] = params[1];
	tmp[2] = params[2];
	tmp[3] = params[3];
    }
    sav_TexGenfv(coord, pname, tmp);
}


void GLAPIENTRY
sav_TexGenf (GLenum coord, GLenum pname, GLfloat param)
{
    sav_TexGenfv(coord, pname, &param);
}


void GLAPIENTRY
sav_TexGenfv (GLenum coord, GLenum pname, const GLfloat *params)
{
    DL_NODE *node = dlist_new_operation(OP_TEXGEN);
    (node + 1)->i = coord;
    (node + 2)->i = pname;
    (node + 3)->f = params[0];
    if (pname != GL_TEXTURE_GEN_MODE) {
	(node + 4)->f = params[1];
	(node + 5)->f = params[2];
	(node + 6)->f = params[3];
    }
}


void GLAPIENTRY
sav_TexGeni (GLenum coord, GLenum pname, GLint param)
{
    sav_TexGeniv(coord, pname, &param);
}


void GLAPIENTRY
sav_TexGeniv (GLenum coord, GLenum pname, const GLint *params)
{
    GLfloat tmp[4];
    tmp[0] = params[0];
    if (pname != GL_TEXTURE_GEN_MODE) {
	tmp[1] = params[1];
	tmp[2] = params[2];
	tmp[3] = params[3];
    }
    sav_TexGenfv(coord, pname, tmp);
}


void GLAPIENTRY
sav_TexImage1D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    NYI();
}


void GLAPIENTRY
sav_TexImage2D (GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
    NYI();
}


void GLAPIENTRY
sav_TexParameterf (GLenum target, GLenum pname, GLfloat param)
{
    sav_TexParameterfv(target, pname, &param);
}


void GLAPIENTRY
sav_TexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{
    DL_NODE *node = dlist_new_operation(OP_TEXPARAM);
    (node + 1)->i = target;
    (node + 2)->i = pname;
    (node + 3)->f = params[0];
    if (pname == GL_TEXTURE_BORDER_COLOR) {
	(node + 4)->f = params[1];
	(node + 5)->f = params[2];
	(node + 6)->f = params[3];
    }
}


void GLAPIENTRY
sav_TexParameteri (GLenum target, GLenum pname, GLint param)
{
    sav_TexParameteriv(target, pname, &param);
}


void GLAPIENTRY
sav_TexParameteriv (GLenum target, GLenum pname, const GLint *params)
{
    GLfloat tmp[4];
    tmp[0] = params[0];
    if (pname == GL_TEXTURE_BORDER_COLOR) {
	tmp[1] = params[1];
	tmp[2] = params[2];
	tmp[3] = params[3];
    }
    sav_TexParameterfv(target, pname, tmp);
}


void GLAPIENTRY
sav_TexSubImage1D (GLenum target, GLint level,
		   GLint xoffset,
		   GLsizei width, GLenum format,
		   GLenum type, const GLvoid *pixels)
{
    NYI();
}


void GLAPIENTRY
sav_TexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels)
{
    NYI();
}


void GLAPIENTRY
sav_Translated (GLdouble x, GLdouble y, GLdouble z)
{
    sav_Translatef(x, y, z);
}


void GLAPIENTRY
sav_Translatef (GLfloat x, GLfloat y, GLfloat z)
{
    DL_NODE *node = dlist_new_operation(OP_TRANSLATEF);
    (node + 1)->f = x;
    (node + 2)->f = y;
    (node + 3)->f = z;
}


void APIENTRY
sav_UnlockArraysEXT (void)
{
    NYI();
}


void GLAPIENTRY
sav_Viewport (GLint x, GLint y, GLsizei width, GLsizei height)
{
    NYI();
}


/******************************************************************************
 * immediate mode entrypoints
 */


GLboolean GLAPIENTRY
imm_IsList (GLuint list)
{
    return (dlist_get_list(list) != NULL);
}


void GLAPIENTRY
imm_DeleteLists (GLuint list, GLsizei range)
{
    while (range--) {
	DLIST *obj = dlist_destroy(list);
	if (obj != NULL) {
	    list_remove(obj);
	    free(obj);
	}
	list++;
    }
}


GLuint GLAPIENTRY
imm_GenLists (GLsizei range)
{
    int start = ctx_dlist_hinumber + 1;
    ctx_dlist_hinumber += range;
    return start;
}


void GLAPIENTRY
imm_NewList (GLuint list, GLenum mode)
{
    DLIST *obj;

    obj = malloc(sizeof(DLIST));
    gl_assert(obj != NULL);
    obj->name = list;
    obj->block = malloc(DLIST_BLOCK_SIZE * sizeof(DL_NODE));
    gl_assert(obj->block != NULL);
    list_append(&ctx_dlist_list, obj);

    /* start the list */
    ctx_dlist_current_list = obj;
    ctx_dlist_current_block = obj->block;
    ctx_dlist_current_size = 0;
    ctx_dlist_mode = mode;

    /* switch to save mode */
    gl_switch_sav();
}


void GLAPIENTRY
imm_EndList (void)
{
    gl_assert(0);
}


void GLAPIENTRY
imm_CallList (GLuint list)
{
    dlist_playback(list);
    /*dlist_print(list);*/
}


void GLAPIENTRY
imm_CallLists (GLsizei n, GLenum type, const GLvoid *lists)
{
    GLuint list;
    GLuint stride;
    const GLubyte *ptr = lists;
    switch (type) {
	case GL_BYTE:		stride = sizeof(GLbyte);	break;
	case GL_UNSIGNED_BYTE:	stride = sizeof(GLubyte);	break;
	case GL_SHORT:		stride = sizeof(GLshort);	break;
	case GL_UNSIGNED_SHORT:	stride = sizeof(GLushort);	break;
	case GL_INT:		stride = sizeof(GLint);		break;
	case GL_UNSIGNED_INT:	stride = sizeof(GLuint);	break;
	case GL_FLOAT:		stride = sizeof(GLfloat);	break;
	case GL_2_BYTES:	stride = sizeof(GLbyte) * 2;	break;
	case GL_3_BYTES:	stride = sizeof(GLbyte) * 3;	break;
	case GL_4_BYTES:	stride = sizeof(GLbyte) * 4;	break;
	default:
	    gl_assert(0);
	    return;
    }
    while (n--) {
	list = 0;
	switch (type) {
	    case GL_BYTE:		list = *(GLbyte *)ptr;		break;
	    case GL_UNSIGNED_BYTE:	list = *(GLubyte *)ptr;		break;
	    case GL_SHORT:		list = *(GLshort *)ptr;		break;
	    case GL_UNSIGNED_SHORT:	list = *(GLushort *)ptr;	break;
	    case GL_INT:		list = *(GLint *)ptr;		break;
	    case GL_UNSIGNED_INT:	list = *(GLuint *)ptr;		break;
	    case GL_FLOAT:		list = *(GLfloat *)ptr;		break;
	    case GL_2_BYTES:
		list = (*(GLubyte *)(ptr + 0) <<  8) +
			*(GLubyte *)(ptr + 1);
		break;
	    case GL_3_BYTES:
		list = (*(GLubyte *)(ptr + 0) << 16) +
		       (*(GLubyte *)(ptr + 1) <<  8) +
			*(GLubyte *)(ptr + 2);
		break;
	    case GL_4_BYTES:
		list = (*(GLubyte *)(ptr + 0) << 24) +
		       (*(GLubyte *)(ptr + 1) << 16) +
		       (*(GLubyte *)(ptr + 2) <<  8) +
			*(GLubyte *)(ptr + 3);
		break;
	}
	ptr += stride;
	dlist_playback(list + ctx_dlist_base);
    }
}


void GLAPIENTRY
imm_ListBase (GLuint base)
{
    ctx_dlist_base = base;
}


/******************************************************************************
 * XXX used for debug (remove)
 */
#if 0
#include <stdio.h>
#undef NDEBUG
#include <assert.h>
static void
dlist_print (GLuint real_id)
{
    DL_NODE *node = dlist_get_block(real_id);
    if (node == NULL) {
	printf("DLIST EMPTY ---\n");
	return;
    }
    printf("DLIST BEGIN --- %u\n", dlist_get_list(real_id)->name);
    while (node->op != OP_EOL) {
	if (node->op == OP_CONTINUE) {
	    node = (node + 1)->next;
	    continue;
	}
	switch (node->op) {
	    case OP_BEGIN:
		printf("--- glBegin(%d)\n", (node + 1)->i);
		break;
	    case OP_END:
		printf("--- glEnd()\n");
		break;
	    case OP_VERTEX2F:
		printf("--- glVertex2f(%.2f, %.2f)\n", (node + 1)->f, (node + 2)->f);
		break;
	    case OP_VERTEX3F:
		printf("--- glVertex3f(%.2f, %.2f, %.2f)\n", (node + 1)->f, (node + 2)->f, (node + 3)->f);
		break;
	    case OP_VERTEX4F:
		printf("--- glVertex4f(%.2f, %.2f, %.2f, %.2f)\n", (node + 1)->f, (node + 2)->f, (node + 3)->f, (node + 4)->f);
		break;
	    case OP_COLOR3F:
		printf("--- glColor3f(%.2f, %.2f, %.2f)\n", (node + 1)->f, (node + 2)->f, (node + 3)->f);
		break;
	    case OP_COLOR4F:
		printf("--- glColor4f(%.2f, %.2f, %.2f, %.2f)\n", (node + 1)->f, (node + 2)->f, (node + 3)->f, (node + 4)->f);
		break;
	    case OP_SECONDARYCOLOR3F:
		printf("--- glSecondaryColor3f(%.2f, %.2f, %.2f)\n", (node + 1)->f, (node + 2)->f, (node + 3)->f);
		break;
	    case OP_NORMAL3F:
		printf("--- glNormal3f(%.2f, %.2f, %.2f)\n", (node + 1)->f, (node + 2)->f, (node + 3)->f);
		break;
	    case OP_TEXCOORD1F:
		printf("--- glTexCoord1f(%.2f)\n", (node + 1)->f);
		break;
	    case OP_TEXCOORD2F:
		printf("--- glTexCoord2f(%.2f, %.2f)\n", (node + 1)->f, (node + 2)->f);
		break;
	    case OP_TEXCOORD3F:
		printf("--- glTexCoord3f(%.2f, %.2f, %.2f)\n", (node + 1)->f, (node + 2)->f, (node + 3)->f);
		break;
	    case OP_TEXCOORD4F:
		printf("--- glTexCoord4f(%.2f, %.2f, %.2f, %.2f)\n", (node + 1)->f, (node + 2)->f, (node + 3)->f, (node + 4)->f);
		break;
	    case OP_MULTITEXCOORD1F:
		printf("--- glMultiTexCoord1f(%d, %.2f)\n", (node + 1)->i, (node + 2)->f);
		break;
	    case OP_MULTITEXCOORD2F:
		printf("--- glMultiTexCoord2f(%d, %.2f, %.2f)\n", (node + 1)->i, (node + 2)->f, (node + 3)->f);
		break;
	    case OP_MULTITEXCOORD3F:
		printf("--- glMultiTexCoord3f(%d, %.2f, %.2f, %.2f)\n", (node + 1)->i, (node + 2)->f, (node + 3)->f, (node + 4)->f);
		break;
	    case OP_MULTITEXCOORD4F:
		printf("--- glMultiTexCoord4f(%d, %.2f, %.2f, %.2f, %.2f)\n", (node + 1)->i, (node + 2)->f, (node + 3)->f, (node + 4)->f, (node + 5)->f);
		break;
	    case OP_MATERIALF:
		printf("--- glMaterialf(%d, %d, %.2f)\n", (node + 1)->i, (node + 2)->i, (node + 3)->f);
		break;
	    case OP_MATERIALFV:
		printf("--- glMaterialfv(%d, %d, [%.2f, %.2f, %.2f, %.2f])\n", (node + 1)->i, (node + 2)->i, (node + 3)->f, (node + 4)->f, (node + 5)->f, (node + 6)->f);
		break;
	    case OP_FOGCOORDF:
		printf("--- glFogCoordf(%.2f)\n", (node + 1)->f);
		break;
	    case OP_EDGEFLAG:
		printf("--- glEdgeFlag(%d)\n", (node + 1)->i);
		break;
	    case OP_RECTF:
		printf("--- glRectf(%.2f, %.2f, %.2f, %.2f)\n", (node + 1)->f, (node + 2)->f, (node + 3)->f, (node + 4)->f);
		break;
	    case OP_PUSHMATRIX:
		printf("--- glPushMatrix()\n");
		break;
	    case OP_POPMATRIX:
		printf("--- glPopMatrix()\n");
		break;
	    case OP_TRANSLATEF:
		printf("--- glTranslatef(%.2f, %.2f, %.2f)\n", (node + 1)->f, (node + 2)->f, (node + 3)->f);
		break;
	    case OP_ROTATEF:
		printf("--- glRotatef(%.2f, %.2f, %.2f, %.2f)\n", (node + 1)->f, (node + 2)->f, (node + 3)->f, (node + 4)->f);
		break;
	    case OP_SCALEF:
		printf("--- glScalef(%.2f, %.2f, %.2f)\n", (node + 1)->f, (node + 2)->f, (node + 3)->f);
		break;
	    case OP_LOADIDENTITY:
		printf("--- glLoadIdentity()\n");
		break;
	    case OP_MATRIXMODE:
		printf("--- glMatrixMode(%x)\n", (node + 1)->i);
		break;
	    case OP_ENABLE:
		printf("--- glEnable(%x)\n", (node + 1)->i);
		break;
	    case OP_DISABLE:
		printf("--- glDisable(%x)\n", (node + 1)->i);
		break;
	    case OP_FRONTFACE:
		printf("--- glFrontFace(%x)\n", (node + 1)->i);
		break;
	    case OP_SHADEMODEL:
		printf("--- glShadeModel(%x)\n", (node + 1)->i);
		break;
	    case OP_CULLFACE:
		printf("--- glCullFace(%x)\n", (node + 1)->i);
		break;
	    case OP_BINDTEX:
		printf("--- glBindTexture(%x, %d)\n", (node + 1)->i, (node + 2)->i);
		break;
	    case OP_ACTIVETEX:
		printf("--- glActiveTexture(%x)\n", (node + 1)->i);
		break;
	    case OP_BLENDFUNC:
		printf("--- glBlendFuncSeparate(%x, %x, %x, %x)\n", (node + 1)->i, (node + 2)->i, (node + 3)->i, (node + 4)->i);
		break;
	    case OP_DEPTHMASK:
		printf("--- glDepthMask(%d)\n", (node + 1)->i);
		break;
	    case OP_DEPTHFUNC:
		printf("--- glDepthFunc(%x)\n", (node + 1)->i);
		break;
	    case OP_ALPHAFUNC:
		printf("--- glAlphaFunc(%x, %f)\n", (node + 1)->i, (node + 2)->f);
		break;
	    case OP_TEXENV:
		printf("--- glTexEnvfv(%x, %x, %p)\n", (node + 1)->i, (node + 2)->i, (void *)(node + 3));
		break;
	    case OP_TEXGEN:
		printf("--- glTexGenfv(%x, %x, %p)\n", (node + 1)->i, (node + 2)->i, (void *)(node + 3));
		break;
	    case OP_TEXPARAM:
		printf("--- glTexParameterfv(%x, %x, %p)\n", (node + 1)->i, (node + 2)->i, (void *)(node + 3));
		break;
	    case OP_CALLLIST:
		printf("--- glCallList(%d)\n", (node + 1)->i);
		break;
	    default:
		printf("--- UNKNOWN DLIST OP(%d)\n", node->op);
		gl_assert(0);
	}
	node += opsize[node->op];
    }
    printf("DLIST END ---\n");
}
#endif
