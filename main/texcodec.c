#include <stdlib.h>
#include <string.h>

/* this requires dxtn library of Daniel Borca, i.e. tc-1.1.tar.gz
 * from http://www.geocities.ws/dborca/opengl/tc.html
 *
 * and NOT libtxc_dxtn from freedesktop.org, which is different:
 * http://people.freedesktop.org/~cbrill/libtxc_dxtn/
 * http://cgit.freedesktop.org/~mareko/libtxc_dxtn/
 * (also see https://github.com/divVerent/s2tc for an alternative)
 *
 * (Mesa6.4 does use libtxc_dxtn, maybe the code can be updated)
 */

#ifndef USE_EXTERNAL_DXTN_LIB
#define USE_EXTERNAL_DXTN_LIB 0
#endif

#if !USE_EXTERNAL_DXTN_LIB

#define _sage_dlopen(name, mode) NULL
#define _sage_dlsym(hndl, proc)  NULL
#define _sage_dlclose(hndl)

#elif defined(__linux__)

#include <dlfcn.h>
#define _sage_dlopen             dlopen
#define _sage_dlsym              dlsym
#define _sage_dlclose            dlclose
#define DXTN_LIB           "libdxtn.so"

#elif defined(__DJGPP__)

#include <dlfcn.h>
#define _sage_dlopen             dlopen
#define _sage_dlsym(hndl, proc)  dlsym(hndl,"_" proc)
#define _sage_dlclose            dlclose
#define DXTN_LIB           "dxtn.dxe"

#elif defined(__WIN32__)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define _sage_dlopen(name, mode) LoadLibrary(name)
#define _sage_dlsym(hndl, proc)  GetProcAddress(hndl, proc)
#define _sage_dlclose(hndl)      FreeLibrary(hndl)
#define DXTN_LIB           "dxtn.dll"

#endif

#include "GL/gl.h"

#include "glinternal.h" /* XXX for unapproved tokens */
#include "context.h"
#include "texstore.h"
#include "texcodec.h"


#define GET_PTR(type,name) if(!(name=(type)_sage_dlsym(h,#name))){tc_fini();return -1;}


typedef void (*TC_FETCHER) (const void *texture, int stride_in_pixels, int i, int j, unsigned char *rgba);
typedef void (*TC_ENCODER) (int width, int height, int comps, const void *source, int srcStrideInBytes, void *dest, int destRowStride);


static void *h = NULL;

static TC_FETCHER fxt1_decode_1 = NULL;
static TC_FETCHER dxt1_rgb_decode_1 = NULL;
static TC_FETCHER dxt1_rgba_decode_1 = NULL;
static TC_FETCHER dxt3_rgba_decode_1 = NULL;
static TC_FETCHER dxt5_rgba_decode_1 = NULL;

static TC_ENCODER fxt1_encode = NULL;
static TC_ENCODER dxt1_rgb_encode = NULL;
static TC_ENCODER dxt1_rgba_encode = NULL;
static TC_ENCODER dxt3_rgba_encode = NULL;
static TC_ENCODER dxt5_rgba_encode = NULL;


int
tc_init (void)
{
#if !USE_EXTERNAL_DXTN_LIB
    return -1;
#else
    if (h != NULL) {
	return 0;
    }

    h = _sage_dlopen(DXTN_LIB, RTLD_NOW | RTLD_LOCAL);
    if (h == NULL) {
	return -1;
    }

    GET_PTR(TC_FETCHER, fxt1_decode_1);
    GET_PTR(TC_FETCHER, dxt1_rgb_decode_1);
    GET_PTR(TC_FETCHER, dxt1_rgba_decode_1);
    GET_PTR(TC_FETCHER, dxt3_rgba_decode_1);
    GET_PTR(TC_FETCHER, dxt5_rgba_decode_1);
    GET_PTR(TC_ENCODER, fxt1_encode);
    GET_PTR(TC_ENCODER, dxt1_rgb_encode);
    GET_PTR(TC_ENCODER, dxt1_rgba_encode);
    GET_PTR(TC_ENCODER, dxt3_rgba_encode);
    GET_PTR(TC_ENCODER, dxt5_rgba_encode);

    return 0;
#endif
}


