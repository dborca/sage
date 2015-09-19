#include "GL/gl.h"
#include "glinternal.h"
#include "main/context.h"
#include "util/macros.h"
#include "tnl.h"
#include "driver.h" /* for drawing functions */

#define LOGGING 0
#include "log.h"


#define CLIP_LINE(PLANE, A, B, C, D)					\
    do {								\
	if (mask & PLANE) {						\
	    GLfloat t, d0, d1;						\
									\
	    d0 = coord[p0][0] * (A) +					\
		 coord[p0][1] * (B) +					\
		 coord[p0][2] * (C) +					\
		 coord[p0][3] * (D);					\
									\
	    d1 = coord[p1][0] * (A) +					\
		 coord[p1][1] * (B) +					\
		 coord[p1][2] * (C) +					\
		 coord[p1][3] * (D);					\
									\
	    if (DIFFERENT_SIGNS(d0, d1)) {				\
		int v = last_clipped++;					\
		/*tnl_vb.clipmask[v] = 0;*/				\
		if (IS_NEGATIVE(d0)) {					\
		    t = d1 / (d1 - d0);					\
		    /*tnl_vb.clipmask[p0] |= PLANE;*/			\
		    INTERP_4F(t, coord[v], coord[p1], coord[p0]);	\
		    drv_interp(t, v, p1, p0);				\
		    p0 = v;						\
		} else {						\
		    t = d0 / (d0 - d1);					\
		    /*tnl_vb.clipmask[p1] |= PLANE;*/			\
		    INTERP_4F(t, coord[v], coord[p0], coord[p1]);	\
		    drv_interp(t, v, p0, p1);				\
		    p1 = v;						\
		}							\
	    } else if (IS_NEGATIVE(d0)) {				\
		return;							\
	    }								\
	}								\
    } while (0)


#define CLIP_POLY(PLANE, A, B, C, D)					\
    do {								\
	if (mask & PLANE) {						\
	    GLfloat t, d, pd;						\
	    int prev, idx;						\
									\
	    prev = inlist[0];						\
	    pd = coord[prev][0] * (A) +					\
		 coord[prev][1] * (B) +					\
		 coord[prev][2] * (C) +					\
		 coord[prev][3] * (D);					\
	    out = 0;							\
	    inlist[n] = prev;						\
									\
	    for (i = 1; i <= n; i++) {					\
		idx = inlist[i];					\
									\
		d = coord[idx][0] * (A) +				\
		    coord[idx][1] * (B) +				\
		    coord[idx][2] * (C) +				\
		    coord[idx][3] * (D);				\
									\
		/*tnl_vb.clipmask[prev] |= PLANE;*/			\
		if (!IS_NEGATIVE(pd)) {					\
		    outlist[out++] = prev;				\
		    /*tnl_vb.clipmask[prev] &= ~PLANE;*/		\
		}							\
									\
		if (DIFFERENT_SIGNS(d, pd)) {				\
		    int v = last_clipped++;				\
		    /*tnl_vb.clipmask[v] = 0;*/				\
		    outlist[out++] = v;					\
		    if (IS_NEGATIVE(d)) {				\
			/* going out */					\
			t = d / (d - pd);				\
			INTERP_4F(t, coord[v], coord[idx], coord[prev]);\
			drv_interp(t, v, idx, prev);			\
		    } else {						\
			/* coming in */					\
			t = pd / (pd - d);				\
			INTERP_4F(t, coord[v], coord[prev], coord[idx]);\
			drv_interp(t, v, prev, idx);			\
		    }							\
		}							\
									\
		prev = idx;						\
		pd = d;							\
	    }								\
									\
	    if (out < 3) {						\
		return;							\
	    }								\
									\
	    n = out;							\
	    tmplist = inlist;						\
	    inlist = outlist;						\
	    outlist = tmplist;						\
	}								\
    } while (0)


