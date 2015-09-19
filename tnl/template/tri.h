/*
 * Mixed-mode rendering.
 *
 * Bits:
 *
 * RENDER_FLAT
 * RENDER_OFFSET
 * RENDER_TWOSIDE
 * RENDER_UNFILLED
 * RENDER_SPECULAR
 * RENDER_TWOSTENCIL
 *
 * User-provided macros:
 *
 * VERTEX_X(i)
 * VERTEX_Y(i)
 * VERTEX_Z(i)
 * SAVE_RGBA(i)
 * LOAD_RGBA(i)
 * COPY_RGBA(i, j)
 * IMPORT_RGBA(i, ptr)
 * SAVE_SPEC(i)
 * LOAD_SPEC(i)
 * COPY_SPEC(i, j)
 * IMPORT_SPEC(i, ptr)
 * INIT()
 * SETUP_STENCIL(f)
 * UNSET_STENCIL(f)
 */


static void
TAG(mix_line) (int v0, int v1)
{
    INIT();

    if (IND & RENDER_FLAT) {
	SAVE_RGBA(0);
	COPY_RGBA(0, 1);
	if (IND & RENDER_SPECULAR) {
	    SAVE_SPEC(0);
	    COPY_SPEC(0, 1);
	}
    }
    DRV_LINE(v0, v1);
    if (IND & RENDER_FLAT) {
	LOAD_RGBA(0);
	if (IND & RENDER_SPECULAR) {
	    LOAD_SPEC(0);
	}
    }
}


static void
TAG(mix_triangle) (int v0, int v1, int v2)
{
    INIT();
    GLfloat offset, z[3];
    GLenum mode = POLYGON_FILL;
    GLfloat x1x0, x2x1, y1y0, y2y1;
    GLfloat area;
    GLboolean facing;

    if (IND & (RENDER_OFFSET|RENDER_UNFILLED|RENDER_TWOSIDE|RENDER_TWOSTENCIL)) {
	y2y1 = VERTEX_Y(2) - VERTEX_Y(1);
	x1x0 = VERTEX_X(1) - VERTEX_X(0);
	y1y0 = VERTEX_Y(1) - VERTEX_Y(0);
	x2x1 = VERTEX_X(2) - VERTEX_X(1);
	area = x1x0 * y2y1 - x2x1 * y1y0;
	if (IND & (RENDER_UNFILLED|RENDER_TWOSIDE|RENDER_TWOSTENCIL)) {
	    facing = IS_NEGATIVE(area) ^ (ctx_front_face == GL_CCW);
	    if (ctx_culling) {
		if (facing) {
		    if (ctx_cull_face != GL_BACK) { /* FRONT[_AND_BACK] */
			return;
		    }
		} else {
		    if (ctx_cull_face != GL_FRONT) { /* [FRONT_AND_]BACK */
			return;
		    }
		}
	    }
	}
    }
    if ((IND & RENDER_TWOSIDE) && !facing) {
	SAVE_RGBA(0);
	SAVE_RGBA(1);
	SAVE_RGBA(2);
	IMPORT_RGBA(2, tnl_vb.c0_back);
	if (IND & RENDER_FLAT) {
	    COPY_RGBA(0, 2);
	    COPY_RGBA(1, 2);
	} else {
	    IMPORT_RGBA(0, tnl_vb.c0_back);
	    IMPORT_RGBA(1, tnl_vb.c0_back);
	}
	if (IND & RENDER_SPECULAR) {
	    SAVE_SPEC(0);
	    SAVE_SPEC(1);
	    SAVE_SPEC(2);
	    IMPORT_SPEC(2, tnl_vb.c1_back);
	    if (IND & RENDER_FLAT) {
		COPY_SPEC(0, 2);
		COPY_SPEC(1, 2);
	    } else {
		IMPORT_SPEC(0, tnl_vb.c1_back);
		IMPORT_SPEC(1, tnl_vb.c1_back);
	    }
	}
    } else if (IND & RENDER_FLAT) {
	SAVE_RGBA(0);
	SAVE_RGBA(1);
	COPY_RGBA(0, 2);
	COPY_RGBA(1, 2);
	if (IND & RENDER_SPECULAR) {
	    SAVE_SPEC(0);
	    SAVE_SPEC(1);
	    COPY_SPEC(0, 2);
	    COPY_SPEC(1, 2);
	}
    }
    if (IND & RENDER_UNFILLED) {
	if (facing) {
	    mode = ctx_polygon_mode[0];
	} else {
	    mode = ctx_polygon_mode[1];
	}
    }
    if ((IND & RENDER_OFFSET) && (mode & ctx_polygon_offset_mode)) {
	GLfloat z1z0, z2z1;
	GLfloat one_area, dzdx, dzdy, dz;
	z[0] = VERTEX_Z(0);
	z[1] = VERTEX_Z(1);
	z[2] = VERTEX_Z(2);
	z1z0 = z[1] - z[0];
	z2z1 = z[2] - z[1];
	one_area = 1.0F / area;
	dzdx = (y2y1 * z1z0 - y1y0 * z2z1) * one_area;
	dzdy = (x1x0 * z2z1 - x2x1 * z1z0) * one_area;
	if (IS_NEGATIVE(dzdx)) dzdx = -dzdx;
	if (IS_NEGATIVE(dzdy)) dzdy = -dzdy;
	dz = MAX2(dzdx, dzdy);
	offset = dz * ctx_polygon_offset_factor + ctx_polygon_offset_bias;
	VERTEX_Z(0) += offset;
	VERTEX_Z(1) += offset;
	VERTEX_Z(2) += offset;
    }

    if (mode == POLYGON_FILL) {
	if ((IND & RENDER_TWOSTENCIL) && 0/* stencil_test_enabled && stencil_twoside_enabled */) {
	    SETUP_STENCIL(facing);
	    DRV_TRIANGLE(v0, v1, v2);
	    UNSET_STENCIL(facing);
	} else {
	    DRV_TRIANGLE(v0, v1, v2);
	}
    } else if (mode == POLYGON_LINE) {
	DRV_LINE(v0, v1);
	DRV_LINE(v1, v2);
	DRV_LINE(v2, v0);
    } else if (mode == POLYGON_POINT) {
	DRV_POINT(v0);
	DRV_POINT(v1);
	DRV_POINT(v2);
    }

    if ((IND & RENDER_OFFSET) && (mode & ctx_polygon_offset_mode)) {
	VERTEX_Z(0) = z[0];
	VERTEX_Z(1) = z[1];
	VERTEX_Z(2) = z[2];
    }
    if ((IND & RENDER_TWOSIDE) && !facing) {
	LOAD_RGBA(0);
	LOAD_RGBA(1);
	LOAD_RGBA(2);
	if (IND & RENDER_SPECULAR) {
	    LOAD_SPEC(0);
	    LOAD_SPEC(1);
	    LOAD_SPEC(2);
	}
    } else if (IND & RENDER_FLAT) {
	LOAD_RGBA(0);
	LOAD_RGBA(1);
	if (IND & RENDER_SPECULAR) {
	    LOAD_SPEC(0);
	    LOAD_SPEC(1);
	}
    }
}


