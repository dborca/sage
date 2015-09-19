#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "context.h"
#include "util/macros.h"
#include "glapi.h"
#include "matrix.h"
#include "texture.h"
#include "tnl/tnl.h" /* XXX for TNL_GEN_* */
#include "driver.h"
#include "util/list.h"


TEX_OBJ *
tex_fill_object (TEX_OBJ *obj)
{
    if (obj == NULL) {
	obj = calloc(1, sizeof(TEX_OBJ));
	if (obj == NULL) {
	    return NULL;
	}
    } else {
	memset(obj, 0, sizeof(TEX_OBJ));
    }
    obj->minLevel = -1000;
    obj->maxLevel = 1000;
    obj->minFilter = GL_NEAREST_MIPMAP_LINEAR;
    obj->magFilter = GL_LINEAR;
    obj->wrapS = GL_REPEAT;
    obj->wrapT = GL_REPEAT;
    obj->wrapR = GL_REPEAT;

    return obj;
}


static GLenum
tex_base_format (GLenum internalFormat)
{
    switch (internalFormat) {
	case 1:
	case GL_LUMINANCE:
	case GL_LUMINANCE8:
	    return GL_LUMINANCE;
	case 2:
	case GL_LUMINANCE_ALPHA:
	case GL_LUMINANCE8_ALPHA8:
	    return GL_LUMINANCE_ALPHA;
	case 3:
	case GL_RGB:
	case GL_RGB4:
	case GL_RGB5:
	case GL_RGB8:
	case GL_COMPRESSED_RGB_FXT1_3DFX:
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_RGB_S3TC:
	case GL_RGB4_S3TC:
	    return GL_RGB;
	case 4:
	case GL_RGBA:
	case GL_RGBA4:
	case GL_RGBA8:
	case GL_COMPRESSED_RGBA_FXT1_3DFX:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
	case GL_RGBA_S3TC:
	case GL_RGBA4_S3TC:
	    return GL_RGBA;
	case GL_ALPHA:
	case GL_ALPHA8:
	    return GL_ALPHA;
	case GL_INTENSITY:
	case GL_INTENSITY8:
	    return GL_INTENSITY;
	case GL_RGB5_A1:
	    return GL_RGBA;
	default:
	    gl_error(NULL, GL_INVALID_ENUM, "%s: bad internal format %x\n", __FUNCTION__, internalFormat);
	    return GL_NONE;
    }
}


static TEX_OBJ *
tex_get_current_object (GLenum target)
{
    switch (target) {
	case GL_TEXTURE_1D:
	    return ctx_active_tex->obj1d;
	case GL_TEXTURE_2D:
	    return ctx_active_tex->obj2d;
	case GL_PROXY_TEXTURE_1D:
	    return ctx_proxy1d_obj;
	case GL_PROXY_TEXTURE_2D:
	    return ctx_proxy2d_obj;
	default:
	    gl_error(NULL, GL_INVALID_VALUE, "3d / cubemap / nv NYI\n");
	    return NULL;
    }
}


static void
tex_unbind (TEX_OBJ *ptr)
{
    int u;

    for (u = 0; u < MAX_TEXTURE_UNITS; u++) {
	if (ptr == ctx_texture[u].obj1d) {
	    if (ctx_texture[u].obj1d == ctx_texture[u].object) {
		ctx_texture[u].object = ctx_tex1d_obj;
	    }
	    ctx_texture[u].obj1d = ctx_tex1d_obj;
	}
	if (ptr == ctx_texture[u].obj2d) {
	    if (ctx_texture[u].obj2d == ctx_texture[u].object) {
		ctx_texture[u].object = ctx_tex2d_obj;
	    }
	    ctx_texture[u].obj2d = ctx_tex2d_obj;
	}
    }
}


void
tex_delete_all (void)
{
    TEX_OBJ *ptr, *tmp;
    
    list_foreach_s (ptr, tmp, ctx_tex_obj_list) {
	drv_DeleteTexture(ptr);
	list_remove(ptr);
	free(ptr);
    }
}


