#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <glide.h>
#include <g3ext.h>
#include "GL/gl.h"

#include "glinternal.h"
#include "main/context.h"
#include "main/texstore.h"
#include "main/texcodec.h"
#include "tnl/tnl.h"
#include "driver.h"
#include "drv.h"


typedef struct {
    GrCombineFunction_t function;
    GrCombineFactor_t factor;
    GrCombineLocal_t local;
    GrCombineOther_t other;
    FxBool invert;
} F_COMBINER;

typedef struct {
    GrCombineFunction_t function;
    GrCombineFactor_t factor; 
    FxBool invert;
} T_COMBINER;


static const F_COMBINER f_local[1] = {{
    GR_COMBINE_FUNCTION_LOCAL,
    GR_COMBINE_FACTOR_NONE,
    GR_COMBINE_LOCAL_ITERATED,
    GR_COMBINE_OTHER_NONE,
    FXFALSE
}};

static const F_COMBINER f_texture[1] = {{
    GR_COMBINE_FUNCTION_SCALE_OTHER,
    GR_COMBINE_FACTOR_ONE,
    GR_COMBINE_LOCAL_ITERATED,
    GR_COMBINE_OTHER_TEXTURE,
    FXFALSE
}};

static const F_COMBINER f_constant[1] = {{
    GR_COMBINE_FUNCTION_SCALE_OTHER,
    GR_COMBINE_FACTOR_ONE,
    GR_COMBINE_LOCAL_ITERATED,
    GR_COMBINE_OTHER_CONSTANT,
    FXFALSE
}};

static const F_COMBINER f_mul_constant[1] = {{
    GR_COMBINE_FUNCTION_SCALE_OTHER,
    GR_COMBINE_FACTOR_LOCAL,
    GR_COMBINE_LOCAL_ITERATED,
    GR_COMBINE_OTHER_CONSTANT,
    FXFALSE
}};

static const F_COMBINER f_mul_texture[1] = {{
    GR_COMBINE_FUNCTION_SCALE_OTHER,
    GR_COMBINE_FACTOR_LOCAL,
    GR_COMBINE_LOCAL_ITERATED,
    GR_COMBINE_OTHER_TEXTURE,
    FXFALSE
}};

static const F_COMBINER f_add_texture[1] = {{
    GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL,
    GR_COMBINE_FACTOR_ONE,
    GR_COMBINE_LOCAL_ITERATED,
    GR_COMBINE_OTHER_TEXTURE,
    FXFALSE
}};

static const F_COMBINER f_custom1[1] = {{ /* TEXTURE_ALP * TEXTURE + (1 - TEXTURE_ALP) * LOCAL */
    GR_COMBINE_FUNCTION_BLEND,
    GR_COMBINE_FACTOR_TEXTURE_ALPHA,
    GR_COMBINE_LOCAL_ITERATED,
    GR_COMBINE_OTHER_TEXTURE,
    FXFALSE
}};

static const F_COMBINER f_custom2[1] = {{ /* TEXTURE_RGB * CONST + (1 - TEXTURE_RGB) * LOCAL */
    GR_COMBINE_FUNCTION_BLEND,
    GR_COMBINE_FACTOR_TEXTURE_RGB, /* XXX Voodoo^2 and up */
    GR_COMBINE_LOCAL_ITERATED,
    GR_COMBINE_OTHER_CONSTANT,
    FXFALSE
}};

static const F_COMBINER f_custom3[1] = {{ /* TEXTURE_ALP * CONST + (1 - TEXTURE_ALP) * LOCAL */
    GR_COMBINE_FUNCTION_BLEND,
    GR_COMBINE_FACTOR_TEXTURE_ALPHA,
    GR_COMBINE_LOCAL_ITERATED,
    GR_COMBINE_OTHER_CONSTANT,
    FXFALSE
}};

static const F_COMBINER f_custom4[1] = {{ /* (1 - LOCAL) * TEXTURE */
    GR_COMBINE_FUNCTION_SCALE_OTHER,
    GR_COMBINE_FACTOR_ONE_MINUS_LOCAL,
    GR_COMBINE_LOCAL_ITERATED,
    GR_COMBINE_OTHER_TEXTURE,
    FXFALSE
}};


static const T_COMBINER t_zero[1] = {{
    GR_COMBINE_FUNCTION_ZERO,
    GR_COMBINE_FACTOR_NONE,
    FXFALSE
}};

static const T_COMBINER t_one[1] = {{
    GR_COMBINE_FUNCTION_ZERO,
    GR_COMBINE_FACTOR_NONE,
    FXTRUE
}};

static const T_COMBINER t_local[1] = {{
    GR_COMBINE_FUNCTION_LOCAL,
    GR_COMBINE_FACTOR_NONE,
    FXFALSE
}};

static const T_COMBINER t_other[1] = {{
    GR_COMBINE_FUNCTION_SCALE_OTHER,
    GR_COMBINE_FACTOR_ONE,
    FXFALSE
}};

static const T_COMBINER t_mul_other[1] = {{
    GR_COMBINE_FUNCTION_SCALE_OTHER,
    GR_COMBINE_FACTOR_LOCAL,
    FXFALSE
}};

static const T_COMBINER t_add_other[1] = {{
    GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL,
    GR_COMBINE_FACTOR_ONE,
    FXFALSE
}};

static const T_COMBINER t_custom1[1] = {{ /* (1 - LOCAL_ALPHA) * OTHER + LOCAL_ALPHA * LOCAL */
    GR_COMBINE_FUNCTION_BLEND,
    GR_COMBINE_FACTOR_ONE_MINUS_LOCAL_ALPHA,
    FXFALSE
}};

static const T_COMBINER t_custom2[1] = {{ /* (1 - LOCAL) * OTHER + LOCAL */
    GR_COMBINE_FUNCTION_SCALE_OTHER_ADD_LOCAL,
    GR_COMBINE_FACTOR_ONE_MINUS_LOCAL,
    FXFALSE
}};

static const T_COMBINER t_custom3[1] = {{ /* 1 - LOCAL_ALPHA * (1 - LOCAL) */
    GR_COMBINE_FUNCTION_SCALE_MINUS_LOCAL_ADD_LOCAL_ALPHA,
    GR_COMBINE_FACTOR_LOCAL_ALPHA,
    FXTRUE
}};

static const T_COMBINER t_custom4[1] = {{ /* OTHER_ALP * OTHER + (1 - OTHER_ALP) * LOCAL */
    GR_COMBINE_FUNCTION_BLEND,
    GR_COMBINE_FACTOR_ONE_MINUS_OTHER_ALPHA,
    FXFALSE
}};


#include "util/macros.h"
static void
scale_down (const TEX_IMG *srcTexImg, TEX_IMG *dstTexImg, const TEXDEF *texDef);


