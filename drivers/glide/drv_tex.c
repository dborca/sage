/**
 * \file drv_tex.c
 * Texture handling.
 */


#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <glide.h>
#include <g3ext.h>
#include "GL/gl.h"

#include "glinternal.h"
#include "main/context.h"
#include "util/macros.h"
#include "main/texstore.h"
#include "main/texcodec.h"
#include "main/texdef.h"
#include "tnl/tnl.h"
#include "driver.h"
#include "drv.h"


/**
 * Calculate upper limit of log2(n).
 *
 * \param n positive integer
 *
 * \return upper limit of log2(n)
 */
static int
logbase2 (int n)
{
    int i, nn;

    if (n <= 0) {
	return -1;
    }

    nn = n;
    i = -1;
    do {
	i++;
	nn >>= 1;
    } while (nn);

    if ((1 << i) != n) {
	i++;
    }

    return i;
}


/**
 * Translate OpenGL to hardware format.
 *
 * \param internalFormat OpenGL internal format
 * \param compress flag for texture compression
 *
 * \return hardware format
 */
static GrTextureFormat_t
tex_hwformat (GLenum internalFormat, GLboolean compress)
{
    switch (internalFormat) {
	case 1:
	case GL_LUMINANCE:
	case GL_LUMINANCE8:
	    return GR_TEXFMT_INTENSITY_8;
	case 2:
	case GL_LUMINANCE_ALPHA:
	case GL_LUMINANCE8_ALPHA8:
	    return GR_TEXFMT_ALPHA_INTENSITY_88;
	case 3:
	case GL_RGB:
	    /*if (srcType == GL_UNSIGNED_SHORT_5_6_5 ||
		srcType == GL_UNSIGNED_SHORT_5_6_5_REV) {
		return GR_TEXFMT_RGB_565;
	    }*/
	case GL_RGB4:
	case GL_RGB5:
	case GL_RGB8:
	    return allow_32bpt ? 0xFF : GR_TEXFMT_RGB_565;
	case 4:
	case GL_RGBA:
	    /*if (srcType == GL_UNSIGNED_SHORT_4_4_4_4 ||
		srcType == GL_UNSIGNED_SHORT_4_4_4_4_REV) {
		return GR_TEXFMT_ARGB_4444;
	    }*/
	case GL_RGBA4:
	case GL_RGBA8:
	    return allow_32bpt ? GR_TEXFMT_ARGB_8888 : GR_TEXFMT_ARGB_4444;
	case GL_ALPHA:
	case GL_ALPHA8:
	    return GR_TEXFMT_ALPHA_8;
	case GL_INTENSITY:
	case GL_INTENSITY8:
	    return GR_TEXFMT_ALPHA_8;
	case GL_RGB5_A1:
	    return GR_TEXFMT_ARGB_1555;
	case GL_COMPRESSED_RGB_FXT1_3DFX:
	    return compress ? GR_TEXFMT_ARGB_CMP_FXT1 : GR_TEXFMT_RGB_565;
	case GL_COMPRESSED_RGBA_FXT1_3DFX:
	    return compress ? GR_TEXFMT_ARGB_CMP_FXT1 : GR_TEXFMT_ARGB_4444;
	case GL_COMPRESSED_RGB:
	case GL_RGB_S3TC:
	case GL_RGB4_S3TC:
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	    return compress ? GR_TEXFMT_ARGB_CMP_DXT1 : GR_TEXFMT_RGB_565;
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	    return compress ? GR_TEXFMT_ARGB_CMP_DXT1 : GR_TEXFMT_ARGB_4444;
	case GL_COMPRESSED_RGBA:
	case GL_RGBA_S3TC:
	case GL_RGBA4_S3TC:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	    return compress ? GR_TEXFMT_ARGB_CMP_DXT3 : GR_TEXFMT_ARGB_4444;
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
	    return compress ? GR_TEXFMT_ARGB_CMP_DXT5 : GR_TEXFMT_ARGB_4444;
	default:
	    gl_error(NULL, GL_INVALID_ENUM, "%s: bad internal format %x\n", __FUNCTION__, internalFormat);
	    return -1;
    }
}


/**
 * Get texture info.
 *
 * \param width texture width in pixels
 * \param height texture height in pixels
 * \param[out] maxlod maximum LOD for this texture
 * \param[out] ar aspect ratio
 * \param[out] ss s-scale factor to accomodate 8x1 hardware limit
 * \param[out] ts t-scale factor to accomodate 1x8 hardware limit
 * \param[out] rwidth real texture width, to compensate for 8x1 limit
 * \param[out] rheight real texture height, to compensate for 1x8 limit
 */
