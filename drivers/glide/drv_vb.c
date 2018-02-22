#include <stdlib.h>
#include <string.h>

#include <glide.h>
#include <g3ext.h>
#include "GL/gl.h"

#include "glinternal.h"
#include "main/context.h"
#include "util/macros.h"
#include "tnl/tnl.h"
#include "main/matrix.h" /* for get_mvp() */
#include "util/alloc.h"
#include "x86/x86.h" /* XXX ? for cpu_bits */
#include "x86/cpu.h" /* XXX ? for cpu_bits */
#include "driver.h"
#include "drv.h"


#define SETUP_TEX0 (1<<0)
#define SETUP_TEX1 (1<<1)
#define SETUP_PTX0 (1<<2)
#define SETUP_PTX1 (1<<3)
#define SETUP_FOGC (1<<4)
#define SETUP_SPEC (1<<5)
#define SETUP_MAX  (1<<6)
#define SETUP_PSIZ (1<<6)


static struct {
    GLbitfield flags;
    void (*emit_func) (int n);
    void (*copypv_func) (int vdst, int vsrc);
    void (*interp_func) (float t, int vdst, int vout, int vin);
} vb_handler[SETUP_MAX];

static GLbitfield stw_flags;
static GLbitfield render_flags;


static void (*tmp_copypv) (int vdst, int vsrc);
static void (*tmp_interp) (float t, int vdst, int vout, int vin);

static void copypv_back (int vdst, int vsrc);
static void copypv_back_spec (int vdst, int vsrc);
static void interp_back (float t, int vdst, int vout, int vin);
static void interp_back_spec (float t, int vdst, int vout, int vin);


static void
setupHints (GrHint_t hintType, FxU32 hintMask)
{
    switch (hintType) {
	case GR_HINT_STWHINT:
	    if (hintMask & GR_STWHINT_W_DIFF_TMU0)
		grVertexLayout(GR_PARAM_Q0, GR_VERTEX_OOW_TMU0_OFFSET << 2,
			       GR_PARAM_ENABLE);
	    else
		grVertexLayout(GR_PARAM_Q0, GR_VERTEX_OOW_TMU0_OFFSET << 2,
			       GR_PARAM_DISABLE);

	    if (hintMask & GR_STWHINT_ST_DIFF_TMU1)
		grVertexLayout(GR_PARAM_ST1, GR_VERTEX_SOW_TMU1_OFFSET << 2,
			       GR_PARAM_ENABLE);
	    else
		grVertexLayout(GR_PARAM_ST1, GR_VERTEX_SOW_TMU1_OFFSET << 2,
			       GR_PARAM_DISABLE);

	    if (hintMask & GR_STWHINT_W_DIFF_TMU1)
		grVertexLayout(GR_PARAM_Q1, GR_VERTEX_OOW_TMU1_OFFSET << 2,
			       GR_PARAM_ENABLE);
	    else
		grVertexLayout(GR_PARAM_Q1, GR_VERTEX_OOW_TMU1_OFFSET << 2,
			       GR_PARAM_DISABLE);
    }
}


void
drv_render_init (void)
{
    render_flags = 0;

    if (ctx_texture[tmu0_source].enabled) {
	render_flags |= SETUP_TEX0;
	if (tnl_extra_flags & (TNL_TEXCOORD0W_BIT << tmu0_source)) {
	    render_flags |= SETUP_PTX0;
	}
    }
    if (ctx_texture[tmu1_source].enabled) {
	render_flags |= SETUP_TEX1;
	if (tnl_extra_flags & (TNL_TEXCOORD0W_BIT << tmu1_source)) {
	    render_flags |= SETUP_PTX1;
	}
    }
    if (ctx_fog.fogging && ctx_fog.source == GL_FOG_COORDINATE) {
	render_flags |= SETUP_FOGC;
    }
    if (NEED_SECONDARY_COLOR()) {
	render_flags |= SETUP_SPEC;
	drv_multipass = drv_multipass_colorsum;
    }

    if (stw_flags != vb_handler[render_flags].flags) {
	stw_flags = vb_handler[render_flags].flags;
	setupHints(GR_HINT_STWHINT, stw_flags);
    }

    drv_emitvertices = vb_handler[render_flags].emit_func;
    drv_copypv = vb_handler[render_flags].copypv_func;
    drv_interp = vb_handler[render_flags].interp_func;

    if (ctx_lighting && ctx_light_model_twoside) {
	tmp_copypv = drv_copypv;
	tmp_interp = drv_interp;
	if (NEED_SECONDARY_COLOR()) {
	    drv_copypv = copypv_back_spec;
	    drv_interp = interp_back_spec;
	} else {
	    drv_copypv = copypv_back;
	    drv_interp = interp_back;
	}
    }

    setup_tri_pointers();
}