/* this routine fixes horizontal padding */
static void
fxWPad (const TEXDEF *texDef, const TEX_IMG *texImage, int sclamp)
{
    TFX_IMG *mml = texImage->driverData;
    GLuint row, col;
    GLuint texelBytes;

    if (texDef->isCompressed) {
	tc_hori_pad(mml->width, texImage->width, mml->height, texImage->data, texDef->iFormat, sclamp);
	return;
    }

    texelBytes = texDef->texelBytes;

    switch (sclamp) {
	case GR_TEXTURECLAMP_WRAP: {
	    /* replicate texture */
	    GLubyte *tex0 = texImage->data;
	    GLuint width = texImage->width;
	    GLuint instride = texImage->width * texelBytes;
	    GLuint outstride = mml->width * texelBytes;
	    for (row = 0; row < mml->height; row++) {
		GLubyte *texture = tex0;
		for (col = width; col < mml->width; col += width) {
		    texture += instride;
		    memcpy(texture, tex0, instride);
		}
		tex0 += outstride;
	    }
	    break;
	}

	case GR_TEXTURECLAMP_CLAMP:
	    /* pad the right area with edge values */
	    switch (texelBytes) {
		case 1: {
		    GLubyte *texture = texImage->data;
		    GLuint delta = mml->width - texImage->width;
		    for (row = 0; row < mml->height; row++) {
			GLubyte edge = texture[texImage->width - 1];
			memset(&texture[texImage->width], edge, delta);
			texture += mml->width;
		    }
		    break;
		}
		case 2: {
		    GLushort *texture = texImage->data;
		    GLuint delta = mml->width - texImage->width;
		    for (row = 0; row < mml->height; row++) {
			GLushort edge = texture[texImage->width - 1];
			for (col = 0; col < delta; col++) {
			    texture[texImage->width + col] = edge;
			}
			texture += mml->width;
		    }
		    break;
		}
		case 4: {
		    GLuint *texture = texImage->data;
		    GLuint delta = mml->width - texImage->width;
		    for (row = 0; row < mml->height; row++) {
			GLuint edge = texture[texImage->width - 1];
			for (col = 0; col < delta; col++) {
			    texture[texImage->width + col] = edge;
			}
			texture += mml->width;
		    }
		    break;
		}
		default:
		    gl_assert(0);
	    }
	    break;

	case GR_TEXTURECLAMP_MIRROR_EXT:
	    /* mirror the texture on the right */
	    switch (texelBytes) {
		case 1: {
		    GLubyte *texture = texImage->data;
		    GLuint width = texImage->width;
		    for (row = 0; row < mml->height; row++) {
			int pp = -1;
			for (col = width; col < mml->width; col += width) {
			    if (pp == -1) {
				GLuint k;
				for (k = 0; k < width; k++) {
				    texture[col + k] = texture[width - 1 - k];
				}
				pp = 1;
			    } else {
				memcpy(&texture[col], texture, width);
				pp = -1;
			    }
			}
			texture += mml->width;
		    }
		    break;
		}
		case 2: {
		    GLushort *texture = texImage->data;
		    GLuint width = texImage->width;
		    for (row = 0; row < mml->height; row++) {
			int pp = -1;
			for (col = width; col < mml->width; col += width) {
			    if (pp == -1) {
				GLuint k;
				for (k = 0; k < width; k++) {
				    texture[col + k] = texture[width - 1 - k];
				}
				pp = 1;
			    } else {
				memcpy(&texture[col], texture, width * 2);
				pp = -1;
			    }
			}
			texture += mml->width;
		    }
		    break;
		}
		case 4: {
		    GLuint *texture = texImage->data;
		    GLuint width = texImage->width;
		    for (row = 0; row < mml->height; row++) {
			int pp = -1;
			for (col = width; col < mml->width; col += width) {
			    if (pp == -1) {
				GLuint k;
				for (k = 0; k < width; k++) {
				    texture[col + k] = texture[width - 1 - k];
				}
				pp = 1;
			    } else {
				memcpy(&texture[col], texture, width * 4);
				pp = -1;
			    }
			}
			texture += mml->width;
		    }
		    break;
		}
		default:
		    gl_assert(0);
	    }
	    break;

	default:
	    gl_assert(0);
    }
}


/* this routine fixes vertical padding */
static void
fxHPad (const TEXDEF *texDef, const TEX_IMG *texImage, int tclamp)
{
    TFX_IMG *mml = texImage->driverData;
    GLuint row;
    GLuint texelBytes;

    if (texDef->isCompressed) {
	tc_vert_pad(mml->width, mml->height, texImage->height, texImage->data, texDef->iFormat, tclamp);
	return;
    }

    texelBytes = texDef->texelBytes;

    switch (tclamp) {
	case GR_TEXTURECLAMP_WRAP: {
	    /* replicate texture */
	    GLubyte *tex0 = texImage->data;
	    GLubyte *texture = texImage->data;
	    GLuint height = texImage->height;
	    GLuint stride = mml->width * texelBytes;
	    GLuint block = stride * height;
	    for (row = height; row < mml->height; row += height) {
		texture += block;
		memcpy(texture, tex0, block);
	    }
	    break;
	}

	case GR_TEXTURECLAMP_CLAMP: {
	    /* pad the bottom area with edge values */
	    GLuint stride = mml->width * texelBytes;
	    GLubyte *tex0 = (GLubyte *)texImage->data + (texImage->height - 1) * stride;
	    GLubyte *texture = tex0;
	    for (row = texImage->height; row < mml->height; row++) {
		texture += stride;
		memcpy(texture, tex0, stride);
	    }
	    break;
	}

	case GR_TEXTURECLAMP_MIRROR_EXT: {
	    /* mirror the texture down */
	    GLubyte *tex0 = texImage->data;
	    GLuint height = texImage->height;
	    GLuint stride = mml->width * texelBytes;
	    GLuint block = stride * height;
	    GLubyte *texture = tex0 + block;
	    int pp = -1;
	    for (row = height; row < mml->height; row += height) {
		if (pp == -1) {
		    int k;
		    for (k = height - 1; k >= 0; k--) {
			memcpy(texture, tex0 + k * stride, stride);
			texture += stride;
		    }
		    pp = 1;
		} else {
		    memcpy(texture, tex0, block);
		    texture += block;
		    pp = -1;
		}
	    }
	    break;
	}

	default:
	    gl_assert(0);
    }
}


static int
is_pow2 (int n)
{
    if (n <= 0) {
	return 0;
    }

    return !(n & (n - 1));
}


