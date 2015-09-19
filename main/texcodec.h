/**
 * \file texcodec.c Texture compression wrapper.
 */


#ifndef TEXCODEC_H_included
#define TEXCODEC_H_included

#define TC_WRAP   0
#define TC_CLAMP  1
#define TC_MIRROR 2


/**
 * Initialize texture compressor/decompressor.
 *
 * \return 0 if ok, -1 if error
 */
int tc_init (void);


/**
 * Stride in bytes for the given compressed format.
 *
 * \param format desired compressed format
 * \param width texture width in pixels
 *
 * \return stride if ok, 0 if unrecognized format
 */
GLint tc_stride (GLenum format, GLsizei width);


/**
 * Size in bytes for the given compressed format.
 *
 * \param format desired compressed format
 * \param width texture width in pixels
 * \param height texture height in pixels
 *
 * \return size if ok, 0 if unrecognized format
 */
GLuint tc_size (GLenum format, GLsizei width, GLsizei height);


/**
 * Decode compressed texture.
 *
 * \param dst destination block (if it is NULL, it will be allocated)
 * \param dst_format ptr to return decoded texture format
 * \param dst_type ptr to return decoded texture type
 * \param width texture width in pixels
 * \param height texture height in pixels
 * \param src compressed data
 * \param src_format compressed texture format
 *
 * \return destination block
 */
void *
tc_decode (void *dst, GLenum *dst_format, GLenum *dst_type,
	   GLsizei width, GLsizei height,
	   const void *src, GLenum src_format);


/**
 * Address of the pixel at (col, row, img) in a compressed texture.
 *
 * \param col, row, img - image position (3D)
 * \param format compressed image format
 * \param width image width in pixels
 * \param image compressed data
 *
 * \return address of pixel at (row, col)
 */
void *
tc_address (GLint col, GLint row, GLint img,
	    GLenum format,
	    GLsizei width, const void *image);


/**
 * Encode texture.
 *
 * \param unpack OpenGL unpack mode
 * \param dst_width compressed width in pixels (>= src_width)
 * \param dst_height compressed height in pixels (>= src_height)
 * \param dst destination block (if it is NULL, it will be allocated)
 * \param dst_format compressed format
 * \param src_width uncompressed width in pixels
 * \param src_height uncompressed height in pixels
 * \param src uncompressed data
 * \param src_format uncompressed format (must be GL_RGB or GL_RGBA)
 * \param src_type uncompressed type (must be GL_UNSIGNED_BYTE)
 *
 * \return destination block
 */
void *
tc_encode (const PACKING *unpack,
	   int dst_width, int dst_height,
	   void *dst,
	   GLenum dst_format,
	   int src_width, int src_height,
	   const GLvoid *src,
	   GLenum src_format,
	   GLenum src_type);


/**
 * Replace portion of compressed texture.
 *
 * \param xoffset column
 * \param yoffset row
 * \param dst_width compressed width in pixels (>= src_width)
 * \param dst_height compressed height in pixels (>= src_height)
 * \param dst destination block
 * \param dst_format compressed format
 * \param src_width compressed width in pixels
 * \param src_height compressed height in pixels
 * \param src compressed data
 * \param src_format compressed format
 */
void
tc_replace (int xoffset, int yoffset,
	    int dst_width, int dst_height,
	    void *dst,
	    GLenum dst_format,
	    int src_width, int src_height,
	    const GLvoid *src,
	    GLenum src_format);


/**
 * Replicate compressed texture horizontally.
 *
 * \param dst_width destination width in pixels (must be multiple of src_width)
 * \param src_width source width in pixels
 * \param height height in pixels
 * \param data compressed texture
 * \param format compressed texture format
 * \param clamp 0 = wrap, 1 = clamp, 2 = miror
 */
void
tc_hori_pad (GLsizei dst_width, GLsizei src_width, GLsizei height,
	     GLubyte *data, GLenum format, GLenum clamp);


/**
 * Replicate compressed texture vertically.
 *
 * \param width width in pixels
 * \param dst_height destination height in pixels (must be multiple of src_height)
 * \param src_height source height in pixels
 * \param data compressed texture
 * \param format compressed texture format
 * \param clamp 0 = wrap, 1 = clamp, 2 = miror
 */
void
tc_vert_pad (GLsizei width, GLsizei dst_height, GLsizei src_height,
	     GLubyte *data, GLenum format, GLenum clamp);


/**
 * Destroy texture compressor/decompressor.
 */
void tc_fini (void);

#endif