void
tc_fini (void)
{
#if USE_EXTERNAL_DXTN_LIB
    if (h != NULL) {
	_sage_dlclose(h);
	h = NULL;

	fxt1_decode_1 = NULL;
	dxt1_rgb_decode_1 = NULL;
	dxt1_rgba_decode_1 = NULL;
	dxt3_rgba_decode_1 = NULL;
	dxt5_rgba_decode_1 = NULL;
	fxt1_encode = NULL;
	dxt1_rgb_encode = NULL;
	dxt1_rgba_encode = NULL;
	dxt3_rgba_encode = NULL;
	dxt5_rgba_encode = NULL;
    }
#endif
}


void *
tc_decode (void *dst, GLenum *dst_format, GLenum *dst_type,
	   GLsizei width, GLsizei height,
	   const void *src, GLenum src_format)
{
#if !USE_EXTERNAL_DXTN_LIB
    return NULL;
#else
    int i, j;
    int comp;
    GLubyte *output;
    TC_FETCHER fetch = NULL;

    if (h == NULL) {
	return NULL;
    }

    switch (src_format) {
	case GL_COMPRESSED_RGB_FXT1_3DFX:
	    fetch = fxt1_decode_1;
	    comp = 3;
	    break;
	case GL_COMPRESSED_RGBA_FXT1_3DFX:
	    fetch = fxt1_decode_1;
	    comp = 4;
	    break;
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	    fetch = dxt1_rgb_decode_1;
	    comp = 3;
	    break;
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	    fetch = dxt1_rgba_decode_1;
	    comp = 4;
	    break;
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	    fetch = dxt3_rgba_decode_1;
	    comp = 4;
	    break;
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
	    fetch = dxt5_rgba_decode_1;
	    comp = 4;
	    break;
	default:
	    return NULL;
    }

    if (dst == NULL) {
	dst = malloc((width * height * comp + 1) * sizeof(GLubyte));
    }
    if (dst == NULL) {
	return NULL;
    }
    output = dst;

    for (j = 0; j < height; j++) {
	for (i = 0; i < width; i++) {
	    fetch(src, width, i, j, output);
	    output += comp;
	}
    }

    *dst_format = (comp == 3) ? GL_RGB : GL_RGBA;
    *dst_type = GL_UNSIGNED_BYTE;
    return dst;
#endif
}


GLint
tc_stride (GLenum format, GLsizei width)
{
    GLint stride;

    switch (format) {
	case GL_COMPRESSED_RGB_FXT1_3DFX:
	case GL_COMPRESSED_RGBA_FXT1_3DFX:
	    stride = ((width + 7) / 8) * 16;	/* 16 bytes per 8x4 tile */
	    break;
	case GL_RGB_S3TC:
	case GL_RGB4_S3TC:
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	    stride = ((width + 3) / 4) * 8;	/* 8 bytes per 4x4 tile */
	    break;
	case GL_RGBA_S3TC:
	case GL_RGBA4_S3TC:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
	    stride = ((width + 3) / 4) * 16;	/* 16 bytes per 4x4 tile */
	    break;
	default:
	    return 0;
    }

    return stride;
}


