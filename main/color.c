#include "GL/gl.h"

#include "glinternal.h"
#include "glapi.h"
#include "context.h"
#include "util/macros.h"
#include "tnl/tnl.h"
#include "driver.h"


void GLAPIENTRY
imm_ColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    FLUSH_VERTICES();

    ctx_color.color_mask_r = red;
    ctx_color.color_mask_g = green;
    ctx_color.color_mask_b = blue;
    ctx_color.color_mask_a = alpha;

    ctx_gl_state |= NEW_COLOR;
}


void GLAPIENTRY
imm_AlphaFunc (GLenum func, GLclampf ref)
{
    FLUSH_VERTICES();

    ctx_color.alpha_func = func;
    ctx_color.alpha_ref = ref;

    ctx_gl_state |= NEW_ALPHA;
}


void GLAPIENTRY
imm_BlendFunc (GLenum sfactor, GLenum dfactor)
{
    FLUSH_VERTICES();

    ctx_color.blend_src_rgb =
    ctx_color.blend_src_alpha = sfactor;

    ctx_color.blend_dst_rgb =
    ctx_color.blend_dst_alpha = dfactor;

    ctx_gl_state |= NEW_BLEND;
}


void GLAPIENTRY
imm_BlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB,
		       GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    FLUSH_VERTICES();

    ctx_color.blend_src_rgb = sfactorRGB;
    ctx_color.blend_src_alpha = sfactorAlpha;

    ctx_color.blend_dst_rgb = dfactorRGB;
    ctx_color.blend_dst_alpha = dfactorAlpha;

    ctx_gl_state |= NEW_BLEND;
}


void GLAPIENTRY
imm_ClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    ctx_color.clear_color[0] = CLAMPF(red);
    ctx_color.clear_color[1] = CLAMPF(green);
    ctx_color.clear_color[2] = CLAMPF(blue);
    ctx_color.clear_color[3] = CLAMPF(alpha);
}


void GLAPIENTRY
imm_DrawBuffer (GLenum mode)
{
    FLUSH_VERTICES();

    ctx_color.draw_buffer = mode;

    /* XXX maybe not in immediate mode (batch with |= NEW_...)? */
    drv_DrawBuffer(mode);
}