static void
_tnl_drawLine_clipped (int v0, int v1, int mask)
{
    GLfloat4 *coord = tnl_vb.clip;
    int p0 = v0, p1 = v1;
    int last_clipped = tnl_vb.num;

    CLIP_LINE(TNL_CLIP_RIGHT,  -1.0F,  0.0F,  0.0F, 1.0F);
    CLIP_LINE(TNL_CLIP_LEFT,    1.0F,  0.0F,  0.0F, 1.0F);
    CLIP_LINE(TNL_CLIP_TOP,     0.0F, -1.0F,  0.0F, 1.0F);
    CLIP_LINE(TNL_CLIP_BOTTOM,  0.0F,  1.0F,  0.0F, 1.0F);
    CLIP_LINE(TNL_CLIP_FAR,     0.0F,  0.0F, -1.0F, 1.0F);
    CLIP_LINE(TNL_CLIP_NEAR,    0.0F,  0.0F,  1.0F, 1.0F);

    if (ctx_userclip) {
	int u;
	int planes = ctx_userclip;
	for (u = 0; u < TNL_USERCLIP_PLANES; u++) {
	    if (planes & 1) {
		CLIP_LINE(TNL_CLIP_USER,
			ctx_userplanes[1][u][0],
			ctx_userplanes[1][u][1],
			ctx_userplanes[1][u][2],
			ctx_userplanes[1][u][3]);
	    }
	    planes >>= 1;
	}
    }

    /* provoking vertex */
    if (ctx_shade_model == GL_FLAT && p1 != v1) {
	drv_copypv(p1, v1);
    }

    /* render clipped line */
    drv_line(p0, p1);
}


static void
_tnl_drawTriangle_clipped (int v0, int v1, int v2, int mask)
{
    GLfloat4 *coord = tnl_vb.clip;
    int vertices[2][TNL_CLIPPED_VERTS];
    int i, n, out, *inlist, *outlist, *tmplist;
    int last_clipped = tnl_vb.num;

    inlist = vertices[0];
    outlist = vertices[1];
    inlist[0] = v2;
    inlist[1] = v0;
    inlist[2] = v1;
    n = 3;

    CLIP_POLY(TNL_CLIP_RIGHT,  -1.0F,  0.0F,  0.0F, 1.0F);
    CLIP_POLY(TNL_CLIP_LEFT,    1.0F,  0.0F,  0.0F, 1.0F);
    CLIP_POLY(TNL_CLIP_TOP,     0.0F, -1.0F,  0.0F, 1.0F);
    CLIP_POLY(TNL_CLIP_BOTTOM,  0.0F,  1.0F,  0.0F, 1.0F);
    CLIP_POLY(TNL_CLIP_FAR,     0.0F,  0.0F, -1.0F, 1.0F);
    CLIP_POLY(TNL_CLIP_NEAR,    0.0F,  0.0F,  1.0F, 1.0F);

    if (ctx_userclip) {
	int u;
	int planes = ctx_userclip;
	for (u = 0; u < TNL_USERCLIP_PLANES; u++) {
	    if (planes & 1) {
		CLIP_POLY(TNL_CLIP_USER,
			ctx_userplanes[1][u][0],
			ctx_userplanes[1][u][1],
			ctx_userplanes[1][u][2],
			ctx_userplanes[1][u][3]);
	    }
	    planes >>= 1;
	}
    }

    /* provoking vertex */
    if (ctx_shade_model == GL_FLAT && inlist[0] != v2) {
	drv_copypv(inlist[0], v2);
    }

    /* render clipped polygon */
    drv_clippedPolygon(n, inlist);
}


