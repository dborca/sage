#include <stdlib.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "context.h"
#include "util/macros.h"
#include "texstore.h"


#define PICK(func) (unpack->swap_bytes) ? func##_swap : func


typedef void (*EXTRACT) (const GLvoid *texture, GLuint n, GLfloat4 *texel);
typedef void (*EXPORT) (void *dst, int dstStrideInBytes, const GLubyte *src, int srcStrideInBytes, int src_width, int src_height, EXTRACT xtract);
typedef void (*INSERT) (void *dst, GLfloat4 texel);


GLint
image_stride (const PACKING *packing, GLint width, GLint bytesPerPixel)
{
    GLint bytesPerRow, remainder;
    if (packing->row_length == 0) {
	bytesPerRow = bytesPerPixel * width;
    } else {
	bytesPerRow = bytesPerPixel * packing->row_length;
    }
    remainder = bytesPerRow % packing->alignment;
    if (remainder > 0) {
	bytesPerRow += (packing->alignment - remainder);
    }
    return bytesPerRow;
}


const GLvoid *
image_address (const PACKING *packing,
	       const GLubyte *src, GLsizei width, GLsizei height,
	       GLint bytesPerPixel,
	       GLint img, GLint row, GLint column)
{
    GLint bytesPerRow, remainder, bytesPerImage;

    if (packing->row_length == 0) {
	bytesPerRow = bytesPerPixel * width;
    } else {
	bytesPerRow = bytesPerPixel * packing->row_length;
    }
    remainder = bytesPerRow % packing->alignment;
    if (remainder > 0) {
	bytesPerRow += (packing->alignment - remainder);
    }

    if (packing->image_height > 0) {
	bytesPerImage = bytesPerRow * packing->image_height;
    } else {
	bytesPerImage = bytesPerRow * height;
    }

    return src
	 + (packing->skip_images + img) * bytesPerImage
	 + (packing->skip_rows + row) * bytesPerRow
	 + (packing->skip_pixels + column) * bytesPerPixel;
}


/* import from specified format to RGBA(float) *******************************/


static void
extract_rgba_ubyte (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLubyte *src = texture;
    do {
	texel[0][0] = UB_TO_FLOAT(src[0]);
	texel[0][1] = UB_TO_FLOAT(src[1]);
	texel[0][2] = UB_TO_FLOAT(src[2]);
	texel[0][3] = UB_TO_FLOAT(src[3]);
	src += 4;
	texel += 1;
    } while (--n);
}


static void
extract_rgba_ushort (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLushort *src = texture;
    do {
	texel[0][0] = US_TO_FLOAT(src[0]);
	texel[0][1] = US_TO_FLOAT(src[1]);
	texel[0][2] = US_TO_FLOAT(src[2]);
	texel[0][3] = US_TO_FLOAT(src[3]);
	src += 4;
	texel += 1;
    } while (--n);
}


static void
extract_rgba_ushort_swap (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLushort *src = texture;
    do {
	texel[0][0] = US_TO_FLOAT(SWAP_SHORT(src[0]));
	texel[0][1] = US_TO_FLOAT(SWAP_SHORT(src[1]));
	texel[0][2] = US_TO_FLOAT(SWAP_SHORT(src[2]));
	texel[0][3] = US_TO_FLOAT(SWAP_SHORT(src[3]));
	src += 4;
	texel += 1;
    } while (--n);
}


static void
extract_rgba_float (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLfloat *src = texture;
    do {
	texel[0][0] = src[0];
	texel[0][1] = src[1];
	texel[0][2] = src[2];
	texel[0][3] = src[3];
	src += 4;
	texel += 1;
    } while (--n);
}


static void
extract_bgra_ubyte (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLubyte *src = texture;
    do {
	texel[0][0] = UB_TO_FLOAT(src[2]);
	texel[0][1] = UB_TO_FLOAT(src[1]);
	texel[0][2] = UB_TO_FLOAT(src[0]);
	texel[0][3] = UB_TO_FLOAT(src[3]);
	src += 4;
	texel += 1;
    } while (--n);
}


static void
extract_bgra_ushort (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLushort *src = texture;
    do {
	texel[0][0] = US_TO_FLOAT(src[2]);
	texel[0][1] = US_TO_FLOAT(src[1]);
	texel[0][2] = US_TO_FLOAT(src[0]);
	texel[0][3] = US_TO_FLOAT(src[3]);
	src += 4;
	texel += 1;
    } while (--n);
}


