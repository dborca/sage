#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "context.h"
#include "matrix.h"
#include "texture.h"
#include "texcodec.h"
#include "tnl/tnl.h"
#include "glapi.h"
#include "util/list.h"
#include "util/pow.h"
#include "driver.h"

#include "x86/x86.h"


void attr_init (void); /* XXX place this one in some header */
void attr_fini (void); /* XXX place this one in some header */


GLenum ctx_error_code;
GLbitfield ctx_gl_state;
GLuint ctx_depthbits;
GLuint ctx_depthmaxi;
GLfloat ctx_depthmaxf;
GLuint ctx_stencilmaxi;
GLuint ctx_redbits, ctx_greenbits, ctx_bluebits, ctx_alphabits, ctx_stencilbits;

GLboolean ctx_texcodec;


/* constants */
GLuint ctx_const_max_texture_size;
GLuint ctx_const_max_texture_units;
GLfloat ctx_const_max_lod_bias;
GLfloat ctx_const_depth_resolution;


/* extensions */
char *ctx_ext_string;


/* color */
gl_color_attrib ctx_color;


/* depth */
GLfloat ctx_clear_depth;
GLenum ctx_depth_func;
GLboolean ctx_depth_mask;
GLboolean ctx_depth_test;


/* culling */
GLboolean ctx_culling;
GLenum ctx_cull_face;
GLenum ctx_front_face;

GLbitfield ctx_polygon_offset_mode;
GLfloat ctx_polygon_offset_factor;
GLfloat ctx_polygon_offset_bias;

GLenum ctx_polygon_mode[2];


/* lighting */
GLboolean ctx_lighting;
GLboolean ctx_colormat;
GLenum ctx_colormat_face;
GLenum ctx_colormat_mode;
LIGHT ctx_light[MAX_LIGHTS];
LIGHT ctx_light_list;
GLfloat4 ctx_light_model_ambient;
GLboolean ctx_light_model_localv;
GLboolean ctx_light_model_twoside;
GLenum ctx_light_model_colctrl;
GLbitfield ctx_normalize;

GLenum ctx_shade_model;


/* texture */
TEX_OBJ *ctx_tex1d_obj;
TEX_OBJ *ctx_tex2d_obj;
TEX_OBJ *ctx_proxy1d_obj;
TEX_OBJ *ctx_proxy2d_obj;
TEX_OBJ *ctx_tex_obj_list;
GLuint ctx_tex_hinumber;
TEX_UNIT ctx_texture[MAX_TEXTURE_UNITS];
TEX_UNIT *ctx_active_tex;


/* fog */
gl_fog_attrib ctx_fog;


/* scissor */
GLboolean ctx_scissor;
GLint ctx_scissor_x1;
GLint ctx_scissor_y1;
GLint ctx_scissor_x2;
GLint ctx_scissor_y2;


/* userclip */
GLbitfield ctx_userclip;
GLfloat4 ctx_userplanes[2][MAX_CLIP_PLANES];


/* packing */
PACKING ctx_unpack, ctx_pack;
const PACKING ctx_string_packing = {
    GL_FALSE, GL_FALSE, 0, 0, 0, 0, 0, 1
};


/* hints */
GLenum ctx_hint_clip_volume;


/* buffer */
GLenum ctx_read_buffer;


/* stencil */
gl_stencil_attrib ctx_stencil;


/* helper data */
GLfloat ub_to_float[256];