static GLboolean
setupTexture (TEX_OBJ *texObj)
{
    int minl, maxl;
    int minLod, maxLod;
    int ar;
    float sscale, tscale;
    int sclamp, tclamp;
    int minFilt, magFilt;
    int mipmap;
    TFX_OBJ *fxObj = (TFX_OBJ *)texObj->driverData;
    GrTexInfo *info = (GrTexInfo *)fxObj;
    TEX_IMG *img;

    switch (texObj->wrapS) {
	case GL_MIRRORED_REPEAT:
	    sclamp = GR_TEXTURECLAMP_MIRROR_EXT;
	    break;
	case GL_CLAMP_TO_BORDER: /* no-no, but don't REPEAT, either */
	case GL_CLAMP_TO_EDGE: /* CLAMP discarding border */
	case GL_CLAMP:
	    sclamp = GR_TEXTURECLAMP_CLAMP;
	    break;
	case GL_REPEAT:
	default:
	    sclamp = GR_TEXTURECLAMP_WRAP;
	    break;
    }
    switch (texObj->wrapT) {
	case GL_MIRRORED_REPEAT:
	    tclamp = GR_TEXTURECLAMP_MIRROR_EXT;
	    break;
	case GL_CLAMP_TO_BORDER: /* no-no, but don't REPEAT, either */
	case GL_CLAMP_TO_EDGE: /* CLAMP discarding border */
	case GL_CLAMP:
	    tclamp = GR_TEXTURECLAMP_CLAMP;
	    break;
	case GL_REPEAT:
	default:
	    tclamp = GR_TEXTURECLAMP_WRAP;
	    break;
    }

    switch (texObj->magFilter) {
	case GL_NEAREST:
	    magFilt = GR_TEXTUREFILTER_POINT_SAMPLED;
	    break;
	case GL_LINEAR:
	default:
	    magFilt = GR_TEXTUREFILTER_BILINEAR;
	    break;
    }
    switch (texObj->minFilter) {
	case GL_NEAREST_MIPMAP_LINEAR: /* XXX trilinear */
	case GL_NEAREST_MIPMAP_NEAREST:
	    minFilt = GR_TEXTUREFILTER_POINT_SAMPLED;
	    mipmap = GR_MIPMAP_NEAREST;
	    break;
	case GL_LINEAR_MIPMAP_LINEAR: /* XXX trilinear */
	case GL_LINEAR_MIPMAP_NEAREST:
	    minFilt = GR_TEXTUREFILTER_BILINEAR;
	    mipmap = GR_MIPMAP_NEAREST;
	    break;
	case GL_NEAREST:
	    minFilt = GR_TEXTUREFILTER_POINT_SAMPLED;
	    mipmap = GR_MIPMAP_DISABLE;
	    break;
	case GL_LINEAR:
	default:
	    minFilt = GR_TEXTUREFILTER_BILINEAR;
	    mipmap = GR_MIPMAP_DISABLE;
	    break;
    }

    minl = texObj->minLevel;
    maxl = texObj->maxLevel;
    if (minl == maxl) {
	mipmap = GR_MIPMAP_DISABLE;
    }

    img = &texObj->image[0][minl];
    if (!is_pow2(img->width) || !is_pow2(img->height)) {
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "NPOT: %d x %d is not supported\n", img->width, img->height);
	return GL_FALSE;
    }
    tex_info(img->width,
	     img->height,
	     &minLod, &ar,
	     &sscale, &tscale,
	     NULL, NULL);

    if (mipmap != GR_MIPMAP_DISABLE) {
#if FX_RESCALEHACK
	if (minLod > max_texture_levels) {
	    /*gl_cry(__FILE__, __LINE__, __FUNCTION__, "texture too large (%d x %d) skipping %d LODs\n", img->width, img->height, minLod - max_texture_levels);*/
	    minl += minLod - max_texture_levels;
	    if (minl > maxl) {
		TEX_IMG *dstTexImg = &texObj->image[0][minl];
		/* XXX changes in texture will not be propagated */
		if (dstTexImg->data == NULL) {
		    scale_down(img, dstTexImg, texObj->t);
		}
		tex_info(dstTexImg->width,
			 dstTexImg->height,
			 NULL, &ar,
			 &sscale, &tscale,
			 NULL, NULL);
		maxl = minl;
	    }
	    minLod = max_texture_levels;
	}
#else
	if (minLod > max_texture_levels) {
	    return GL_FALSE;
	}
#endif
	img = &texObj->image[0][maxl];
	tex_info(img->width,
		 img->height,
		 &maxLod, NULL,
		 NULL, NULL,
		 NULL, NULL);
    } else {
#if FX_RESCALEHACK
	/* XXX changes in texture will not be propagated */
	if (minLod > max_texture_levels) {
	    /*gl_cry(__FILE__, __LINE__, __FUNCTION__, "texture too large (%d x %d) rescaling\n", img->width, img->height);*/
	    scale_down(img, img, texObj->t);
	    tex_info(img->width,
		     img->height,
		     NULL, &ar,
		     &sscale, &tscale,
		     NULL, NULL);
	    minLod = max_texture_levels;
	}
#else
	if (minLod > max_texture_levels) {
	    return GL_FALSE;
	}
#endif
	maxLod = minLod;
	maxl = minl;
    }

    info->smallLodLog2 = maxLod;
    info->largeLodLog2 = minLod;
    info->aspectRatioLog2 = ar;

    fxObj->minLevel = minl;
    fxObj->maxLevel = maxl;
    fxObj->sscale = sscale;
    fxObj->tscale = tscale;

    if (mipmap != fxObj->mipmap) {
	fxObj->validated = GL_FALSE;
    }
    if (!fxObj->validated) {
	int i;
	/* maybe move this in tmu_movein?!? */
	for (i = minl; i <= maxl; i++) {
	    img = &texObj->image[0][i];
	    if (img->width != ((TFX_IMG *)img->driverData)->width) {
		fxWPad(texObj->t, img, sclamp);
	    }
	    if (img->height != ((TFX_IMG *)img->driverData)->height) {
		fxHPad(texObj->t, img, tclamp);
	    }
	}
	fxObj->validated = GL_TRUE;
	tmu_moveout(texObj); /* XXX wo-hooo */
    }

    fxObj->sclamp = sclamp;
    fxObj->tclamp = tclamp;
    fxObj->mipmap = mipmap;
    fxObj->minFilt = minFilt;
    fxObj->magFilt = magFilt;

    /* XXX remove the ret value and fix norescale path properly */
    return GL_TRUE;
}


#define CMB_STEP1
#include "drv_setup2.c"


