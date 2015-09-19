#include "GL/gl.h"

#include "glinternal.h"
#include "glapi.h"
#include "context.h"


void GLAPIENTRY
imm_StencilFunc (GLenum func, GLint ref, GLuint mask)
{
    if (ref < 0) {
	ref = 0;
    } else if ((GLuint)ref > ctx_stencilmaxi) {
	ref = ctx_stencilmaxi;
    }

    FLUSH_VERTICES();

    ctx_stencil.func = func;
    ctx_stencil.ref = ref;
    ctx_stencil.valMask = mask & ctx_stencilmaxi;

    ctx_gl_state |= NEW_STENCIL;
}


void GLAPIENTRY
imm_StencilMask (GLuint mask)
{
    FLUSH_VERTICES();

    ctx_stencil.writeMask = mask & ctx_stencilmaxi;

    ctx_gl_state |= NEW_STENCIL;
}


void GLAPIENTRY
imm_StencilOp (GLenum fail, GLenum zfail, GLenum zpass)
{
    FLUSH_VERTICES();

    ctx_stencil.fail = fail;
    ctx_stencil.zfail = zfail;
    ctx_stencil.zpass = zpass;

    ctx_gl_state |= NEW_STENCIL;
}


void GLAPIENTRY
imm_ClearStencil (GLint s)
{
    ctx_stencil.clear = s & ctx_stencilmaxi;
}