int
ctx_init (int db_flag,
          int red_size, int green_size, int blue_size,
	  int alpha_size, int depth_size, int stencil_size)
{
    int i, rv;

    ctx_error_code = GL_NO_ERROR;
    ctx_gl_state = 0;
    ctx_depthbits = depth_size;
    ctx_depthmaxi = (1 << depth_size) - 1;
    ctx_depthmaxf = (GLfloat)ctx_depthmaxi;
    ctx_stencilbits = stencil_size;
    ctx_stencilmaxi = (1 << stencil_size) - 1;
    ctx_redbits = red_size;
    ctx_greenbits = green_size;
    ctx_bluebits = blue_size;
    ctx_alphabits = alpha_size;

    ctx_const_max_texture_size = 256;
    ctx_const_max_texture_units = 1;
    ctx_const_max_lod_bias = 8.0F;
    ctx_const_depth_resolution = 1.0F;

    ctx_ext_string = NULL;

    ctx_color.blend_src_rgb = GL_ONE;
    ctx_color.blend_dst_rgb = GL_ZERO;
    ctx_color.blend_src_alpha = GL_ONE;
    ctx_color.blend_dst_alpha = GL_ZERO;
    ctx_color.blending = GL_FALSE;

    ctx_color.alpha_test = GL_FALSE;
    ctx_color.alpha_func = GL_ALWAYS;
    ctx_color.alpha_ref = 1.0F;

    ctx_color.clear_color[0] = 0.0F;
    ctx_color.clear_color[1] = 0.0F;
    ctx_color.clear_color[2] = 0.0F;
    ctx_color.clear_color[3] = 0.0F;

    ctx_color.color_mask_r = GL_TRUE;
    ctx_color.color_mask_g = GL_TRUE;
    ctx_color.color_mask_b = GL_TRUE;
    ctx_color.color_mask_a = GL_TRUE;

    ctx_fog.secondary_color = GL_FALSE;

    ctx_clear_depth = 1.0F;
    ctx_depth_func = GL_LESS;
    ctx_depth_mask = GL_TRUE;
    ctx_depth_test = GL_FALSE;

    ctx_culling = GL_FALSE;
    ctx_cull_face = GL_BACK;
    ctx_front_face = GL_CCW;

    ctx_polygon_offset_mode = 0;
    ctx_polygon_offset_factor = 0.0F;
    ctx_polygon_offset_bias = 0.0F;

    ctx_polygon_mode[0] =
    ctx_polygon_mode[1] = POLYGON_FILL;

    ctx_lighting = GL_FALSE;
    ctx_colormat = GL_FALSE;
    ctx_colormat_face = GL_FRONT_AND_BACK;
    ctx_colormat_mode = GL_AMBIENT_AND_DIFFUSE;
    for (i = 0; i < MAX_LIGHTS; i++) {
	ctx_light[i].enabled = GL_FALSE;
	ctx_light[i].ambient[0] = 0.0F;
	ctx_light[i].ambient[1] = 0.0F;
	ctx_light[i].ambient[2] = 0.0F;
	ctx_light[i].ambient[3] = 1.0F;
	ctx_light[i].diffuse[0] = 0.0F;
	ctx_light[i].diffuse[1] = 0.0F;
	ctx_light[i].diffuse[2] = 0.0F;
	ctx_light[i].diffuse[3] = 1.0F;
	ctx_light[i].specular[0] = 0.0F;
	ctx_light[i].specular[1] = 0.0F;
	ctx_light[i].specular[2] = 0.0F;
	ctx_light[i].specular[3] = 1.0F;
	ctx_light[i].position[0] = 0.0F;
	ctx_light[i].position[1] = 0.0F;
	ctx_light[i].position[2] = 1.0F;
	ctx_light[i].position[3] = 0.0F;
	ctx_light[i].spot_direction[0] = 0.0F;
	ctx_light[i].spot_direction[1] = 0.0F;
	ctx_light[i].spot_direction[2] = -1.0F;
	ctx_light[i].spot_direction[3] = 0.0F;
	ctx_light[i].spot_exponent = 0.0F;
	ctx_light[i].spot_cutoff = 180.0F;
	ctx_light[i].attenuation[0] = 1.0F;
	ctx_light[i].attenuation[1] = 0.0F;
	ctx_light[i].attenuation[2] = 0.0F;
	ctx_light[i].attenuation[3] = 0.0F;

	ctx_light[i]._posnorm[0] = 0.0F;
	ctx_light[i]._posnorm[1] = 0.0F;
	ctx_light[i]._posnorm[2] = 1.0F;
	ctx_light[i]._posnorm[3] = 0.0F;
	ctx_light[i]._spot_eye_norm[0] = 0.0F;
	ctx_light[i]._spot_eye_norm[1] = 0.0F;
	ctx_light[i]._spot_eye_norm[2] = 1.0F;
	ctx_light[i]._spot_eye_norm[3] = 0.0F;
	ctx_light[i]._spot_cutcos = -1.0F;
    }
    ctx_light[0].diffuse[0] = 1.0F;
    ctx_light[0].diffuse[1] = 1.0F;
    ctx_light[0].diffuse[2] = 1.0F;
    ctx_light[0].specular[0] = 1.0F;
    ctx_light[0].specular[1] = 1.0F;
    ctx_light[0].specular[2] = 1.0F;
    list_create(&ctx_light_list);
    ctx_light_model_ambient[0] = 0.2;
    ctx_light_model_ambient[1] = 0.2;
    ctx_light_model_ambient[2] = 0.2;
    ctx_light_model_ambient[3] = 1.0;
    ctx_light_model_localv = GL_FALSE;
    ctx_light_model_twoside = GL_FALSE;
    ctx_light_model_colctrl = GL_SINGLE_COLOR;
    ctx_normalize = 0;

    ctx_shade_model = GL_SMOOTH;

    ctx_tex_hinumber = 0;
    ctx_active_tex = ctx_texture;

    ctx_tex_obj_list = malloc(2 * sizeof(void *));
    ctx_tex1d_obj = tex_fill_object(NULL);
    ctx_tex2d_obj = tex_fill_object(NULL);
    ctx_proxy1d_obj = tex_fill_object(NULL);
    ctx_proxy2d_obj = tex_fill_object(NULL);
    if (ctx_tex_obj_list == NULL ||
	ctx_tex1d_obj == NULL || ctx_tex2d_obj == NULL ||
	ctx_proxy1d_obj == NULL || ctx_proxy2d_obj == NULL) {
	ctx_fini();
	return -1;
    }
    list_create(ctx_tex_obj_list);
    ctx_tex1d_obj->target = GL_TEXTURE_1D;
    ctx_tex2d_obj->target = GL_TEXTURE_2D;
    ctx_proxy1d_obj->target = GL_PROXY_TEXTURE_1D;
    ctx_proxy2d_obj->target = GL_PROXY_TEXTURE_2D;

    memset(ctx_texture, 0, sizeof(ctx_texture));
    for (i = 0; i < MAX_TEXTURE_UNITS; i++) {
	ctx_texture[i].texenv = GL_MODULATE;
	ctx_texture[i].obj1d = ctx_tex1d_obj;
	ctx_texture[i].obj2d = ctx_tex2d_obj;
	ctx_texture[i].envcolor[0] =
	ctx_texture[i].envcolor[1] =
	ctx_texture[i].envcolor[2] =
	ctx_texture[i].envcolor[3] = 1.0F;
	ctx_texture[i].genmode[0] =
	ctx_texture[i].genmode[1] =
	ctx_texture[i].genmode[2] =
	ctx_texture[i].genmode[3] = TNL_GEN_EYE;
	ctx_texture[i].objplane[0][0] = 1.0F;
	ctx_texture[i].objplane[1][1] = 1.0F;
	ctx_texture[i].eyeplane[0][0] = 1.0F;
	ctx_texture[i].eyeplane[1][1] = 1.0F;
	ctx_texture[i].combineRGB.mode =
	ctx_texture[i].combineAlpha.mode = GL_MODULATE;
	ctx_texture[i].combineRGB.source[0] =
	ctx_texture[i].combineAlpha.source[0] = GL_TEXTURE;
	ctx_texture[i].combineRGB.source[1] =
	ctx_texture[i].combineAlpha.source[1] = GL_PREVIOUS;
	ctx_texture[i].combineRGB.source[2] =
	ctx_texture[i].combineAlpha.source[2] = GL_CONSTANT;
	ctx_texture[i].combineRGB.operand[0] =
	ctx_texture[i].combineRGB.operand[1] = GL_SRC_COLOR;
	ctx_texture[i].combineRGB.operand[2] =
	ctx_texture[i].combineAlpha.operand[0] =
	ctx_texture[i].combineAlpha.operand[1] =
	ctx_texture[i].combineAlpha.operand[2] = GL_SRC_ALPHA;
	ctx_texture[i].combineRGB.scale =
	ctx_texture[i].combineAlpha.scale = 1;
    }

    ctx_fog.fogging = GL_FALSE;
    ctx_fog.mode = GL_EXP;
    ctx_fog.density = 1.0F;
    ctx_fog.start = 0.0F;
    ctx_fog.end = 1.0F;
    ctx_fog.color[0] = 0.0F;
    ctx_fog.color[1] = 0.0F;
    ctx_fog.color[2] = 0.0F;
    ctx_fog.color[3] = 0.0F;
    ctx_fog.source = GL_FRAGMENT_DEPTH;

    ctx_scissor = GL_FALSE;

    ctx_userclip = 0;

    ctx_hint_clip_volume = GL_DONT_CARE;

    memcpy(&ctx_unpack, &ctx_string_packing, sizeof(PACKING));
    memcpy(&ctx_pack, &ctx_string_packing, sizeof(PACKING));
    ctx_unpack.alignment = 4;
    ctx_pack.alignment = 4;

    ctx_color.draw_buffer = db_flag ? GL_BACK : GL_FRONT;
    ctx_read_buffer = db_flag ? GL_BACK_LEFT : GL_FRONT_LEFT;

    ctx_stencil.enabled = GL_FALSE;
    ctx_stencil.func = GL_ALWAYS;
    ctx_stencil.ref = 0;
    ctx_stencil.valMask = ctx_stencilmaxi;
    ctx_stencil.fail = GL_KEEP;
    ctx_stencil.zfail = GL_KEEP;
    ctx_stencil.zpass = GL_KEEP;
    ctx_stencil.writeMask = ctx_stencilmaxi;
    ctx_stencil.clear = 0;

    for (i = 0; i < 256; i++) {
	/* 0x3b808081 // = 1.0F / 255.0F */
	ub_to_float[i] = (GLfloat)i / 255.0F;
    }

    list_create(&ctx_dlist_list);
    ctx_dlist_base = 0;
    ctx_dlist_mode = GL_COMPILE;

    gl_init();
    matrix_init();

    rv = tnl_init();
    if (rv != 0) {
	ctx_fini();
	return rv;
    }

#ifdef X86
    x86_init();
#endif
    pow_init();
    attr_init();
    ctx_texcodec = (tc_init() == 0);
    return 0;
}


