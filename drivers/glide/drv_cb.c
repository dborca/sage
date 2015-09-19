/**
 * \file drv_cb.c
 * OpenGL callbacks.
 */


#include <stdlib.h>
#include <string.h>

#include <glide.h>
#include <g3ext.h>
#include "GL/gl.h"

#include "glinternal.h"
#include "main/context.h"
#include "util/macros.h"
#include "main/texstore.h"
#include "tnl/tnl.h"
#include "driver.h"
#include "drv.h"


/**
 * Disable output to color buffer.
 */
static void
disableColor (void)
{
    if (fb_color == 24) {
	gfColorMaskExt(FXFALSE, FXFALSE, FXFALSE, FXFALSE);
    } else {
	grColorMask(FXFALSE, FXFALSE);
    }
}


void
drv_Clear (GLbitfield mask)
{
    GLubyte cred, cgreen, cblue, calpha;
    GLuint ccolor;
    GLuint cdepth;

    mask &= (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    cred = ctx_color.clear_color[0] * 255.0F;
    cgreen = ctx_color.clear_color[1] * 255.0F;
    cblue = ctx_color.clear_color[2] * 255.0F;
    calpha = ctx_color.clear_color[3] * 255.0F;

    ccolor = (cblue << 16) | (cgreen << 8) | cred;
    cdepth = ctx_depthmaxf * ctx_clear_depth;

    ctx_validate_state(NEW_COLOR|NEW_SCISSOR);

    if (ctx_stencilmaxi) {
	if (mask & GL_STENCIL_BUFFER_BIT) {
	    grEnable(GR_STENCIL_MODE_EXT);
	    gfStencilOpExt(GR_STENCILOP_REPLACE,
			   GR_STENCILOP_REPLACE,
			   GR_STENCILOP_REPLACE);
	    gfStencilFuncExt(GR_CMP_ALWAYS,
			     ctx_stencil.clear,
			     0xff);
	    gfStencilMaskExt(ctx_stencil.writeMask);
	} else {
	    grDisable(GR_STENCIL_MODE_EXT);
	}
    }

    /* XXX have to determine front/back buffers */
    switch (mask & ~GL_STENCIL_BUFFER_BIT) {
	case GL_COLOR_BUFFER_BIT:
	    grDepthMask(FXFALSE);
	    if (ctx_stencilmaxi) {
		gfBufferClearExt(ccolor, calpha, cdepth, ctx_stencil.clear);
	    } else {
		grBufferClear(ccolor, calpha, cdepth);
	    }
	    grDepthMask(ctx_depth_mask);
	    break;
	case GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT:
	    grDepthMask(FXTRUE);
	    if (ctx_stencilmaxi) {
		gfBufferClearExt(ccolor, calpha, cdepth, ctx_stencil.clear);
	    } else {
		grBufferClear(ccolor, calpha, cdepth);
	    }
	    grDepthMask(ctx_depth_mask); /* XXX ctx_depth_mask is TRUE if we get here */
	    break;
	case GL_DEPTH_BUFFER_BIT:
	    disableColor();
	    grDepthMask(FXTRUE);
    	    if (ctx_stencilmaxi) {
		gfBufferClearExt(ccolor, calpha, cdepth, ctx_stencil.clear);
	    } else {
		grBufferClear(ccolor, calpha, cdepth);
	    }
	    grDepthMask(ctx_depth_mask); /* XXX ctx_depth_mask is TRUE if we get here */
	    drv_setupColor();
	    break;
	default:
	    if (ctx_stencilmaxi && (mask & GL_STENCIL_BUFFER_BIT)) {
		disableColor();
		grDepthMask(FXFALSE);
		gfBufferClearExt(ccolor, calpha, cdepth, ctx_stencil.clear);
		grDepthMask(ctx_depth_mask);
		drv_setupColor();
	    }
    }

    drv_setupStencil();
}


void
drv_ClearColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
}


void
drv_ClearDepth (GLclampd depth)
{
}


const char *
drv_GetString (int name)
{
    if (name == GL_RENDERER) {
	return renderer_name;
    } else if (name == GL_VERSION) {
	return opengl_ver;
    }
    return NULL;
}


void
drv_DrawBuffer (GLenum mode)
{
    switch (mode) {
	case GL_FRONT:
	case GL_FRONT_LEFT:
	    render_buffer = GR_BUFFER_FRONTBUFFER;
	    break;
	case GL_BACK:
	case GL_BACK_LEFT:
	    render_buffer = GR_BUFFER_BACKBUFFER;
	    break;
	case GL_FRONT_AND_BACK:
	    /* XXX we'll fix clears, but rendering not */
	    return;
    }
    grRenderBuffer(render_buffer);
}


/**
 * Read pixels from 565 format.
 *
 * \param x x-coordinate of the first pixel
 * \param y y-coordinate of the first pixel
 * \param width x-dimension of the pixel rectangle
 * \param height y-dimension of the pixel rectangle
 * \param format format of the pixel data
 * \param type data type of the pixel data
 * \param pixels pixel data
 * \param info hardware buffer info
 */
