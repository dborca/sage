#include "GL/gl.h"

#include "glinternal.h"
#include "main/context.h"
#include "tnl/tnl.h"
#include "driver.h"
#include "drv.h"

#define LOGGING 0
#include "log.h"


void (*drv_point) (int v0);
void (*drv_line) (int v0, int v1);
void (*drv_triangle) (int v0, int v1, int v2);
void (*drv_quad) (int v0, int v1, int v2, int v3);
void (*drv_clippedPolygon) (int n, int *elts);
const TNL_DRAW_FUNC *drv_prim_tab;


static void
full_Points (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
full_Lines (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
full_LineLoop (int start, int count, int flags)
{
    int j = 0;

    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));

    if (!(flags & TNL_PRIM_BEGIN)) {
	j++;
    }

    if (flags & TNL_PRIM_END) {
    }
}


static void
full_LineStrip (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
full_Triangles (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
full_TriangleStrip (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
full_TriangleFan (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
full_Quads (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
full_QuadStrip (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static const TNL_DRAW_FUNC drv_prim_full[GL_POLYGON + 1] = {
    full_Points,
    full_Lines,
    full_LineLoop,
    full_LineStrip,
    full_Triangles,
    full_TriangleStrip,
    full_TriangleFan,
    full_Quads,
    full_QuadStrip,
    full_TriangleFan
};


void
full_point (int v0)
{
    LOG(("%s:\n", __FUNCTION__));
}


void
full_line (int v0, int v1)
{
    LOG(("%s:\n", __FUNCTION__));
}


void
full_triangle (int v0, int v1, int v2)
{
    LOG(("%s:\n", __FUNCTION__));
}


void
full_quad (int v0, int v1, int v2, int v3)
{
    LOG(("%s:\n", __FUNCTION__));
}


void
full_clippedPolygon (int n, int *elts)
{
    LOG(("%s: %d\n", __FUNCTION__, n));
}


static void
mix_Points (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
mix_Lines (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
mix_LineLoop (int start, int count, int flags)
{
    int j = 0;

    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));

    if (!(flags & TNL_PRIM_BEGIN)) {
	j++;
    }

    if (flags & TNL_PRIM_END) {
    }
}


static void
mix_LineStrip (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
mix_Triangles (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
mix_TriangleStrip (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
mix_TriangleFan (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
mix_Quads (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
mix_QuadStrip (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static void
mix_Polygon (int start, int count, int flags)
{
    LOG(("%s: %d + %d\n", __FUNCTION__, start, count));
}


static const TNL_DRAW_FUNC drv_prim_mix[GL_POLYGON + 1] = {
    mix_Points,
    mix_Lines,
    mix_LineLoop,
    mix_LineStrip,
    mix_Triangles,
    mix_TriangleStrip,
    mix_TriangleFan,
    mix_Quads,
    mix_QuadStrip,
    mix_Polygon
};


void
setup_tri_pointers (void)
{
    drv_point = full_point;
    drv_line = full_line;
    drv_triangle = full_triangle;
    drv_quad = full_quad;
    drv_clippedPolygon = full_clippedPolygon;
    drv_prim_tab = drv_prim_full;
}
