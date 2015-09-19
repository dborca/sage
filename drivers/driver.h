/**
 * \file driver.h
 * Glue between SAGE and drivers.
 */


#ifndef DRIVER_H_included
#define DRIVER_H_included

/**
 * Clear buffers within the viewport.
 *
 * \param mask active buffers to be cleared
 *
 * \sa glClear
 */
void drv_Clear (GLbitfield mask);

/**
 * Specify clear values for the color buffers.
 *
 * \param red red component
 * \param green green component
 * \param blue blue component
 * \param alpha alpha component
 *
 * \sa glClearColor
 */
void drv_ClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

/**
 * Specify the clear value for the depth buffer.
 *
 * \param depth depth value
 *
 * \sa glClearDepth
 */
void drv_ClearDepth (GLclampd depth);

/**
 * Returns a string describing the current GL connection.
 *
 * \param name variable
 *
 * \return value
 *
 * \sa glGetString
 */
const char *drv_GetString (int name);

/**
 * Specify which color buffers are to be drawn into.
 *
 * \param mode color buffer to write to
 *
 * \sa glDrawBuffer
 */
void drv_DrawBuffer (GLenum mode);

/**
 * Read a block of pixels from the frame buffer.
 *
 * \param x x-coordinate of the first pixel
 * \param y y-coordinate of the first pixel
 * \param width x-dimension of the pixel rectangle
 * \param height y-dimension of the pixel rectangle
 * \param format format of the pixel data
 * \param type data type of the pixel data
 * \param pixels pixel data
 *
 * \sa glReadPixels
 */
void drv_ReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);

void drv_TexImage2D (TEX_OBJ *texObj, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels, const PACKING *unpack);
void drv_TexSubImage2D (TEX_OBJ *texObj, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels, const PACKING *unpack);
void drv_DeleteTexture (TEX_OBJ *texObj);
void drv_CompressedTexImage2D (TEX_OBJ *texObj, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei imageSize, const GLvoid *data);
void drv_GetTexImage (TEX_OBJ *texObj, GLint level, GLenum format, GLenum type, GLvoid *pixels);
void drv_GetCompressedTexImage (TEX_OBJ *texObj, GLint lod, GLvoid *img);
void drv_CompressedTexSubImage2D (TEX_OBJ *texObj, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);

/**
 * Called right before rendering phase.
 */
void drv_render_init (void);

/**
 * Called right after rendering phase.
 */
void drv_render_fini (void);

/**
 * Translate vertices to hardware format.
 *
 * \param n number of vertices
 */
extern void (*drv_emitvertices) (int n);

/**
 * Copy vertex colors (primary, secondary, front, back) for flatshading.
 *
 * \param vdst destination vertex position
 * \param vsrc source vertex position
 */
extern void (*drv_copypv) (int vdst, int vsrc);

/**
 * Interpolate vertex attributes for clipping.
 *
 * \param t interpolation factor
 * \param vdst destination vertex callback
 * \param vout endpoint
 * \param vin endpoint
 */
extern void (*drv_interp) (float t, int vdst, int vout, int vin);

/* setup functions */
void drv_setupTexture (void);
void drv_setupBlend (void);
void drv_setupDepth (void);
void drv_setupAlpha (void);
void drv_setupColor (void);
void drv_setupCull (void);
void drv_setupFog (void);
void drv_setupScissor (void);
void drv_setupStencil (void);

/* rendering functions */
extern const TNL_DRAW_FUNC *drv_prim_tab;
extern void (*drv_point) (int v0);
extern void (*drv_line) (int v0, int v1);
extern void (*drv_triangle) (int v0, int v1, int v2);
extern void (*drv_quad) (int v0, int v1, int v2, int v3);
extern void (*drv_clippedPolygon) (int n, int *elts);

/**
 * Multipass callback.
 *
 * \param pass pass number, starting from 1
 *
 * \return 0 if no more passes requested
 */
extern int (*drv_multipass) (int pass);

#endif