GLuint
tc_size (GLenum format, GLsizei width, GLsizei height)
{
    GLuint size;

    switch (format) {
	case GL_COMPRESSED_RGB_FXT1_3DFX:
	case GL_COMPRESSED_RGBA_FXT1_3DFX:
	    /* round up width to next multiple of 8, height to next multiple of 4 */
	    width = (width + 7) & ~7;
	    height = (height + 3) & ~3;
	    /* 16 bytes per 8x4 tile of RGB[A] texels */
	    size = width * height / 2;
	    /* Textures smaller than 8x4 will effectively be made into 8x4 and
	     * take 16 bytes.
	     */
	    if (size < 16)
		size = 16;
	    return size;
	case GL_RGB_S3TC:
	case GL_RGB4_S3TC:
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	    /* round up width, height to next multiple of 4 */
	    width = (width + 3) & ~3;
	    height = (height + 3) & ~3;
	    /* 8 bytes per 4x4 tile of RGB[A] texels */
	    size = width * height / 2;
	    /* Textures smaller than 4x4 will effectively be made into 4x4 and
	     * take 8 bytes.
	     */
	    if (size < 8)
		size = 8;
	    return size;
	case GL_RGBA_S3TC:
	case GL_RGBA4_S3TC:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
	    /* round up width, height to next multiple of 4 */
	    width = (width + 3) & ~3;
	    height = (height + 3) & ~3;
	    /* 16 bytes per 4x4 tile of RGBA texels */
	    size = width * height;	/* simple! */
	    /* Textures smaller than 4x4 will effectively be made into 4x4 and
	     * take 16 bytes.
	     */
	    if (size < 16)
		size = 16;
	    return size;
	default:
	    return 0;
    }
}


void *
tc_address (GLint col, GLint row, GLint img,
	    GLenum format,
	    GLsizei width, const void *image)
{
    GLubyte *addr;

    (void)img;

    switch (format) {
	case GL_COMPRESSED_RGB_FXT1_3DFX:
	case GL_COMPRESSED_RGBA_FXT1_3DFX:
	    addr = (GLubyte *)image + 16 * (((width + 7) / 8) * (row / 4) + col / 8);
	    break;
	case GL_RGB_S3TC:
	case GL_RGB4_S3TC:
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	    addr = (GLubyte *)image + 8 * (((width + 3) / 4) * (row / 4) + col / 4);
	    break;
	case GL_RGBA_S3TC:
	case GL_RGBA4_S3TC:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
	    addr = (GLubyte *)image + 16 * (((width + 3) / 4) * (row / 4) + col / 4);
	    break;
	default:
	    return NULL;
    }

    return addr;
}


void *
tc_encode (const PACKING *unpack,
	   int dst_width, int dst_height,
	   void *dst,
	   GLenum dst_format,
	   int src_width, int src_height,
	   const GLvoid *src,
	   GLenum src_format,
	   GLenum src_type)
{
#if !USE_EXTERNAL_DXTN_LIB
    return NULL;
#else
    TC_ENCODER encoder = NULL;
    int srcStrideInBytes;
    int srcTexelBytes;
    int destRowStride;
    int size;

    if (h == NULL) {
	return NULL;
    }

    if ((src_format != GL_RGB && src_format != GL_RGBA) || (src_type != GL_UNSIGNED_BYTE)) {
	return NULL;
    }
    srcTexelBytes = (src_format == GL_RGB) ? 3 : 4;

    switch (dst_format) {
	case GL_COMPRESSED_RGB_FXT1_3DFX:
	case GL_COMPRESSED_RGBA_FXT1_3DFX:
	    encoder = fxt1_encode;
	    break;
	case GL_RGB_S3TC:
	case GL_RGB4_S3TC:
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	    encoder = dxt1_rgb_encode;
	    break;
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	    encoder = dxt1_rgba_encode;
	    break;
	case GL_RGBA_S3TC:
	case GL_RGBA4_S3TC:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	    encoder = dxt3_rgba_encode;
	    break;
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
	    encoder = dxt5_rgba_encode;
	    break;
	default:
	    return NULL;
    }

    srcStrideInBytes = image_stride(unpack, src_width, srcTexelBytes);
    destRowStride = tc_stride(dst_format, dst_width);
    size = tc_size(dst_format, dst_width, dst_height);

    if (dst == NULL) {
	dst = malloc(size);
    }
    if (dst == NULL) {
	return NULL;
    }

    encoder(src_width, src_height, srcTexelBytes, src, srcStrideInBytes, dst, destRowStride);

    return dst;
#endif
}