static void
_tnl_drawQuad_clipped (int v0, int v1, int v2, int v3, int mask)
{
    GLfloat4 *coord = tnl_vb.clip;
    int vertices[2][TNL_CLIPPED_VERTS];
    int i, n, out, *inlist, *outlist, *tmplist;
    int last_clipped = tnl_vb.num;

    inlist = vertices[0];
    outlist = vertices[1];
    inlist[0] = v3;
    inlist[1] = v0;
    inlist[2] = v1;
    inlist[3] = v2;
    n = 4;

    CLIP_POLY(TNL_CLIP_RIGHT,  -1.0F,  0.0F,  0.0F, 1.0F);
    CLIP_POLY(TNL_CLIP_LEFT,    1.0F,  0.0F,  0.0F, 1.0F);
    CLIP_POLY(TNL_CLIP_TOP,     0.0F, -1.0F,  0.0F, 1.0F);
    CLIP_POLY(TNL_CLIP_BOTTOM,  0.0F,  1.0F,  0.0F, 1.0F);
    CLIP_POLY(TNL_CLIP_FAR,     0.0F,  0.0F, -1.0F, 1.0F);
    CLIP_POLY(TNL_CLIP_NEAR,    0.0F,  0.0F,  1.0F, 1.0F);

    if (ctx_userclip) {
	int u;
	int planes = ctx_userclip;
	for (u = 0; u < TNL_USERCLIP_PLANES; u++) {
	    if (planes & 1) {
		CLIP_POLY(TNL_CLIP_USER,
			ctx_userplanes[1][u][0],
			ctx_userplanes[1][u][1],
			ctx_userplanes[1][u][2],
			ctx_userplanes[1][u][3]);
	    }
	    planes >>= 1;
	}
    }

    /* provoking vertex */
    if (ctx_shade_model == GL_FLAT && inlist[0] != v3) {
	drv_copypv(inlist[0], v3);
    }

    /* render clipped polygon */
    drv_clippedPolygon(n, inlist);
}



/******************************************************************************
 * Clipped Vtx primitives
 * - the vertex buffer is processed in-order
 * - clipped primitives are drawn with drv_line, drv_clippedPolygon
 * - hw primitives are drawn with drv_point, drv_line, drv_triangle, drv_quad
 */


static void
_vtx_Points (int start, int count, int mode)
{
    int i;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start; i < count; i++) {
	if (!tnl_vb.clipmask[i]) {
	    drv_point(i);
	}
    }
}


static void
_vtx_Lines (int start, int count, int mode)
{
    int i;
    int c0, c1;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 1; i < count; i += 2) {
	c0 = tnl_vb.clipmask[i - 1];
	c1 = tnl_vb.clipmask[i];
	ormask = c0 | c1;
	if (!ormask) {
	    drv_line(i - 1, i);
	} else if (!(c0 & c1)) {
	    _tnl_drawLine_clipped(i - 1, i, ormask);
	}
    }
}


static void
_vtx_LineLoop (int start, int count, int mode)
{
    int i, j = 0;
    int c0, c1;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    if (!(mode & TNL_PRIM_BEGIN)) {
	j++;
    }
    count += start;

    for (i = start + j + 1; i < count; i++) {
	c0 = tnl_vb.clipmask[i - 1];
	c1 = tnl_vb.clipmask[i];
	ormask = c0 | c1;
	if (!ormask) {
	    drv_line(i - 1, i);
	} else if (!(c0 & c1)) {
	    _tnl_drawLine_clipped(i - 1, i, ormask);
	}
    }

    if (mode & TNL_PRIM_END) {
	c0 = tnl_vb.clipmask[count - 1];
	c1 = tnl_vb.clipmask[start];
	ormask = c0 | c1;
	if (!ormask) {
	    drv_line(count - 1, start);
	} else if (!(c0 & c1)) {
	    _tnl_drawLine_clipped(count - 1, start, ormask);
	}
    }
}


static void
_vtx_LineStrip (int start, int count, int mode)
{
    int i;
    int c0, c1;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 1; i < count; i++) {
	c0 = tnl_vb.clipmask[i - 1];
	c1 = tnl_vb.clipmask[i];
	ormask = c0 | c1;
	if (!ormask) {
	    drv_line(i - 1, i);
	} else if (!(c0 & c1)) {
	    _tnl_drawLine_clipped(i - 1, i, ormask);
	}
    }
}


static void
_vtx_Triangles (int start, int count, int mode)
{
    int i;
    int c0, c1, c2;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 2; i < count; i += 3) {
	c0 = tnl_vb.clipmask[i - 2];
	c1 = tnl_vb.clipmask[i - 1];
	c2 = tnl_vb.clipmask[i];
	ormask = c0 | c1 | c2;
	if (!ormask) {
	    drv_triangle(i - 2, i - 1, i);
	} else if (!(c0 & c1 & c2)) {
	    _tnl_drawTriangle_clipped(i - 2, i - 1, i, ormask);
	}
    }
}