static void
setupTextureSingle (int unit)
{
    TEX_OBJ *texObj = ctx_texture[unit].object;
    TFX_OBJ *fxObj = (TFX_OBJ *)texObj->driverData;
    GLenum baseFormat;
    int tmu;

    const F_COMBINER *CC, *AC;

    if (!setupTexture(texObj)) {
	return;
    }

    if (!fxObj->in) {
	tmu = tm_best_tmu(unit, texObj);
	tmu_movein(texObj, tmu);
    } else {
	tmu = fxObj->tmu;
	if (fxObj->tmu == GR_TMU_BOTH) {
	    tmu = unit;
	}
    }

    grTexClampMode(tmu, fxObj->sclamp, fxObj->tclamp);
    grTexMipMapMode(tmu, fxObj->mipmap, FXFALSE);
    grTexFilterMode(tmu, fxObj->minFilt, fxObj->magFilt);
    grTexLodBiasValue(tmu, texObj->bias + ctx_texture[unit].bias); /* XXX clamp */
    grTexSource(tmu,
                fxObj->range[tmu]->start,
                GR_MIPMAPLEVELMASK_BOTH,
                (GrTexInfo *)fxObj);

    baseFormat = texObj->baseFormat;

    CC = f_mul_texture;
    AC = f_mul_texture;
    if (baseFormat == GL_ALPHA) {
	CC = f_local;
    }

    switch (ctx_texture[unit].texenv) {
	case GL_REPLACE:
	    if (baseFormat != GL_ALPHA) {
		CC = f_texture;
	    }
	    if (baseFormat == GL_LUMINANCE || baseFormat == GL_RGB) {
		AC = f_local;
	    } else {
		AC = f_texture;
	    }
	    break;
	case GL_MODULATE:
	    break;
	case GL_DECAL:
	    CC = f_custom1;
	    AC = f_local;
	    break;
	case GL_BLEND:
	    if (baseFormat != GL_ALPHA) {
		CC = f_custom2; /* XXX Voodoo^2 and up */
	    }
	    if (baseFormat == GL_INTENSITY) {
		AC = f_custom3;
	    }
	    grConstantColorValue(
		(((GLuint)(ctx_texture[unit].envcolor[0] * 255.0f))      ) |
		(((GLuint)(ctx_texture[unit].envcolor[1] * 255.0f)) <<  8) |
		(((GLuint)(ctx_texture[unit].envcolor[2] * 255.0f)) << 16) |
		(((GLuint)(ctx_texture[unit].envcolor[3] * 255.0f)) << 24));
	    break;
	case GL_ADD:
	    if (baseFormat != GL_ALPHA) {
		CC = f_add_texture;
	    }
	    if (baseFormat == GL_INTENSITY) {
		AC = f_add_texture;
	    }
	    break;
#define CMB_STEP2
#include "drv_setup2.c"
	default:
	    gl_error(NULL, GL_INVALID_ENUM, "%s: %x\n", __FUNCTION__, ctx_texture[unit].texenv);
    }

    grColorCombine(CC->function,
		   CC->factor,
		   CC->local,
		   CC->other,
		   CC->invert);
    grAlphaCombine(AC->function,
		   AC->factor,
		   AC->local,
		   AC->other,
		   AC->invert);

    if (tmu == unit) {
	tmu0_source = GR_TMU0;
	tmu1_source = GR_TMU1;
    } else {
	tmu0_source = GR_TMU1;
	tmu1_source = GR_TMU0;
    }

    if (tmu == GR_TMU0) {
	if (allow_multitex) {
	    grTexCombine(GR_TMU1,
		     GR_COMBINE_FUNCTION_ZERO,
		     GR_COMBINE_FACTOR_NONE,
		     GR_COMBINE_FUNCTION_ZERO,
		     GR_COMBINE_FACTOR_NONE,
		     FXFALSE,
		     FXFALSE);
	}
	grTexCombine(GR_TMU0,
		     GR_COMBINE_FUNCTION_LOCAL,
		     GR_COMBINE_FACTOR_NONE,
		     GR_COMBINE_FUNCTION_LOCAL,
		     GR_COMBINE_FACTOR_NONE,
		     FXFALSE,
		     FXFALSE);
    } else {
	if (allow_multitex) {
	    grTexCombine(GR_TMU1,
		     GR_COMBINE_FUNCTION_LOCAL,
		     GR_COMBINE_FACTOR_NONE,
		     GR_COMBINE_FUNCTION_LOCAL,
		     GR_COMBINE_FACTOR_NONE,
		     FXFALSE,
		     FXFALSE);
	}
#if !FX_PASSTMUHACK
	grTexCombine(GR_TMU0,
		     GR_COMBINE_FUNCTION_SCALE_OTHER,
		     GR_COMBINE_FACTOR_ONE,
		     GR_COMBINE_FUNCTION_SCALE_OTHER,
		     GR_COMBINE_FACTOR_ONE,
		     FXFALSE,
		     FXFALSE);
#else
	grTexCombine(GR_TMU0,
		     GR_COMBINE_FUNCTION_BLEND,
		     GR_COMBINE_FACTOR_ONE,
		     GR_COMBINE_FUNCTION_BLEND,
		     GR_COMBINE_FACTOR_ONE,
		     FXFALSE,
		     FXFALSE);
#endif
    }

#if FX_PASSTMUHACK
    if (0 == unit) {
	tmu0_source = GR_TMU0;
	tmu1_source = GR_TMU1;
    } else {
	tmu0_source = GR_TMU1;
	tmu1_source = GR_TMU0;
    }
#endif
}