void
drv_render_fini (void)
{
}


static void
copypv (int vdst, int vsrc)
{
    const GrVertex *src = &vb[vsrc];
    GrVertex *dst = &vb[vdst];
#if FX_PACKEDCOLOR
    *(GLuint *)dst->pargb = *(GLuint *)src->pargb;
#else  /* !FX_PACKEDCOLOR */
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
    dst->a = src->a;
#endif /* !FX_PACKEDCOLOR */
}


static void
copypv_spec (int vdst, int vsrc)
{
    const GrVertex *src = &vb[vsrc];
    GrVertex *dst = &vb[vdst];
#if FX_PACKEDCOLOR
    *(GLuint *)dst->pargb = *(GLuint *)src->pargb;
    *(GLuint *)dst->pspec = *(GLuint *)src->pspec;
#else  /* !FX_PACKEDCOLOR */
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
    dst->a = src->a;
    dst->r1 = src->r1;
    dst->g1 = src->g1;
    dst->b1 = src->b1;
#endif /* !FX_PACKEDCOLOR */
}


static void
copypv_back (int vdst, int vsrc)
{
    GLfloat4 *src;
    GLfloat4 *dst;

    src = &tnl_vb.c0_back[vsrc];
    dst = &tnl_vb.c0_back[vdst];

    dst[0][0] = src[0][0];
    dst[0][1] = src[0][1];
    dst[0][2] = src[0][2];
    dst[0][3] = src[0][3];

    tmp_copypv(vdst, vsrc);
}


static void
interp_back (float t, int vdst, int vout, int vin)
{
    GLfloat4 *dst;
    GLfloat4 *in;
    GLfloat4 *out;

    dst = &tnl_vb.c0_back[vdst];
    in  = &tnl_vb.c0_back[vin];
    out = &tnl_vb.c0_back[vout];

    INTERP_F(t, dst[0][0], out[0][0], in[0][0]);
    INTERP_F(t, dst[0][1], out[0][1], in[0][1]);
    INTERP_F(t, dst[0][2], out[0][2], in[0][2]);
    INTERP_F(t, dst[0][3], out[0][3], in[0][3]);

    tmp_interp(t, vdst, vout, vin);
}


static void
copypv_back_spec (int vdst, int vsrc)
{
    GLfloat4 *src;
    GLfloat4 *dst;

    src = &tnl_vb.c0_back[vsrc];
    dst = &tnl_vb.c0_back[vdst];

    dst[0][0] = src[0][0];
    dst[0][1] = src[0][1];
    dst[0][2] = src[0][2];
    dst[0][3] = src[0][3];

    src = &tnl_vb.c1_back[vsrc];
    dst = &tnl_vb.c1_back[vdst];

    dst[0][0] = src[0][0];
    dst[0][1] = src[0][1];
    dst[0][2] = src[0][2];

    tmp_copypv(vdst, vsrc);
}


