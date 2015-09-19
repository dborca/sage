#include "GL/gl.h"

#include "glinternal.h"
#include "glapi.h"
#include "context.h"


void GLAPIENTRY
imm_DepthFunc (GLenum func)
{
    FLUSH_VERTICES();

    ctx_depth_func = func;

    ctx_gl_state |= NEW_DEPTH;
}


void GLAPIENTRY
imm_DepthMask (GLboolean flag)
{
    FLUSH_VERTICES();

    ctx_depth_mask = flag;

    ctx_gl_state |= NEW_DEPTH;
}


void GLAPIENTRY
imm_ClearDepth (GLclampd depth)
{
    ctx_clear_depth = depth;
}