static void
setupTextureMulti (void)
{
    TEX_OBJ *texObj0 = ctx_texture[0].object;
    TEX_OBJ *texObj1 = ctx_texture[1].object;
    TFX_OBJ *fxObj0 = texObj0->driverData;
    TFX_OBJ *fxObj1 = texObj1->driverData;
    GLenum baseFormat0, baseFormat1;
    GLenum texEnv0, texEnv1;

    const F_COMBINER *CC, *AC;
    const T_COMBINER *TC0C, *TC0A, *TC1C, *TC1A;

    /* OpenGL: 0 -> 1
     * Glide : 1 -> 0
     */

    if (!setupTexture(texObj0)) {
	return;
    }
    if (!setupTexture(texObj1)) {
	return;
    }

    if (fxObj0->in) {
	if (fxObj0->tmu != GR_TMU1 && fxObj0->tmu != GR_TMU_BOTH) {
	    if (fxObj0->tmu == GR_TMU_SPLIT) {
		tmu_moveout(texObj0);
	    }
	    tmu_movein(texObj0, GR_TMU1);
	}
    } else {
	tmu_movein(texObj0, GR_TMU1);
    }
    if (fxObj1->in) {
	if (fxObj1->tmu != GR_TMU0 && fxObj1->tmu != GR_TMU_BOTH) {
	    if (fxObj1->tmu == GR_TMU_SPLIT) {
		tmu_moveout(texObj1);
	    }
	    tmu_movein(texObj1, GR_TMU0);
	}
    } else {
	tmu_movein(texObj1, GR_TMU0);
    }

    grTexClampMode(GR_TMU0, fxObj1->sclamp, fxObj1->tclamp);
    grTexMipMapMode(GR_TMU0, fxObj1->mipmap, FXFALSE);
    grTexFilterMode(GR_TMU0, fxObj1->minFilt, fxObj1->magFilt);
    grTexLodBiasValue(GR_TMU0, texObj1->bias + ctx_texture[1].bias); /* XXX clamp */
    grTexSource(GR_TMU0,
                fxObj1->range[GR_TMU0]->start,
                GR_MIPMAPLEVELMASK_BOTH,
                (GrTexInfo *)fxObj1);
    grTexClampMode(GR_TMU1, fxObj0->sclamp, fxObj0->tclamp);
    grTexMipMapMode(GR_TMU1, fxObj0->mipmap, FXFALSE);
    grTexFilterMode(GR_TMU1, fxObj0->minFilt, fxObj0->magFilt);
    grTexLodBiasValue(GR_TMU1, texObj0->bias + ctx_texture[0].bias); /* XXX clamp */
    grTexSource(GR_TMU1,
                fxObj0->range[GR_TMU1]->start,
                GR_MIPMAPLEVELMASK_BOTH,
                (GrTexInfo *)fxObj0);

    tmu0_source = GR_TMU1;
    tmu1_source = GR_TMU0;

    /* these are in OpenGL order */
    baseFormat0 = texObj0->baseFormat;
    baseFormat1 = texObj1->baseFormat;
    texEnv0 = ctx_texture[0].texenv;
    texEnv1 = ctx_texture[1].texenv;

    TC1C = t_local;
    TC0C = t_mul_other;
    CC = f_mul_texture;
    TC1A = t_local;
    TC0A = t_mul_other;
    AC = f_mul_texture;
    if (baseFormat0 == GL_ALPHA) {
	TC1C = t_one;
    }
    if (baseFormat1 == GL_ALPHA) {
	TC0C = t_other;
    }

    if (texEnv0 == GL_MODULATE && texEnv1 == GL_MODULATE) {
    } else if (texEnv0 == GL_REPLACE && texEnv1 == GL_ADD) {
	TC1C = t_local;
	TC0C = t_add_other;
	CC = f_texture;
	TC1A = t_local;
	TC0A = t_mul_other;
	AC = f_texture;
	if (baseFormat1 == GL_ALPHA) {
	    TC0C = t_other;
	}
	if (baseFormat0 == GL_ALPHA) {
	    TC1C = t_zero;
	    TC0C = t_local;
	    CC = f_add_texture;
	    if (baseFormat1 == GL_ALPHA) {
		TC0C = t_zero;
	    }
	}
	if (baseFormat1 == GL_INTENSITY) {
	    TC0A = t_add_other;
	}
	if (baseFormat0 == GL_LUMINANCE || baseFormat0 == GL_RGB) {
	    TC0A = t_local;
	    AC = f_mul_texture;
	    if (baseFormat1 == GL_INTENSITY) {
		AC = f_add_texture;
	    }
	}
    } else if (texEnv0 == GL_REPLACE && texEnv1 == GL_MODULATE) {
	TC1C = t_local;
	TC0C = t_mul_other;
	CC = f_texture;
	TC1A = t_local;
	TC0A = t_mul_other;
	AC = f_texture;
	if (baseFormat1 == GL_ALPHA) {
	    TC0C = t_other;
	}
	if (baseFormat0 == GL_ALPHA) {
	    TC1C = t_one;
	    CC = f_mul_texture;
	}
	if (baseFormat0 == GL_LUMINANCE || baseFormat0 == GL_RGB) {
	    TC1A = t_zero;
	    TC0A = t_local;
	    AC = f_mul_texture;
	}
    } else if (texEnv0 == GL_MODULATE && texEnv1 == GL_DECAL) {
	if (baseFormat1 == GL_RGB) {
	    TC1C = t_zero;
	    TC0C = t_local;
	    CC = f_texture;
	} else if (baseFormat0 == GL_ALPHA) {
	    /* XXX should be: f * (1 - A0) + C0 * A0 */
	    /* XXX really is: f * (1 - A0) + C0 * A0 * f */
	    TC1C = t_one;
	    TC0C = t_custom1;
	    CC = f_mul_texture;
	} else {
	    /* XXX should be: f * C1 * (1 - A0) + C0 * A0 */
	    /* XXX really is: f * C1 * (1 - A0) + C0 * A0 * f */
	    TC1C = t_local;
	    TC0C = t_custom1;
	    CC = f_mul_texture;
	}

	TC1A = t_local;
	TC0A = t_other;
	AC = f_mul_texture;
    } else if (texEnv0 == GL_MODULATE && texEnv1 == GL_BLEND) {
	TC1C = t_local;
	/* XXX should be: f * C1 * (1 - C0) + c * C0 */
	/* XXX really is: f * C1 * (1 - C0) + f * C0 */
	TC0C = t_custom2;
	CC = f_mul_texture;
	TC1A = t_local;
	TC0A = t_mul_other;
	AC = f_mul_texture;
	if (baseFormat0 == GL_ALPHA) {
	    TC1C = t_one;
	}
	if (baseFormat1 == GL_ALPHA) {
	    TC0C = t_other;
	}
	if (baseFormat1 == GL_INTENSITY) {
	    /* XXX should be: f * A1 * (1 - A0) + a * A0 */
	    /* XXX really is: f * A1 * (1 - A0) + f * A0 */
	    TC0A = t_custom2;
	}
    } else if (texEnv0 == GL_DECAL && texEnv1 == GL_MODULATE) {
	/* XXX should be: f * C0 * (1 - A1) + C0 * C1 * A1 */
	/* XXX really is: f * C0 * (1 - A1) + C0 * C1 * A1 * f */
	/* XXX could  be: f * C0 * (1 - A1) + C1 *  f * A1 */
	TC1C = t_custom3;
	TC0C = t_mul_other;
	CC = f_mul_texture;
	TC1A = t_zero;
	TC0A = t_local;
	AC = f_mul_texture;
	if (baseFormat1 == GL_ALPHA) {
	    TC0C = t_other;
	}
	if (baseFormat0 == GL_RGB) {
	    TC1C = t_local;
	    CC = f_texture;
	}
    } else if (texEnv0 == GL_MODULATE && texEnv1 == GL_REPLACE) {
	TC1C = t_zero;
	TC0C = t_local;
	CC = f_texture;
	TC1A = t_zero;
	TC0A = t_local;
	AC = f_texture;
	if (baseFormat1 == GL_ALPHA) {
	    TC1C = t_local;
	    TC0C = t_other;
	    CC = f_mul_texture;
	    if (baseFormat0 == GL_ALPHA) {
		TC1C = t_one;
	    }
	}
	if (baseFormat1 == GL_LUMINANCE || baseFormat1 == GL_RGB) {
	    TC1A = t_local;
	    TC0A = t_other;
	    AC = f_mul_texture;
	}
#define CMB_STEP3
#include "drv_setup2.c"
    } else {
	gl_error(NULL, GL_INVALID_ENUM, "%s: %x / %x\n", __FUNCTION__, texEnv0, texEnv1);
    }

    grColorCombine(CC->function,
		   CC->factor,
		   CC->local,
		   CC->other,
		   CC->invert);
    grAlphaCombine(AC->function,
		   AC->factor,
		   AC->local,
		   AC->other,
		   AC->invert);

    grTexCombine(GR_TMU1,
		 TC1C->function,
		 TC1C->factor,
		 TC1A->function,
		 TC1A->factor,
		 TC1C->invert,
		 TC1A->invert);
    grTexCombine(GR_TMU0,
		 TC0C->function,
		 TC0C->factor,
		 TC0A->function,
		 TC0A->factor,
		 TC0C->invert,
		 TC0A->invert);
}


