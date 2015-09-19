#include <math.h>
#include <stdlib.h>

#include "GL/gl.h"
#include "glinternal.h"
#include "main/context.h"
#include "main/matrix.h" /* for get_mvp() */
#include "util/macros.h"
#include "x86/x86.h" /* for fast_math */
#include "tnl.h"
#include "driver.h"

#define LOGGING 0
#include "log.h"


static GLbitfield tnl_render_state;
/******************************************************************************
 * vertex transform
 */
static void
tnl_vertex_transform (void)
{
    const GLfloat *mvp = get_mvp();

    /* clip coords: the data pointer _CAN_ be indexed! */
    if (tnl_extra_flags & TNL_VERTEXW_BIT) {
	matrix_mul_vec4_batch(tnl_vb.clip, mvp, (const GLfloat4 *)tnl_vb.attr[TNL_VERTEX].data, tnl_vb.len);
    } else {
	matrix_mul_vec3_batch(tnl_vb.clip, mvp, (const GLfloat4 *)tnl_vb.attr[TNL_VERTEX].data, tnl_vb.len);
    }
    if (tnl_render_state & D_NEED_VEYE) {
	const GLfloat *mv = ctx_mx_modelview_top->mat;
	if (tnl_render_state & D_NEED_VEYN) {
	    tnl_veyn_func[(tnl_extra_flags & TNL_VERTEXW_BIT) ? 1 : 0](mv);
	} else {
	    if (tnl_extra_flags & TNL_VERTEXW_BIT) {
		matrix_mul_vec4_batch(tnl_vb.veye, mv, (const GLfloat4 *)tnl_vb.attr[TNL_VERTEX].data, tnl_vb.len);
	    } else {
		matrix_mul_vec3_batch(tnl_vb.veye, mv, (const GLfloat4 *)tnl_vb.attr[TNL_VERTEX].data, tnl_vb.len);
	    }
	}
    }
    if (tnl_render_state & D_NEED_NEYE) {
	tnl_calc_neye_tab[ctx_normalize]();
    }

    if (ctx_hint_clip_volume == GL_FASTEST) {
	tnl_clipmask_tab[V_NOCLIP]();
    } else if (ctx_userclip) {
	tnl_clipmask_tab[V_USERCLIP]();
    } else {
	tnl_clipmask_tab[0]();
    }
}


/******************************************************************************
 * lighting stage
 */
static void
tnl_lighting (void)
{
    if (ctx_lighting) {
	if (ctx_light_model_twoside) {
	    /* XXX make this test a derived state? */
	    if (ctx_light_model_colctrl == GL_SEPARATE_SPECULAR_COLOR) {
		tnl_light_twoside_sepspec();
	    } else {
		tnl_light_twoside();
	    }
	} else {
	    /* XXX make this test a derived state? */
	    if (ctx_light_model_colctrl == GL_SEPARATE_SPECULAR_COLOR) {
		tnl_light_oneside_sepspec();
	    } else {
		tnl_light_oneside();
	    }
	}
    }
}


/******************************************************************************
 * fogging
 */
static void
tnl_fogging (void)
{
    int i, n;
    GLfloat d;

    if (!ctx_fog.fogging) {
	return;
    }

    if (ctx_fog.source == GL_FOG_COORDINATE) {
	GLfloat4 *fogcoord = tnl_vb.attr[TNL_FOGCOORD].data;
	const GLuint fogcoord_stride = tnl_vb.attr[TNL_FOGCOORD].stride;
	n = fogcoord_stride ? tnl_vb.len : 1;
	switch (ctx_fog.mode) {
	    case GL_LINEAR:
		if (ctx_fog.start == ctx_fog.end) {
		    d = 1.0F;
		} else {
		    d = 1.0F / (ctx_fog.end - ctx_fog.start);
		}
		for (i = 0; i < n; i++) {
		    const GLfloat z = FABS(fogcoord[0][0]);
		    GLfloat f = (ctx_fog.end - z) * d;
		    tnl_vb.fogcoord[i][0] = CLAMPF(f);
		    fogcoord += fogcoord_stride;
		}
		break;
	    case GL_EXP:
		d = ctx_fog.density;
		for (i = 0; i < n; i++) {
		    const GLfloat z = FABS(fogcoord[0][0]);
		    tnl_vb.fogcoord[i][0] = CLAMPF(EXP(- d * z));
		    fogcoord += fogcoord_stride;
		}
		break;
	    case GL_EXP2:
		d = ctx_fog.density * ctx_fog.density;
		for (i = 0; i < n; i++) {
		    const GLfloat z = FABS(fogcoord[0][0]);
		    tnl_vb.fogcoord[i][0] = CLAMPF(EXP(- d * z * z));
		    fogcoord += fogcoord_stride;
		}
		break;
	}
	tnl_vb.attr[TNL_FOGCOORD].data = tnl_vb.fogcoord;
    } else {
	/* XXX not implemented */
    }
}


