#include "GL/gl.h"

#include "glinternal.h"
#include "glapi.h"
#include "tnl/tnl.h"
#include "context.h"
#include "util/list.h"


void GLAPIENTRY
imm_Hint (GLenum target, GLenum mode)
{
    switch (target) {
	case GL_CLIP_VOLUME_CLIPPING_HINT_EXT:
	    FLUSH_VERTICES();
	    ctx_hint_clip_volume = mode;
	    ctx_gl_state |= NEW_USERCLIP; /* XXX for hw tnl? */
	    break;
    }
}


void GLAPIENTRY
imm_Enable (GLenum cap)
{
    FLUSH_VERTICES();

    switch (cap) {
	case GL_BLEND:
	    ctx_color.blending = GL_TRUE;
	    ctx_gl_state |= NEW_BLEND;
	    break;
	case GL_ALPHA_TEST:
	    ctx_color.alpha_test = GL_TRUE;
	    ctx_gl_state |= NEW_ALPHA;
	    break;
	case GL_DEPTH_TEST:
	    ctx_depth_test = GL_TRUE;
	    ctx_gl_state |= NEW_DEPTH;
	    break;
	case GL_CULL_FACE:
	    ctx_culling = GL_TRUE;
	    ctx_gl_state |= NEW_CULL;
	    break;
	case GL_LIGHT0:
	case GL_LIGHT1:
	case GL_LIGHT2:
	case GL_LIGHT3:
	case GL_LIGHT4:
	case GL_LIGHT5:
	case GL_LIGHT6:
	case GL_LIGHT7:
	    if (!ctx_light[cap - GL_LIGHT0].enabled) {
		list_append(&ctx_light_list, &ctx_light[cap - GL_LIGHT0]);
		ctx_light[cap - GL_LIGHT0].enabled = GL_TRUE;
		ctx_gl_state |= NEW_LIGHT;
	    }
	    break;
	case GL_LIGHTING:
	    ctx_lighting = GL_TRUE;
	    ctx_gl_state |= NEW_LIGHT;
	    break;
	case GL_NORMALIZE:
	    ctx_normalize |= NORM_NORMALIZE;
	    /* NEW normals, for HW TCL? */
	    break;
	case GL_RESCALE_NORMAL:
	    ctx_normalize |= NORM_RESCALE;
	    /* NEW normals, for HW TCL? */
	    break;
	case GL_COLOR_MATERIAL:
	    ctx_colormat = GL_TRUE;
	    break;
	case GL_TEXTURE_1D:
	    ctx_active_tex->object = ctx_active_tex->obj1d;
	    ctx_active_tex->enabled = TEXTURE_1D;
	    ctx_gl_state |= NEW_TEXTURE;
	    break;
	case GL_TEXTURE_2D:
	    ctx_active_tex->object = ctx_active_tex->obj2d;
	    ctx_active_tex->enabled = TEXTURE_2D;
	    ctx_gl_state |= NEW_TEXTURE;
	    break;
	case GL_TEXTURE_GEN_S:
	    ctx_active_tex->texgen |= TNL_GEN_S_BIT;
	    ctx_gl_state |= NEW_TEXTURE;
	    break;
	case GL_TEXTURE_GEN_T:
	    ctx_active_tex->texgen |= TNL_GEN_T_BIT;
	    ctx_gl_state |= NEW_TEXTURE;
	    break;
	case GL_TEXTURE_GEN_R:
	    ctx_active_tex->texgen |= TNL_GEN_R_BIT;
	    ctx_gl_state |= NEW_TEXTURE;
	    break;
	case GL_TEXTURE_GEN_Q:
	    ctx_active_tex->texgen |= TNL_GEN_Q_BIT;
	    ctx_gl_state |= NEW_TEXTURE;
	    break;
	case GL_FOG:
	    ctx_fog.fogging = GL_TRUE;
	    ctx_gl_state |= NEW_FOG;
	    break;
	case GL_SCISSOR_TEST:
	    ctx_scissor = GL_TRUE;
	    ctx_gl_state |= NEW_SCISSOR;
	    break;
	case GL_COLOR_SUM:
	    ctx_fog.secondary_color = GL_TRUE;
	    /* NEW ???, for HW TCL? */
	    break;
	case GL_CLIP_PLANE0:
	case GL_CLIP_PLANE1:
	case GL_CLIP_PLANE2:
	case GL_CLIP_PLANE3:
	case GL_CLIP_PLANE4:
	case GL_CLIP_PLANE5:
	    ctx_userclip |= 1 << (cap - GL_CLIP_PLANE0);
	    ctx_gl_state |= NEW_USERCLIP;
	    break;
	case GL_POLYGON_OFFSET_FILL:
	    ctx_polygon_offset_mode |= POLYGON_FILL;
	    /* NEW ???, for HW TCL? */
	    break;
	case GL_POLYGON_OFFSET_LINE:
	    ctx_polygon_offset_mode |= POLYGON_LINE;
	    /* NEW ???, for HW TCL? */
	    break;
	case GL_POLYGON_OFFSET_POINT:
	    ctx_polygon_offset_mode |= POLYGON_POINT;
	    /* NEW ???, for HW TCL? */
	    break;
	case GL_STENCIL_TEST:
	    ctx_stencil.enabled = GL_TRUE;
	    ctx_gl_state |= NEW_STENCIL;
	    break;
	case GL_POINT_SMOOTH:
	case GL_DITHER:
	case GL_TEXTURE_RECTANGLE_ARB:
	    break;
	default:
	    gl_cry(__FILE__, __LINE__, __FUNCTION__, "%x\n", cap);
    }
}


