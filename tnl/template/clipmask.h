static void
TAG(_tnl_clipmask) (void)
{
    int i = 0, j;
    GLuint k;
    GLfloat oow;

    for (k = 0; k < tnl_prim_num; k++) {
	tnl_prim[k].ormask = 0;
	for (j = 0; j < tnl_prim[k].count; j++) {
	    GLbitfield mask = 0;
#if (!(IND & V_NOCLIP))
	    /* set up clipmask */
	    const GLfloat cx = tnl_vb.clip[i][0];
	    const GLfloat cy = tnl_vb.clip[i][1];
	    const GLfloat cz = tnl_vb.clip[i][2];
	    const GLfloat cw = tnl_vb.clip[i][3];
#if 0
	    if (-cx + cw < 0) mask |= TNL_CLIP_RIGHT;
	    if ( cx + cw < 0) mask |= TNL_CLIP_LEFT;
	    if (-cy + cw < 0) mask |= TNL_CLIP_TOP;
	    if ( cy + cw < 0) mask |= TNL_CLIP_BOTTOM;
	    if (-cz + cw < 0) mask |= TNL_CLIP_FAR;
	    if ( cz + cw < 0) mask |= TNL_CLIP_NEAR;
#else
#define TNL_CLIP_NEAR_SHIFT   (0)
#define TNL_CLIP_FAR_SHIFT    (1)
#define TNL_CLIP_TOP_SHIFT    (2)
#define TNL_CLIP_BOTTOM_SHIFT (3)
#define TNL_CLIP_LEFT_SHIFT   (4)
#define TNL_CLIP_RIGHT_SHIFT  (5)
#define TNL_CLIP_USER_SHIFT   (6)
	    mask |= IS_NEGATIVE(cw - cx) << TNL_CLIP_RIGHT_SHIFT;
	    mask |= IS_NEGATIVE(cw + cx) << TNL_CLIP_LEFT_SHIFT;
	    mask |= IS_NEGATIVE(cw - cy) << TNL_CLIP_TOP_SHIFT;
	    mask |= IS_NEGATIVE(cw + cy) << TNL_CLIP_BOTTOM_SHIFT;
	    mask |= IS_NEGATIVE(cw - cz) << TNL_CLIP_FAR_SHIFT;
	    mask |= IS_NEGATIVE(cw + cz) << TNL_CLIP_NEAR_SHIFT;
#endif
#if (IND & V_USERCLIP)
	    {
		int u, planes = ctx_userclip;
		for (u = 0; u < TNL_USERCLIP_PLANES; u++) {
		    if (planes & 1) {
			GLfloat dp = cx * ctx_userplanes[1][u][0] +
				     cy * ctx_userplanes[1][u][1] +
				     cz * ctx_userplanes[1][u][2] +
				     cw * ctx_userplanes[1][u][3];
			if (IS_NEGATIVE(dp)) {
			    mask |= TNL_CLIP_USER;
			    break;
			}
		    }
		    planes >>= 1;
		}
	    }
#endif
#endif
	    tnl_vb.clipmask[i] = mask;
	    tnl_prim[k].ormask |= mask;
	    if (!mask) {
		/* ndc (normalized device coords) */
		oow = (tnl_vb.clip[i][3] == 0.0F) ? 1.0F : (1.0F / tnl_vb.clip[i][3]);
		tnl_vb.ndc[i][0] = tnl_vb.clip[i][0] * oow;
		tnl_vb.ndc[i][1] = tnl_vb.clip[i][1] * oow;
		tnl_vb.ndc[i][2] = tnl_vb.clip[i][2] * oow;
		tnl_vb.ndc[i][3] = oow; /* essentially 1.0, but we keep oow */
	    }
	    i++;
	}
    }
}


static void
TAG(_tnl_init_clipmask) (void)
{
    tnl_clipmask_tab[IND] = TAG(_tnl_clipmask);
}


#undef TAG
#undef IND
