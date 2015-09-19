#include "GL/gl.h"

#include "glinternal.h"
#include "glapi.h"
#include "context.h"
#include "tnl/tnl.h"
#include "driver.h"


void GLAPIENTRY
imm_Clear (GLbitfield mask)
{
    if (!ctx_depth_mask) {
	mask &= ~GL_DEPTH_BUFFER_BIT;
    }
    if (!ctx_stencil.writeMask) {
	mask &= ~GL_STENCIL_BUFFER_BIT;
    }
    if (!mask) {
	return;
    }

    FLUSH_VERTICES();

    drv_Clear(mask);
}


void GLAPIENTRY
imm_Scissor (GLint x, GLint y, GLsizei width, GLsizei height)
{
    FLUSH_VERTICES();

    ctx_scissor_x1 = x;
    ctx_scissor_y1 = y;
    ctx_scissor_x2 = x + width;
    ctx_scissor_y2 = y + height;

    ctx_gl_state |= NEW_SCISSOR;
}


void GLAPIENTRY
imm_ReadPixels (GLint x, GLint y,
                GLsizei width, GLsizei height,
                GLenum format, GLenum type,
                GLvoid *pixels)
{
    FLUSH_VERTICES();

    drv_ReadPixels(x, y, width, height, format, type, pixels);
}


void GLAPIENTRY
imm_ReadBuffer (GLenum mode)
{
    if (mode == GL_FRONT || mode == GL_LEFT) {
	mode = GL_FRONT_LEFT;
    }
    if (mode == GL_BACK) {
	mode = GL_BACK_LEFT;
    }
    if (mode == GL_RIGHT) {
	mode = GL_FRONT_RIGHT;
    }

    ctx_read_buffer = mode;
    /* XXX should probably have a callback here */
    /* XXX should we keep the value here, or just pass it down to the driver? */
}