static void
TAG(mix_quad) (int v0, int v1, int v2, int v3)
{
#if 0
    TAG(mix_triangle)(v0, v1, v3);
    TAG(mix_triangle)(v1, v2, v3);
#else
    INIT();
    GLfloat offset, z[4];
    GLenum mode = POLYGON_FILL;
    GLfloat x1x0, x3x1, y1y0, y3y1;
    GLfloat area;
    GLboolean facing;

    if (IND & (RENDER_OFFSET|RENDER_UNFILLED|RENDER_TWOSIDE|RENDER_TWOSTENCIL)) {
	y3y1 = VERTEX_Y(3) - VERTEX_Y(1);
	x1x0 = VERTEX_X(1) - VERTEX_X(0);
	y1y0 = VERTEX_Y(1) - VERTEX_Y(0);
	x3x1 = VERTEX_X(3) - VERTEX_X(1);
	area = x1x0 * y3y1 - x3x1 * y1y0;
	if (IND & (RENDER_UNFILLED|RENDER_TWOSIDE|RENDER_TWOSTENCIL)) {
	    facing = IS_NEGATIVE(area) ^ (ctx_front_face == GL_CCW);
	    if (ctx_culling) {
		if (facing) {
		    if (ctx_cull_face != GL_BACK) { /* FRONT[_AND_BACK] */
			return;
		    }
		} else {
		    if (ctx_cull_face != GL_FRONT) { /* [FRONT_AND_]BACK */
			return;
		    }
		}
	    }
	}
    }
    if ((IND & RENDER_TWOSIDE) && !facing) {
	SAVE_RGBA(0);
	SAVE_RGBA(1);
	SAVE_RGBA(2);
	SAVE_RGBA(3);
	IMPORT_RGBA(3, tnl_vb.c0_back);
	if (IND & RENDER_FLAT) {
	    COPY_RGBA(0, 3);
	    COPY_RGBA(1, 3);
	    COPY_RGBA(2, 3);
	} else {
	    IMPORT_RGBA(0, tnl_vb.c0_back);
	    IMPORT_RGBA(1, tnl_vb.c0_back);
	    IMPORT_RGBA(2, tnl_vb.c0_back);
	}
	if (IND & RENDER_SPECULAR) {
	    SAVE_SPEC(0);
	    SAVE_SPEC(1);
	    SAVE_SPEC(2);
	    SAVE_SPEC(3);
	    IMPORT_SPEC(3, tnl_vb.c1_back);
	    if (IND & RENDER_FLAT) {
		COPY_SPEC(0, 3);
		COPY_SPEC(1, 3);
		COPY_SPEC(2, 3);
	    } else {
		IMPORT_SPEC(0, tnl_vb.c1_back);
		IMPORT_SPEC(1, tnl_vb.c1_back);
		IMPORT_SPEC(2, tnl_vb.c1_back);
	    }
	}
    } else if (IND & RENDER_FLAT) {
	SAVE_RGBA(0);
	SAVE_RGBA(1);
	SAVE_RGBA(2);
	COPY_RGBA(0, 3);
	COPY_RGBA(1, 3);
	COPY_RGBA(2, 3);
	if (IND & RENDER_SPECULAR) {
	    SAVE_SPEC(0);
	    SAVE_SPEC(1);
	    SAVE_SPEC(2);
	    COPY_SPEC(0, 3);
	    COPY_SPEC(1, 3);
	    COPY_SPEC(2, 3);
	}
    }
    if (IND & RENDER_UNFILLED) {
	if (facing) {
	    mode = ctx_polygon_mode[0];
	} else {
	    mode = ctx_polygon_mode[1];
	}
    }
    if ((IND & RENDER_OFFSET) && (mode & ctx_polygon_offset_mode)) {
	GLfloat z1z0, z3z1;
	GLfloat one_area, dzdx, dzdy, dz;
	z[0] = VERTEX_Z(0);
	z[1] = VERTEX_Z(1);
	z[2] = VERTEX_Z(2);
	z[3] = VERTEX_Z(3);
	z1z0 = z[1] - z[0];
	z3z1 = z[3] - z[1];
	one_area = 1.0F / area;
	dzdx = (y3y1 * z1z0 - y1y0 * z3z1) * one_area;
	dzdy = (x1x0 * z3z1 - x3x1 * z1z0) * one_area;
	if (IS_NEGATIVE(dzdx)) dzdx = -dzdx;
	if (IS_NEGATIVE(dzdy)) dzdy = -dzdy;
	dz = MAX2(dzdx, dzdy);
	offset = dz * ctx_polygon_offset_factor + ctx_polygon_offset_bias;
	VERTEX_Z(0) += offset;
	VERTEX_Z(1) += offset;
	VERTEX_Z(2) += offset;
	VERTEX_Z(3) += offset;
    }

    if (mode == POLYGON_FILL) {
	if ((IND & RENDER_TWOSTENCIL) && 0/* stencil_test_enabled && stencil_twoside_enabled */) {
	    SETUP_STENCIL(facing);
	    DRV_QUAD(v0, v1, v2, v3);
	    UNSET_STENCIL(facing);
	} else {
	    DRV_QUAD(v0, v1, v2, v3);
	}
    } else if (mode == POLYGON_LINE) {
	DRV_LINE(v0, v1);
	DRV_LINE(v1, v2);
	DRV_LINE(v2, v3);
	DRV_LINE(v3, v0);
    } else if (mode == POLYGON_POINT) {
	DRV_POINT(v0);
	DRV_POINT(v1);
	DRV_POINT(v2);
	DRV_POINT(v3);
    }

    if ((IND & RENDER_OFFSET) && (mode & ctx_polygon_offset_mode)) {
	VERTEX_Z(0) = z[0];
	VERTEX_Z(1) = z[1];
	VERTEX_Z(2) = z[2];
	VERTEX_Z(3) = z[3];
    }
    if ((IND & RENDER_TWOSIDE) && !facing) {
	LOAD_RGBA(0);
	LOAD_RGBA(1);
	LOAD_RGBA(2);
	LOAD_RGBA(3);
	if (IND & RENDER_SPECULAR) {
	    LOAD_SPEC(0);
	    LOAD_SPEC(1);
	    LOAD_SPEC(2);
	    LOAD_SPEC(3);
	}
    } else if (IND & RENDER_FLAT) {
	LOAD_RGBA(0);
	LOAD_RGBA(1);
	LOAD_RGBA(2);
	if (IND & RENDER_SPECULAR) {
	    LOAD_SPEC(0);
	    LOAD_SPEC(1);
	    LOAD_SPEC(2);
	}
    }
#endif
}


static void
TAG(mix_init) (void)
{
    mix_line_tab[IND] = TAG(mix_line);
    mix_triangle_tab[IND] = TAG(mix_triangle);
    mix_quad_tab[IND] = TAG(mix_quad);
}


#undef TAG
#undef IND