static void
_vtx_TriangleStrip (int start, int count, int mode)
{
    int i, j = 0;
    int c0, c1, c2;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    if (mode & TNL_PRIM_PARITY) {
	j++;
    }
    count += start;
    for (i = start + 2; i < count; i++, j ^= 1) {
	int j1 = i - 2 + j;
	int j2 = i - 1 - j;
	c0 = tnl_vb.clipmask[i - 2];
	c1 = tnl_vb.clipmask[i - 1];
	c2 = tnl_vb.clipmask[i];
	ormask = c0 | c1 | c2;
	if (!ormask) {
	    drv_triangle(j1, j2, i);
	} else if (!(c0 & c1 & c2)) {
	    _tnl_drawTriangle_clipped(j1, j2, i, ormask);
	}
    }
}


static void
_vtx_TriangleFan (int start, int count, int mode)
{
    int i;
    int c0, c1, c2;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start - 1;
    c0 = tnl_vb.clipmask[start];
    for (i = start + 1; i < count; i++) {
	c1 = tnl_vb.clipmask[i];
	c2 = tnl_vb.clipmask[i + 1];
	ormask = c0 | c1 | c2;
	if (!ormask) {
	    drv_triangle(start, i, i + 1);
	} else if (!(c0 & c1 & c2)) {
	    _tnl_drawTriangle_clipped(start, i, i + 1, ormask);
	}
    }
}


static void
_vtx_Quads (int start, int count, int mode)
{
    int i;
    int c0, c1, c2, c3;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 3; i < count; i += 4) {
	c0 = tnl_vb.clipmask[i - 3];
	c1 = tnl_vb.clipmask[i - 2];
	c2 = tnl_vb.clipmask[i - 1];
	c3 = tnl_vb.clipmask[i];
	ormask = c0 | c1 | c2 | c3;
	if (!ormask) {
	    drv_quad(i - 3, i - 2, i - 1, i);
	} else if (!(c0 & c1 & c2 & c3)) {
	    _tnl_drawQuad_clipped(i - 3, i - 2, i - 1, i, ormask);
	}
    }
}


static void
_vtx_QuadStrip (int start, int count, int mode)
{
    int i;
    int c0, c1, c2, c3;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 3; i < count; i += 2) {
	c0 = tnl_vb.clipmask[i - 3];
	c1 = tnl_vb.clipmask[i - 2];
	c2 = tnl_vb.clipmask[i - 1];
	c3 = tnl_vb.clipmask[i];
	ormask = c0 | c1 | c2 | c3;
	if (!ormask) {
	    drv_quad(i - 3, i - 2, i, i - 1);
	} else if (!(c0 & c1 & c2 & c3)) {
	    _tnl_drawQuad_clipped(i - 3, i - 2, i, i - 1, ormask);
	}
    }
}


static void
_vtx_Polygon (int start, int count, int mode)
{
    int i;
    int c0, c1, c2;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start - 1;
    c0 = tnl_vb.clipmask[start];
    for (i = start + 1; i < count; i++) {
	c1 = tnl_vb.clipmask[i];
	c2 = tnl_vb.clipmask[i + 1];
	ormask = c0 | c1 | c2;
	if (!ormask) {
	    drv_triangle(i, i + 1, start);
	} else if (!(c0 & c1 & c2)) {
	    _tnl_drawTriangle_clipped(i, i + 1, start, ormask);
	}
    }
}


static const TNL_DRAW_FUNC vtx_prim_tab[GL_POLYGON + 1] = {
    _vtx_Points,
    _vtx_Lines,
    _vtx_LineLoop,
    _vtx_LineStrip,
    _vtx_Triangles,
    _vtx_TriangleStrip,
    _vtx_TriangleFan,
    _vtx_Quads,
    _vtx_QuadStrip,
    _vtx_Polygon
};


/******************************************************************************
 * Clipped Elt primitives
 * - the vertex buffer is processed out-of-order (according to tnl_vb_elt)
 * - clipped primitives are drawn with drv_line, drv_clippedPolygon
 * - hw primitives are drawn with drv_point, drv_line, drv_triangle, drv_quad
 */