static void
setupTextureNone (void)
{
    if (allow_multitex) {
	grTexCombine(GR_TMU1,
		     GR_COMBINE_FUNCTION_ZERO,
		     GR_COMBINE_FACTOR_NONE,
		     GR_COMBINE_FUNCTION_ZERO,
		     GR_COMBINE_FACTOR_NONE,
		     FXFALSE,
		     FXFALSE);
    }
    grTexCombine(GR_TMU0,
		 GR_COMBINE_FUNCTION_ZERO,
		 GR_COMBINE_FACTOR_NONE,
		 GR_COMBINE_FUNCTION_ZERO,
		 GR_COMBINE_FACTOR_NONE,
		 FXFALSE,
		 FXFALSE);
    grColorCombine(GR_COMBINE_FUNCTION_LOCAL,
		   GR_COMBINE_FACTOR_NONE,
		   GR_COMBINE_LOCAL_ITERATED,
		   GR_COMBINE_OTHER_NONE,
		   FXFALSE);
    grAlphaCombine(GR_COMBINE_FUNCTION_LOCAL,
		   GR_COMBINE_FACTOR_NONE,
		   GR_COMBINE_LOCAL_ITERATED,
		   GR_COMBINE_OTHER_NONE,
		   FXFALSE);
}


#include "drv_setup3.c"


void
drv_setupTexture (void)
{
    GLbitfield enabled0 = ctx_texture[0].enabled;
    GLbitfield enabled1 = ctx_texture[1].enabled;

    if (enabled0) {
	TEX_OBJ *texObj0 = ctx_texture[0].object;
	if (texObj0->driverData == NULL || texObj0->t == NULL) {
	    enabled0 = 0;
	}
    }
    if (enabled1) {
	TEX_OBJ *texObj1 = ctx_texture[1].object;
	if (texObj1->driverData == NULL || texObj1->t == NULL) {
	    enabled1 = 0;
	}
    }

    if (allow_combine > 0) {
	if ((enabled0 & (TEXTURE_1D | TEXTURE_2D)) &&
	    (enabled1 & (TEXTURE_1D | TEXTURE_2D))) {
	    setupTextureMultiNapalm();
	} else if (enabled0 & (TEXTURE_1D | TEXTURE_2D)) {
	    setupTextureSingleNapalm(0);
	} else if (enabled1 & (TEXTURE_1D | TEXTURE_2D)) {
	    setupTextureSingleNapalm(1);
	} else {
	    setupTextureNoneNapalm();
	}
    } else {
	if ((enabled0 & (TEXTURE_1D | TEXTURE_2D)) &&
	    (enabled1 & (TEXTURE_1D | TEXTURE_2D))) {
	    setupTextureMulti();
	} else if (enabled0 & (TEXTURE_1D | TEXTURE_2D)) {
	    setupTextureSingle(0);
	} else if (enabled1 & (TEXTURE_1D | TEXTURE_2D)) {
	    setupTextureSingle(1);
	} else {
	    setupTextureNone();
	}
    }
}