static void
_readpixels565 (GLint x, GLint y,
		GLsizei width, GLsizei height,
		GLenum format, GLenum type,
		GLvoid *pixels,
		GrLfbInfo_t *info)
{
    const GLint srcStride = info->strideInBytes / 2;
    const GLushort *src = (const GLushort *)info->lfbPtr + y * srcStride + x;
    if (format == GL_RGB && type == GL_UNSIGNED_BYTE) {
	/* convert 5R6G5B into 8R8G8B */
	GLint row, col;
	GLint dstStride = image_stride(&ctx_pack, width, 3);
	GLubyte *dst = (GLubyte *)image_address(&ctx_pack, pixels, width, height, 3, 0, 0, 0);
	for (row = 0; row < height; row++) {
	    GLubyte *d = dst;
	    for (col = 0; col < width; col++) {
		const GLushort pixel = src[col];
		GLuint r = (pixel & 0xf800) >> 8;
		GLuint g = (pixel & 0x07e0) >> 3;
		GLuint b = (pixel & 0x001f) << 3;
		*d++ = r | (r >> 5);
		*d++ = g | (g >> 6);
		*d++ = b | (b >> 5);
	    }
	    dst += dstStride;
	    src -= srcStride;
	}
    } else if (format == GL_RGBA && type == GL_UNSIGNED_BYTE) {
	/* convert 5R6G5B into 8R8G8B8A */
	GLint row, col;
	GLint dstStride = image_stride(&ctx_pack, width, 4);
	GLubyte *dst = (GLubyte *)image_address(&ctx_pack, pixels, width, height, 4, 0, 0, 0);
	for (row = 0; row < height; row++) {
	    GLubyte *d = dst;
	    for (col = 0; col < width; col++) {
		const GLushort pixel = src[col];
		GLuint r = (pixel & 0xf800) >> 8;
		GLuint g = (pixel & 0x07e0) >> 3;
		GLuint b = (pixel & 0x001f) << 3;
		*d++ = r | (r >> 5);
		*d++ = g | (g >> 6);
		*d++ = b | (b >> 5);
		*d++ = 0xff;
	    }
	    dst += dstStride;
	    src -= srcStride;
	}
    } else if (format == GL_RGBA && type == GL_FLOAT) {
	/* convert 5R6G5B into fRfGfBfA */
	GLint row, col;
	GLint dstStride = image_stride(&ctx_pack, width, 16);
	GLubyte *dst = (GLubyte *)image_address(&ctx_pack, pixels, width, height, 16, 0, 0, 0);
	for (row = 0; row < height; row++) {
	    GLfloat *d = (GLfloat *)dst;
	    for (col = 0; col < width; col++) {
		const GLushort pixel = src[col];
		GLuint r = (pixel & 0xf800) >> 8;
		GLuint g = (pixel & 0x07e0) >> 3;
		GLuint b = (pixel & 0x001f) << 3;
		*d++ = UB_TO_FLOAT(r | (r >> 5));
		*d++ = UB_TO_FLOAT(g | (g >> 6));
		*d++ = UB_TO_FLOAT(b | (b >> 5));
		*d++ = 1.0F;
	    }
	    dst += dstStride;
	    src -= srcStride;
	}
    } else if (format == GL_DEPTH_COMPONENT && type == GL_FLOAT) {
	/* convert depth16 into float */
	GLint row, col;
	GLint dstStride = image_stride (&ctx_pack, width, 4);
	GLubyte *dst = (GLubyte *)image_address(&ctx_pack, pixels, width, height, 4, 0, 0, 0);
	for (row = 0; row < height; row++) {
	    GLfloat *d = (GLfloat *)dst;
	    for (col = 0; col < width; col++) {
		const GLushort pixel = src[col];
		*d++ = US_TO_FLOAT(pixel);
	    }
	    dst += dstStride;
	    src -= srcStride;
	}
    } else {
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "format %x, type %x\n", format, type);
    }
}


/**
 * Read pixels from 8888 format.
 *
 * \param x x-coordinate of the first pixel
 * \param y y-coordinate of the first pixel
 * \param width x-dimension of the pixel rectangle
 * \param height y-dimension of the pixel rectangle
 * \param format format of the pixel data
 * \param type data type of the pixel data
 * \param pixels pixel data
 * \param info hardware buffer info
 */