/******************************************************************************
 * texture generation/transform stage
 */
static void
tnl_texgen (void)
{
    int j;

    if (tnl_render_state & D_NEED_REFL) { /* sphere_map, reflection_map */
	tnl_refl_func[(tnl_render_state & D_NEED_MVEC) ? 1 : 0]();
    }

    for (j = 0; j < TNL_MAX_TEXCOORD; j++) {
	if (ctx_texture[j].enabled && ctx_texture[j].texgen) {
	    tnl_texgen_tab[ctx_texture[j].texgen](j);

	    tnl_vb.attr[j + TNL_TEXCOORD0].data = tnl_vb.texgen[j];
	    tnl_vb.attr[j + TNL_TEXCOORD0].stride = 1;

	    if (ctx_texture[j].texgen & TNL_GEN_Q_BIT) {
		tnl_extra_flags |= TNL_TEXCOORD0W_BIT << j;
	    }
	}
    }
}


static void
tnl_texture_transform (void)
{
    int j;

    for (j = 0; j < TNL_MAX_TEXCOORD; j++) {
	if (ctx_texture[j].enabled) {
	    const MATRIX *tx = ctx_mx_texture_top[j];
	    if (!MAT_IDENTITY(tx)) {
		GLfloat4 *texcoord = tnl_vb.attr[j + TNL_TEXCOORD0].data;
		const GLuint texcoord_stride = tnl_vb.attr[j + TNL_TEXCOORD0].stride;
		if (texcoord_stride) {
		    matrix_mul_vec4_batch(tnl_vb.texcoord[j], tx->mat, (const GLfloat4 *)texcoord[0], tnl_vb.len);
		} else {
		    matrix_mul_vec4(tnl_vb.texcoord[j][0], tx->mat, texcoord[0]);
		}
		tnl_vb.attr[j + TNL_TEXCOORD0].data = tnl_vb.texcoord[j];

		tnl_extra_flags |= TNL_TEXCOORD0W_BIT << j;
	    }
	}
    }
}


/******************************************************************************
 * actual render stage
 */
static void
tnl_render (void)
{
    GLuint k;
    GLuint pass = 0;

    drv_render_init();
    drv_emitvertices(tnl_vb.len);

    do {
	for (k = 0; k < tnl_prim_num; k++) {
	    GLuint count = tnl_prim[k].count;
	    if (count) {
		GLuint start = tnl_prim[k].start;
		GLenum mode = tnl_prim[k].name;
		GLenum reduced = mode & 0xffff;
		if (!tnl_prim[k].ormask) {
		    /* directly to the driver */
		    drv_prim_tab[reduced](start, count, mode);
		} else {
		    /* go through clipping */
		    tnl_prim_tab[reduced](start, count, mode);
		}
	    }
	}
    } while (drv_multipass(++pass));

    drv_render_fini();
}


/******************************************************************************
 * pipeline
 */
void
tnl_run_pipeline (void)
{
    LOG(("%s:\n", __FUNCTION__));
    ctx_validate_state(-1U);
    tnl_render_state = tnl_light_state | tnl_texgen_state;
    if (tnl_vb.len) {
#ifdef X86
	GLuint tmp;
	START_FAST_MATH(tmp);
#endif
	tnl_vertex_transform();
	tnl_lighting();
	tnl_fogging();
	tnl_texgen();
	tnl_texture_transform();
	tnl_render();
#ifdef X86
	END_FAST_MATH(tmp);
#endif
    }
}