static void
extract_bgra_ushort_swap (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLushort *src = texture;
    do {
	texel[0][0] = US_TO_FLOAT(SWAP_SHORT(src[2]));
	texel[0][1] = US_TO_FLOAT(SWAP_SHORT(src[1]));
	texel[0][2] = US_TO_FLOAT(SWAP_SHORT(src[0]));
	texel[0][3] = US_TO_FLOAT(SWAP_SHORT(src[3]));
	src += 4;
	texel += 1;
    } while (--n);
}


static void
extract_bgra_4444_rev (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLushort *src = texture;
    do {
	texel[0][0] = (GLfloat)((src[0] & 0x0f00) >>  8) / 15.0F;
	texel[0][1] = (GLfloat)((src[0] & 0x00f0) >>  4) / 15.0F;
	texel[0][2] = (GLfloat)( src[0] & 0x000f       ) / 15.0F;
	texel[0][3] = (GLfloat)((src[0] & 0xf000) >> 12) / 15.0F;
	src += 1;
	texel += 1;
    } while (--n);
}


static void
extract_bgra_8888_rev (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLuint *src = texture;
    do {
	texel[0][0] = UB_TO_FLOAT((src[0] & 0x00ff0000) >> 16);
	texel[0][1] = UB_TO_FLOAT((src[0] & 0x0000ff00) >>  8);
	texel[0][2] = UB_TO_FLOAT( src[0] & 0x000000ff       );
	texel[0][3] = UB_TO_FLOAT((src[0] & 0xff000000) >> 24);
	src += 1;
	texel += 1;
    } while (--n);
}


static void
extract_bgr1_8888_rev (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLuint *src = texture;
    do {
	texel[0][0] = UB_TO_FLOAT((src[0] & 0x00ff0000) >> 16);
	texel[0][1] = UB_TO_FLOAT((src[0] & 0x0000ff00) >>  8);
	texel[0][2] = UB_TO_FLOAT( src[0] & 0x000000ff       );
	texel[0][3] = 1.0F;
	src += 1;
	texel += 1;
    } while (--n);
}


static void
extract_rgb_ubyte (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLubyte *src = texture;
    do {
	texel[0][0] = UB_TO_FLOAT(src[0]);
	texel[0][1] = UB_TO_FLOAT(src[1]);
	texel[0][2] = UB_TO_FLOAT(src[2]);
	texel[0][3] = 1.0F;
	src += 3;
	texel += 1;
    } while (--n);
}


static void
extract_rgb_ushort (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLushort *src = texture;
    do {
	texel[0][0] = US_TO_FLOAT(src[0]);
	texel[0][1] = US_TO_FLOAT(src[1]);
	texel[0][2] = US_TO_FLOAT(src[2]);
	texel[0][3] = 1.0F;
	src += 3;
	texel += 1;
    } while (--n);
}


static void
extract_rgb_ushort_swap (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLushort *src = texture;
    do {
	texel[0][0] = US_TO_FLOAT(SWAP_SHORT(src[0]));
	texel[0][1] = US_TO_FLOAT(SWAP_SHORT(src[1]));
	texel[0][2] = US_TO_FLOAT(SWAP_SHORT(src[2]));
	texel[0][3] = 1.0F;
	src += 3;
	texel += 1;
    } while (--n);
}


static void
extract_rgb_565 (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLushort *src = texture;
    do {
	texel[0][0] = (GLfloat)((src[0] & 0xf800) >> 11) / 31.0F;
	texel[0][1] = (GLfloat)((src[0] & 0x07e0) >>  5) / 63.0F;
	texel[0][2] = (GLfloat)( src[0] & 0x001f       ) / 31.0F;
	texel[0][3] = 1.0F;
	src += 1;
	texel += 1;
    } while (--n);
}


static void
extract_bgr_ubyte (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLubyte *src = texture;
    do {
	texel[0][0] = UB_TO_FLOAT(src[2]);
	texel[0][1] = UB_TO_FLOAT(src[1]);
	texel[0][2] = UB_TO_FLOAT(src[0]);
	texel[0][3] = 1.0F;
	src += 3;
	texel += 1;
    } while (--n);
}


static void
extract_l_ubyte (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLubyte *src = texture;
    do {
	texel[0][0] = UB_TO_FLOAT(src[0]);
	texel[0][1] = UB_TO_FLOAT(src[0]);
	texel[0][2] = UB_TO_FLOAT(src[0]);
	texel[0][3] = 0.0F;
	src += 1;
	texel += 1;
    } while (--n);
}