static void
interp_back_spec (float t, int vdst, int vout, int vin)
{
    GLfloat4 *dst;
    GLfloat4 *in;
    GLfloat4 *out;

    dst = &tnl_vb.c0_back[vdst];
    in  = &tnl_vb.c0_back[vin];
    out = &tnl_vb.c0_back[vout];

    INTERP_F(t, dst[0][0], out[0][0], in[0][0]);
    INTERP_F(t, dst[0][1], out[0][1], in[0][1]);
    INTERP_F(t, dst[0][2], out[0][2], in[0][2]);
    INTERP_F(t, dst[0][3], out[0][3], in[0][3]);

    dst = &tnl_vb.c1_back[vdst];
    in  = &tnl_vb.c1_back[vin];
    out = &tnl_vb.c1_back[vout];

    INTERP_F(t, dst[0][0], out[0][0], in[0][0]);
    INTERP_F(t, dst[0][1], out[0][1], in[0][1]);
    INTERP_F(t, dst[0][2], out[0][2], in[0][2]);

    tmp_interp(t, vdst, vout, vin);
}


#define TAG(x) x##_g
#define IND (0)
#include "drv_vbtmp.h"

#define TAG(x) x##_t0
#define IND (SETUP_TEX0)
#include "drv_vbtmp.h"

#define TAG(x) x##_q0
#define IND (SETUP_TEX0|SETUP_PTX0)
#include "drv_vbtmp.h"

#define TAG(x) x##_t1
#define IND (SETUP_TEX1)
#include "drv_vbtmp.h"

#define TAG(x) x##_q1
#define IND (SETUP_TEX1|SETUP_PTX1)
#include "drv_vbtmp.h"

#define TAG(x) x##_t0t1
#define IND (SETUP_TEX0|SETUP_TEX1)
#include "drv_vbtmp.h"

#define TAG(x) x##_q0t1
#define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1)
#include "drv_vbtmp.h"

#define TAG(x) x##_t0q1
#define IND (SETUP_TEX0|SETUP_TEX1|SETUP_PTX1)
#include "drv_vbtmp.h"

#define TAG(x) x##_q0q1
#define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1)
#include "drv_vbtmp.h"

#define TAG(x) x##_f
#define IND (SETUP_FOGC)
#include "drv_vbtmp.h"

#define TAG(x) x##_t0f
#define IND (SETUP_TEX0|SETUP_FOGC)
#include "drv_vbtmp.h"

#define TAG(x) x##_q0f
#define IND (SETUP_TEX0|SETUP_PTX0|SETUP_FOGC)
#include "drv_vbtmp.h"

#define TAG(x) x##_t1f
#define IND (SETUP_TEX1|SETUP_FOGC)
#include "drv_vbtmp.h"

#define TAG(x) x##_q1f
#define IND (SETUP_TEX1|SETUP_PTX1|SETUP_FOGC)
#include "drv_vbtmp.h"

#define TAG(x) x##_t0t1f
#define IND (SETUP_TEX0|SETUP_TEX1|SETUP_FOGC)
#include "drv_vbtmp.h"

#define TAG(x) x##_q0t1f
#define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_FOGC)
#include "drv_vbtmp.h"

#define TAG(x) x##_t0q1f
#define IND (SETUP_TEX0|SETUP_TEX1|SETUP_PTX1|SETUP_FOGC)
#include "drv_vbtmp.h"

#define TAG(x) x##_q0q1f
#define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1|SETUP_FOGC)
#include "drv_vbtmp.h"


#define TAG(x) x##_s
#define IND (SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_t0s
#define IND (SETUP_TEX0|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_q0s
#define IND (SETUP_TEX0|SETUP_PTX0|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_t1s
#define IND (SETUP_TEX1|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_q1s
#define IND (SETUP_TEX1|SETUP_PTX1|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_t0t1s
#define IND (SETUP_TEX0|SETUP_TEX1|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_q0t1s
#define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_t0q1s
#define IND (SETUP_TEX0|SETUP_TEX1|SETUP_PTX1|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_q0q1s
#define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_fs
#define IND (SETUP_FOGC|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_t0fs
#define IND (SETUP_TEX0|SETUP_FOGC|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_q0fs
#define IND (SETUP_TEX0|SETUP_PTX0|SETUP_FOGC|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_t1fs
#define IND (SETUP_TEX1|SETUP_FOGC|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_q1fs
#define IND (SETUP_TEX1|SETUP_PTX1|SETUP_FOGC|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_t0t1fs
#define IND (SETUP_TEX0|SETUP_TEX1|SETUP_FOGC|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_q0t1fs
#define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_FOGC|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_t0q1fs
#define IND (SETUP_TEX0|SETUP_TEX1|SETUP_PTX1|SETUP_FOGC|SETUP_SPEC)
#include "drv_vbtmp.h"