void
tex_info (int width, int height,
	  int *maxlod, int *ar, float *ss, float *ts,
	  int *rwidth, int *rheight)
{
    int lodw = logbase2(width);
    int lodh = logbase2(height);
    int aspect = lodw - lodh;
    float sscale, tscale;
    int realw, realh;

    if (maxlod != NULL) {
	int lod = MAX2(lodw, lodh);
	*maxlod = lod;
    }

    realw = 1 << lodw;
    realh = 1 << lodh;
    sscale = 256.0F;
    tscale = 256.0F;
    if (aspect < 0) {
	sscale /= 1 << -aspect;
	if (aspect < GR_ASPECT_LOG2_1x8) {
/*gl_error(NULL, GL_INVALID_ENUM, "%s: aspect = %d\n", __FUNCTION__, aspect);*/
	    realw <<= -aspect - GR_ASPECT_LOG2_1x8;
	    aspect = GR_ASPECT_LOG2_1x8;
	}
    } else if (aspect > 0) {
	tscale /= 1 << aspect;
	if (aspect > GR_ASPECT_LOG2_8x1) {
/*gl_error(NULL, GL_INVALID_ENUM, "%s: aspect = %d\n", __FUNCTION__, aspect);*/
	    realh <<= aspect - GR_ASPECT_LOG2_8x1;
	    aspect = GR_ASPECT_LOG2_8x1;
	}
    }
    if (ar != NULL) {
	*ar = aspect;
    }
    if (ss != NULL) {
	*ss = sscale;
	*ts = tscale;
    }
    if (rwidth != NULL) {
	*rwidth = realw;
	*rheight = realh;
    }
}


/**
 * Get or allocate driver specific data for texture object.
 *
 * \param texObj texture object
 *
 * \return driver specific data for texture object
 */
TFX_OBJ *
getFxObj (TEX_OBJ *texObj)
{
    TFX_OBJ *fxObj = texObj->driverData;
    if (fxObj == NULL) {
	fxObj = malloc(sizeof(TFX_OBJ));
	if (fxObj != NULL) {
	    fxObj->parent = texObj;
	    texObj->driverData = fxObj;

	    fxObj->in = GL_FALSE;
	}
    }
    return fxObj;
}


/**
 * Get or allocate driver specific data for texture image.
 *
 * \param texImg texture image
 *
 * \return driver specific data for texture image
 */
TFX_IMG *
getFxImg (TEX_IMG *texImg)
{
    TFX_IMG *fxImg = texImg->driverData;
    if (fxImg == NULL) {
	fxImg = malloc(sizeof(TFX_IMG));
	if (fxImg != NULL) {
	    fxImg->parent = texImg;
	    texImg->driverData = fxImg;
	}
    }
    return fxImg;
}


/**
 * Specify a two-dimensional texture image.
 *
 * \param texObj texture object
 * \param level LOD
 * \param internalFormat internal format
 * \param width width in pixels
 * \param height height in pixels
 * \param format the format of the pixel data
 * \param type the data type of the pixel data
 * \param pixels pointer to the image data in memory
 * \param unpack unpacking mode
 *
 * \note This routine is responsible for setting up:
 *	texObj->t,
 *	texObj->driverData,
 *	texImg[k]->data,
 *	texImg[k]->width,
 *	texImg[k]->height,
 *	texImg[k]->compressedSize,
 *	texImg[k]->driverData
 * \note This routine must be able to process
 *	uncompressed -> uncompressed,
 *	uncompressed ->   compressed
 *
 * \sa glTexImage2D
 */
