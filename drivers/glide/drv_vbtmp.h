static void
TAG(emitvertices) (int n)
{
    int i;
    GrVertex *vertex = vb;
    const GLfloat *view = ctx_mx_viewport.mat;
    GLfloat oow;
    GLfloat sscale0, tscale0;
    GLfloat sscale1, tscale1;
    TEX_OBJ *obj;

    GLfloat4 *color0 = tnl_vb.attr[TNL_COLOR0].data;
    const GLuint color0_stride = tnl_vb.attr[TNL_COLOR0].stride;
#if (IND & SETUP_SPEC)
    GLfloat4 *color1 = tnl_vb.attr[TNL_COLOR1].data;
    const GLuint color1_stride = tnl_vb.attr[TNL_COLOR1].stride;
#endif
    GLfloat4 *tex0;
    GLuint tex0_stride;
    GLfloat4 *tex1;
    GLuint tex1_stride;
#if (IND & SETUP_FOGC)
    GLfloat4 *fog = tnl_vb.attr[TNL_FOGCOORD].data;
    const GLuint fog_stride = tnl_vb.attr[TNL_FOGCOORD].stride;
#endif

#if (IND & SETUP_TEX0)
    tex0 = tnl_vb.attr[TNL_TEXCOORD0 + tmu0_source].data;
    tex0_stride = tnl_vb.attr[TNL_TEXCOORD0 + tmu0_source].stride;
    sscale0 = tscale0 = 0.0F;
    obj = ctx_texture[tmu0_source].object;
    if (obj->driverData != NULL) {
	sscale0 = ((TFX_OBJ *)obj->driverData)->sscale;
	tscale0 = ((TFX_OBJ *)obj->driverData)->tscale;
    }
#endif

#if (IND & SETUP_TEX1)
    tex1 = tnl_vb.attr[TNL_TEXCOORD0 + tmu1_source].data;
    tex1_stride = tnl_vb.attr[TNL_TEXCOORD0 + tmu1_source].stride;
    sscale1 = tscale1 = 0.0F;
    obj = ctx_texture[tmu1_source].object;
    if (obj->driverData != NULL) {
	sscale1 = ((TFX_OBJ *)obj->driverData)->sscale;
	tscale1 = ((TFX_OBJ *)obj->driverData)->tscale;
    }
#endif

    for (i = 0; i < n; i++) {
#if FX_PACKEDCOLOR
	vertex->pargb[2] = CLAMPF(color0[0][0]) * 255.0F;
	vertex->pargb[1] = CLAMPF(color0[0][1]) * 255.0F;
	vertex->pargb[0] = CLAMPF(color0[0][2]) * 255.0F;
	vertex->pargb[3] = CLAMPF(color0[0][3]) * 255.0F;
#else
	vertex->r = CLAMPF(color0[0][0]) * 255.0F;
	vertex->g = CLAMPF(color0[0][1]) * 255.0F;
	vertex->b = CLAMPF(color0[0][2]) * 255.0F;
	vertex->a = CLAMPF(color0[0][3]) * 255.0F;
#endif
	color0 += color0_stride;

#if (IND & SETUP_SPEC)
#if FX_PACKEDCOLOR
	vertex->pspec[2] = CLAMPF(color1[0][0]) * 255.0F;
	vertex->pspec[1] = CLAMPF(color1[0][1]) * 255.0F;
	vertex->pspec[0] = CLAMPF(color1[0][2]) * 255.0F;
#else
	vertex->r1 = CLAMPF(color1[0][0]) * 255.0F;
	vertex->g1 = CLAMPF(color1[0][1]) * 255.0F;
	vertex->b1 = CLAMPF(color1[0][2]) * 255.0F;
#endif
	color1 += color1_stride;
#endif

	if (tnl_vb.clipmask[i] == 0) {
	    vertex->x   = tnl_vb.ndc[i][0] * view[0]  + view[12];
	    vertex->y   = tnl_vb.ndc[i][1] * view[5]  + view[13];
	    vertex->ooz = tnl_vb.ndc[i][2] * view[10] + view[14];
	    vertex->oow = tnl_vb.ndc[i][3];
	} else {
	    vertex->oow = 1.0F;
	}

#if (IND & (SETUP_TEX0 | SETUP_TEX1))
	oow = vertex->oow;
#endif

#if (IND & SETUP_TEX0)
	vertex->tmuvtx[0].sow = tex0[0][0] * sscale0 * oow;
	vertex->tmuvtx[0].tow = tex0[0][1] * tscale0 * oow;
#if (IND & SETUP_PTX0)
	vertex->tmuvtx[0].oow = tex0[0][3] * oow;
#endif
	tex0 += tex0_stride;
#endif

#if (IND & SETUP_TEX1)
	vertex->tmuvtx[1].sow = tex1[0][0] * sscale1 * oow;
	vertex->tmuvtx[1].tow = tex1[0][1] * tscale1 * oow;
#if (IND & SETUP_PTX1)
	vertex->tmuvtx[1].oow = tex1[0][3] * oow;
#endif
#if ((IND & (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1)) == (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1))
	/* tex0 projected + tex1 unprojected = emit fake q1 */
	vertex->tmuvtx[1].oow = oow;
#endif
	tex1 += tex1_stride;
#endif

#if (IND & SETUP_FOGC)
	vertex->fog = fog[0][0];
	fog += fog_stride;
#endif

	vertex++;
    }
}