#define MUH 4 /* XXX FXT1/DXTC */


void
tc_replace (int xoffset, int yoffset,
	    int dst_width, int dst_height,
	    void *dst,
	    GLenum dst_format,
	    int src_width, int src_height,
	    const GLvoid *src,
	    GLenum src_format)
{
    GLint destRowStride, srcRowStride;
    GLint i, rows;
    GLubyte *dest;

    if (dst_format != GL_COMPRESSED_RGB_FXT1_3DFX ||
	dst_format != GL_COMPRESSED_RGBA_FXT1_3DFX ||
	dst_format != GL_RGB_S3TC ||
	dst_format != GL_RGB4_S3TC ||
	dst_format != GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
	dst_format != GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
	dst_format != GL_RGBA_S3TC ||
	dst_format != GL_RGBA4_S3TC ||
	dst_format != GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
	dst_format != GL_COMPRESSED_RGBA_S3TC_DXT5_EXT) {
	return;
    }
    if (dst_format != src_format) {
	return;
    }

    srcRowStride = tc_stride(src_format, src_width);
    destRowStride = tc_stride(dst_format, dst_width);
    dest = tc_address(xoffset, yoffset, 0, dst_format, dst_width, dst);

    rows = src_height / MUH;

    for (i = 0; i < rows; i++) {
	memcpy(dest, src, srcRowStride);
	dest += destRowStride;
	src = (GLvoid *)((GLubyte *)src + srcRowStride);
    }
}


void
tc_hori_pad (GLsizei dst_width, GLsizei src_width, GLsizei height,
	     GLubyte *data, GLenum format, GLenum clamp)
{
    GLint row, col;
    const GLsizei MUW = ((format == GL_COMPRESSED_RGB_FXT1_3DFX) || (format == GL_COMPRESSED_RGBA_FXT1_3DFX)) ? 8 : 4;
    const GLboolean can_decode = (h != NULL);

    TC_FETCHER fetch = NULL;
    TC_ENCODER encoder = NULL;

    switch (format) {
	case GL_COMPRESSED_RGB_FXT1_3DFX:
	    fetch = fxt1_decode_1;
	    encoder = fxt1_encode;
	    break;
	case GL_COMPRESSED_RGBA_FXT1_3DFX:
	    fetch = fxt1_decode_1;
	    encoder = fxt1_encode;
	    break;
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	    fetch = dxt1_rgb_decode_1;
	    encoder = dxt1_rgb_encode;
	    break;
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	    fetch = dxt1_rgba_decode_1;
	    encoder = dxt1_rgba_encode;
	    break;
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	    fetch = dxt3_rgba_decode_1;
	    encoder = dxt3_rgba_encode;
	    break;
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
	    fetch = dxt5_rgba_decode_1;
	    encoder = dxt5_rgba_encode;
	    break;
	default:
	    return;
    }

    /* XXX implement mirroring */
    if (!can_decode || (clamp == TC_WRAP) || (clamp == TC_MIRROR)) {
	/* replicate texture */
	GLubyte *tex0 = data;
	GLsizei width = (src_width + (MUW-1)) / MUW;
	GLuint instride = tc_stride(format, src_width);
	GLuint outstride = tc_stride(format, dst_width);
	for (row = 0; row < ((height + (MUH-1)) & ~(MUH-1)); row += MUH) {
	    GLubyte *texture = tex0;
	    for (col = width; col < ((dst_width + (MUW-1)) / MUW); col += width) {
		texture += instride;
		memcpy(texture, tex0, instride);
	    }
	    tex0 += outstride;
	}
    } else {
	/* pad the right area with edge values */
	GLubyte *texture = data;
	GLuint width = src_width;
	GLuint instride = tc_stride(format, width);
	GLint outstride = tc_stride(format, dst_width);
	GLuint tstride = tc_stride(format, MUW);
	GLubyte *tex0 = malloc(tc_size(format, MUW, MUH));
	GLubyte *tex1 = malloc(MUW * MUH * 4);
	GLuint i = 0;
	for (row = 0; row < ((height + (MUH-1)) & ~(MUH-1)); row += MUH) {
	    GLuint j;
	    for (j = 0; j < MUH; j++, i++) {
		fetch(data, dst_width, width - 1, i, &tex1[j * MUW * 4]);
		for (col = 1; col < MUW; col++) {
		    ((GLuint *)tex1)[j * MUW + col] = ((GLuint *)tex1)[j * MUW];
		}
	    }
	    encoder(MUW, MUH, 4, tex1, MUW * 4, tex0, tstride);
	    texture += instride;
	    for (col = instride; col < outstride; col += tstride) {
		memcpy(texture, tex0, tstride);
		texture += tstride;
	    }
	}
	free(tex1);
	free(tex0);
    }
}