void
drv_TexImage2D (TEX_OBJ *texObj, GLint level, GLint internalFormat,
		GLsizei width, GLsizei height, GLenum format, GLenum type,
		const GLvoid *pixels, const PACKING *unpack)
{
    int hwformat;
    int rwidth, rheight;
    GrTexInfo *info;
    TFX_OBJ *fxObj;
    TFX_IMG *fxImg;
    void *data = NULL;
    TEX_IMG *texImg = &texObj->image[0][level];

    hwformat = tex_hwformat(internalFormat, allow_compressed == 3);
    tex_info(width, height,
	     NULL, NULL,
	     NULL, NULL,
	     &rwidth, &rheight);

    switch (hwformat) {
	case GR_TEXFMT_RGB_565:
	    texObj->t = texdef_rgb_565;
	    break;
	case GR_TEXFMT_ARGB_4444:
	    texObj->t = texdef_bgra_4444_rev;
	    break;
	case GR_TEXFMT_INTENSITY_8:
	    texObj->t = texdef_l_ubyte;
	    break;
	case GR_TEXFMT_ALPHA_8:
	    if (texObj->baseFormat == GL_ALPHA) {
		texObj->t = texdef_a_ubyte;
	    } else {
		texObj->t = texdef_i_ubyte;
	    }
	    break;
	case GR_TEXFMT_ALPHA_INTENSITY_88:
	    texObj->t = texdef_la_ubyte;
	    break;
	case GR_TEXFMT_ARGB_1555:
	    texObj->t = texdef_bgra_1555_rev;
	    break;
	case GR_TEXFMT_ARGB_8888:
	    texObj->t = texdef_bgra_8888_rev;
	    break;
	case 0xFF:
	    hwformat = GR_TEXFMT_ARGB_8888;
	    texObj->t = texdef_bgr1_8888_rev;
	    break;
	case GR_TEXFMT_ARGB_CMP_FXT1:
	    if (texObj->baseFormat == GL_RGB) {
		texObj->t = texdef_rgb_fxt1;
	    } else {
		texObj->t = texdef_rgba_fxt1;
	    }
	    break;
	case GR_TEXFMT_ARGB_CMP_DXT1:
	    if (texObj->baseFormat == GL_RGB) {
		texObj->t = texdef_rgb_dxt1;
	    } else {
		texObj->t = texdef_rgba_dxt1;
	    }
	    break;
	case GR_TEXFMT_ARGB_CMP_DXT3:
	    texObj->t = texdef_rgba_dxt3;
	    break;
	case GR_TEXFMT_ARGB_CMP_DXT5:
	    texObj->t = texdef_rgba_dxt3;
	    break;
	/* XXX return, to prevent texObj->t be NULL later */
    }
    if (!texObj->t->isCompressed) {
	data = cvt2d(unpack, &ctx_string_packing, 0, 0,
		     rwidth, rheight, NULL,
		     texObj->t->iFormat, texObj->t->iType,
		     width, height, pixels,
		     format, type);
    } else {
	texImg->compressedSize = tc_size(internalFormat, rwidth, rheight);
	data = tc_encode(unpack,
			 rwidth, rheight, NULL,
			 internalFormat,
			 width, height, pixels,
			 format, type);
    }

    if (texImg->data != NULL) {
	free(texImg->data);
    }
    texImg->data = data;
    texImg->width = width;
    texImg->height = height;
    if (data == NULL) {
	texObj->t = NULL;
	return;
    }

    fxObj = getFxObj(texObj);
    fxImg = getFxImg(texImg);

    info = (GrTexInfo *)fxObj;
    info->format = hwformat;
    fxImg->width = rwidth;
    fxImg->height = rheight;

    fxObj->validated = GL_FALSE;
}


/**
 * Specify a portion of an existing two-dimensional texture image.
 *
 * \param texObj texture object
 * \param level LOD
 * \param xoffset texel offset in the x direction within the texture array
 * \param yoffset texel offset in the y direction within the texture array
 * \param width width in pixels
 * \param height height in pixels
 * \param format the format of the pixel data
 * \param type the data type of the pixel data
 * \param pixels pointer to the image data in memory
 * \param unpack unpacking mode
 *
 * \note This routine must be able to process
 *	uncompressed -> uncompressed,
 *	uncompressed ->   compressed
 *
 * \sa glTexSubImage2D
 */