static void
_elt_Points (int start, int count, int mode)
{
    int i;
    int v0;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start; i < count; i++) {
	v0 = tnl_vb_elt[i];
	if (!tnl_vb.clipmask[v0]) {
	    drv_point(v0);
	}
    }
}


static void
_elt_Lines (int start, int count, int mode)
{
    int i;
    int v0, v1;
    int c0, c1;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 1; i < count; i += 2) {
	v0 = tnl_vb_elt[i - 1];
	v1 = tnl_vb_elt[i];
	c0 = tnl_vb.clipmask[v0];
	c1 = tnl_vb.clipmask[v1];
	ormask = c0 | c1;
	if (!ormask) {
	    drv_line(v0, v1);
	} else if (!(c0 & c1)) {
	    _tnl_drawLine_clipped(v0, v1, ormask);
	}
    }
}


static void
_elt_LineLoop (int start, int count, int mode)
{
    int i, j = 0;
    int v0, v1;
    int c0, c1;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    if (!(mode & TNL_PRIM_BEGIN)) {
	j++;
    }
    count += start;

    for (i = start + j + 1; i < count; i++) {
	v0 = tnl_vb_elt[i - 1];
	v1 = tnl_vb_elt[i];
	c0 = tnl_vb.clipmask[v0];
	c1 = tnl_vb.clipmask[v1];
	ormask = c0 | c1;
	if (!ormask) {
	    drv_line(v0, v1);
	} else if (!(c0 & c1)) {
	    _tnl_drawLine_clipped(v0, v1, ormask);
	}
    }

    if (mode & TNL_PRIM_END) {
	v0 = tnl_vb_elt[count - 1];
	v1 = tnl_vb_elt[start];
	c0 = tnl_vb.clipmask[v0];
	c1 = tnl_vb.clipmask[v1];
	ormask = c0 | c1;
	if (!ormask) {
	    drv_line(v0, v1);
	} else if (!(c0 & c1)) {
	    _tnl_drawLine_clipped(v0, v1, ormask);
	}
    }
}


static void
_elt_LineStrip (int start, int count, int mode)
{
    int i;
    int v0, v1;
    int c0, c1;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 1; i < count; i++) {
	v0 = tnl_vb_elt[i - 1];
	v1 = tnl_vb_elt[i];
	c0 = tnl_vb.clipmask[v0];
	c1 = tnl_vb.clipmask[v1];
	ormask = c0 | c1;
	if (!ormask) {
	    drv_line(v0, v1);
	} else if (!(c0 & c1)) {
	    _tnl_drawLine_clipped(v0, v1, ormask);
	}
    }
}


static void
_elt_Triangles (int start, int count, int mode)
{
    int i;
    int v0, v1, v2;
    int c0, c1, c2;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 2; i < count; i += 3) {
	v0 = tnl_vb_elt[i - 2];
	v1 = tnl_vb_elt[i - 1];
	v2 = tnl_vb_elt[i];
	c0 = tnl_vb.clipmask[v0];
	c1 = tnl_vb.clipmask[v1];
	c2 = tnl_vb.clipmask[v2];
	ormask = c0 | c1 | c2;
	if (!ormask) {
	    drv_triangle(v0, v1, v2);
	} else if (!(c0 & c1 & c2)) {
	    _tnl_drawTriangle_clipped(v0, v1, v2, ormask);
	}
    }
}


static void
_elt_TriangleStrip (int start, int count, int mode)
{
    int i, j = 0;
    int v0, v1, v2;
    int c0, c1, c2;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    if (mode & TNL_PRIM_PARITY) {
	j++;
    }
    count += start;
    for (i = start + 2; i < count; i++, j ^= 1) {
	int j1 = i - 2 + j;
	int j2 = i - 1 - j;
	v0 = tnl_vb_elt[j1];
	v1 = tnl_vb_elt[j2];
	v2 = tnl_vb_elt[i];
	c0 = tnl_vb.clipmask[v0];
	c1 = tnl_vb.clipmask[v1];
	c2 = tnl_vb.clipmask[v2];
	ormask = c0 | c1 | c2;
	if (!ormask) {
	    drv_triangle(v0, v1, v2);
	} else if (!(c0 & c1 & c2)) {
	    _tnl_drawTriangle_clipped(v0, v1, v2, ormask);
	}
    }
}


