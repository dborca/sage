#include <glide.h>
#include <g3ext.h>
#include "GL/gl.h"

#include "glinternal.h"
#include "main/context.h"
#include "util/macros.h"
#include "tnl/tnl.h"
#include "driver.h"
#include "drv.h"

#define LOGGING 0
#include "log.h"


#define DRV_POINT(v0)		 grDrawPoint(&vb[v0])
#define DRV_LINE(v0, v1)	 grDrawLine(&vb[v0], &vb[v1])
#define DRV_TRIANGLE(v0, v1, v2) grDrawTriangle(&vb[v0], &vb[v1], &vb[v2])
#define DRV_QUAD(v0, v1, v2, v3) \
    do {						\
	GrVertex *fxvb[4];				\
	fxvb[0] = &vb[v3];				\
	fxvb[1] = &vb[v0];				\
	fxvb[2] = &vb[v1];				\
	fxvb[3] = &vb[v2];				\
	grDrawVertexArray(GR_TRIANGLE_FAN, 4, fxvb);	\
    } while (0)


void (*drv_point) (int v0);
void (*drv_line) (int v0, int v1);
void (*drv_triangle) (int v0, int v1, int v2);
void (*drv_quad) (int v0, int v1, int v2, int v3);
void (*drv_clippedPolygon) (int n, int *elts);
const TNL_DRAW_FUNC *drv_prim_tab;


static void
full_clippedPolygon (int n, int *elts)
{
    int i;
    GrVertex *first;
    GrVertex *fxvb = vb;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    first = vb + elts[0];
    for (i = 2; i < n; i++) {
	grDrawTriangle(&vb[elts[i - 1]], &vb[elts[i]], first);
    }
}


static void
full_point (int v0)
{
    LOG(("%s:\n", __FUNCTION__));
    DRV_POINT(v0);
}


static void
full_line (int v0, int v1)
{
    LOG(("%s:\n", __FUNCTION__));
    DRV_LINE(v0, v1);
}


static void
full_triangle (int v0, int v1, int v2)
{
    LOG(("%s:\n", __FUNCTION__));
    DRV_TRIANGLE(v0, v1, v2);
}


static void
full_quad (int v0, int v1, int v2, int v3)
{
    LOG(("%s:\n", __FUNCTION__));
    DRV_QUAD(v0, v1, v2, v3);
}


static void
mix_clippedPolygon (int n, int *elts)
{
    int i;
    int first;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    first = elts[0];
    for (i = 2; i < n; i++) {
	drv_triangle(elts[i - 1], elts[i], first);
    }
}


#define VERTEX_X(i)	vb[v##i].x
#define VERTEX_Y(i)	vb[v##i].y
#define VERTEX_Z(i)	vb[v##i].ooz


#if FX_PACKEDCOLOR