void
drv_TexSubImage2D (TEX_OBJ *texObj, GLint level, GLint xoffset, GLint yoffset,
		   GLsizei width, GLsizei height, GLenum format, GLenum type,
		   const GLvoid *pixels, const PACKING *unpack)
{
    GrTexInfo *info;
    TFX_OBJ *fxObj;
    TFX_IMG *fxImg;
    TEX_IMG *texImg = &texObj->image[0][level];

    /* XXX RESCALEHACK woes */
    if (xoffset + width > (int)texImg->width || yoffset + height > (int)texImg->height) {
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "%d x %d + %d x %d in %d, %d\n", xoffset, yoffset, width, height, texImg->width, texImg->height);
	return;
    }

    fxObj = texObj->driverData;
    fxImg = texImg->driverData;

    if (texObj->t->isCompressed) {
	tc_encode(unpack,
		  fxImg->width, fxImg->height,
		  tc_address(xoffset, yoffset, 0, texObj->t->iFormat, fxImg->width, texImg->data),
		  texObj->t->iFormat,
		  width, height, pixels,
		  format, type);
    } else {
	cvt2d(unpack, &ctx_string_packing, xoffset, yoffset,
	      fxImg->width, fxImg->height, texImg->data,
	      texObj->t->iFormat, texObj->t->iType,
	      width, height, pixels,
	      format, type);
    }

    fxObj->validated = GL_FALSE; /* XXX too heavy */
}


/**
 * Specify a two-dimensional compressed texture image.
 *
 * \param texObj texture object
 * \param level LOD
 * \param internalformat internal format
 * \param width width in pixels
 * \param height height in pixels
 * \param imageSize compressed image size
 * \param data pointer to the image data in memory
 *
 * \note This routine must be able to decompress images.
 *
 * \sa glCompressedTexImage2D
 */
void
drv_CompressedTexImage2D (TEX_OBJ *texObj, GLint level, GLenum internalformat,
			  GLsizei width, GLsizei height, GLsizei imageSize,
			  const GLvoid *data)
{
    int hwformat;
    int rwidth, rheight;
    GrTexInfo *info;
    TFX_OBJ *fxObj;
    TFX_IMG *fxImg;
    void *pixels = NULL;
    TEX_IMG *texImg = &texObj->image[0][level];

    gl_cry(__FILE__, __LINE__, __FUNCTION__, "%d, %x, %dx%d, %d, %p\n", level, internalformat, width, height, imageSize, data);

    if (allow_compressed == 1) {
	GLenum fmt;
	GLenum typ;
	pixels = tc_decode(NULL, &fmt, &typ, width, height, data, internalformat);
	if (pixels == NULL) {
	    return;
	}
	drv_TexImage2D(texObj, level, fmt, width, height, fmt, typ, pixels, &ctx_string_packing);
	free(pixels);
	return;
    }

    hwformat = tex_hwformat(internalformat, GL_TRUE);
    tex_info(width, height,
	     NULL, NULL,
	     NULL, NULL,
	     &rwidth, &rheight);

    switch (hwformat) {
	case GR_TEXFMT_ARGB_CMP_FXT1:
	    if (texObj->baseFormat == GL_RGB) {
		texObj->t = texdef_rgb_fxt1;
	    } else {
		texObj->t = texdef_rgba_fxt1;
	    }
	    break;
	case GR_TEXFMT_ARGB_CMP_DXT1:
	    if (texObj->baseFormat == GL_RGB) {
		texObj->t = texdef_rgb_dxt1;
	    } else {
		texObj->t = texdef_rgba_dxt1;
	    }
	    break;
	case GR_TEXFMT_ARGB_CMP_DXT3:
	    texObj->t = texdef_rgba_dxt3;
	    break;
	case GR_TEXFMT_ARGB_CMP_DXT5:
	    texObj->t = texdef_rgba_dxt3;
	    break;
	/* XXX return, to prevent texObj->t be NULL later */
    }
    texImg->compressedSize = tc_size(internalformat, rwidth, rheight);
    pixels = malloc(texImg->compressedSize);
    memcpy(pixels, data, imageSize);

    if (texImg->data != NULL) {
	free(texImg->data);
    }
    texImg->data = pixels;
    texImg->width = width;
    texImg->height = height;
    if (pixels == NULL) {
	texObj->t = NULL;
	return;
    }

    fxObj = getFxObj(texObj);
    fxImg = getFxImg(texImg);

    info = (GrTexInfo *)fxObj;
    info->format = hwformat;
    fxImg->width = rwidth;
    fxImg->height = rheight;

    fxObj->validated = GL_FALSE;
}


/**
 * Specify a portion of an existing two-dimensional compressed texture image.
 *
 * \param texObj texture object
 * \param level LOD
 * \param xoffset texel offset in the x direction within the texture array
 * \param yoffset texel offset in the y direction within the texture array
 * \param width width in pixels
 * \param height height in pixels
 * \param format the format of the pixel data
 * \param imageSize compressed image size
 * \param data pointer to the image data in memory
 *
 * \note This routine must be able to decompress images.
 *
 * \sa glCompressedTexSubImage2D
 */