void
ctx_fini (void)
{
    tc_fini();
    attr_fini();
    pow_fini();
    gl_fini();

    tex_delete_all();

    if (ctx_tex_obj_list != NULL) {
	free(ctx_tex_obj_list);
    }
    if (ctx_tex1d_obj != NULL) {
	drv_DeleteTexture(ctx_tex1d_obj);
	free(ctx_tex1d_obj);
    }
    if (ctx_tex2d_obj != NULL) {
	drv_DeleteTexture(ctx_tex2d_obj);
	free(ctx_tex2d_obj);
    }
    if (ctx_proxy1d_obj != NULL) {
	drv_DeleteTexture(ctx_proxy1d_obj);
	free(ctx_proxy1d_obj);
    }
    if (ctx_proxy2d_obj != NULL) {
	drv_DeleteTexture(ctx_proxy2d_obj);
	free(ctx_proxy2d_obj);
    }

    ctx_tex_obj_list =
    ctx_tex1d_obj =
    ctx_tex2d_obj =
    ctx_proxy1d_obj =
    ctx_proxy2d_obj = NULL;

    tnl_fini();

    if (ctx_ext_string != NULL) {
	free(ctx_ext_string);
	ctx_ext_string = NULL;
    }
}


static void
ctx_setupUserClip (void)
{
    int u;
    if (ctx_userclip && ctx_hint_clip_volume != GL_FASTEST) {
	int planes = ctx_userclip;
	GLfloat itp[16];
	matrix_invt(itp, ctx_mx_projection_top);
	for (u = 0; u < TNL_USERCLIP_PLANES; u++) {
	    if (planes & 1) {
		matrix_mul_vec4(ctx_userplanes[1][u], itp, ctx_userplanes[0][u]);
	    }
	    planes >>= 1;
	}
    }
}