void GLAPIENTRY
imm_GenTextures (GLsizei n, GLuint *textures)
{
    int i;
    GLuint new_name = ctx_tex_hinumber;
    TEX_OBJ *last = list_last(ctx_tex_obj_list);
    for (i = 0; i < n; i++) {
	TEX_OBJ *ptr = tex_fill_object(NULL);
	if (ptr == NULL) {
	    /* free all newly added objects */
	    TEX_OBJ *t;
	    for (ptr = list_next(last), t = list_next(ptr);
		 !list_at_end(ctx_tex_obj_list, ptr);
		 ptr = t, t = list_next(t)) {
		 list_remove(ptr);
		 free(ptr);
	    }
	    return;
	}
	list_append(ctx_tex_obj_list, ptr);
	ptr->name = ++new_name;
	textures[i] = new_name;
    }
    ctx_tex_hinumber = new_name;
}


void GLAPIENTRY
imm_DeleteTextures (GLsizei n, const GLuint *textures)
{
    int i;
    for (i = 0; i < n; i++) {
	if (textures[i]) {
	    TEX_OBJ *ptr;
	    list_foreach (ptr, ctx_tex_obj_list) {
		if (ptr->name == textures[i]) {
		    break;
		}
	    }
	    if (!list_at_end(ctx_tex_obj_list, ptr)) {
		tex_unbind(ptr);
		drv_DeleteTexture(ptr);
		list_remove(ptr);
		free(ptr);
	    }
	}
    }
}