static void
_readpixels8888 (GLint x, GLint y,
		 GLsizei width, GLsizei height,
		 GLenum format, GLenum type,
		 GLvoid *pixels,
		 GrLfbInfo_t *info)
{
    const GLint srcStride = info->strideInBytes / 4;
    const GLuint *src = (const GLuint *)info->lfbPtr + y * srcStride + x;
    if (format == GL_RGB && type == GL_UNSIGNED_BYTE) {
	/* convert 8R8G8B8A into 8R8G8B */
	GLint row, col;
	GLint dstStride = image_stride(&ctx_pack, width, 3);
	GLubyte *dst = (GLubyte *)image_address(&ctx_pack, pixels, width, height, 3, 0, 0, 0);
	for (row = 0; row < height; row++) {
	    GLubyte *d = dst;
	    for (col = 0; col < width; col++) {
		const GLuint pixel = src[col];
		GLubyte r = pixel >> 16;
		GLubyte g = pixel >>  8;
		GLubyte b = pixel >>  0;
		*d++ = r;
		*d++ = g;
		*d++ = b;
	    }
	    dst += dstStride;
	    src -= srcStride;
	}
    } else if (format == GL_RGBA && type == GL_UNSIGNED_BYTE) {
	/* convert 8R8G8B8A into 8R8G8B8A */
	GLint row, col;
	GLint dstStride = image_stride(&ctx_pack, width, 4);
	GLubyte *dst = (GLubyte *)image_address(&ctx_pack, pixels, width, height, 4, 0, 0, 0);
	for (row = 0; row < height; row++) {
	    GLubyte *d = dst;
	    for (col = 0; col < width; col++) {
		const GLuint pixel = src[col];
		GLubyte r = pixel >> 16;
		GLubyte g = pixel >>  8;
		GLubyte b = pixel >>  0;
		GLubyte a = pixel >> 24;
		*d++ = r;
		*d++ = g;
		*d++ = b;
		*d++ = a;
	    }
	    dst += dstStride;
	    src -= srcStride;
	}
    } else if (format == GL_RGBA && type == GL_FLOAT) {
	/* convert 8R8G8B8A into fRfGfBfA */
	GLint row, col;
	GLint dstStride = image_stride(&ctx_pack, width, 16);
	GLubyte *dst = (GLubyte *)image_address(&ctx_pack, pixels, width, height, 16, 0, 0, 0);
	for (row = 0; row < height; row++) {
	    GLfloat *d = (GLfloat *)dst;
	    for (col = 0; col < width; col++) {
		const GLuint pixel = src[col];
		GLubyte r = pixel >> 16;
		GLubyte g = pixel >>  8;
		GLubyte b = pixel >>  0;
		GLubyte a = pixel >> 24;
		*d++ = UB_TO_FLOAT(r);
		*d++ = UB_TO_FLOAT(g);
		*d++ = UB_TO_FLOAT(b);
		*d++ = UB_TO_FLOAT(a);
	    }
	    dst += dstStride;
	    src -= srcStride;
	}
    } else if (format == GL_DEPTH_COMPONENT && type == GL_FLOAT) {
	/* convert depth24 into float */
	GLint row, col;
	GLint dstStride = image_stride (&ctx_pack, width, 4);
	GLubyte *dst = (GLubyte *)image_address(&ctx_pack, pixels, width, height, 4, 0, 0, 0);
	for (row = 0; row < height; row++) {
	    GLfloat *d = (GLfloat *)dst;
	    for (col = 0; col < width; col++) {
		const GLuint pixel = src[col];
		*d++ = (pixel & 0xffffff) / 16777215.0F;
	    }
	    dst += dstStride;
	    src -= srcStride;
	}
    } else {
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "format %x, type %x\n", format, type);
    }
}


void
drv_ReadPixels (GLint x, GLint y,
                GLsizei width, GLsizei height,
                GLenum format, GLenum type,
                GLvoid *pixels)
{
    GrBuffer_t buffer = GR_BUFFER_BACKBUFFER;

    if (format == GL_DEPTH_COMPONENT) {
	buffer = GR_BUFFER_AUXBUFFER; /*GR_BUFFER_DEPTHBUFFER*/
    } else if (ctx_read_buffer == GL_FRONT_LEFT) {
	buffer = GR_BUFFER_FRONTBUFFER;
    }

    if (x + width > screen_width || y + height > screen_height) {
	return; /* XXX why do we get this?!? */
    }

    if (1/* XXX some tests here */) {
	GrLfbInfo_t info;

	info.size = sizeof(info);

	if (grLfbLock(GR_LFB_READ_ONLY,
		      buffer,
		      GR_LFBWRITEMODE_ANY,
		      GR_ORIGIN_UPPER_LEFT, FXFALSE, &info)) {
	    const GLint winX = 0;
	    const GLint winY = screen_height - 1; /* XXX */
	    if (fb_color == 16) {
		_readpixels565(winX + x, winY - y, width, height, format, type, pixels, &info);
	    } else if (fb_color == 24) {
		_readpixels8888(winX + x, winY - y, width, height, format, type, pixels, &info);
	    } else {
		gl_cry(__FILE__, __LINE__, __FUNCTION__, "fb_color = %d\n", fb_color);
	    }
	    grLfbUnlock(GR_LFB_READ_ONLY, buffer);
	}
    }
}