#define TAG(x) x##_q0q1fs
#define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1|SETUP_FOGC|SETUP_SPEC)
#include "drv_vbtmp.h"


int
vb_init (void)
{
    vb = malloc_a((tnl_vb.max + TNL_CLIPPED_VERTS) * sizeof(GrVertex), 4);
    if (vb == NULL) {
	return -1;
    }

    init_g();
    init_t0();
    init_q0();
    init_t1();
    init_q1();
    init_t0t1();
    init_q0t1();
    init_t0q1();
    init_q0q1();
    init_f();
    init_t0f();
    init_q0f();
    init_t1f();
    init_q1f();
    init_t0t1f();
    init_q0t1f();
    init_t0q1f();
    init_q0q1f();

    init_s();
    init_t0s();
    init_q0s();
    init_t1s();
    init_q1s();
    init_t0t1s();
    init_q0t1s();
    init_t0q1s();
    init_q0q1s();
    init_fs();
    init_t0fs();
    init_q0fs();
    init_t1fs();
    init_q1fs();
    init_t0t1fs();
    init_q0t1fs();
    init_t0q1fs();
    init_q0q1fs();

#ifdef X86
    if ((x86_cpu_bits & _CPU_FEATURE_SSE) && x86_enable_sse) {
	extern void sse_emitvertices_g (int n);
	extern void sse_emitvertices_t0 (int n);
	extern void sse_emitvertices_q0 (int n);
	extern void sse_emitvertices_t1 (int n);
	extern void sse_emitvertices_q1 (int n);
	extern void sse_emitvertices_t0t1 (int n);
	extern void sse_emitvertices_q0t1 (int n);
	extern void sse_emitvertices_t0q1 (int n);
	extern void sse_emitvertices_q0q1 (int n);
	extern void sse_emitvertices_f (int n);
	extern void sse_emitvertices_t0f (int n);
	extern void sse_emitvertices_q0f (int n);
	extern void sse_emitvertices_t1f (int n);
	extern void sse_emitvertices_q1f (int n);
	extern void sse_emitvertices_t0t1f (int n);
	extern void sse_emitvertices_q0t1f (int n);
	extern void sse_emitvertices_t0q1f (int n);
	extern void sse_emitvertices_q0q1f (int n);
	extern void sse_emitvertices_s (int n);
	extern void sse_emitvertices_t0s (int n);
	extern void sse_emitvertices_q0s (int n);
	extern void sse_emitvertices_t1s (int n);
	extern void sse_emitvertices_q1s (int n);
	extern void sse_emitvertices_t0t1s (int n);
	extern void sse_emitvertices_q0t1s (int n);
	extern void sse_emitvertices_t0q1s (int n);
	extern void sse_emitvertices_q0q1s (int n);
	extern void sse_emitvertices_fs (int n);
	extern void sse_emitvertices_t0fs (int n);
	extern void sse_emitvertices_q0fs (int n);
	extern void sse_emitvertices_t1fs (int n);
	extern void sse_emitvertices_q1fs (int n);
	extern void sse_emitvertices_t0t1fs (int n);
	extern void sse_emitvertices_q0t1fs (int n);
	extern void sse_emitvertices_t0q1fs (int n);
	extern void sse_emitvertices_q0q1fs (int n);
	vb_handler[0].emit_func = sse_emitvertices_g;
	vb_handler[SETUP_TEX0].emit_func = sse_emitvertices_t0;
	vb_handler[SETUP_TEX0|SETUP_PTX0].emit_func = sse_emitvertices_q0;
	vb_handler[SETUP_TEX1].emit_func = sse_emitvertices_t1;
	vb_handler[SETUP_TEX1|SETUP_PTX1].emit_func = sse_emitvertices_q1;
	vb_handler[SETUP_TEX0|SETUP_TEX1].emit_func = sse_emitvertices_t0t1;
	vb_handler[SETUP_TEX0|SETUP_PTX0|SETUP_TEX1].emit_func = sse_emitvertices_q0t1;
	vb_handler[SETUP_TEX0|SETUP_TEX1|SETUP_PTX1].emit_func = sse_emitvertices_t0q1;
	vb_handler[SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1].emit_func = sse_emitvertices_q0q1;
    }
    else
    if ((x86_cpu_bits & _CPU_FEATURE_3DNOW) && x86_enable_3dnow) {
	extern void k3d_emitvertices_g (int n);
	extern void k3d_emitvertices_t0 (int n);
	extern void k3d_emitvertices_q0 (int n);
	extern void k3d_emitvertices_t1 (int n);
	extern void k3d_emitvertices_q1 (int n);
	extern void k3d_emitvertices_t0t1 (int n);
	extern void k3d_emitvertices_q0t1 (int n);
	extern void k3d_emitvertices_t0q1 (int n);
	extern void k3d_emitvertices_q0q1 (int n);
	extern void k3d_emitvertices_f (int n);
	extern void k3d_emitvertices_t0f (int n);
	extern void k3d_emitvertices_q0f (int n);
	extern void k3d_emitvertices_t1f (int n);
	extern void k3d_emitvertices_q1f (int n);
	extern void k3d_emitvertices_t0t1f (int n);
	extern void k3d_emitvertices_q0t1f (int n);
	extern void k3d_emitvertices_t0q1f (int n);
	extern void k3d_emitvertices_q0q1f (int n);
	extern void k3d_emitvertices_s (int n);
	extern void k3d_emitvertices_t0s (int n);
	extern void k3d_emitvertices_q0s (int n);
	extern void k3d_emitvertices_t1s (int n);
	extern void k3d_emitvertices_q1s (int n);
	extern void k3d_emitvertices_t0t1s (int n);
	extern void k3d_emitvertices_q0t1s (int n);
	extern void k3d_emitvertices_t0q1s (int n);
	extern void k3d_emitvertices_q0q1s (int n);
	extern void k3d_emitvertices_fs (int n);
	extern void k3d_emitvertices_t0fs (int n);
	extern void k3d_emitvertices_q0fs (int n);
	extern void k3d_emitvertices_t1fs (int n);
	extern void k3d_emitvertices_q1fs (int n);
	extern void k3d_emitvertices_t0t1fs (int n);
	extern void k3d_emitvertices_q0t1fs (int n);
	extern void k3d_emitvertices_t0q1fs (int n);
	extern void k3d_emitvertices_q0q1fs (int n);
	vb_handler[0].emit_func = k3d_emitvertices_g;
	vb_handler[SETUP_TEX0].emit_func = k3d_emitvertices_t0;
	vb_handler[SETUP_TEX0|SETUP_PTX0].emit_func = k3d_emitvertices_q0;
	vb_handler[SETUP_TEX1].emit_func = k3d_emitvertices_t1;
	vb_handler[SETUP_TEX1|SETUP_PTX1].emit_func = k3d_emitvertices_q1;
	vb_handler[SETUP_TEX0|SETUP_TEX1].emit_func = k3d_emitvertices_t0t1;
	vb_handler[SETUP_TEX0|SETUP_PTX0|SETUP_TEX1].emit_func = k3d_emitvertices_q0t1;
	vb_handler[SETUP_TEX0|SETUP_TEX1|SETUP_PTX1].emit_func = k3d_emitvertices_t0q1;
	vb_handler[SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1].emit_func = k3d_emitvertices_q0q1;
    }
#endif

    return 0;
}


