#include <string.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "glapi.h"
#include "context.h"
#include "matrix.h"
#include "tnl/tnl.h"
#include "driver.h"


GLenum GLAPIENTRY
imm_GetError (void)
{
    GLenum err = ctx_error_code;
    ctx_error_code = GL_NO_ERROR;
    return err;
}


const GLubyte * GLAPIENTRY
imm_GetString (GLenum name)
{
    const char *p;
    p = drv_GetString(name);
    if (p != NULL) {
	return (const GLubyte *)p;
    }
    switch (name) {
	case GL_VENDOR:
	    return (const GLubyte *)"Daniel Borca";
	case GL_RENDERER:
	    return (const GLubyte *)"XXX";
	case GL_VERSION:
	    return (const GLubyte *)"1.1";
	case GL_EXTENSIONS:
	    return (const GLubyte *)ext_create_string();
    }
    gl_error(NULL, GL_INVALID_ENUM, "bad request in glGetString()\n");
    return NULL;
}


void GLAPIENTRY
imm_GetIntegerv (GLenum pname, GLint *params)
{
    switch (pname) {
	case GL_MATRIX_MODE:
	    params[0] = ctx_mx_mode;
	    break;
	case GL_SUBPIXEL_BITS:
	    params[0] = SUB_PIXEL_BITS;
	    break;
	case GL_AUX_BUFFERS:
	    params[0] = 0;
	    break;
	case GL_MAX_TEXTURE_STACK_DEPTH:
	    params[0] = MAX_TEXTURE;
	    break;
	case GL_MAX_PROJECTION_STACK_DEPTH:
	    params[0] = MAX_PROJECTION;
	    break;
	case GL_MAX_MODELVIEW_STACK_DEPTH:
	    params[0] = MAX_MODELVIEW;
	    break;
	case GL_MAX_CLIP_PLANES:
	    params[0] = TNL_USERCLIP_PLANES;
	    break;
	case GL_MAX_LIGHTS:
	    params[0] = MAX_LIGHTS;
	    break;
	case GL_MAX_TEXTURE_SIZE:
	    params[0] = ctx_const_max_texture_size;
	    break;
	case GL_MAX_TEXTURE_UNITS:
	    params[0] = ctx_const_max_texture_units;
	    break;
	case GL_UNPACK_ALIGNMENT:
	    params[0] = ctx_unpack.alignment;
	    break;
	case GL_UNPACK_LSB_FIRST:
	    params[0] = ctx_unpack.lsb_first;
	    break;
	case GL_UNPACK_ROW_LENGTH:
	    params[0] = ctx_unpack.row_length;
	    break;
	case GL_UNPACK_SKIP_PIXELS:
	    params[0] = ctx_unpack.skip_pixels;
	    break;
	case GL_UNPACK_SKIP_ROWS:
	    params[0] = ctx_unpack.skip_rows;
	    break;
	case GL_UNPACK_SWAP_BYTES:
	    params[0] = ctx_unpack.swap_bytes;
	    break;
	case GL_PACK_ALIGNMENT:
	    params[0] = ctx_pack.alignment;
	    break;
	case GL_PACK_LSB_FIRST:
	    params[0] = ctx_pack.lsb_first;
	    break;
	case GL_PACK_ROW_LENGTH:
	    params[0] = ctx_pack.row_length;
	    break;
	case GL_PACK_SKIP_PIXELS:
	    params[0] = ctx_pack.skip_pixels;
	    break;
	case GL_PACK_SKIP_ROWS:
	    params[0] = ctx_pack.skip_rows;
	    break;
	case GL_PACK_SWAP_BYTES:
	    params[0] = ctx_pack.swap_bytes;
	    break;
	case GL_STENCIL_BITS:
	    params[0] = 0;
	    break;
	case GL_DEPTH_BITS:
	    params[0] = ctx_depthbits;
	    break;
	case GL_ALPHA_BITS:
	    params[0] = ctx_alphabits;
	    break;
	case GL_RED_BITS:
	    params[0] = ctx_redbits;
	    break;
	case GL_GREEN_BITS:
	    params[0] = ctx_greenbits;
	    break;
	case GL_BLUE_BITS:
	    params[0] = ctx_bluebits;
	    break;
	case GL_VIEWPORT:
	    /* XXX cache them as ints? */
	    params[0] = ctx_mx_viewport.mat[12] - ctx_mx_viewport.mat[0];
	    params[1] = ctx_mx_viewport.mat[13] - ctx_mx_viewport.mat[5];
	    params[2] = 2.0F * ctx_mx_viewport.mat[0];
	    params[3] = 2.0F * ctx_mx_viewport.mat[5];
	    break;
	case GL_LIGHTING:
	    params[0] = ctx_lighting;
	    break;
	case GL_MAX_TEXTURE_COORDS_ARB:
	case GL_MAX_TEXTURE_IMAGE_UNITS_ARB:
	    /* XXX this is correct only on Voodoos? req'd by Doom3 */
	    params[0] = ctx_const_max_texture_units;
	    break;
	default:
	    gl_error(NULL, GL_INVALID_ENUM, "%s: %x\n", __FUNCTION__, pname);
    }
}


void GLAPIENTRY
imm_GetFloatv (GLenum pname, GLfloat *params)
{
    switch (pname) {
	case GL_MODELVIEW_MATRIX:
	    memcpy(params, ctx_mx_modelview_top->mat, 16 * sizeof(GLfloat));
	    break;
	case GL_PROJECTION_MATRIX:
	    memcpy(params, ctx_mx_projection_top->mat, 16 * sizeof(GLfloat));
	    break;
	case GL_MAX_TEXTURE_LOD_BIAS:
	    params[0] = ctx_const_max_lod_bias;
	    break;
	case GL_COLOR_CLEAR_VALUE:
	    params[0] = ctx_color.clear_color[0];
	    params[1] = ctx_color.clear_color[1];
	    params[2] = ctx_color.clear_color[2];
	    params[3] = ctx_color.clear_color[3];
	    break;
	default:
	    gl_error(NULL, GL_INVALID_ENUM, "%s: %x\n", __FUNCTION__, pname);
    }
}


void GLAPIENTRY
imm_GetBooleanv (GLenum pname, GLboolean *params)
{
    switch (pname) {
	case GL_INDEX_MODE:
	    params[0] = GL_FALSE;
	    break;
	case GL_RGBA_MODE:
	    params[0] = GL_TRUE;
	    break;
	case GL_LIGHTING:
	    params[0] = ctx_lighting;
	    break;
	case GL_FOG:
	    params[0] = ctx_fog.fogging;
	    break;
	case GL_DEPTH_WRITEMASK:
	    params[0] = ctx_depth_mask;
	    break;
	case GL_TEXTURE_2D:
	    params[0] = (ctx_active_tex->enabled == TEXTURE_2D);
	    break;
	default:
	    gl_error(NULL, GL_INVALID_ENUM, "%s: %x\n", __FUNCTION__, pname);
    }
}


void GLAPIENTRY
imm_GetDoublev (GLenum pname, GLdouble *params)
{
    int i;

    switch (pname) {
	case GL_PROJECTION_MATRIX:
	    for (i = 0; i < 16; i++) {
		params[i] = ctx_mx_projection_top->mat[i];
	    }
	    break;
	case GL_MODELVIEW_MATRIX:
	    for (i = 0; i < 16; i++) {
		params[i] = ctx_mx_modelview_top->mat[i];
	    }
	    break;
	default:
	    gl_error(NULL, GL_INVALID_ENUM, "%s: %x\n", __FUNCTION__, pname);
    }
}