void
drv_setupBlend (void)
{
    GrAlphaBlendFnc_t sfact, dfact, asfact, adfact;

    if (!ctx_color.blending) {
	grAlphaBlendFunction(GR_BLEND_ONE, GR_BLEND_ZERO,
			     GR_BLEND_ONE, GR_BLEND_ZERO);
	return;
    }

    /*
     * 15/16 BPP alpha channel alpha blending modes
     *   0x0	AZERO		Zero
     *   0x4	AONE		One
     *
     * 32 BPP alpha channel alpha blending modes
     *   0x0	AZERO		Zero
     *   0x1	ASRC_ALPHA	Source alpha
     *   0x3	ADST_ALPHA	Destination alpha
     *   0x4	AONE		One
     *   0x5	AOMSRC_ALPHA	1 - Source alpha
     *   0x7	AOMDST_ALPHA	1 - Destination alpha
     *
     * If we don't have HW alpha buffer:
     *   DST_ALPHA == 1
     *   ONE_MINUS_DST_ALPHA == 0
     * Unsupported modes are:
     *   1 if used as src blending factor
     *   0 if used as dst blending factor
     */

    switch (ctx_color.blend_src_rgb) {
	case GL_ZERO:
	    sfact = GR_BLEND_ZERO;
	    break;
	case GL_ONE:
	    sfact = GR_BLEND_ONE;
	    break;
	case GL_DST_COLOR:
	    sfact = GR_BLEND_DST_COLOR;
	    break;
	case GL_ONE_MINUS_DST_COLOR:
	    sfact = GR_BLEND_ONE_MINUS_DST_COLOR;
	    break;
	case GL_SRC_ALPHA:
	    sfact = GR_BLEND_SRC_ALPHA;
	    break;
	case GL_ONE_MINUS_SRC_ALPHA:
	    sfact = GR_BLEND_ONE_MINUS_SRC_ALPHA;
	    break;
	case GL_DST_ALPHA:
	    sfact = fb_alpha ? GR_BLEND_DST_ALPHA : GR_BLEND_ONE/*bad*/;
	    break;
	case GL_ONE_MINUS_DST_ALPHA:
	    sfact = fb_alpha ? GR_BLEND_ONE_MINUS_DST_ALPHA : GR_BLEND_ZERO/*bad*/;
	    break;
	case GL_SRC_ALPHA_SATURATE:
	    sfact = GR_BLEND_ALPHA_SATURATE;
	    break;
	case GL_SRC_COLOR:
	    if (allow_blendsquare) {
		sfact = GR_BLEND_SAME_COLOR_EXT;
		break;
	    }
	case GL_ONE_MINUS_SRC_COLOR:
	    if (allow_blendsquare) {
		sfact = GR_BLEND_ONE_MINUS_SAME_COLOR_EXT;
		break;
	    }
	default:
	    sfact = GR_BLEND_ONE;
	    break;
    }

    switch (ctx_color.blend_src_alpha) {
	case GL_ZERO:
	    asfact = GR_BLEND_ZERO;
	    break;
	case GL_ONE:
	    asfact = GR_BLEND_ONE;
	    break;
	case GL_SRC_COLOR:
	case GL_SRC_ALPHA:
	    asfact = (fb_color == 24) ? GR_BLEND_SRC_ALPHA : GR_BLEND_ONE/*bad*/;
	    break;
	case GL_ONE_MINUS_SRC_COLOR:
	case GL_ONE_MINUS_SRC_ALPHA:
	    asfact = (fb_color == 24) ? GR_BLEND_ONE_MINUS_SRC_ALPHA : GR_BLEND_ONE/*bad*/;
	    break;
	case GL_DST_COLOR:
	case GL_DST_ALPHA:
	    asfact = ((fb_color == 24) && fb_alpha) ? GR_BLEND_DST_ALPHA : GR_BLEND_ONE/*bad*/;
	    break;
	case GL_ONE_MINUS_DST_COLOR:
	case GL_ONE_MINUS_DST_ALPHA:
	    asfact = ((fb_color == 24) && fb_alpha) ? GR_BLEND_ONE_MINUS_DST_ALPHA : GR_BLEND_ZERO/*bad*/;
	    break;
	case GL_SRC_ALPHA_SATURATE:
	    asfact = GR_BLEND_ONE;
	    break;
	default:
	    asfact = GR_BLEND_ONE;
	    break;
    }

    switch (ctx_color.blend_dst_rgb) {
	case GL_ZERO:
	    dfact = GR_BLEND_ZERO;
	    break;
	case GL_ONE:
	    dfact = GR_BLEND_ONE;
	    break;
	case GL_SRC_COLOR:
	    dfact = GR_BLEND_SRC_COLOR;
	    break;
	case GL_ONE_MINUS_SRC_COLOR:
	    dfact = GR_BLEND_ONE_MINUS_SRC_COLOR;
	    break;
	case GL_SRC_ALPHA:
	    dfact = GR_BLEND_SRC_ALPHA;
	    break;
	case GL_ONE_MINUS_SRC_ALPHA:
	    dfact = GR_BLEND_ONE_MINUS_SRC_ALPHA;
	    break;
	case GL_DST_ALPHA:
	    dfact = fb_alpha ? GR_BLEND_DST_ALPHA : GR_BLEND_ONE/*bad*/;
	    break;
	case GL_ONE_MINUS_DST_ALPHA:
	    dfact = fb_alpha ? GR_BLEND_ONE_MINUS_DST_ALPHA : GR_BLEND_ZERO/*bad*/;
	    break;
	case GL_DST_COLOR:
	    if (allow_blendsquare) {
		dfact = GR_BLEND_SAME_COLOR_EXT;
		break;
	    }
	case GL_ONE_MINUS_DST_COLOR:
	    if (allow_blendsquare) {
		dfact = GR_BLEND_ONE_MINUS_SAME_COLOR_EXT;
		break;
	    }
	default:
	    dfact = GR_BLEND_ZERO;
	    break;
    }

    switch (ctx_color.blend_dst_alpha) {
	case GL_ZERO:
	    adfact = GR_BLEND_ZERO;
	    break;
	case GL_ONE:
	    adfact = GR_BLEND_ONE;
	    break;
	case GL_SRC_COLOR:
	case GL_SRC_ALPHA:
	    adfact = (fb_color == 24) ? GR_BLEND_SRC_ALPHA : GR_BLEND_ZERO/*bad*/;
	    break;
	case GL_ONE_MINUS_SRC_COLOR:
	case GL_ONE_MINUS_SRC_ALPHA:
	    adfact = (fb_color == 24) ? GR_BLEND_ONE_MINUS_SRC_ALPHA : GR_BLEND_ZERO/*bad*/;
	    break;
	case GL_DST_COLOR:
	case GL_DST_ALPHA:
	    adfact = ((fb_color == 24) && fb_alpha) ? GR_BLEND_DST_ALPHA : GR_BLEND_ONE/*bad*/;
	    break;
	case GL_ONE_MINUS_DST_COLOR:
	case GL_ONE_MINUS_DST_ALPHA:
	    adfact = ((fb_color == 24) && fb_alpha) ? GR_BLEND_ONE_MINUS_DST_ALPHA : GR_BLEND_ZERO/*bad*/;
	    break;
	default:
	    adfact = GR_BLEND_ZERO;
	    break;
    }

    grAlphaBlendFunction(sfact, dfact, asfact, adfact);
}


void
drv_setupDepth (void)
{
    if (ctx_depth_test) {
	grDepthBufferFunction(ctx_depth_func - GL_NEVER + GR_CMP_NEVER);
	grDepthMask(ctx_depth_mask);
    } else {
	grDepthBufferFunction(GR_CMP_ALWAYS);
	grDepthMask(GL_FALSE);
    }
}


void
drv_setupAlpha (void)
{
    if (ctx_color.alpha_test) {
	grAlphaTestFunction(ctx_color.alpha_func - GL_NEVER + GR_CMP_NEVER);
	grAlphaTestReferenceValue((GrAlpha_t)(ctx_color.alpha_ref * 255.0F));
    } else {
	grAlphaTestFunction(GR_CMP_ALWAYS);
    }
}


void
drv_setupColor (void)
{
    if (fb_color == 24) {
	gfColorMaskExt(ctx_color.color_mask_r, ctx_color.color_mask_g,
		       ctx_color.color_mask_b, ctx_color.color_mask_a);
    } else {
	gl_assert(ctx_color.color_mask_r == ctx_color.color_mask_g);
	gl_assert(ctx_color.color_mask_g == ctx_color.color_mask_b);
	grColorMask(ctx_color.color_mask_r | ctx_color.color_mask_g |
		    ctx_color.color_mask_b, ctx_color.color_mask_a);
    }
}


