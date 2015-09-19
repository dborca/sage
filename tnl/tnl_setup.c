#include <math.h>
#include <stdlib.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "main/context.h"
#include "util/macros.h"
#include "main/matrix.h"
#include "util/list.h"
#include "util/pow.h"
#include "tnl.h"


typedef GLfloat (*GEN_FUNC) (int unit, int i);

TEXGEN_FUNC tnl_texgen_tab[16];
FUNC tnl_calc_neye_tab[1<<NORM_NORMALIZE];
FUNC tnl_clipmask_tab[4];


/******************************************************************************
 * TexGen workers
 */
#define IND 0
#define TAG(x) x##_0
#include "template/tcoord.h"

#define IND 1
#define TAG(x) x##_1
#include "template/tcoord.h"

#define IND 2
#define TAG(x) x##_2
#include "template/tcoord.h"

#define IND 3
#define TAG(x) x##_3
#include "template/tcoord.h"


/******************************************************************************
 * reflection functions
 */
#define TAG(x) x
#define IND 0
#include "template/reflect.h"

#define TAG(x) x##_mvec
#define IND (D_NEED_MVEC)
#include "template/reflect.h"


FUNC tnl_refl_func[] = {
    tnl_reflect,
    tnl_reflect_mvec
};


/******************************************************************************
 * TexGen functions
 */
#define TAG(x) x##_s
#define IND (TNL_GEN_S_BIT)
#include "template/texgen.h"

#define TAG(x) x##_t
#define IND (TNL_GEN_T_BIT)
#include "template/texgen.h"

#define TAG(x) x##_st
#define IND (TNL_GEN_S_BIT|TNL_GEN_T_BIT)
#include "template/texgen.h"

#define TAG(x) x##_r
#define IND (TNL_GEN_R_BIT)
#include "template/texgen.h"

#define TAG(x) x##_sr
#define IND (TNL_GEN_S_BIT|TNL_GEN_R_BIT)
#include "template/texgen.h"

#define TAG(x) x##_tr
#define IND (TNL_GEN_T_BIT|TNL_GEN_R_BIT)
#include "template/texgen.h"

#define TAG(x) x##_str
#define IND (TNL_GEN_S_BIT|TNL_GEN_T_BIT|TNL_GEN_R_BIT)
#include "template/texgen.h"

#define TAG(x) x##_q
#define IND (TNL_GEN_Q_BIT)
#include "template/texgen.h"

#define TAG(x) x##_sq
#define IND (TNL_GEN_S_BIT|TNL_GEN_Q_BIT)
#include "template/texgen.h"

#define TAG(x) x##_tq
#define IND (TNL_GEN_T_BIT|TNL_GEN_Q_BIT)
#include "template/texgen.h"

#define TAG(x) x##_stq
#define IND (TNL_GEN_S_BIT|TNL_GEN_T_BIT|TNL_GEN_Q_BIT)
#include "template/texgen.h"

#define TAG(x) x##_rq
#define IND (TNL_GEN_R_BIT|TNL_GEN_Q_BIT)
#include "template/texgen.h"

#define TAG(x) x##_srq
#define IND (TNL_GEN_S_BIT|TNL_GEN_R_BIT|TNL_GEN_Q_BIT)
#include "template/texgen.h"

#define TAG(x) x##_trq
#define IND (TNL_GEN_T_BIT|TNL_GEN_R_BIT|TNL_GEN_Q_BIT)
#include "template/texgen.h"

#define TAG(x) x##_strq
#define IND (TNL_GEN_S_BIT|TNL_GEN_T_BIT|TNL_GEN_R_BIT|TNL_GEN_Q_BIT)
#include "template/texgen.h"


/******************************************************************************
 * Lighting functions
 */
#define LIGHT_TWOSIDE 1 /* a better name?!? */
#define LIGHT_SEPSPEC 2 /* a better name?!? */

#define TAG(x) x##_oneside
#define IND 0
#include "template/lighting.h"

#define TAG(x) x##_twoside
#define IND LIGHT_TWOSIDE
#include "template/lighting.h"

#define TAG(x) x##_oneside_sepspec
#define IND LIGHT_SEPSPEC
#include "template/lighting.h"

#define TAG(x) x##_twoside_sepspec
#define IND (LIGHT_TWOSIDE|LIGHT_SEPSPEC)
#include "template/lighting.h"


/******************************************************************************
 * Clipmask functions
 */
#define TAG(x) x
#define IND 0
#include "template/clipmask.h"

#define TAG(x) x##_user
#define IND V_USERCLIP
#include "template/clipmask.h"

#define TAG(x) x##_none
#define IND V_NOCLIP
#include "template/clipmask.h"


/******************************************************************************
 * utility functions
 */
#define TAG(x) x
#define IND 0
#include "template/normals.h"

#define TAG(x) x##_rescale
#define IND NORM_RESCALE
#include "template/normals.h"

#define TAG(x) x##_normalize
#define IND NORM_NORMALIZE
#include "template/normals.h"

#define TAG(x) x##_rescale_normalize
#define IND (NORM_RESCALE|NORM_NORMALIZE)
#include "template/normals.h"


/* XXX to test: is faster and then normalize in two loops?  we can use batch */
static void
_tnl_calc_veyn4 (const GLfloat *mv)
{
    int i;
    GLfloat4 *vi = tnl_vb.attr[TNL_VERTEX].data;
    GLfloat4 *ve = tnl_vb.veye;
    GLfloat4 *vn = tnl_vb.veyn;
    for (i = 0; i < tnl_vb.len; i++) {
	matrix_mul_vec4(ve[0], mv, vi[0]);
	NORM3(vn[0], +, ve[0]);
	vi++;
	ve++;
	vn++;
    }
}