static void
extract_l_ushort (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLushort *src = texture;
    do {
	texel[0][0] = US_TO_FLOAT(src[0]);
	texel[0][1] = US_TO_FLOAT(src[0]);
	texel[0][2] = US_TO_FLOAT(src[0]);
	texel[0][3] = 0.0F;
	src += 1;
	texel += 1;
    } while (--n);
}


static void
extract_l_ushort_swap (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLushort *src = texture;
    do {
	texel[0][0] = US_TO_FLOAT(SWAP_SHORT(src[0]));
	texel[0][1] = US_TO_FLOAT(SWAP_SHORT(src[0]));
	texel[0][2] = US_TO_FLOAT(SWAP_SHORT(src[0]));
	texel[0][3] = 0.0F;
	src += 1;
	texel += 1;
    } while (--n);
}


static void
extract_a_ubyte (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLubyte *src = texture;
    do {
	texel[0][0] = 0.0F;
	texel[0][1] = 0.0F;
	texel[0][2] = 0.0F;
	texel[0][3] = UB_TO_FLOAT(src[0]);
	src += 1;
	texel += 1;
    } while (--n);
}


static void
extract_la_ubyte (const GLvoid *texture, GLuint n, GLfloat4 *texel)
{
    const GLubyte *src = texture;
    do {
	texel[0][0] = UB_TO_FLOAT(src[0]);
	texel[0][1] = UB_TO_FLOAT(src[0]);
	texel[0][2] = UB_TO_FLOAT(src[0]);
	texel[0][3] = UB_TO_FLOAT(src[1]);
	src += 2;
	texel += 1;
    } while (--n);
}


/* export from RGBA(float) to specified format *******************************/


static void
export_bgra_4444_rev (void *dst, int dstStrideInBytes,
		      const GLubyte *src, int srcStrideInBytes,
		      int src_width, int src_height,
		      EXTRACT xtract)
{
    int i, j;
    GLushort *output = dst;
    GLfloat4 texel[MAX_TEXTURE_SIZE];

    for (j = 0; j < src_height; j++) {
	xtract(src, src_width, texel);
	for (i = 0; i < src_width; i++) {
	    GLubyte r = texel[i][0] * 15.0F + 0.5F;
	    GLubyte g = texel[i][1] * 15.0F + 0.5F;
	    GLubyte b = texel[i][2] * 15.0F + 0.5F;
	    GLubyte a = texel[i][3] * 15.0F + 0.5F;
	    output[i] = (a << 12) | (r << 8) | (g << 4) | b;
	}

	PTR_ADD_STRIDE(GLushort, output, dstStrideInBytes);
	src += srcStrideInBytes;
    }
}


static void
export_bgra_8888_rev (void *dst, int dstStrideInBytes,
		      const GLubyte *src, int srcStrideInBytes,
		      int src_width, int src_height,
		      EXTRACT xtract)
{
    int i, j;
    GLuint *output = dst;
    GLfloat4 texel[MAX_TEXTURE_SIZE];

    for (j = 0; j < src_height; j++) {
	xtract(src, src_width, texel);
	for (i = 0; i < src_width; i++) {
	    GLubyte r = texel[i][0] * 255.0F + 0.5F;
	    GLubyte g = texel[i][1] * 255.0F + 0.5F;
	    GLubyte b = texel[i][2] * 255.0F + 0.5F;
	    GLubyte a = texel[i][3] * 255.0F + 0.5F;
	    output[i] = (a << 24) | (r << 16) | (g << 8) | b;
	}

	PTR_ADD_STRIDE(GLuint, output, dstStrideInBytes);
	src += srcStrideInBytes;
    }
}


static void
export_bgr1_8888_rev (void *dst, int dstStrideInBytes,
		      const GLubyte *src, int srcStrideInBytes,
		      int src_width, int src_height,
		      EXTRACT xtract)
{
    int i, j;
    GLuint *output = dst;
    GLfloat4 texel[MAX_TEXTURE_SIZE];

    for (j = 0; j < src_height; j++) {
	xtract(src, src_width, texel);
	for (i = 0; i < src_width; i++) {
	    GLubyte r = texel[i][0] * 255.0F + 0.5F;
	    GLubyte g = texel[i][1] * 255.0F + 0.5F;
	    GLubyte b = texel[i][2] * 255.0F + 0.5F;
	    output[i] = 0xFF000000 | (r << 16) | (g << 8) | b;
	}

	PTR_ADD_STRIDE(GLuint, output, dstStrideInBytes);
	src += srcStrideInBytes;
    }
}