void
ctx_validate_state (GLuint which)
{
    GLuint state = ctx_gl_state & which;
    if (state) {
	if (state & NEW_BLEND) {
	    drv_setupBlend();
	}
	if (state & NEW_ALPHA) {
	    drv_setupAlpha();
	}
	if (state & NEW_COLOR) {
	    drv_setupColor();
	}
	if (state & NEW_DEPTH) {
	    drv_setupDepth();
	}
	if (state & NEW_CULL) {
	    drv_setupCull();
	}
	if (state & NEW_LIGHT) {
	    tnl_setupLight();
	}
	if (state & NEW_TEXTURE) {
	    tnl_setupTexture();
	    drv_setupTexture();
	}
	if (state & NEW_FOG) {
	    drv_setupFog();
	}
	if (state & NEW_SCISSOR) {
	    drv_setupScissor();
	}
	if (state & NEW_USERCLIP) {
	    ctx_setupUserClip();
	}
	if (state & NEW_STENCIL) {
	    drv_setupStencil();
	}
	ctx_gl_state &= ~which;
    }
}


const char *
gl_enum (GLenum e)
{
    switch (e) {
	case GL_STACK_OVERFLOW:
	    return "GL_STACK_OVERFLOW";
	case GL_STACK_UNDERFLOW:
	    return "GL_STACK_UNDERFLOW";
	case GL_INVALID_ENUM:
	    return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:
	default:
	    return "GL_INVALID_VALUE";
    }
}


void
gl_error (void *ctx, int code, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "GLerror: %s: ", gl_enum(code));
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    ctx_error_code = code;
    (void)ctx;
}


void
gl_warning (void *ctx, int code, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "GLwarning: %s: ", gl_enum(code));
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    (void)ctx;
}


void
gl_cry (const char *file, int line, const char *func, const char *fmt, ...)
{
    FILE *f, *p;
    va_list ap;
    va_start(ap, fmt);
    p = f = fopen("sage.log", "at");
    if (f == NULL) {
	f = stderr;
    }
    fprintf(f, "%s(%d): %s: ", file, line, func);
    vfprintf(f, fmt, ap);
    if (p != NULL) {
	fclose(f);
    }
    va_end(ap);
}