static void
TAG(interp) (float t, int vdst, int vout, int vin)
{
    const GLfloat *view = ctx_mx_viewport.mat;
    GLfloat4 *clip = &tnl_vb.clip[vdst];
    GrVertex *dst = &vb[vdst];
    const GrVertex *in  = &vb[vin];
    const GrVertex *out = &vb[vout];
    const GLfloat oow = (clip[0][3] == 0.0F) ? 1.0F : (1.0F / clip[0][3]);
#if (IND & (SETUP_TEX0 | SETUP_TEX1))
    const GLfloat wout = oow / out->oow;
    const GLfloat win = oow / in->oow;
#endif

    dst->x   = clip[0][0] * oow * view[0]  + view[12];
    dst->y   = clip[0][1] * oow * view[5]  + view[13];
    dst->ooz = clip[0][2] * oow * view[10] + view[14];
    dst->oow = oow;

#if FX_PACKEDCOLOR
    INTERP_UB(t, dst->pargb[0], out->pargb[0], in->pargb[0]);
    INTERP_UB(t, dst->pargb[1], out->pargb[1], in->pargb[1]);
    INTERP_UB(t, dst->pargb[2], out->pargb[2], in->pargb[2]);
    INTERP_UB(t, dst->pargb[3], out->pargb[3], in->pargb[3]);
#else  /* !FX_PACKEDCOLOR */
    INTERP_F(t, dst->r, out->r, in->r);
    INTERP_F(t, dst->g, out->g, in->g);
    INTERP_F(t, dst->b, out->b, in->b);
    INTERP_F(t, dst->a, out->a, in->a);
#endif /* !FX_PACKEDCOLOR */

#if (IND & SETUP_SPEC)
#if FX_PACKEDCOLOR
    INTERP_UB(t, dst->pspec[0], out->pspec[0], in->pspec[0]);
    INTERP_UB(t, dst->pspec[1], out->pspec[1], in->pspec[1]);
    INTERP_UB(t, dst->pspec[2], out->pspec[2], in->pspec[2]);
#else  /* !FX_PACKEDCOLOR */
    INTERP_F(t, dst->r1, out->r1, in->r1);
    INTERP_F(t, dst->g1, out->g1, in->g1);
    INTERP_F(t, dst->b1, out->b1, in->b1);
#endif /* !FX_PACKEDCOLOR */
#endif

#if (IND & SETUP_TEX0)
    INTERP_F(t,
	     dst->tmuvtx[0].sow,
	     out->tmuvtx[0].sow * wout,
	     in->tmuvtx[0].sow * win);
    INTERP_F(t,
	     dst->tmuvtx[0].tow,
	     out->tmuvtx[0].tow * wout,
	     in->tmuvtx[0].tow * win);
#if (IND & SETUP_PTX0)
    INTERP_F(t,
	     dst->tmuvtx[0].oow,
	     out->tmuvtx[0].oow * wout,
	     in->tmuvtx[0].oow * win);
#endif
#endif

#if (IND & SETUP_TEX1)
    INTERP_F(t,
	     dst->tmuvtx[1].sow,
	     out->tmuvtx[1].sow * wout,
	     in->tmuvtx[1].sow * win);
    INTERP_F(t,
	     dst->tmuvtx[1].tow,
	     out->tmuvtx[1].tow * wout,
	     in->tmuvtx[1].tow * win);
#if (IND & SETUP_PTX1)
    INTERP_F(t,
	     dst->tmuvtx[1].oow,
	     out->tmuvtx[1].oow * wout,
	     in->tmuvtx[1].oow * win);
#endif
#if ((IND & (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1)) == (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1))
    /* tex0 projected + tex1 unprojected = interpolate fake q1 */
    INTERP_F(t,
	     dst->tmuvtx[1].oow,
	     out->tmuvtx[1].oow * wout,
	     in->tmuvtx[1].oow * win);
#endif
#endif

#if (IND & SETUP_FOGC)
    INTERP_F(t, dst->fog, out->fog, in->fog);
#endif
}


static void
TAG(init) (void)
{
    GLbitfield flags = 0;
#if (IND & SETUP_TEX1)
    flags |= GR_STWHINT_ST_DIFF_TMU1;
#endif
#if ((IND & (SETUP_TEX1|SETUP_PTX1)) == (SETUP_TEX1|SETUP_PTX1))
    flags |= GR_STWHINT_W_DIFF_TMU1;
#endif
#if ((IND & (SETUP_TEX0|SETUP_PTX0)) == (SETUP_TEX0|SETUP_PTX0))
    flags |= GR_STWHINT_W_DIFF_TMU0;
#endif
#if ((IND & (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1)) == (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1))
    /* tex0 projected + tex1 unprojected = detach from q0 */
    flags |= GR_STWHINT_W_DIFF_TMU1;
#endif
    vb_handler[IND].flags = flags;
    vb_handler[IND].emit_func = TAG(emitvertices);
#if (IND & SETUP_SPEC)
    vb_handler[IND].copypv_func = copypv;
#else
    vb_handler[IND].copypv_func = copypv_spec;
#endif
    vb_handler[IND].interp_func = TAG(interp);
}


#undef TAG
#undef IND