static void
export_bgra_1555_rev (void *dst, int dstStrideInBytes,
		      const GLubyte *src, int srcStrideInBytes,
		      int src_width, int src_height,
		      EXTRACT xtract)
{
    int i, j;
    GLushort *output = dst;
    GLfloat4 texel[MAX_TEXTURE_SIZE];

    for (j = 0; j < src_height; j++) {
	xtract(src, src_width, texel);
	for (i = 0; i < src_width; i++) {
	    GLubyte r = texel[i][0] * 31.0F + 0.5F;
	    GLubyte g = texel[i][1] * 31.0F + 0.5F;
	    GLubyte b = texel[i][2] * 31.0F + 0.5F;
	    GLubyte a = texel[i][3] *  1.0F + 0.5F;
	    output[i] = (a << 15) | (r << 10) | (g << 5) | b;
	}
	PTR_ADD_STRIDE(GLushort, output, dstStrideInBytes);
	src += srcStrideInBytes;
    }
}


static void
export_rgba_float (void *dst, int dstStrideInBytes,
		   const GLubyte *src, int srcStrideInBytes,
		   int src_width, int src_height,
		   EXTRACT xtract)
{
    int i, j;
    GLfloat *output = dst;
    GLfloat4 texel[MAX_TEXTURE_SIZE];

    for (j = 0; j < src_height; j++) {
	xtract(src, src_width, texel);
	for (i = 0; i < src_width; i++) {
	    output[i * 4 + 0] = texel[i][0];
	    output[i * 4 + 1] = texel[i][1];
	    output[i * 4 + 2] = texel[i][2];
	    output[i * 4 + 3] = texel[i][3];
	}
	PTR_ADD_STRIDE(GLfloat, output, dstStrideInBytes);
	src += srcStrideInBytes;
    }
}


static void
export_rgb_565 (void *dst, int dstStrideInBytes,
		const GLubyte *src, int srcStrideInBytes,
		int src_width, int src_height,
		EXTRACT xtract)
{
    int i, j;
    GLushort *output = dst;
    GLfloat4 texel[MAX_TEXTURE_SIZE];

    for (j = 0; j < src_height; j++) {
	xtract(src, src_width, texel);
	for (i = 0; i < src_width; i++) {
	    GLubyte r = texel[i][0] * 31.0F + 0.5F;
	    GLubyte g = texel[i][1] * 63.0F + 0.5F;
	    GLubyte b = texel[i][2] * 31.0F + 0.5F;
	    output[i] = (r << 11) | (g << 5) | b;
	}
	PTR_ADD_STRIDE(GLushort, output, dstStrideInBytes);
	src += srcStrideInBytes;
    }
}


static void
export_l_ubyte (void *dst, int dstStrideInBytes,
		const GLubyte *src, int srcStrideInBytes,
		int src_width, int src_height,
		EXTRACT xtract)
{
    int i, j;
    GLubyte *output = dst;
    GLfloat4 texel[MAX_TEXTURE_SIZE];

    for (j = 0; j < src_height; j++) {
	xtract(src, src_width, texel);
	for (i = 0; i < src_width; i++) {
	    GLubyte r = texel[i][0] * 255.0F + 0.5F;
	    output[i] = r;
	}
	PTR_ADD_STRIDE(GLubyte, output, dstStrideInBytes);
	src += srcStrideInBytes;
    }
}


static void
export_a_ubyte (void *dst, int dstStrideInBytes,
		const GLubyte *src, int srcStrideInBytes,
		int src_width, int src_height,
		EXTRACT xtract)
{
    int i, j;
    GLubyte *output = dst;
    GLfloat4 texel[MAX_TEXTURE_SIZE];

    for (j = 0; j < src_height; j++) {
	xtract(src, src_width, texel);
	for (i = 0; i < src_width; i++) {
	    GLubyte a = texel[i][3] * 255.0F + 0.5F;
	    output[i] = a;
	}
	PTR_ADD_STRIDE(GLubyte, output, dstStrideInBytes);
	src += srcStrideInBytes;
    }
}


