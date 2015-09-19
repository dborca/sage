static void
TAG(_tnl_texgen) (int unit)
{
    int i;

    GLfloat4 *output = tnl_vb.texgen[unit];
    GLfloat4 *texcoord = tnl_vb.attr[unit + TNL_TEXCOORD0].data;
    const GLuint texcoord_stride = tnl_vb.attr[unit + TNL_TEXCOORD0].stride;
    for (i = 0; i < tnl_vb.len; i++) {
#if (IND & TNL_GEN_S_BIT)
	output[0][0] = _tnl_gen_tab_0[ctx_texture[unit].genmode[0]](unit, i);
#else
	output[0][0] = texcoord[0][0];
#endif
#if (IND & TNL_GEN_T_BIT)
	output[0][1] = _tnl_gen_tab_1[ctx_texture[unit].genmode[1]](unit, i);
#else
	output[0][1] = texcoord[0][1];
#endif
#if (IND & TNL_GEN_R_BIT)
	output[0][2] = _tnl_gen_tab_2[ctx_texture[unit].genmode[2]](unit, i);
#else
	output[0][2] = texcoord[0][2];
#endif
#if (IND & TNL_GEN_Q_BIT)
	output[0][3] = _tnl_gen_tab_3[ctx_texture[unit].genmode[3]](unit, i);
#else
	output[0][3] = texcoord[0][3];
#endif
	output++;
	texcoord += texcoord_stride;
    }
}


static void
TAG(_tnl_texgen_init) (void)
{
    tnl_texgen_tab[IND] = TAG(_tnl_texgen);
}


#undef TAG
#undef IND
