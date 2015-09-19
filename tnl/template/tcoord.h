static GLfloat
TAG(_tnl_gen_object_linear) (int unit, int i)
{
    return DOT4(ctx_texture[unit].objplane[IND], tnl_vb.attr[TNL_VERTEX].data[i]);
}


static GLfloat
TAG(_tnl_gen_eye_linear) (int unit, int i)
{
    return DOT4(ctx_texture[unit].eyeplane[IND], tnl_vb.veye[i]);
}


static GLfloat
TAG(_tnl_gen_sphere_map) (int unit, int i)
{
    return tnl_vb.refl[i][IND] * tnl_vb.refl[i][3] + 0.5F;
}


static GLfloat
TAG(_tnl_gen_reflection_map) (int unit, int i)
{
    return tnl_vb.refl[i][IND];
}


static GLfloat
TAG(_tnl_gen_normal_map) (int unit, int i)
{
    return tnl_vb.neye[i][IND];
}


static GEN_FUNC TAG(_tnl_gen_tab)[] = {
    TAG(_tnl_gen_object_linear),
    TAG(_tnl_gen_eye_linear),
    TAG(_tnl_gen_sphere_map),
    TAG(_tnl_gen_reflection_map),
    TAG(_tnl_gen_normal_map)
};


#undef TAG
#undef IND