static void
export_la_ubyte (void *dst, int dstStrideInBytes,
		 const GLubyte *src, int srcStrideInBytes,
		 int src_width, int src_height,
		 EXTRACT xtract)
{
    int i, j;
    GLushort *output = dst;
    GLfloat4 texel[MAX_TEXTURE_SIZE];

    for (j = 0; j < src_height; j++) {
	xtract(src, src_width, texel);
	for (i = 0; i < src_width; i++) {
	    GLubyte r = texel[i][0] * 255.0F + 0.5F;
	    GLubyte a = texel[i][3] * 255.0F + 0.5F;
	    output[i] = (a << 8) | r;
	}
	PTR_ADD_STRIDE(GLushort, output, dstStrideInBytes);
	src += srcStrideInBytes;
    }
}


/* insert from RGBA(float) to specified format *******************************/


static void
insert_bgra_4444_rev (void *dst, GLfloat4 texel)
{
    GLushort *output = dst;
    GLubyte r = texel[0] * 15.0F + 0.5F;
    GLubyte g = texel[1] * 15.0F + 0.5F;
    GLubyte b = texel[2] * 15.0F + 0.5F;
    GLubyte a = texel[3] * 15.0F + 0.5F;
    output[0] = (a << 12) | (r << 8) | (g << 4) | b;
}


static void
insert_bgra_8888_rev (void *dst, GLfloat4 texel)
{
    GLuint *output = dst;
    GLubyte r = texel[0] * 255.0F + 0.5F;
    GLubyte g = texel[1] * 255.0F + 0.5F;
    GLubyte b = texel[2] * 255.0F + 0.5F;
    GLubyte a = texel[3] * 255.0F + 0.5F;
    output[0] = (a << 24) | (r << 16) | (g << 8) | b;
}


static void
insert_bgr1_8888_rev (void *dst, GLfloat4 texel)
{
    GLuint *output = dst;
    GLubyte r = texel[0] * 255.0F + 0.5F;
    GLubyte g = texel[1] * 255.0F + 0.5F;
    GLubyte b = texel[2] * 255.0F + 0.5F;
    output[0] = 0xFF000000 | (r << 16) | (g << 8) | b;
}


static void
insert_bgra_1555_rev (void *dst, GLfloat4 texel)
{
    GLushort *output = dst;
    GLubyte r = texel[0] * 31.0F + 0.5F;
    GLubyte g = texel[1] * 31.0F + 0.5F;
    GLubyte b = texel[2] * 31.0F + 0.5F;
    GLubyte a = texel[3] *  1.0F + 0.5F;
    output[0] = (a << 15) | (r << 10) | (g << 5) | b;
}


static void
insert_rgba_float (void *dst, GLfloat4 texel)
{
    GLfloat *output = dst;
    output[0] = texel[0];
    output[1] = texel[1];
    output[2] = texel[2];
    output[3] = texel[3];
}


static void
insert_rgb_565 (void *dst, GLfloat4 texel)
{
    GLushort *output = dst;
    GLubyte r = texel[0] * 31.0F + 0.5F;
    GLubyte g = texel[1] * 63.0F + 0.5F;
    GLubyte b = texel[2] * 31.0F + 0.5F;
    output[0] = (r << 11) | (g << 5) | b;
}


static void
insert_l_ubyte (void *dst, GLfloat4 texel)
{
    GLubyte *output = dst;
    GLubyte r = texel[0] * 255.0F + 0.5F;
    output[0] = r;
}


static void
insert_a_ubyte (void *dst, GLfloat4 texel)
{
    GLubyte *output = dst;
    GLubyte a = texel[3] * 255.0F + 0.5F;
    output[0] = a;
}


static void
insert_la_ubyte (void *dst, GLfloat4 texel)
{
    GLushort *output = dst;
    GLubyte r = texel[0] * 255.0F + 0.5F;
    GLubyte a = texel[3] * 255.0F + 0.5F;
    output[0] = (a << 8) | r;
}


/* API ***********************************************************************/


