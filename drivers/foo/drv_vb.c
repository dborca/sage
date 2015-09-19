#include <stdio.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "util/macros.h"
#include "main/context.h"
#include "tnl/tnl.h"
#include "main/matrix.h" /* for get_mvp() */
#include "driver.h"
#include "drv.h"


static void d_emitvertices (int n);
static void d_copypv (int vdst, int vsrc);
static void d_interp (float t, int vdst, int vout, int vin);


void
drv_render_init (void)
{
    drv_emitvertices = d_emitvertices;
    drv_copypv = d_copypv;
    drv_interp = d_interp;

    setup_tri_pointers();
}


void
drv_render_fini (void)
{
}


static void
d_emitvertices (int n)
{
    int i;
    SWvertex *vertex = vb;
    const GLfloat *view = ctx_mx_viewport.mat;
    for (i = 0; i < n; i++) {
	if (tnl_vb.clipmask[i] == 0) {
	    vertex->x   = tnl_vb.ndc[i][0] * view[0]  + view[12];
	    vertex->y   = tnl_vb.ndc[i][1] * view[5]  + view[13];
	    vertex->z   = tnl_vb.ndc[i][2] * view[10] + view[14];
	    vertex->oow = tnl_vb.ndc[i][3];
	} else {
	    vertex->oow = 1.0F;
	}

	vertex->r = tnl_vb.attr[TNL_COLOR0].data[i][0];
	vertex->g = tnl_vb.attr[TNL_COLOR0].data[i][1];
	vertex->b = tnl_vb.attr[TNL_COLOR0].data[i][2];
	vertex->a = tnl_vb.attr[TNL_COLOR0].data[i][3];
	printf("Color0: %f, %f, %f, %f\n",
	    vertex->r, vertex->g, vertex->b, vertex->a);
	printf("Vertex: %f, %f, %f, %f\n",
	    tnl_vb.attr[TNL_VERTEX].data[i][0],
	    tnl_vb.attr[TNL_VERTEX].data[i][1],
	    tnl_vb.attr[TNL_VERTEX].data[i][2],
	    tnl_vb.attr[TNL_VERTEX].data[i][3]);
	printf("\n");
	vertex++;
    }
}


static void
d_copypv (int vdst, int vsrc)
{
    const SWvertex *src = &vb[vsrc];
    SWvertex *dst = &vb[vdst];
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
    dst->a = src->a;
}


static void
d_interp (float t, int vdst, int vout, int vin)
{
    const GLfloat *view = ctx_mx_viewport.mat;
    GLfloat4 *clip = &tnl_vb.clip[vdst];
    SWvertex *dst = &vb[vdst];
    SWvertex *in  = &vb[vin];
    SWvertex *out = &vb[vout];
    const GLfloat oow = (clip[0][3] == 0.0F) ? 1.0F : (1.0F / clip[0][3]);
    const GLfloat wout = oow / out->oow;
    const GLfloat win = oow / in->oow;

    dst->x   = clip[0][0] * oow * view[0]  + view[12];
    dst->y   = clip[0][1] * oow * view[5]  + view[13];
    dst->z   = clip[0][2] * oow * view[10] + view[14];
    dst->oow = oow;

    INTERP_F(t, dst->r, out->r, in->r);
    INTERP_F(t, dst->g, out->g, in->g);
    INTERP_F(t, dst->b, out->b, in->b);
    INTERP_F(t, dst->a, out->a, in->a);
}


int
drv_multipass_none (int pass)
{
    return 0;
}