static void
_tnl_calc_veyn3 (const GLfloat *mv)
{
    int i;
    GLfloat4 *vi = tnl_vb.attr[TNL_VERTEX].data;
    GLfloat4 *ve = tnl_vb.veye;
    GLfloat4 *vn = tnl_vb.veyn;
    for (i = 0; i < tnl_vb.len; i++) {
	matrix_mul_vec3(ve[0], mv, vi[0]);
	NORM3(vn[0], +, ve[0]);
	vi++;
	ve++;
	vn++;
    }
}


VEYN_FUNC tnl_veyn_func[] = {
    _tnl_calc_veyn3,
    _tnl_calc_veyn4
};


/******************************************************************************
 * callbacks
 */
void
tnl_setupLight (void)
{
    tnl_light_state = 0;

    if (ctx_lighting) {
	tnl_light_state |= D_NEED_NEYE;

	if (ctx_light_model_localv) {
	    tnl_light_state |= D_NEED_VEYE | D_NEED_VEYN;
	} else {
	    LIGHT *li;
	    list_foreach (li, &ctx_light_list) {
		if (li->position[3] != 0.0F) {
		    tnl_light_state |= D_NEED_VEYE;
		} else {
		    /* calculate s vector for specularity */
		    /* FIXMEEEE TODO XXX apply M, Mt?!? normalize 001?!? */
		    li->_svector[0] = li->_posnorm[0];
		    li->_svector[1] = li->_posnorm[1];
		    li->_svector[2] = li->_posnorm[2] + 1.0F;
		    NORM3(li->_svector, +, li->_svector);
		}
	    }
	}
    }
}


void
tnl_setupTexture (void)
{
    int j;

    tnl_texgen_state = 0;

    for (j = 0; j < TNL_MAX_TEXCOORD; j++) {
	if (ctx_texture[j].enabled) {
	    if (ctx_texture[j].texgen & TNL_GEN_S_BIT) {
		if (ctx_texture[j].genmode[0] == TNL_GEN_EYE) {
		    tnl_texgen_state |= D_NEED_VEYE;
		} else if (ctx_texture[j].genmode[0] == TNL_GEN_NORMAL) {
		    tnl_texgen_state |= D_NEED_VEYE | D_NEED_NEYE;
		} else if (ctx_texture[j].genmode[0] == TNL_GEN_REFLECT) {
		    tnl_texgen_state |= D_NEED_VEYE | D_NEED_NEYE | D_NEED_REFL | D_NEED_VEYN;
		} else if (ctx_texture[j].genmode[0] == TNL_GEN_SPHERE) {
		    tnl_texgen_state |= D_NEED_VEYE | D_NEED_NEYE | D_NEED_REFL | D_NEED_MVEC | D_NEED_VEYN;
		}
	    }
	    if (ctx_texture[j].texgen & TNL_GEN_T_BIT) {
		if (ctx_texture[j].genmode[1] == TNL_GEN_EYE) {
		    tnl_texgen_state |= D_NEED_VEYE;
		} else if (ctx_texture[j].genmode[1] == TNL_GEN_NORMAL) {
		    tnl_texgen_state |= D_NEED_VEYE | D_NEED_NEYE;
		} else if (ctx_texture[j].genmode[1] == TNL_GEN_REFLECT) {
		    tnl_texgen_state |= D_NEED_VEYE | D_NEED_NEYE | D_NEED_REFL | D_NEED_VEYN;
		} else if (ctx_texture[j].genmode[1] == TNL_GEN_SPHERE) {
		    tnl_texgen_state |= D_NEED_VEYE | D_NEED_NEYE | D_NEED_REFL | D_NEED_MVEC | D_NEED_VEYN;
		}
	    }
	    if (ctx_texture[j].texgen & TNL_GEN_R_BIT) {
		if (ctx_texture[j].genmode[2] == TNL_GEN_EYE) {
		    tnl_texgen_state |= D_NEED_VEYE;
		} else if (ctx_texture[j].genmode[2] == TNL_GEN_NORMAL) {
		    tnl_texgen_state |= D_NEED_VEYE | D_NEED_NEYE;
		} else if (ctx_texture[j].genmode[2] == TNL_GEN_REFLECT) {
		    tnl_texgen_state |= D_NEED_VEYE | D_NEED_NEYE | D_NEED_REFL | D_NEED_VEYN;
		}
	    }
	    if (ctx_texture[j].texgen & TNL_GEN_Q_BIT) {
		if (ctx_texture[j].genmode[3] == TNL_GEN_EYE) {
		    tnl_texgen_state |= D_NEED_VEYE;
		}
	    }
	}
    }
}


/******************************************************************************
 * public API
 */
void
tnl_setup_init (void)
{
    _tnl_texgen_init_s();
    _tnl_texgen_init_t();
    _tnl_texgen_init_st();
    _tnl_texgen_init_r();
    _tnl_texgen_init_sr();
    _tnl_texgen_init_tr();
    _tnl_texgen_init_str();
    _tnl_texgen_init_q();
    _tnl_texgen_init_sq();
    _tnl_texgen_init_tq();
    _tnl_texgen_init_stq();
    _tnl_texgen_init_rq();
    _tnl_texgen_init_srq();
    _tnl_texgen_init_trq();
    _tnl_texgen_init_strq();

    _tnl_init_neye();
    _tnl_init_neye_rescale();
    _tnl_init_neye_normalize();
    _tnl_init_neye_rescale_normalize();

    _tnl_init_clipmask();
    _tnl_init_clipmask_user();
    _tnl_init_clipmask_none();
}