void GLAPIENTRY
imm_Disable (GLenum cap)
{
    FLUSH_VERTICES();

    switch (cap) {
	case GL_BLEND:
	    ctx_color.blending = GL_FALSE;
	    ctx_gl_state |= NEW_BLEND;
	    break;
	case GL_ALPHA_TEST:
	    ctx_color.alpha_test = GL_FALSE;
	    ctx_gl_state |= NEW_ALPHA;
	    break;
	case GL_DEPTH_TEST:
	    ctx_depth_test = GL_FALSE;
	    ctx_gl_state |= NEW_DEPTH;
	    break;
	case GL_CULL_FACE:
	    ctx_culling = GL_FALSE;
	    ctx_gl_state |= NEW_CULL;
	    break;
	case GL_LIGHT0:
	case GL_LIGHT1:
	case GL_LIGHT2:
	case GL_LIGHT3:
	case GL_LIGHT4:
	case GL_LIGHT5:
	case GL_LIGHT6:
	case GL_LIGHT7:
	    if (ctx_light[cap - GL_LIGHT0].enabled) {
		list_remove(&ctx_light[cap - GL_LIGHT0]);
		ctx_light[cap - GL_LIGHT0].enabled = GL_FALSE;
		ctx_gl_state |= NEW_LIGHT;
	    }
	    break;
	case GL_LIGHTING:
	    ctx_lighting = GL_FALSE;
	    ctx_gl_state |= NEW_LIGHT;
	    break;
	case GL_NORMALIZE:
	    ctx_normalize &= ~NORM_NORMALIZE;
	    /* NEW normals, for HW TCL? */
	    break;
	case GL_RESCALE_NORMAL:
	    ctx_normalize &= ~NORM_RESCALE;
	    /* NEW normals, for HW TCL? */
	    break;
	case GL_COLOR_MATERIAL:
	    ctx_colormat = GL_FALSE;
	    break;
	case GL_TEXTURE_1D:
	    ctx_active_tex->enabled = 0;
	    ctx_gl_state |= NEW_TEXTURE;
	    break;
	case GL_TEXTURE_2D:
	    ctx_active_tex->enabled = 0;
	    ctx_gl_state |= NEW_TEXTURE;
	    break;
	case GL_TEXTURE_GEN_S:
	    ctx_active_tex->texgen &= ~TNL_GEN_S_BIT;
	    ctx_gl_state |= NEW_TEXTURE;
	    break;
	case GL_TEXTURE_GEN_T:
	    ctx_active_tex->texgen &= ~TNL_GEN_T_BIT;
	    ctx_gl_state |= NEW_TEXTURE;
	    break;
	case GL_TEXTURE_GEN_R:
	    ctx_active_tex->texgen &= ~TNL_GEN_R_BIT;
	    ctx_gl_state |= NEW_TEXTURE;
	    break;
	case GL_TEXTURE_GEN_Q:
	    ctx_active_tex->texgen &= ~TNL_GEN_Q_BIT;
	    ctx_gl_state |= NEW_TEXTURE;
	    break;
	case GL_FOG:
	    ctx_fog.fogging = GL_FALSE;
	    ctx_gl_state |= NEW_FOG;
	    break;
	case GL_SCISSOR_TEST:
	    ctx_scissor = GL_FALSE;
	    ctx_gl_state |= NEW_SCISSOR;
	    break;
	case GL_COLOR_SUM:
	    ctx_fog.secondary_color = GL_FALSE;
	    /* NEW ???, for HW TCL? */
	    break;
	case GL_CLIP_PLANE0:
	case GL_CLIP_PLANE1:
	case GL_CLIP_PLANE2:
	case GL_CLIP_PLANE3:
	case GL_CLIP_PLANE4:
	case GL_CLIP_PLANE5:
	    ctx_userclip &= ~(1 << (cap - GL_CLIP_PLANE0));
	    ctx_gl_state |= NEW_USERCLIP;
	    break;
	case GL_POLYGON_OFFSET_FILL:
	    ctx_polygon_offset_mode &= ~POLYGON_FILL;
	    /* NEW ???, for HW TCL? */
	    break;
	case GL_POLYGON_OFFSET_LINE:
	    ctx_polygon_offset_mode &= ~POLYGON_LINE;
	    /* NEW ???, for HW TCL? */
	    break;
	case GL_POLYGON_OFFSET_POINT:
	    ctx_polygon_offset_mode &= ~POLYGON_POINT;
	    /* NEW ???, for HW TCL? */
	    break;
	case GL_STENCIL_TEST:
	    ctx_stencil.enabled = GL_FALSE;
	    ctx_gl_state |= NEW_STENCIL;
	    break;
	case GL_DITHER:
	case GL_TEXTURE_RECTANGLE_ARB:
	/* XXX UT2003 */
	case GL_TEXTURE_CUBE_MAP:
	    break;
	default:
	    gl_cry(__FILE__, __LINE__, __FUNCTION__, "%x\n", cap);
    }
}


GLboolean GLAPIENTRY
imm_IsEnabled (GLenum cap)
{
    switch (cap) {
	case GL_FOG:
	    return ctx_fog.fogging;
	default:
	    gl_cry(__FILE__, __LINE__, __FUNCTION__, "%x\n", cap);
	    return GL_FALSE;
    }
}