void
drv_setupCull (void)
{
    GrCullMode_t mode = GR_CULL_DISABLE;
    /* XXX Glide does not cull points/lines, which is correct
     * (RENDER_UNFILLED|RENDER_TWOSIDE|RENDER_TWOSTENCIL) are culled in template/tri.h
     *	GL_FRONT_AND_BACK is correct
     * other primitives are culled inside Glide
     *	GL_FRONT_AND_BACK is not correct
     */
    if (ctx_culling) {
	switch (ctx_cull_face) {
	    case GL_BACK:
		if (ctx_front_face == GL_CCW) {
		    mode = GR_CULL_NEGATIVE;
		} else {
		    mode = GR_CULL_POSITIVE;
		}
		break;
	    case GL_FRONT:
		if (ctx_front_face == GL_CW) {
		    mode = GR_CULL_NEGATIVE;
		} else {
		    mode = GR_CULL_POSITIVE;
		}
		break;
	}
    }
    grCullMode(mode);
}


void
drv_setupFog (void)
{
    if (ctx_fog.fogging) {
	/* update fog color */
	grFogColorValue(
	    (((GLuint)(ctx_fog.color[0] * 255.0f))      ) |
	    (((GLuint)(ctx_fog.color[1] * 255.0f)) <<  8) |
	    (((GLuint)(ctx_fog.color[2] * 255.0f)) << 16) |
	    (((GLuint)(ctx_fog.color[3] * 255.0f)) << 24));

	/* reload the fog table */
	switch (ctx_fog.mode) {
	    case GL_LINEAR:
		guFogGenerateLinear(fogtable, ctx_fog.start, ctx_fog.end);
		if (fogtable[0] > 63) {
		    fogtable[0] = 63; /* XXX horrible */
		}
		break;
	    case GL_EXP:
		guFogGenerateExp(fogtable, ctx_fog.density);
		break;
	    case GL_EXP2:
		guFogGenerateExp2(fogtable, ctx_fog.density);
		break;
	    default:
		;
	}

	grFogTable(fogtable);
	if (ctx_fog.source == GL_FOG_COORDINATE) {
	    grVertexLayout(GR_PARAM_FOG_EXT, GR_VERTEX_FOG_OFFSET << 2,
					     GR_PARAM_ENABLE);
	    grFogMode(GR_FOG_WITH_TABLE_ON_FOGCOORD_EXT);
	} else {
	    grVertexLayout(GR_PARAM_FOG_EXT, GR_VERTEX_FOG_OFFSET << 2,
					     GR_PARAM_DISABLE);
	    grFogMode(GR_FOG_WITH_TABLE_ON_Q);
	}
    } else {
	grFogMode(GR_FOG_DISABLE);
    }
}


void
drv_setupScissor (void)
{
    if (ctx_scissor) {
	grClipWindow(ctx_scissor_x1, ctx_scissor_y1,
		     ctx_scissor_x2, ctx_scissor_y2);
    } else {
	grClipWindow(0, 0, screen_width, screen_height);
    }
}


static GrStencilOp_t
convertStencilOp (GLenum op)
{
    switch (op) {
	case GL_KEEP:
	    return GR_STENCILOP_KEEP;
	case GL_ZERO:
	    return GR_STENCILOP_ZERO;
	case GL_REPLACE:
	    return GR_STENCILOP_REPLACE;
	case GL_INCR:
	    return GR_STENCILOP_INCR_CLAMP;
	case GL_DECR:
	    return GR_STENCILOP_DECR_CLAMP;
	case GL_INVERT:
	    return GR_STENCILOP_INVERT;
	case GL_INCR_WRAP_EXT:
	    return GR_STENCILOP_INCR_WRAP;
	case GL_DECR_WRAP_EXT:
	    return GR_STENCILOP_DECR_WRAP;
	default:
	    gl_assert(0);
	    return GR_STENCILOP_KEEP;
   }
}


void 
drv_setupStencil (void)
{
    if (ctx_stencilmaxi) {
	if (ctx_stencil.enabled) {
	    GrCmpFnc_t stencilFailFunc = GR_STENCILOP_KEEP;
	    GrCmpFnc_t stencilZFailFunc = GR_STENCILOP_KEEP;
	    GrCmpFnc_t stencilZPassFunc = GR_STENCILOP_KEEP;
	    if (1/*XXX not multipassing*/) {
		stencilFailFunc = convertStencilOp(ctx_stencil.fail);
		stencilZFailFunc = convertStencilOp(ctx_stencil.zfail);
		stencilZPassFunc = convertStencilOp(ctx_stencil.zpass);
	    }
	    grEnable(GR_STENCIL_MODE_EXT);
	    gfStencilOpExt(stencilFailFunc,
			   stencilZFailFunc,
			   stencilZPassFunc);
	    gfStencilFuncExt(ctx_stencil.func - GL_NEVER + GR_CMP_NEVER,
			     ctx_stencil.ref,
			     ctx_stencil.valMask);
	    gfStencilMaskExt(ctx_stencil.writeMask);
	} else {
	    grDisable(GR_STENCIL_MODE_EXT);
	}
    }
}


#if FX_RESCALEHACK
static void
scale_down (const TEX_IMG *srcTexImg, TEX_IMG *dstTexImg, const TEXDEF *texDef)
{
    const TFX_IMG *srcFxImg = (TFX_IMG *)srcTexImg->driverData;
    const int maxSize = 1 << max_texture_levels;
    const int userSrcWidth = srcTexImg->width;
    const int userSrcHeight = srcTexImg->height;
    const int userDstWidth = MIN2(maxSize, userSrcWidth);
    const int userDstHeight = MIN2(maxSize, userSrcHeight);
    const int realSrcWidth = srcFxImg->width;
    TFX_IMG *dstFxImg = (TFX_IMG *)dstTexImg->driverData;
    int realDstWidth;
    int realDstHeight;
    void *dstImage;
    if (texDef->isCompressed) {
	return;
    }
    gl_cry(__FILE__, __LINE__, __FUNCTION__, "%d x %d -> %d x %d\n", userSrcWidth, userSrcHeight, userDstWidth, userDstHeight);
    tex_info(userDstWidth, userDstHeight,
	     NULL, NULL,
	     NULL, NULL,
	     &realDstWidth, &realDstHeight);
    dstImage = malloc(realDstWidth * realDstHeight * texDef->texelBytes);
    /*dstImage = */avm2d(dstImage, userDstWidth, userDstHeight, realDstWidth,
		     texDef->iFormat, texDef->iType,
		     srcTexImg->data, userSrcWidth, userSrcHeight, realSrcWidth,
		     texDef->iFormat, texDef->iType);
    if (dstTexImg->data != NULL) {
	free(dstTexImg->data);
    }
    dstTexImg->data = dstImage;
    dstTexImg->width = userDstWidth;
    dstTexImg->height = userDstHeight;
    if (dstFxImg == NULL) {
	dstFxImg = dstTexImg->driverData = malloc(sizeof(TFX_IMG));
    }
    dstFxImg->width = realDstWidth;
    dstFxImg->height = realDstHeight;
}
#endif