GLvoid *
cvt2d (const PACKING *unpack, const PACKING *pack,
       int xoffset, int yoffset,
       int dst_width, int dst_height,
       GLvoid *dst,
       GLenum dst_format,
       GLenum dst_type,
       int src_width, int src_height,
       const GLvoid *src,
       GLenum src_format,
       GLenum src_type)
{
    EXTRACT xtract = NULL;
    EXPORT xport = NULL;
    int srcTexelBytes = 0;
    int srcStrideInBytes;
    int dstTexelBytes = 0;
    int dstStrideInBytes;
    int offset;

    if (src == NULL) {
	return NULL;
    }

    if (src_format == GL_RGBA) {
	if (src_type == GL_UNSIGNED_BYTE) {
	    srcTexelBytes = 4;
	    xtract = extract_rgba_ubyte;
	}
	if (src_type == GL_UNSIGNED_SHORT) {
	    srcTexelBytes = 8;
	    xtract = PICK(extract_rgba_ushort);
	}
	if (src_type == GL_FLOAT) {
	    srcTexelBytes = 16;
	    xtract = extract_rgba_float;
	}
    }
    if (src_format == GL_BGRA) {
	if (src_type == GL_UNSIGNED_BYTE) {
	    srcTexelBytes = 4;
	    xtract = extract_bgra_ubyte;
	}
	if (src_type == GL_UNSIGNED_SHORT) {
	    srcTexelBytes = 8;
	    xtract = PICK(extract_bgra_ushort);
	}
	if (src_type == GL_UNSIGNED_SHORT_4_4_4_4_REV) {
	    srcTexelBytes = 2;
	    xtract = extract_bgra_4444_rev;
	}
	if (src_type == GL_UNSIGNED_INT_8_8_8_8_REV) {
	    srcTexelBytes = 4;
	    xtract = extract_bgra_8888_rev;
	}
    }
    if (src_format == GL_RGB) {
	if (src_type == GL_UNSIGNED_BYTE) {
	    srcTexelBytes = 3;
	    xtract = extract_rgb_ubyte;
	}
	if (src_type == GL_UNSIGNED_SHORT) {
	    srcTexelBytes = 6;
	    xtract = PICK(extract_rgb_ushort);
	}
	if (src_type == GL_UNSIGNED_SHORT_5_6_5) {
	    srcTexelBytes = 2;
	    xtract = extract_rgb_565;
	}
    }
    if (src_format == GL_BGR) {
	if (src_type == GL_UNSIGNED_BYTE) {
	    srcTexelBytes = 3;
	    xtract = extract_bgr_ubyte;
	}
	if (src_type == GL_UNSIGNED_INT_8_8_8_8_REV) {
	    srcTexelBytes = 4;
	    xtract = extract_bgr1_8888_rev;
	}
    }
    if (src_format == GL_LUMINANCE) {
	if (src_type == GL_UNSIGNED_BYTE) {
	    srcTexelBytes = 1;
	    xtract = extract_l_ubyte;
	}
	if (src_type == GL_UNSIGNED_SHORT) {
	    srcTexelBytes = 2;
	    xtract = PICK(extract_l_ushort);
	}
    }
    if (src_format == GL_ALPHA) {
	if (src_type == GL_UNSIGNED_BYTE) {
	    srcTexelBytes = 1;
	    xtract = extract_a_ubyte;
	}
    }
    if (src_format == GL_LUMINANCE_ALPHA) {
	if (src_type == GL_UNSIGNED_BYTE) {
	    srcTexelBytes = 2;
	    xtract = extract_la_ubyte;
	}
    }

    if (xtract == NULL) {
	gl_error(NULL, GL_INVALID_ENUM, "src_format %x, src_type = %x\n", src_format, src_type);
	return NULL;
    }

    if (dst_format == GL_BGRA && dst_type == GL_UNSIGNED_SHORT_4_4_4_4_REV) {
	dstTexelBytes = 2;
	xport = export_bgra_4444_rev;
    }
    if (dst_format == GL_BGRA && dst_type == GL_UNSIGNED_INT_8_8_8_8_REV) {
	dstTexelBytes = 4;
	xport = export_bgra_8888_rev;
    }
    if (dst_format == GL_BGRA && dst_type == GL_UNSIGNED_SHORT_1_5_5_5_REV) {
	dstTexelBytes = 2;
	xport = export_bgra_1555_rev;
    }
    if (dst_format == GL_RGBA && dst_type == GL_FLOAT) {
	dstTexelBytes = 16;
	xport = export_rgba_float;
    }
    if (dst_format == GL_RGB && dst_type == GL_UNSIGNED_SHORT_5_6_5) {
	dstTexelBytes = 2;
	xport = export_rgb_565;
    }
    if (dst_format == GL_BGR && dst_type == GL_UNSIGNED_INT_8_8_8_8_REV) {
	dstTexelBytes = 4;
	xport = export_bgr1_8888_rev;
    }
    if (dst_format == GL_LUMINANCE && dst_type == GL_UNSIGNED_BYTE) {
	dstTexelBytes = 1;
	xport = export_l_ubyte;
    }
    if (dst_format == GL_LUMINANCE_ALPHA && dst_type == GL_UNSIGNED_BYTE) {
	dstTexelBytes = 2;
	xport = export_la_ubyte;
    }
    if (dst_format == GL_ALPHA && dst_type == GL_UNSIGNED_BYTE) {
	dstTexelBytes = 1;
	xport = export_a_ubyte;
    }
    if (dst_format == GL_INTENSITY && dst_type == GL_UNSIGNED_BYTE) {
	dstTexelBytes = 1;
	xport = export_l_ubyte;
    }

    if (xport == NULL) {
	gl_error(NULL, GL_INVALID_ENUM, "dst_format %x, dst_type = %x\n", dst_format, dst_type);
	return NULL;
    }

    srcStrideInBytes = image_stride(unpack, src_width, srcTexelBytes);
    src = image_address(unpack, src, src_width, src_height, srcTexelBytes, 0, 0, 0);
    dstStrideInBytes = image_stride(pack, dst_width, dstTexelBytes);
    offset = (GLubyte *)image_address(pack, dst, dst_width, dst_height, dstTexelBytes, 0, yoffset, xoffset) - (GLubyte *)dst;

    if (dst == NULL) {
	dst = malloc(dstStrideInBytes * dst_height + offset);
    }
    if (dst == NULL) {
	return NULL;
    }

    xport((GLubyte *)dst + offset, dstStrideInBytes,
	  src, srcStrideInBytes, src_width, src_height, 
	  xtract);

    return dst;
}