int
drv_multipass_none (int pass)
{
    return 0;
}


/* XXX does not work with blending */
int
drv_multipass_colorsum (int pass)
{
    static GLbitfield t0, t1;

    static GLboolean blending;
    static GLenum blend_src_rgb;
    static GLenum blend_src_alpha;
    static GLenum blend_dst_rgb;
    static GLenum blend_dst_alpha;

    static GLboolean depth_test;
    static GLenum depth_func;
    static GLboolean depth_mask;

    switch (pass) {
	case 1: /* first pass: the TEXTURED triangles are drawn */
	    /* save per pass data */
	    t0 = ctx_texture[0].enabled;
	    t1 = ctx_texture[1].enabled;
	    blending = ctx_color.blending;
	    blend_src_rgb = ctx_color.blend_src_rgb;
	    blend_src_alpha = ctx_color.blend_src_alpha;
	    blend_dst_rgb = ctx_color.blend_dst_rgb;
	    blend_dst_alpha = ctx_color.blend_dst_alpha;
	    depth_test = ctx_depth_test;
	    depth_func = ctx_depth_func;
	    depth_mask = ctx_depth_mask;
	    /* turn off texturing */
	    ctx_texture[0].enabled = 0;
	    ctx_texture[1].enabled = 0;
	    /* SUM the colors */
	    ctx_color.blending = GL_TRUE;
	    ctx_color.blend_src_rgb = GL_ONE;
	    ctx_color.blend_dst_rgb = GL_ONE;
	    ctx_color.blend_src_alpha = GL_ONE;
	    ctx_color.blend_dst_alpha = GL_ZERO;
	    /* make sure we draw only where we want to */
	    if (ctx_depth_test) {
		switch (ctx_depth_func) {
		    default:
			ctx_depth_func = GL_EQUAL;
		    case GL_NEVER:
		    case GL_ALWAYS:
			;
		}
		ctx_depth_mask = GL_FALSE;
	    }
	    /* switch to secondary colors */
#if FX_PACKEDCOLOR
	    grVertexLayout(GR_PARAM_PARGB, GR_VERTEX_PSPEC_OFFSET << 2, GR_PARAM_ENABLE);
#else  /* !FX_PACKEDCOLOR */
	    grVertexLayout(GR_PARAM_RGB, GR_VERTEX_SPEC_OFFSET << 2, GR_PARAM_ENABLE);
#endif /* !FX_PACKEDCOLOR */
	    break;
	case 2: /* 2nd pass (last): the secondary color is summed over texture */
	    drv_multipass = drv_multipass_none;
	    /* restore original state */
	    ctx_texture[0].enabled = t0;
	    ctx_texture[1].enabled = t1;
	    ctx_color.blending = blending;
	    ctx_color.blend_src_rgb = blend_src_rgb;
	    ctx_color.blend_src_alpha = blend_src_alpha;
	    ctx_color.blend_dst_rgb = blend_dst_rgb;
	    ctx_color.blend_dst_alpha = blend_dst_alpha;
	    ctx_depth_test = depth_test;
	    ctx_depth_func = depth_func;
	    ctx_depth_mask = depth_mask;
	    /* revert to primary colors */
#if FX_PACKEDCOLOR
	    grVertexLayout(GR_PARAM_PARGB, GR_VERTEX_PARGB_OFFSET << 2, GR_PARAM_ENABLE);
#else  /* !FX_PACKEDCOLOR */
	    grVertexLayout(GR_PARAM_RGB, GR_VERTEX_RGB_OFFSET << 2, GR_PARAM_ENABLE);
#endif /* !FX_PACKEDCOLOR */
	    break;
    }

    drv_setupTexture();
    drv_setupBlend();
    drv_setupDepth();
    return (pass == 1);
}