void GLAPIENTRY
imm_BindTexture (GLenum target, GLuint texture)
{
    TEX_OBJ *ptr;

    FLUSH_VERTICES();

    if (target != GL_TEXTURE_1D && target != GL_TEXTURE_2D) {
	gl_error(NULL, GL_INVALID_VALUE, "3d / cubemap / nv NYI\n");
	return;
    }

    if (texture == 0) {
	switch (target) {
	    case GL_TEXTURE_1D:
		ctx_active_tex->object = ctx_tex1d_obj;
		ctx_active_tex->obj1d = ctx_tex1d_obj;
		break;
	    case GL_TEXTURE_2D:
		ctx_active_tex->object = ctx_tex2d_obj;
		ctx_active_tex->obj2d = ctx_tex2d_obj;
		break;
	}
    } else {
	list_foreach (ptr, ctx_tex_obj_list) {
	    if (ptr->name == texture) {
		if (ptr->target != GL_NONE && ptr->target != target) {
		    gl_error(NULL, GL_INVALID_VALUE, "texture already bound to different dimensionality\n");
		    return;
		}
		break;
	    }
	}
	if (list_at_end(ctx_tex_obj_list, ptr)) {
	    ptr = tex_fill_object(NULL);
	    if (ptr == NULL) {
		gl_error(NULL, GL_INVALID_VALUE, "out of memory\n");
		return;
	    }
	    ptr->name = texture;
	    if (texture > ctx_tex_hinumber) {
		ctx_tex_hinumber = texture;
	    }
	} else {
	    list_remove(ptr);
	}
	list_prepend(ctx_tex_obj_list, ptr);
	ptr->target = target;
	ctx_active_tex->object = ptr;
	switch (target) {
	    case GL_TEXTURE_1D:
		ctx_active_tex->obj1d = ptr;
		break;
	    case GL_TEXTURE_2D:
		ctx_active_tex->obj2d = ptr;
		break;
	}
    }

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_TexImage1D (GLenum target, GLint level,
		GLint internalFormat,
		GLsizei width,
		GLint border, GLenum format, GLenum type,
		const GLvoid *pixels)
{
    TEX_OBJ *ptr = tex_get_current_object(target);

    if (ptr == NULL || ptr->target != target || level < 0 || level >= MAX_TEXTURE_LEVELS) {
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "%x, %d, %x, %d, %d, %x, %x, %p\n", target, level, internalFormat, width, border, format, type, pixels);
	gl_error(NULL, GL_INVALID_VALUE, "invalid texture\n");
	return;
    }

    FLUSH_VERTICES();

    if (ptr->maxLevel == 1000 || ptr->maxLevel < level) {
	ptr->maxLevel = level;
    }
    if (ptr->minLevel == -1000 || ptr->minLevel > level) {
	ptr->minLevel = level;
    }

    ptr->baseFormat = tex_base_format(internalFormat);
    drv_TexImage2D(ptr, level, internalFormat,
		   width, 1, format, type, pixels, &ctx_unpack);

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_TexSubImage1D (GLenum target, GLint level,
		   GLint xoffset,
		   GLsizei width, GLenum format,
		   GLenum type, const GLvoid *pixels)
{
    TEX_OBJ *ptr = tex_get_current_object(target);

    if (ptr == NULL) {
	return;
    }

    FLUSH_VERTICES();

    drv_TexSubImage2D(ptr, level, xoffset, 0, width, 1, format, type, pixels, &ctx_unpack);

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_TexImage2D (GLenum target, GLint level,
		GLint internalFormat,
		GLsizei width, GLsizei height,
		GLint border, GLenum format, GLenum type,
		const GLvoid *pixels)
{
    TEX_OBJ *ptr = tex_get_current_object(target);

    if (ptr == NULL || ptr->target != target || level < 0 || level >= MAX_TEXTURE_LEVELS) {
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "%x, %d, %x, %d, %d, %d, %x, %x, %p\n", target, level, internalFormat, width, height, border, format, type, pixels);
	gl_error(NULL, GL_INVALID_VALUE, "invalid texture\n");
	return;
    }

    FLUSH_VERTICES();

    if (ptr->maxLevel == 1000 || ptr->maxLevel < level) {
	ptr->maxLevel = level;
    }
    if (ptr->minLevel == -1000 || ptr->minLevel > level) {
	ptr->minLevel = level;
    }

    ptr->baseFormat = tex_base_format(internalFormat);
    drv_TexImage2D(ptr, level, internalFormat,
		   width, height, format, type, pixels, &ctx_unpack);

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_TexSubImage2D (GLenum target, GLint level,
		   GLint xoffset, GLint yoffset,
		   GLsizei width, GLsizei height,
		   GLenum format, GLenum type,
		   const GLvoid *pixels)
{
    TEX_OBJ *ptr = tex_get_current_object(target);

    if (ptr == NULL) {
	return;
    }

    FLUSH_VERTICES();

    drv_TexSubImage2D(ptr, level, xoffset, yoffset, width, height, format, type, pixels, &ctx_unpack);

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_CopyTexSubImage2D (GLenum target, GLint level,
		       GLint xoffset, GLint yoffset,
		       GLint x, GLint y,
		       GLsizei width, GLsizei height)
{
GLenum format, type;
void *pixels;
    TEX_OBJ *ptr = tex_get_current_object(target);

    if (ptr == NULL) {
	return;
    }
pixels = malloc(width * height * sizeof(GLubyte) * 4);
if (pixels == NULL) {
    return;
}
format = GL_RGBA;
type = GL_UNSIGNED_BYTE;
/* XXX check viewport overflow, ubyte might be lossy */
imm_ReadPixels(x, y, width, height, format, type, pixels);

    FLUSH_VERTICES();

    drv_TexSubImage2D(ptr, level, xoffset, yoffset, width, height, format, type, pixels, &ctx_unpack);

free(pixels);
    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_CompressedTexImage2D (GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data)
{
    TEX_OBJ *texObj = tex_get_current_object(target);

    if (texObj == NULL || texObj->target != target || level < 0 || level >= MAX_TEXTURE_LEVELS) {
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "%x, %d, %x, %d, %d, %d, %d, %p\n", target, level, internalFormat, width, height, border, imageSize, data);
	gl_error(NULL, GL_INVALID_VALUE, "invalid texture\n");
	return;
    }

    FLUSH_VERTICES();

    if (texObj->maxLevel == 1000 || texObj->maxLevel < level) {
	texObj->maxLevel = level;
    }
    if (texObj->minLevel == -1000 || texObj->minLevel > level) {
	texObj->minLevel = level;
    }

    texObj->baseFormat = tex_base_format(internalFormat);
    drv_CompressedTexImage2D(texObj, level, internalFormat, width, height, imageSize, data);

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_CompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    TEX_OBJ *texObj = tex_get_current_object(target);

    if (texObj == NULL) {
	return;
    }

    FLUSH_VERTICES();

    drv_CompressedTexSubImage2D(texObj, level, xoffset, yoffset, width, height, format, imageSize, data);

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_ActiveTexture (GLenum texture)
{
    FLUSH_VERTICES();

    ctx_active_tex = &ctx_texture[texture - GL_TEXTURE0];

    ctx_gl_state |= NEW_TEXTURE;
}


GLboolean GLAPIENTRY
imm_IsTexture (GLuint texture)
{
    TEX_OBJ *texObj;

    list_foreach (texObj, ctx_tex_obj_list) {
	if (texObj->name == texture) {
	    return (texObj->target != GL_NONE);
	}
    }

    return GL_FALSE;
}


void GLAPIENTRY
imm_TexEnvfv (GLenum target, GLenum pname, const GLfloat *params)
{
    FLUSH_VERTICES();

    switch (target) {
	case GL_TEXTURE_ENV:
	    switch (pname) {
		case GL_TEXTURE_ENV_MODE:
		    ctx_active_tex->texenv = params[0];
		    break;
		case GL_TEXTURE_ENV_COLOR:
		    ctx_active_tex->envcolor[0] = params[0];
		    ctx_active_tex->envcolor[1] = params[1];
		    ctx_active_tex->envcolor[2] = params[2];
		    ctx_active_tex->envcolor[3] = params[3];
		    break;
		/* XXX UT2003 */
		case GL_COMBINE_RGB:
		    ctx_active_tex->combineRGB.mode = params[0];
		    break;
		case GL_COMBINE_ALPHA:
		    ctx_active_tex->combineAlpha.mode = params[0];
		    break;
		case GL_RGB_SCALE:
		    ctx_active_tex->combineRGB.scale = params[0];
		    break;
		case GL_ALPHA_SCALE:
		    ctx_active_tex->combineAlpha.scale = params[0];
		    break;
		case GL_SOURCE0_RGB:
		    ctx_active_tex->combineRGB.source[0] = params[0];
		    break;
		case GL_SOURCE1_RGB:
		    ctx_active_tex->combineRGB.source[1] = params[0];
		    break;
		case GL_SOURCE2_RGB:
		    ctx_active_tex->combineRGB.source[2] = params[0];
		    break;
		case GL_SOURCE0_ALPHA:
		    ctx_active_tex->combineAlpha.source[0] = params[0];
		    break;
		case GL_SOURCE1_ALPHA:
		    ctx_active_tex->combineAlpha.source[1] = params[0];
		    break;
		case GL_SOURCE2_ALPHA:
		    ctx_active_tex->combineAlpha.source[2] = params[0];
		    break;
		case GL_OPERAND0_RGB:
		    ctx_active_tex->combineRGB.operand[0] = params[0];
		    break;
		case GL_OPERAND1_RGB:
		    ctx_active_tex->combineRGB.operand[1] = params[0];
		    break;
		case GL_OPERAND2_RGB:
		    ctx_active_tex->combineRGB.operand[2] = params[0];
		    break;
		case GL_OPERAND0_ALPHA:
		    ctx_active_tex->combineAlpha.operand[0] = params[0];
		    break;
		case GL_OPERAND1_ALPHA:
		    ctx_active_tex->combineAlpha.operand[1] = params[0];
		    break;
		case GL_OPERAND2_ALPHA:
		    ctx_active_tex->combineAlpha.operand[2] = params[0];
		    break;
		default:
		    gl_assert(0);
	    }
	    break;
	case GL_TEXTURE_FILTER_CONTROL:
	    switch (pname) {
		case GL_TEXTURE_LOD_BIAS:
		    ctx_active_tex->bias = params[0];
		    break;
	    }
	    break;
	default:
	    gl_assert(0);
    }

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_TexEnvf (GLenum target, GLenum pname, GLfloat param)
{
    imm_TexEnvfv(target, pname, &param);
}


void GLAPIENTRY
imm_TexParameterfv (GLenum target, GLenum pname, const GLfloat *params)
{
    TEX_OBJ *ptr = tex_get_current_object(target);

    if (ptr == NULL) {
	return;
    }

    FLUSH_VERTICES();

    switch (pname) {
	case GL_TEXTURE_WRAP_S:
	    ptr->wrapS = params[0];
	    break;
	case GL_TEXTURE_WRAP_T:
	    ptr->wrapT = params[0];
	    break;
	case GL_TEXTURE_MIN_FILTER:
	    ptr->minFilter = params[0];
	    break;
	case GL_TEXTURE_MAG_FILTER:
	    ptr->magFilter = params[0];
	    break;
	case GL_TEXTURE_MAX_LEVEL:
	    ptr->maxLevel = params[0];
	    break;
	case GL_TEXTURE_LOD_BIAS:
	    ptr->bias = params[0];
	    break;
	case GL_TEXTURE_BORDER_COLOR:		/* XXX Quake3 */
	case GL_TEXTURE_WRAP_R:			/* XXX UT2003 */
	case GL_TEXTURE_MAX_ANISOTROPY_EXT:	/* XXX SWKotOR2 */
	    break;
	default:
	    gl_error(NULL, GL_INVALID_VALUE, "%s: %x\n", __FUNCTION__, pname);
	    gl_assert(0);
    }

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_TexParameterf (GLenum target, GLenum pname, GLfloat param)
{
    imm_TexParameterfv(target, pname, &param);
}


void GLAPIENTRY
imm_TexGenfv (GLenum coord, GLenum pname, const GLfloat *params)
{
    FLUSH_VERTICES(); /* XXX maybe don't do it if texture gen not enabled */

    coord -= GL_S;
    switch (pname) {
	case GL_TEXTURE_GEN_MODE:
	    switch ((GLenum)params[0]) {
		case GL_OBJECT_LINEAR:
		    ctx_active_tex->genmode[coord] = TNL_GEN_OBJECT;
		    break;
		case GL_EYE_LINEAR:
		    ctx_active_tex->genmode[coord] = TNL_GEN_EYE;
		    break;
		case GL_SPHERE_MAP:
		    ctx_active_tex->genmode[coord] = TNL_GEN_SPHERE;
		    break;
		case GL_REFLECTION_MAP:
		    ctx_active_tex->genmode[coord] = TNL_GEN_REFLECT;
		    break;
		case GL_NORMAL_MAP:
		    ctx_active_tex->genmode[coord] = TNL_GEN_NORMAL;
		    break;
		default:
		    gl_assert(0);
	    }
	    break;
	case GL_OBJECT_PLANE:
	    ctx_active_tex->objplane[coord][0] = params[0];
	    ctx_active_tex->objplane[coord][1] = params[1];
	    ctx_active_tex->objplane[coord][2] = params[2];
	    ctx_active_tex->objplane[coord][3] = params[3];
	    break;
	case GL_EYE_PLANE:
	    matrix_mul_vec4(ctx_active_tex->eyeplane[coord], get_imv(), params);
	    break;
	default:
	    gl_assert(0);
    }

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_TexGenf (GLenum coord, GLenum pname, GLfloat param)
{
    imm_TexGenfv(coord, pname, &param);
}


void GLAPIENTRY
imm_GetTexLevelParameterfv (GLenum target, GLint level,
			    GLenum pname, GLfloat *params)
{
    TEX_OBJ *texObj;
    TEX_IMG *texImg;

    texObj = tex_get_current_object(target);
    texImg = &texObj->image[0][level];

    switch (pname) {
	case GL_TEXTURE_WIDTH:
	    params[0] = (texImg->width > ctx_const_max_texture_size) ? 0 : texImg->width;
	    break;
	case GL_TEXTURE_HEIGHT:
	    params[0] = (texImg->height > ctx_const_max_texture_size) ? 0 : texImg->height;
	    break;
	case GL_TEXTURE_COMPRESSED:
	    params[0] = texObj->t->isCompressed;
	    break;
	case GL_TEXTURE_GREEN_SIZE:
	    params[0] = texObj->t->greenBits;
	    break;
	case GL_TEXTURE_COMPRESSED_IMAGE_SIZE:
	    params[0] = texImg->compressedSize;
	    break;
	default:
	    gl_cry(__FILE__, __LINE__, __FUNCTION__, "%x, %d, %x\n", target, level, pname);
    }
}


void GLAPIENTRY
imm_GetTexImage (GLenum target, GLint level,
		 GLenum format, GLenum type,
		 GLvoid *pixels)
{
    TEX_OBJ *texObj;

    texObj = tex_get_current_object(target);
    if (texObj == NULL) {
	/* XXX GLError */
	return;
    }

    drv_GetTexImage(texObj, level, format, type, pixels);
}


void GLAPIENTRY
imm_GetCompressedTexImage (GLenum target, GLint lod, GLvoid *img)
{
    TEX_OBJ *texObj;

    texObj = tex_get_current_object(target);
    if (texObj == NULL) {
	/* XXX GLError */
	return;
    }

    drv_GetCompressedTexImage(texObj, lod, img);
}


/* variations */


void GLAPIENTRY
imm_TexEnviv (GLenum target, GLenum pname, const GLint *params)
{
    FLUSH_VERTICES();

    switch (target) {
	case GL_TEXTURE_ENV:
	    switch (pname) {
		case GL_TEXTURE_ENV_MODE:
		    ctx_active_tex->texenv = params[0];
		    break;
		case GL_TEXTURE_ENV_COLOR:
		    ctx_active_tex->envcolor[0] = I_TO_FLOAT(params[0]);
		    ctx_active_tex->envcolor[1] = I_TO_FLOAT(params[1]);
		    ctx_active_tex->envcolor[2] = I_TO_FLOAT(params[2]);
		    ctx_active_tex->envcolor[3] = I_TO_FLOAT(params[3]);
		    break;
		/* XXX UT2003 */
		case GL_COMBINE_RGB:
		    ctx_active_tex->combineRGB.mode = params[0];
		    break;
		case GL_COMBINE_ALPHA:
		    ctx_active_tex->combineAlpha.mode = params[0];
		    break;
		case GL_RGB_SCALE:
		    ctx_active_tex->combineRGB.scale = params[0];
		    break;
		case GL_ALPHA_SCALE:
		    ctx_active_tex->combineAlpha.scale = params[0];
		    break;
		case GL_SOURCE0_RGB:
		    ctx_active_tex->combineRGB.source[0] = params[0];
		    break;
		case GL_SOURCE1_RGB:
		    ctx_active_tex->combineRGB.source[1] = params[0];
		    break;
		case GL_SOURCE2_RGB:
		    ctx_active_tex->combineRGB.source[2] = params[0];
		    break;
		case GL_SOURCE0_ALPHA:
		    ctx_active_tex->combineAlpha.source[0] = params[0];
		    break;
		case GL_SOURCE1_ALPHA:
		    ctx_active_tex->combineAlpha.source[1] = params[0];
		    break;
		case GL_SOURCE2_ALPHA:
		    ctx_active_tex->combineAlpha.source[2] = params[0];
		    break;
		case GL_OPERAND0_RGB:
		    ctx_active_tex->combineRGB.operand[0] = params[0];
		    break;
		case GL_OPERAND1_RGB:
		    ctx_active_tex->combineRGB.operand[1] = params[0];
		    break;
		case GL_OPERAND2_RGB:
		    ctx_active_tex->combineRGB.operand[2] = params[0];
		    break;
		case GL_OPERAND0_ALPHA:
		    ctx_active_tex->combineAlpha.operand[0] = params[0];
		    break;
		case GL_OPERAND1_ALPHA:
		    ctx_active_tex->combineAlpha.operand[1] = params[0];
		    break;
		case GL_OPERAND2_ALPHA:
		    ctx_active_tex->combineAlpha.operand[2] = params[0];
		    break;
		default:
		    gl_assert(0);
	    }
	    break;
	case GL_TEXTURE_FILTER_CONTROL:
	    switch (pname) {
		case GL_TEXTURE_LOD_BIAS:
		    ctx_active_tex->bias = params[0];
		    break;
	    }
	    break;
	default:
	    gl_assert(0);
    }

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_TexEnvi (GLenum target, GLenum pname, GLint param)
{
    imm_TexEnviv(target, pname, &param);
}


void GLAPIENTRY
imm_TexParameteriv (GLenum target, GLenum pname, const GLint *params)
{
    TEX_OBJ *ptr = tex_get_current_object(target);

    if (ptr == NULL) {
	return;
    }

    FLUSH_VERTICES();

    switch (pname) {
	case GL_TEXTURE_WRAP_S:
	    ptr->wrapS = params[0];
	    break;
	case GL_TEXTURE_WRAP_T:
	    ptr->wrapT = params[0];
	    break;
	case GL_TEXTURE_MIN_FILTER:
	    ptr->minFilter = params[0];
	    break;
	case GL_TEXTURE_MAG_FILTER:
	    ptr->magFilter = params[0];
	    break;
	case GL_TEXTURE_MAX_LEVEL:
	    ptr->maxLevel = params[0];
	    break;
	case GL_TEXTURE_LOD_BIAS:
	    ptr->bias = params[0];
	    break;
	case GL_TEXTURE_BORDER_COLOR:		/* XXX Quake3 */
	case GL_TEXTURE_WRAP_R:			/* XXX UT2003 */
	case GL_TEXTURE_MAX_ANISOTROPY_EXT:	/* XXX SWKotOR2 */
	    break;
	default:
	    gl_error(NULL, GL_INVALID_VALUE, "%s: %x\n", __FUNCTION__, pname);
	    gl_assert(0);
    }

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_TexParameteri (GLenum target, GLenum pname, GLint param)
{
    imm_TexParameteriv(target, pname, &param);
}


void GLAPIENTRY
imm_TexGeniv (GLenum coord, GLenum pname, const GLint *params)
{
    FLUSH_VERTICES(); /* XXX maybe don't do it if texture gen not enabled */

    coord -= GL_S;
    switch (pname) {
	case GL_TEXTURE_GEN_MODE:
	    switch ((GLenum)params[0]) {
		case GL_OBJECT_LINEAR:
		    ctx_active_tex->genmode[coord] = TNL_GEN_OBJECT;
		    break;
		case GL_EYE_LINEAR:
		    ctx_active_tex->genmode[coord] = TNL_GEN_EYE;
		    break;
		case GL_SPHERE_MAP:
		    ctx_active_tex->genmode[coord] = TNL_GEN_SPHERE;
		    break;
		case GL_REFLECTION_MAP:
		    ctx_active_tex->genmode[coord] = TNL_GEN_REFLECT;
		    break;
		case GL_NORMAL_MAP:
		    ctx_active_tex->genmode[coord] = TNL_GEN_NORMAL;
		    break;
		default:
		    gl_assert(0);
	    }
	    break;
	case GL_OBJECT_PLANE:
	    ctx_active_tex->objplane[coord][0] = params[0];
	    ctx_active_tex->objplane[coord][1] = params[1];
	    ctx_active_tex->objplane[coord][2] = params[2];
	    ctx_active_tex->objplane[coord][3] = params[3];
	    break;
	case GL_EYE_PLANE: {
	    GLfloat4 tmp;
	    tmp[0] = params[0];
	    tmp[1] = params[1];
	    tmp[2] = params[2];
	    tmp[3] = params[3];
	    matrix_mul_vec4(ctx_active_tex->eyeplane[coord], get_imv(), tmp);
	    break;
	}
	default:
	    gl_assert(0);
    }

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_TexGeni (GLenum coord, GLenum pname, GLint param)
{
    imm_TexGeniv(coord, pname, &param);
}


void GLAPIENTRY
imm_TexGendv (GLenum coord, GLenum pname, const GLdouble *params)
{
    FLUSH_VERTICES(); /* XXX maybe don't do it if texture gen not enabled */

    coord -= GL_S;
    switch (pname) {
	case GL_TEXTURE_GEN_MODE:
	    switch ((GLenum)params[0]) {
		case GL_OBJECT_LINEAR:
		    ctx_active_tex->genmode[coord] = TNL_GEN_OBJECT;
		    break;
		case GL_EYE_LINEAR:
		    ctx_active_tex->genmode[coord] = TNL_GEN_EYE;
		    break;
		case GL_SPHERE_MAP:
		    ctx_active_tex->genmode[coord] = TNL_GEN_SPHERE;
		    break;
		case GL_REFLECTION_MAP:
		    ctx_active_tex->genmode[coord] = TNL_GEN_REFLECT;
		    break;
		case GL_NORMAL_MAP:
		    ctx_active_tex->genmode[coord] = TNL_GEN_NORMAL;
		    break;
		default:
		    gl_assert(0);
	    }
	    break;
	case GL_OBJECT_PLANE:
	    ctx_active_tex->objplane[coord][0] = params[0];
	    ctx_active_tex->objplane[coord][1] = params[1];
	    ctx_active_tex->objplane[coord][2] = params[2];
	    ctx_active_tex->objplane[coord][3] = params[3];
	    break;
	case GL_EYE_PLANE: {
	    GLfloat4 tmp;
	    tmp[0] = params[0];
	    tmp[1] = params[1];
	    tmp[2] = params[2];
	    tmp[3] = params[3];
	    matrix_mul_vec4(ctx_active_tex->eyeplane[coord], get_imv(), tmp);
	    break;
	}
	default:
	    gl_assert(0);
    }

    ctx_gl_state |= NEW_TEXTURE;
}


void GLAPIENTRY
imm_TexGend (GLenum coord, GLenum pname, GLdouble param)
{
    imm_TexGendv(coord, pname, &param);
}


void GLAPIENTRY
imm_GetTexLevelParameteriv (GLenum target, GLint level,
			    GLenum pname, GLint *params)
{
    TEX_OBJ *texObj;
    TEX_IMG *texImg;

    texObj = tex_get_current_object(target);
    texImg = &texObj->image[0][level];

    switch (pname) {
	case GL_TEXTURE_WIDTH:
	    params[0] = (texImg->width > ctx_const_max_texture_size) ? 0 : texImg->width;
	    break;
	case GL_TEXTURE_HEIGHT:
	    params[0] = (texImg->height > ctx_const_max_texture_size) ? 0 : texImg->height;
	    break;
	case GL_TEXTURE_COMPRESSED:
	    params[0] = texObj->t->isCompressed;
	    break;
	case GL_TEXTURE_GREEN_SIZE:
	    params[0] = texObj->t->greenBits;
	    break;
	case GL_TEXTURE_COMPRESSED_IMAGE_SIZE:
	    params[0] = texImg->compressedSize;
	    break;
	default:
	    gl_cry(__FILE__, __LINE__, __FUNCTION__, "%x, %d, %x\n", target, level, pname);
    }
}