/*
 * average downsampling 2D
 * srcWidth = M(dstWidth)
 * srcHeight = M(dstHeight)
 */
static void
resample_avm (void *dst, int dstWidth, int dstHeight,
	      int dstStrideInBytes, int dstTexelBytes,
	      INSERT nsert,
	      const void *src, int srcWidth, int srcHeight,
	      int srcStrideInBytes, int srcTexelBytes,
	      EXTRACT xtract)
{
    GLfloat4 texel[MAX_TEXTURE_SIZE];
    const int wDelta = srcWidth / dstWidth;
    const int hDelta = srcHeight / dstHeight;
    const float one_area = 1.0F / (wDelta * hDelta);
    const int dstAdjust = dstStrideInBytes - dstWidth * dstTexelBytes;
    const int srcAdjust1 = wDelta * srcTexelBytes - hDelta * srcStrideInBytes;
    const int srcAdjust2 = hDelta * srcStrideInBytes - srcWidth * srcTexelBytes;
    int dstOffset = 0;
    int srcOffset = 0;
    int w, h;

    h = 0;
    do {
	w = 0;
	do {
	    GLfloat4 sum = { 0.0F, 0.0F, 0.0F, 0.0F };
	    int j = hDelta;
	    do {
		int i;
		/* extract texel span */
		xtract((GLubyte *)src + srcOffset, wDelta, texel);
		/* sum the span */
		for (i = 0; i < wDelta; i++) {
		    sum[0] += texel[i][0];
		    sum[1] += texel[i][1];
		    sum[2] += texel[i][2];
		    sum[3] += texel[i][3];
		}
		/* next row */
		srcOffset += srcStrideInBytes;
	    } while (--j);
	    /* calculate mean value */
	    sum[0] *= one_area;
	    sum[1] *= one_area;
	    sum[2] *= one_area;
	    sum[3] *= one_area;
	    /* write resulting texel */
	    nsert((GLubyte *)dst + dstOffset, sum);
	    dstOffset += dstTexelBytes;
	    srcOffset += srcAdjust1;
	    w += wDelta;
	} while (w < srcWidth);
	dstOffset += dstAdjust;
	srcOffset += srcAdjust2;
	h += hDelta;
    } while (h < srcHeight);
}