static void
_elt_TriangleFan (int start, int count, int mode)
{
    int i;
    int v0, v1, v2;
    int c0, c1, c2;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start - 1;
    v0 = tnl_vb_elt[start];
    c0 = tnl_vb.clipmask[v0];
    for (i = start + 1; i < count; i++) {
	v1 = tnl_vb_elt[i];
	v2 = tnl_vb_elt[i + 1];
	c1 = tnl_vb.clipmask[v1];
	c2 = tnl_vb.clipmask[v2];
	ormask = c0 | c1 | c2;
	if (!ormask) {
	    drv_triangle(v0, v1, v2);
	} else if (!(c0 & c1 & c2)) {
	    _tnl_drawTriangle_clipped(v0, v1, v2, ormask);
	}
    }
}


static void
_elt_Quads (int start, int count, int mode)
{
    int i;
    int v0, v1, v2, v3;
    int c0, c1, c2, c3;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 3; i < count; i += 4) {
	v0 = tnl_vb_elt[i - 3];
	v1 = tnl_vb_elt[i - 2];
	v2 = tnl_vb_elt[i - 1];
	v3 = tnl_vb_elt[i];
	c0 = tnl_vb.clipmask[v0];
	c1 = tnl_vb.clipmask[v1];
	c2 = tnl_vb.clipmask[v2];
	c3 = tnl_vb.clipmask[v3];
	ormask = c0 | c1 | c2 | c3;
	if (!ormask) {
	    drv_quad(v0, v1, v2, v3);
	} else if (!(c0 & c1 & c2 & c3)) {
	    _tnl_drawQuad_clipped(v0, v1, v2, v3, ormask);
	}
    }
}


static void
_elt_QuadStrip (int start, int count, int mode)
{
    int i;
    int v0, v1, v2, v3;
    int c0, c1, c2, c3;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 3; i < count; i += 2) {
	v0 = tnl_vb_elt[i - 3];
	v1 = tnl_vb_elt[i - 2];
	v2 = tnl_vb_elt[i - 1];
	v3 = tnl_vb_elt[i];
	c0 = tnl_vb.clipmask[v0];
	c1 = tnl_vb.clipmask[v1];
	c2 = tnl_vb.clipmask[v2];
	c3 = tnl_vb.clipmask[v3];
	ormask = c0 | c1 | c2 | c3;
	if (!ormask) {
	    drv_quad(v0, v1, v3, v2);
	} else if (!(c0 & c1 & c2 & c3)) {
	    _tnl_drawQuad_clipped(v0, v1, v3, v2, ormask);
	}
    }
}


static void
_elt_Polygon (int start, int count, int mode)
{
    int i;
    int v0, v1, v2;
    int c0, c1, c2;
    int ormask;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start - 1;
    v0 = tnl_vb_elt[start];
    c0 = tnl_vb.clipmask[v0];
    for (i = start + 1; i < count; i++) {
	v1 = tnl_vb_elt[i];
	v2 = tnl_vb_elt[i + 1];
	c1 = tnl_vb.clipmask[v1];
	c2 = tnl_vb.clipmask[v2];
	ormask = c0 | c1 | c2;
	if (!ormask) {
	    drv_triangle(v1, v2, v0);
	} else if (!(c0 & c1 & c2)) {
	    _tnl_drawTriangle_clipped(v1, v2, v0, ormask);
	}
    }
}


const TNL_DRAW_FUNC elt_prim_tab[GL_POLYGON + 1] = {
    _elt_Points,
    _elt_Lines,
    _elt_LineLoop,
    _elt_LineStrip,
    _elt_Triangles,
    _elt_TriangleStrip,
    _elt_TriangleFan,
    _elt_Quads,
    _elt_QuadStrip,
    _elt_Polygon
};


const TNL_DRAW_FUNC *tnl_prim_tab = vtx_prim_tab;