void
tc_vert_pad (GLsizei width, GLsizei dst_height, GLsizei src_height,
	     GLubyte *data, GLenum format, GLenum clamp)
{
    GLint row;
    const GLboolean can_decode = (h != NULL);

    TC_FETCHER fetch = NULL;
    TC_ENCODER encoder = NULL;

    switch (format) {
	case GL_COMPRESSED_RGB_FXT1_3DFX:
	    fetch = fxt1_decode_1;
	    encoder = fxt1_encode;
	    break;
	case GL_COMPRESSED_RGBA_FXT1_3DFX:
	    fetch = fxt1_decode_1;
	    encoder = fxt1_encode;
	    break;
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	    fetch = dxt1_rgb_decode_1;
	    encoder = dxt1_rgb_encode;
	    break;
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	    fetch = dxt1_rgba_decode_1;
	    encoder = dxt1_rgba_encode;
	    break;
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	    fetch = dxt3_rgba_decode_1;
	    encoder = dxt3_rgba_encode;
	    break;
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
	    fetch = dxt5_rgba_decode_1;
	    encoder = dxt5_rgba_encode;
	    break;
	default:
	    return;
    }

    /* XXX implement mirroring */
    if (!can_decode || (clamp == TC_WRAP) || (clamp == TC_MIRROR)) {
	/* replicate texture */
	GLubyte *tex0 = data;
	GLubyte *texture = data;
	GLubyte height = (src_height + (MUH-1)) / MUH;
	GLuint block = tc_size(format, width, src_height);
	for (row = height; row < ((dst_height + (MUH-1)) / MUH); row += height) {
	    texture += block;
	    memcpy(texture, tex0, block);
	}
    } else {
	GLint col;
	/* pad the bottom area with edge values */
	GLubyte *texture = data;
	GLuint cstride = tc_stride(format, width);
	GLuint ustride = width * 4;
	GLubyte *tex0 = malloc(tc_size(format, width, MUH));
	GLubyte *tex1 = malloc(ustride * MUH);
	for (col = 0; col < width; col++) {
	    fetch(data, width, col, src_height - 1, &tex1[col * 4]);
	}
	for (row = 1; row < MUH; row++) {
	    memcpy(&tex1[ustride * row], tex1, ustride);
	}
	encoder(width, MUH, 4, tex1, ustride, tex0, cstride);
	free(tex1);
	texture += tc_size(format, width, src_height);
	for (row = (src_height + (MUH-1)) & ~(MUH-1); row < ((dst_height + (MUH-1)) & ~(MUH-1)); row += MUH) {
	    memcpy(texture, tex0, cstride);
	    texture += cstride;
	}
	free(tex0);
    }
}