void *
avm2d (void *dst, int dst_width, int dst_height, int dstStrideInPixels,
       GLenum dst_format, GLenum dst_type,
       const void *src, int src_width, int src_height, int srcStrideInPixels,
       GLenum src_format, GLenum src_type)
{
    EXTRACT xtract = NULL;
    INSERT nsert = NULL;
    int srcTexelBytes = 0;
    int srcStrideInBytes;
    int dstTexelBytes = 0;
    int dstStrideInBytes;

    if (src == NULL) {
	return NULL;
    }

    if (src_format == GL_BGRA && src_type == GL_UNSIGNED_SHORT_4_4_4_4_REV) {
	srcTexelBytes = 2;
	xtract = extract_bgra_4444_rev;
    }
    if (src_format == GL_BGRA && src_type == GL_UNSIGNED_INT_8_8_8_8_REV) {
	srcTexelBytes = 4;
	xtract = extract_bgra_8888_rev;
    }
    if (src_format == GL_RGBA && src_type == GL_FLOAT) {
	srcTexelBytes = 16;
	xtract = extract_rgba_float;
    }
    if (src_format == GL_RGB && src_type == GL_UNSIGNED_SHORT_5_6_5) {
	srcTexelBytes = 2;
	xtract = extract_rgb_565;
    }
    if (src_format == GL_BGR && src_type == GL_UNSIGNED_INT_8_8_8_8_REV) {
	srcTexelBytes = 4;
	xtract = extract_bgr1_8888_rev;
    }
    if (src_format == GL_LUMINANCE && src_type == GL_UNSIGNED_BYTE) {
	srcTexelBytes = 1;
	xtract = extract_l_ubyte;
    }
    if (src_format == GL_LUMINANCE_ALPHA && src_type == GL_UNSIGNED_BYTE) {
	srcTexelBytes = 2;
	xtract = extract_la_ubyte;
    }
    if (src_format == GL_ALPHA && src_type == GL_UNSIGNED_BYTE) {
	srcTexelBytes = 1;
	xtract = extract_a_ubyte;
    }
    if (src_format == GL_INTENSITY && src_type == GL_UNSIGNED_BYTE) {
	srcTexelBytes = 1;
	xtract = extract_l_ubyte;
    }

    if (xtract == NULL) {
	gl_error(NULL, GL_INVALID_ENUM, "src_format %x, src_type = %x\n", src_format, src_type);
	return NULL;
    }

    if (dst_format == GL_BGRA && dst_type == GL_UNSIGNED_SHORT_4_4_4_4_REV) {
	dstTexelBytes = 2;
	nsert = insert_bgra_4444_rev;
    }
    if (dst_format == GL_BGRA && dst_type == GL_UNSIGNED_INT_8_8_8_8_REV) {
	dstTexelBytes = 4;
	nsert = insert_bgra_8888_rev;
    }
    if (dst_format == GL_RGBA && dst_type == GL_FLOAT) {
	dstTexelBytes = 16;
	nsert = insert_rgba_float;
    }
    if (dst_format == GL_RGB && dst_type == GL_UNSIGNED_SHORT_5_6_5) {
	dstTexelBytes = 2;
	nsert = insert_rgb_565;
    }
    if (dst_format == GL_BGR && dst_type == GL_UNSIGNED_INT_8_8_8_8_REV) {
	dstTexelBytes = 4;
	nsert = insert_bgr1_8888_rev;
    }
    if (dst_format == GL_LUMINANCE && dst_type == GL_UNSIGNED_BYTE) {
	dstTexelBytes = 1;
	nsert = insert_l_ubyte;
    }
    if (dst_format == GL_LUMINANCE_ALPHA && dst_type == GL_UNSIGNED_BYTE) {
	dstTexelBytes = 2;
	nsert = insert_la_ubyte;
    }
    if (dst_format == GL_ALPHA && dst_type == GL_UNSIGNED_BYTE) {
	dstTexelBytes = 1;
	nsert = insert_a_ubyte;
    }
    if (dst_format == GL_INTENSITY && dst_type == GL_UNSIGNED_BYTE) {
	dstTexelBytes = 1;
	nsert = insert_l_ubyte;
    }

    if (nsert == NULL) {
	gl_error(NULL, GL_INVALID_ENUM, "dst_format %x, dst_type = %x\n", dst_format, dst_type);
	return NULL;
    }

    srcStrideInBytes = srcStrideInPixels * srcTexelBytes;
    dstStrideInBytes = dstStrideInPixels * dstTexelBytes;

    if (dst == NULL) {
	dst = malloc(dstStrideInBytes * dst_height);
    }
    if (dst == NULL) {
	return NULL;
    }

    resample_avm(dst, dst_width, dst_height,
		 dstStrideInBytes, dstTexelBytes,
		 nsert,
		 src, src_width, src_height,
		 srcStrideInBytes, srcTexelBytes,
		 xtract);

    return dst;
}