#define IMPORT_SPEC(i, ptr)		\
    do {				\
	vb[v##i].pspec[2] = CLAMPF(ptr[v##i][0]) * 255.0F;\
	vb[v##i].pspec[1] = CLAMPF(ptr[v##i][1]) * 255.0F;\
	vb[v##i].pspec[0] = CLAMPF(ptr[v##i][2]) * 255.0F;\
    } while (0)

#define SAVE_SPEC(i)	save_spec[i] = *(GLuint *)vb[v##i].pspec
#define LOAD_SPEC(i)	*(GLuint *)vb[v##i].pspec = save_spec[i]
#define COPY_SPEC(i, j)	*(GLuint *)vb[v##i].pspec = *(GLuint *)vb[v##j].pspec

#define IMPORT_RGBA(i, ptr)		\
    do {				\
	vb[v##i].pargb[2] = CLAMPF(ptr[v##i][0]) * 255.0F;\
	vb[v##i].pargb[1] = CLAMPF(ptr[v##i][1]) * 255.0F;\
	vb[v##i].pargb[0] = CLAMPF(ptr[v##i][2]) * 255.0F;\
	vb[v##i].pargb[3] = CLAMPF(ptr[v##i][3]) * 255.0F;\
    } while (0)

#define SAVE_RGBA(i)	save_rgba[i] = *(GLuint *)vb[v##i].pargb
#define LOAD_RGBA(i)	*(GLuint *)vb[v##i].pargb = save_rgba[i]
#define COPY_RGBA(i, j)	*(GLuint *)vb[v##i].pargb = *(GLuint *)vb[v##j].pargb

#define INIT()				\
    GLuint save_rgba[4], save_spec[4]

#else  /* !FX_PACKEDCOLOR */

#define IMPORT_SPEC(i, ptr)		\
    do {				\
	vb[v##i].r1 = CLAMPF(ptr[v##i][0]) * 255.0F;\
	vb[v##i].g1 = CLAMPF(ptr[v##i][1]) * 255.0F;\
	vb[v##i].b1 = CLAMPF(ptr[v##i][2]) * 255.0F;\
    } while (0)

#define SAVE_SPEC(i)			\
    do {				\
	save_spec[i][0] = vb[v##i].r1;	\
	save_spec[i][1] = vb[v##i].g1;	\
	save_spec[i][2] = vb[v##i].b1;	\
    } while (0)
#define LOAD_SPEC(i)			\
    do {				\
	vb[v##i].r1 = save_spec[i][0];	\
	vb[v##i].g1 = save_spec[i][1];	\
	vb[v##i].b1 = save_spec[i][2];	\
    } while (0)
#define COPY_SPEC(i, j)			\
    do {				\
	vb[v##i].r1 = vb[v##j].r1;	\
	vb[v##i].g1 = vb[v##j].g1;	\
	vb[v##i].b1 = vb[v##j].b1;	\
    } while (0)

#define IMPORT_RGBA(i, ptr)		\
    do {				\
	vb[v##i].r = CLAMPF(ptr[v##i][0]) * 255.0F;\
	vb[v##i].g = CLAMPF(ptr[v##i][1]) * 255.0F;\
	vb[v##i].b = CLAMPF(ptr[v##i][2]) * 255.0F;\
	vb[v##i].a = CLAMPF(ptr[v##i][3]) * 255.0F;\
    } while (0)

#define SAVE_RGBA(i)			\
    do {				\
	save_rgba[i][0] = vb[v##i].r;	\
	save_rgba[i][1] = vb[v##i].g;	\
	save_rgba[i][2] = vb[v##i].b;	\
	save_rgba[i][3] = vb[v##i].a;	\
    } while (0)
#define LOAD_RGBA(i)			\
    do {				\
	vb[v##i].r = save_rgba[i][0];	\
	vb[v##i].g = save_rgba[i][1];	\
	vb[v##i].b = save_rgba[i][2];	\
	vb[v##i].a = save_rgba[i][3];	\
    } while (0)
#define COPY_RGBA(i, j)			\
    do {				\
	vb[v##i].r = vb[v##j].r;	\
	vb[v##i].g = vb[v##j].g;	\
	vb[v##i].b = vb[v##j].b;	\
	vb[v##i].a = vb[v##j].a;	\
    } while (0)

#define INIT()				\
    GLfloat save_rgba[4][4], save_spec[4][4]

#endif /* !FX_PACKEDCOLOR */


#define SETUP_STENCIL(f) /* XXX todo */
#define UNSET_STENCIL(f) /* XXX todo */


static RENDER_FUNC mix_line_tab[RENDER_MAX];
static RENDER_FUNC mix_triangle_tab[RENDER_MAX];
static RENDER_FUNC mix_quad_tab[RENDER_MAX];


#define TAG(x) x
#define IND 0
#include "tnl/template/tri.h"

#define TAG(x) x##_flat
#define IND (RENDER_FLAT)
#include "tnl/template/tri.h"

#define TAG(x) x##_offset
#define IND (RENDER_OFFSET)
#include "tnl/template/tri.h"

#define TAG(x) x##_flat_offset
#define IND (RENDER_FLAT|RENDER_OFFSET)
#include "tnl/template/tri.h"

#define TAG(x) x##_unfilled
#define IND (RENDER_UNFILLED)
#include "tnl/template/tri.h"

#define TAG(x) x##_flat_unfilled
#define IND (RENDER_FLAT|RENDER_UNFILLED)
#include "tnl/template/tri.h"

#define TAG(x) x##_offset_unfilled
#define IND (RENDER_OFFSET|RENDER_UNFILLED)
#include "tnl/template/tri.h"

#define TAG(x) x##_flat_offset_unfilled
#define IND (RENDER_FLAT|RENDER_OFFSET|RENDER_UNFILLED)
#include "tnl/template/tri.h"

#define TAG(x) x##_twoside
#define IND (RENDER_TWOSIDE)
#include "tnl/template/tri.h"

#define TAG(x) x##_flat_twoside
#define IND (RENDER_FLAT|RENDER_TWOSIDE)
#include "tnl/template/tri.h"

#define TAG(x) x##_offset_twoside
#define IND (RENDER_OFFSET|RENDER_TWOSIDE)
#include "tnl/template/tri.h"

#define TAG(x) x##_flat_offset_twoside
#define IND (RENDER_FLAT|RENDER_OFFSET|RENDER_TWOSIDE)
#include "tnl/template/tri.h"

#define TAG(x) x##_unfilled_twoside
#define IND (RENDER_UNFILLED|RENDER_TWOSIDE)
#include "tnl/template/tri.h"

#define TAG(x) x##_flat_unfilled_twoside
#define IND (RENDER_FLAT|RENDER_UNFILLED|RENDER_TWOSIDE)
#include "tnl/template/tri.h"

#define TAG(x) x##_offset_unfilled_twoside
#define IND (RENDER_OFFSET|RENDER_UNFILLED|RENDER_TWOSIDE)
#include "tnl/template/tri.h"

#define TAG(x) x##_flat_offset_unfilled_twoside
#define IND (RENDER_FLAT|RENDER_OFFSET|RENDER_UNFILLED|RENDER_TWOSIDE)
#include "tnl/template/tri.h"


/******************************************************************************
 * Full Vtx primitives
 * - the vertex buffer is processed in-order
 * - hw primitives are sent directly to the driver
 */


static void
fv_Points (int start, int count, int flags)
{
    GrVertex *fxvb = vb;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    grDrawVertexArrayContiguous(GR_POINTS, count,
                                fxvb + start, sizeof(GrVertex));
}


static void
fv_Lines (int start, int count, int flags)
{
    GrVertex *fxvb = vb;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    grDrawVertexArrayContiguous(GR_LINES, count,
                                fxvb + start, sizeof(GrVertex));
}


static void
fv_LineLoop (int start, int count, int flags)
{
    GrVertex *fxvb = vb;
    int j = 0;

    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));

    if (!(flags & TNL_PRIM_BEGIN)) {
	j++;
    }

    grDrawVertexArrayContiguous(GR_LINE_STRIP, count - j,
                                fxvb + start + j, sizeof(GrVertex));

    if (flags & TNL_PRIM_END) {
	grDrawLine(fxvb + start + (count - 1), fxvb + start);
    }
}


static void
fv_LineStrip (int start, int count, int flags)
{
    GrVertex *fxvb = vb;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    grDrawVertexArrayContiguous(GR_LINE_STRIP, count,
                                fxvb + start, sizeof(GrVertex));
}


static void
fv_Triangles (int start, int count, int flags)
{
    int i;
    GrVertex *fxvb = vb + start;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    for (i = 2; i < count; i += 3) {
	grDrawTriangle(fxvb, fxvb + 1, fxvb + 2);
	fxvb += 3;
    }
}


static void
fv_TriangleStrip (int start, int count, int flags)
{
    GrVertex *fxvb = vb;
    int mode = GR_TRIANGLE_STRIP;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    if (flags & TNL_PRIM_PARITY) {
	/* XXX incorrect if we have statechanges inbetween? */
	mode = GR_TRIANGLE_STRIP_CONTINUE;
    }
    grDrawVertexArrayContiguous(mode, count,
                                fxvb + start, sizeof(GrVertex));
}


static void
fv_TriangleFan (int start, int count, int flags)
{
    GrVertex *fxvb = vb;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    grDrawVertexArrayContiguous(GR_TRIANGLE_FAN, count,
                                fxvb + start, sizeof(GrVertex));
}


static void
fv_Quads (int start, int count, int flags)
{
    int i;
    GrVertex *fxvb = vb + start;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    for (i = 3; i < count; i += 4) {
	grDrawVertexArrayContiguous(GR_TRIANGLE_FAN, 4,
                                fxvb, sizeof(GrVertex));
	fxvb += 4;
    }
}


static void
fv_QuadStrip (int start, int count, int flags)
{
    GrVertex *fxvb = vb;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    grDrawVertexArrayContiguous(GR_TRIANGLE_STRIP, count,
                                fxvb + start, sizeof(GrVertex));
}


static const TNL_DRAW_FUNC fv_prim_tab[GL_POLYGON + 1] = {
    fv_Points,
    fv_Lines,
    fv_LineLoop,
    fv_LineStrip,
    fv_Triangles,
    fv_TriangleStrip,
    fv_TriangleFan,
    fv_Quads,
    fv_QuadStrip,
    fv_TriangleFan
};


/******************************************************************************
 * Full Elt primitives
 * - the vertex buffer is processed out-of-order (according to tnl_vb_elt)
 * - hw primitives are drawn with DRV_POINT, DRV_LINE, DRV_TRIANGLE, DRV_QUAD
 */


static void
fe_Points (int start, int count, int flags)
{
    int i;
    int v0;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start; i < count; i++) {
	v0 = tnl_vb_elt[i];
	DRV_POINT(v0);
    }
}


static void
fe_Lines (int start, int count, int flags)
{
    int i;
    int v0, v1;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 1; i < count; i += 2) {
	v0 = tnl_vb_elt[i - 1];
	v1 = tnl_vb_elt[i];
	DRV_LINE(v0, v1);
    }
}


static void
fe_LineLoop (int start, int count, int flags)
{
    int i, j = 0;
    int v0, v1;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    if (!(flags & TNL_PRIM_BEGIN)) {
	j++;
    }
    count += start;

    for (i = start + j + 1; i < count; i++) {
	v0 = tnl_vb_elt[i - 1];
	v1 = tnl_vb_elt[i];
	DRV_LINE(v0, v1);
    }

    if (flags & TNL_PRIM_END) {
	v0 = tnl_vb_elt[count - 1];
	v1 = tnl_vb_elt[start];
	DRV_LINE(v0, v1);
    }
}


static void
fe_LineStrip (int start, int count, int flags)
{
    int i;
    int v0, v1;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 1; i < count; i++) {
	v0 = tnl_vb_elt[i - 1];
	v1 = tnl_vb_elt[i];
	DRV_LINE(v0, v1);
    }
}


static void
fe_Triangles (int start, int count, int flags)
{
    int i;
    int v0, v1, v2;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 2; i < count; i += 3) {
	v0 = tnl_vb_elt[i - 2];
	v1 = tnl_vb_elt[i - 1];
	v2 = tnl_vb_elt[i];
	DRV_TRIANGLE(v0, v1, v2);
    }
}


static void
fe_TriangleStrip (int start, int count, int flags)
{
    int i, j = 0;
    int v0, v1, v2;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    if (flags & TNL_PRIM_PARITY) {
	j++;
    }
    count += start;
    for (i = start + 2; i < count; i++, j ^= 1) {
	int j1 = i - 2 + j;
	int j2 = i - 1 - j;
	v0 = tnl_vb_elt[j1];
	v1 = tnl_vb_elt[j2];
	v2 = tnl_vb_elt[i];
	DRV_TRIANGLE(v0, v1, v2);
    }
}


static void
fe_TriangleFan (int start, int count, int flags)
{
    int i;
    int v0, v1, v2;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start - 1;
    v0 = tnl_vb_elt[start];
    for (i = start + 1; i < count; i++) {
	v1 = tnl_vb_elt[i];
	v2 = tnl_vb_elt[i + 1];
	DRV_TRIANGLE(v0, v1, v2);
    }
}


static void
fe_Quads (int start, int count, int flags)
{
    int i;
    int v0, v1, v2, v3;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 3; i < count; i += 4) {
	v0 = tnl_vb_elt[i - 3];
	v1 = tnl_vb_elt[i - 2];
	v2 = tnl_vb_elt[i - 1];
	v3 = tnl_vb_elt[i];
	DRV_QUAD(v0, v1, v2, v3);
    }
}


static void
fe_QuadStrip (int start, int count, int flags)
{
    int i;
    int v0, v1, v2, v3;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 3; i < count; i += 2) {
	v0 = tnl_vb_elt[i - 3];
	v1 = tnl_vb_elt[i - 2];
	v2 = tnl_vb_elt[i - 1];
	v3 = tnl_vb_elt[i];
	DRV_QUAD(v0, v1, v3, v2);
    }
}


static void
fe_Polygon (int start, int count, int flags)
{
    int i;
    int v0, v1, v2;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start - 1;
    v0 = tnl_vb_elt[start];
    for (i = start + 1; i < count; i++) {
	v1 = tnl_vb_elt[i];
	v2 = tnl_vb_elt[i + 1];
	DRV_TRIANGLE(v1, v2, v0);
    }
}


static const TNL_DRAW_FUNC fe_prim_tab[GL_POLYGON + 1] = {
    fe_Points,
    fe_Lines,
    fe_LineLoop,
    fe_LineStrip,
    fe_Triangles,
    fe_TriangleStrip,
    fe_TriangleFan,
    fe_Quads,
    fe_QuadStrip,
    fe_Polygon
};


/******************************************************************************
 * Mixed mode Vtx primitives
 * - the vertex buffer is processed in-order
 * - hw primitives are drawn with drv_point, drv_line, drv_triangle, drv_quad
 */


static void
mv_Points (int start, int count, int flags)
{
    int i;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start; i < count; i++) {
	drv_point(i);
    }
}


static void
mv_Lines (int start, int count, int flags)
{
    int i;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 1; i < count; i += 2) {
	drv_line(i - 1, i);
    }
}


static void
mv_LineLoop (int start, int count, int flags)
{
    int i, j = 0;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    if (!(flags & TNL_PRIM_BEGIN)) {
	j++;
    }
    count += start;

    for (i = start + j + 1; i < count; i++) {
	drv_line(i - 1, i);
    }

    if (flags & TNL_PRIM_END) {
	drv_line(count - 1, start);
    }
}


static void
mv_LineStrip (int start, int count, int flags)
{
    int i;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 1; i < count; i++) {
	drv_line(i - 1, i);
    }
}


static void
mv_Triangles (int start, int count, int flags)
{
    int i;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 2; i < count; i += 3) {
	drv_triangle(i - 2, i - 1, i);
    }
}


static void
mv_TriangleStrip (int start, int count, int flags)
{
    int i, j = 0;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    if (flags & TNL_PRIM_PARITY) {
	j++;
    }
    count += start;
    for (i = start + 2; i < count; i++, j ^= 1) {
	int j1 = i - 2 + j;
	int j2 = i - 1 - j;
	drv_triangle(j1, j2, i);
    }
}


static void
mv_TriangleFan (int start, int count, int flags)
{
    int i;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start - 1;
    for (i = start + 1; i < count; i++) {
	drv_triangle(start, i, i + 1);
    }
}


static void
mv_Quads (int start, int count, int flags)
{
    int i;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 3; i < count; i += 4) {
	drv_quad(i - 3, i - 2, i - 1, i);
    }
}


static void
mv_QuadStrip (int start, int count, int flags)
{
    int i;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 3; i < count; i += 2) {
	drv_quad(i - 3, i - 2, i, i - 1);
    }
}


static void
mv_Polygon (int start, int count, int flags)
{
    int i;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start - 1;
    for (i = start + 1; i < count; i++) {
	drv_triangle(i, i + 1, start);
    }
}


static const TNL_DRAW_FUNC mv_prim_tab[GL_POLYGON + 1] = {
    mv_Points,
    mv_Lines,
    mv_LineLoop,
    mv_LineStrip,
    mv_Triangles,
    mv_TriangleStrip,
    mv_TriangleFan,
    mv_Quads,
    mv_QuadStrip,
    mv_Polygon
};


/******************************************************************************
 * Mixed mode Elt primitives
 * - the vertex buffer is processed out-of-order (according to tnl_vb_elt)
 * - hw primitives are drawn with drv_point, drv_line, drv_triangle, drv_quad
 */


static void
me_Points (int start, int count, int flags)
{
    int i;
    int v0;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start; i < count; i++) {
	v0 = tnl_vb_elt[i];
	drv_point(v0);
    }
}


static void
me_Lines (int start, int count, int flags)
{
    int i;
    int v0, v1;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 1; i < count; i += 2) {
	v0 = tnl_vb_elt[i - 1];
	v1 = tnl_vb_elt[i];
	drv_line(v0, v1);
    }
}


static void
me_LineLoop (int start, int count, int flags)
{
    int i, j = 0;
    int v0, v1;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    if (!(flags & TNL_PRIM_BEGIN)) {
	j++;
    }
    count += start;

    for (i = start + j + 1; i < count; i++) {
	v0 = tnl_vb_elt[i - 1];
	v1 = tnl_vb_elt[i];
	drv_line(v0, v1);
    }

    if (flags & TNL_PRIM_END) {
	v0 = tnl_vb_elt[count - 1];
	v1 = tnl_vb_elt[start];
	drv_line(v0, v1);
    }
}


static void
me_LineStrip (int start, int count, int flags)
{
    int i;
    int v0, v1;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 1; i < count; i++) {
	v0 = tnl_vb_elt[i - 1];
	v1 = tnl_vb_elt[i];
	drv_line(v0, v1);
    }
}


static void
me_Triangles (int start, int count, int flags)
{
    int i;
    int v0, v1, v2;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 2; i < count; i += 3) {
	v0 = tnl_vb_elt[i - 2];
	v1 = tnl_vb_elt[i - 1];
	v2 = tnl_vb_elt[i];
	drv_triangle(v0, v1, v2);
    }
}


static void
me_TriangleStrip (int start, int count, int flags)
{
    int i, j = 0;
    int v0, v1, v2;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    if (flags & TNL_PRIM_PARITY) {
	j++;
    }
    count += start;
    for (i = start + 2; i < count; i++, j ^= 1) {
	int j1 = i - 2 + j;
	int j2 = i - 1 - j;
	v0 = tnl_vb_elt[j1];
	v1 = tnl_vb_elt[j2];
	v2 = tnl_vb_elt[i];
	drv_triangle(v0, v1, v2);
    }
}


static void
me_TriangleFan (int start, int count, int flags)
{
    int i;
    int v0, v1, v2;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start - 1;
    v0 = tnl_vb_elt[start];
    for (i = start + 1; i < count; i++) {
	v1 = tnl_vb_elt[i];
	v2 = tnl_vb_elt[i + 1];
	drv_triangle(v0, v1, v2);
    }
}


static void
me_Quads (int start, int count, int flags)
{
    int i;
    int v0, v1, v2, v3;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 3; i < count; i += 4) {
	v0 = tnl_vb_elt[i - 3];
	v1 = tnl_vb_elt[i - 2];
	v2 = tnl_vb_elt[i - 1];
	v3 = tnl_vb_elt[i];
	drv_quad(v0, v1, v2, v3);
    }
}


static void
me_QuadStrip (int start, int count, int flags)
{
    int i;
    int v0, v1, v2, v3;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start;
    for (i = start + 3; i < count; i += 2) {
	v0 = tnl_vb_elt[i - 3];
	v1 = tnl_vb_elt[i - 2];
	v2 = tnl_vb_elt[i - 1];
	v3 = tnl_vb_elt[i];
	drv_quad(v0, v1, v3, v2);
    }
}


static void
me_Polygon (int start, int count, int flags)
{
    int i;
    int v0, v1, v2;
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
    count += start - 1;
    v0 = tnl_vb_elt[start];
    for (i = start + 1; i < count; i++) {
	v1 = tnl_vb_elt[i];
	v2 = tnl_vb_elt[i + 1];
	drv_triangle(v1, v2, v0);
    }
}


static const TNL_DRAW_FUNC me_prim_tab[GL_POLYGON + 1] = {
    me_Points,
    me_Lines,
    me_LineLoop,
    me_LineStrip,
    me_Triangles,
    me_TriangleStrip,
    me_TriangleFan,
    me_Quads,
    me_QuadStrip,
    me_Polygon
};


void
init_tri_pointers (void)
{
    mix_init();
    mix_init_flat();
    mix_init_offset();
    mix_init_flat_offset();
    mix_init_unfilled();
    mix_init_flat_unfilled();
    mix_init_offset_unfilled();
    mix_init_flat_offset_unfilled();
    mix_init_twoside();
    mix_init_flat_twoside();
    mix_init_offset_twoside();
    mix_init_flat_offset_twoside();
    mix_init_unfilled_twoside();
    mix_init_flat_unfilled_twoside();
    mix_init_offset_unfilled_twoside();
    mix_init_flat_offset_unfilled_twoside();
}


void
setup_tri_pointers (void)
{
    GLbitfield index = 0;

    if (ctx_shade_model == GL_FLAT) {
	index |= RENDER_FLAT;
    }
    if (ctx_polygon_offset_mode) {
	index |= RENDER_OFFSET;
    }
    if (ctx_polygon_mode[0] != POLYGON_FILL || ctx_polygon_mode[1] != POLYGON_FILL) {
	index |= RENDER_UNFILLED;
    }
    if (ctx_lighting && ctx_light_model_twoside) {
	index |= RENDER_TWOSIDE;
    }

    drv_point = full_point;
    drv_line = full_line;
    drv_triangle = full_triangle;
    drv_quad = full_quad;
    drv_clippedPolygon = full_clippedPolygon;
    drv_prim_tab = fv_prim_tab;

    if (index) {
	/*drv_point = mix_point_tab[index];*/
	drv_line = mix_line_tab[index];
	drv_triangle = mix_triangle_tab[index];
	drv_quad = mix_quad_tab[index];
	drv_clippedPolygon = mix_clippedPolygon;
	drv_prim_tab = mv_prim_tab;
    }
}