void
drv_CompressedTexSubImage2D (TEX_OBJ *texObj, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    GrTexInfo *info;
    TFX_OBJ *fxObj;
    TFX_IMG *fxImg;
    TEX_IMG *texImg = &texObj->image[0][level];

    if (allow_compressed == 1) {
	GLenum fmt;
	GLenum typ;
	void *pixels = tc_decode(NULL, &fmt, &typ, width, height, data, format);
	if (pixels == NULL) {
	    return;
	}
	drv_TexSubImage2D(texObj, level, xoffset, yoffset,
			  width, height, fmt, typ,
			  pixels, &ctx_string_packing);
	free(pixels);
	return;
    }

    /* XXX RESCALEHACK woes */
    if (xoffset + width > (int)texImg->width || yoffset + height > (int)texImg->height) {
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "%d x %d + %d x %d in %d, %d\n", xoffset, yoffset, width, height, texImg->width, texImg->height);
	return;
    }

    fxObj = texObj->driverData;
    fxImg = texImg->driverData;

    tc_replace(xoffset, yoffset,
	       fxImg->width, fxImg->height,
	       texImg->data,
	       texObj->t->iFormat,
	       width, height,
	       data,
	       format);

    fxObj->validated = GL_FALSE; /* XXX too heavy */
}


/**
 * Return a texture image.
 *
 * \param texObj texture object
 * \param level LOD
 * \param format the format of the pixel data
 * \param type the data type of the pixel data
 * \param[out] pixels pointer to the image data in memory
 *
 * \note This routine must be able to decompress images.
 *
 * \sa glGetTexImage
 */
void
drv_GetTexImage (TEX_OBJ *texObj, GLint level, GLenum format, GLenum type, GLvoid *pixels)
{
    GrTexInfo *info;
    TFX_OBJ *fxObj;
    TFX_IMG *fxImg;
    TEX_IMG *texImg = &texObj->image[0][level];
    int rwidth, rheight;
    int width, height;
    void *data = texImg->data;

    fxObj = texObj->driverData;
    fxImg = texImg->driverData;

    rwidth = fxImg->width;
    rheight = fxImg->height;
    width = texImg->width;
    height = texImg->height;

    if (texObj->t->isCompressed) {
	GLenum fmt;
	GLenum typ;
	data = tc_decode(NULL, &fmt, &typ, width, height, data, texObj->t->iFormat);
	pixels = cvt2d(&ctx_string_packing, &ctx_pack, 0, 0,
		       width, height, pixels,
		       format, type,
		       width, height, data,
		       fmt, typ);
	free(data);
	return;
    }

    pixels = cvt2d(&ctx_string_packing, &ctx_pack, 0, 0,
		   width, height, pixels,
		   format, type,
		   rwidth, rheight, data,
		   texObj->t->iFormat, texObj->t->iType);
}


/**
 * Return a compressed texture image.
 *
 * \param texObj texture object
 * \param lod LOD
 * \param[out] img pointer to the image data in memory
 *
 * \sa glGetCompressedTexImage
 */
void
drv_GetCompressedTexImage (TEX_OBJ *texObj, GLint lod, GLvoid *img)
{
    TEX_IMG *texImg = &texObj->image[0][lod];
    GLsizei imageSize;

    if (texObj->t->isCompressed) {
	imageSize = texImg->compressedSize;
    } else {
	TFX_IMG *fxImg = texImg->driverData;
	imageSize = fxImg->width * fxImg->height * texObj->t->texelBytes;
    }
    memcpy(img, texImg->data, imageSize);
}


/**
 * Delete texture.
 *
 * \param texObj texture object
 *
 * \sa glDeleteTextures
 */
void
drv_DeleteTexture (TEX_OBJ *texObj)
{
    int j, k;

    gl_assert(texObj != NULL);

    if (texObj->driverData != NULL) {
	tmu_moveout(texObj);
	free(texObj->driverData);
    }
    for (j = 0; j < 6; j++) {
	for (k = 0; k < MAX_TEXTURE_LEVELS; k++) {
	    if (texObj->image[j][k].data != NULL) {
		free(texObj->image[j][k].data);
	    }
	    if (texObj->image[j][k].driverData != NULL) {
		free(texObj->image[j][k].driverData);
	    }
	}
    }
}
