void
TAG(tnl_light) (void)
{
    LIGHT *li;

    int i;

    GLfloat att;
    GLfloat dp;
    GLfloat4 l;
    GLfloat4 s;
    GLfloat4 c_front;
#if (IND & LIGHT_TWOSIDE)
    GLfloat4 c_back;
    GLfloat bp;
#endif

    GLfloat4 *mat_diff_front;
    GLuint mat_diff_front_stride;
    GLfloat4 *mat_amb_front;
    GLuint mat_amb_front_stride;
    GLfloat4 *mat_spec_front;
    GLuint mat_spec_front_stride;
    GLfloat4 *mat_shi_front;
    GLuint mat_shi_front_stride;
    GLfloat4 *mat_ems_front;
    GLuint mat_ems_front_stride;
#if (IND & LIGHT_TWOSIDE)
    GLfloat4 *mat_diff_back;
    GLuint mat_diff_back_stride;
    GLfloat4 *mat_amb_back;
    GLuint mat_amb_back_stride;
    GLfloat4 *mat_spec_back;
    GLuint mat_spec_back_stride;
    GLfloat4 *mat_shi_back;
    GLuint mat_shi_back_stride;
    GLfloat4 *mat_ems_back;
    GLuint mat_ems_back_stride;
#endif

    mat_diff_front = tnl_vb.attr[TNL_DIFFUSE_f].data;
    mat_diff_front_stride = tnl_vb.attr[TNL_DIFFUSE_f].stride;
    mat_amb_front = tnl_vb.attr[TNL_AMBIENT_f].data;
    mat_amb_front_stride = tnl_vb.attr[TNL_AMBIENT_f].stride;
    mat_spec_front = tnl_vb.attr[TNL_SPECULAR_f].data;
    mat_spec_front_stride = tnl_vb.attr[TNL_SPECULAR_f].stride;
    mat_shi_front = tnl_vb.attr[TNL_SHININESS_f].data;
    mat_shi_front_stride = tnl_vb.attr[TNL_SHININESS_f].stride;
    mat_ems_front = tnl_vb.attr[TNL_EMISSION_f].data;
    mat_ems_front_stride = tnl_vb.attr[TNL_EMISSION_f].stride;
#if (IND & LIGHT_TWOSIDE)
    mat_diff_back = tnl_vb.attr[TNL_DIFFUSE_b].data;
    mat_diff_back_stride = tnl_vb.attr[TNL_DIFFUSE_b].stride;
    mat_amb_back = tnl_vb.attr[TNL_AMBIENT_b].data;
    mat_amb_back_stride = tnl_vb.attr[TNL_AMBIENT_b].stride;
    mat_spec_back = tnl_vb.attr[TNL_SPECULAR_b].data;
    mat_spec_back_stride = tnl_vb.attr[TNL_SPECULAR_b].stride;
    mat_shi_back = tnl_vb.attr[TNL_SHININESS_b].data;
    mat_shi_back_stride = tnl_vb.attr[TNL_SHININESS_b].stride;
    mat_ems_back = tnl_vb.attr[TNL_EMISSION_b].data;
    mat_ems_back_stride = tnl_vb.attr[TNL_EMISSION_b].stride;
#endif

    if (ctx_colormat) {
	if (ctx_colormat_face != GL_BACK) { /* front */
	    switch (ctx_colormat_mode) {
		case GL_EMISSION:
		    mat_ems_front = tnl_vb.attr[TNL_COLOR0].data;
		    mat_ems_front_stride = tnl_vb.attr[TNL_COLOR0].stride;
		    break;
		case GL_AMBIENT:
		    mat_amb_front = tnl_vb.attr[TNL_COLOR0].data;
		    mat_amb_front_stride = tnl_vb.attr[TNL_COLOR0].stride;
		    break;
		case GL_DIFFUSE:
		    mat_diff_front = tnl_vb.attr[TNL_COLOR0].data;
		    mat_diff_front_stride = tnl_vb.attr[TNL_COLOR0].stride;
		    break;
		case GL_SPECULAR:
		    mat_spec_front = tnl_vb.attr[TNL_COLOR0].data;
		    mat_spec_front_stride = tnl_vb.attr[TNL_COLOR0].stride;
		    break;
		case GL_AMBIENT_AND_DIFFUSE:
		    mat_amb_front = tnl_vb.attr[TNL_COLOR0].data;
		    mat_amb_front_stride = tnl_vb.attr[TNL_COLOR0].stride;
		    mat_diff_front = tnl_vb.attr[TNL_COLOR0].data;
		    mat_diff_front_stride = tnl_vb.attr[TNL_COLOR0].stride;
		    break;
	    }
	}
#if (IND & LIGHT_TWOSIDE)
	if (ctx_colormat_face != GL_FRONT) { /* back */
	    switch (ctx_colormat_mode) {
		case GL_EMISSION:
		    mat_ems_back = tnl_vb.attr[TNL_COLOR0].data;
		    mat_ems_back_stride = tnl_vb.attr[TNL_COLOR0].stride;
		    break;
		case GL_AMBIENT:
		    mat_amb_back = tnl_vb.attr[TNL_COLOR0].data;
		    mat_amb_back_stride = tnl_vb.attr[TNL_COLOR0].stride;
		    break;
		case GL_DIFFUSE:
		    mat_diff_back = tnl_vb.attr[TNL_COLOR0].data;
		    mat_diff_back_stride = tnl_vb.attr[TNL_COLOR0].stride;
		    break;
		case GL_SPECULAR:
		    mat_spec_back = tnl_vb.attr[TNL_COLOR0].data;
		    mat_spec_back_stride = tnl_vb.attr[TNL_COLOR0].stride;
		    break;
		case GL_AMBIENT_AND_DIFFUSE:
		    mat_amb_back = tnl_vb.attr[TNL_COLOR0].data;
		    mat_amb_back_stride = tnl_vb.attr[TNL_COLOR0].stride;
		    mat_diff_back = tnl_vb.attr[TNL_COLOR0].data;
		    mat_diff_back_stride = tnl_vb.attr[TNL_COLOR0].stride;
		    break;
	    }
	}
#endif
    }

    /* do the lighting stage */
    tnl_vb.attr[TNL_COLOR0].data = tnl_vb.c0_front;
    tnl_vb.attr[TNL_COLOR0].stride = 1;
#if (IND & LIGHT_SEPSPEC)
    tnl_vb.attr[TNL_COLOR1].data = tnl_vb.c1_front;
    tnl_vb.attr[TNL_COLOR1].stride = 1;
#endif

    for (i = 0; i < tnl_vb.len; i++) {
	/* material emission and global ambient light */
	tnl_vb.c0_front[i][0] = mat_ems_front[0][0] + mat_amb_front[0][0] * ctx_light_model_ambient[0];
	tnl_vb.c0_front[i][1] = mat_ems_front[0][1] + mat_amb_front[0][1] * ctx_light_model_ambient[1];
	tnl_vb.c0_front[i][2] = mat_ems_front[0][2] + mat_amb_front[0][2] * ctx_light_model_ambient[2];
	tnl_vb.c0_front[i][3] = mat_diff_front[0][3];
#if (IND & LIGHT_SEPSPEC)
	tnl_vb.c1_front[i][0] = 0.0F;
	tnl_vb.c1_front[i][1] = 0.0F;
	tnl_vb.c1_front[i][2] = 0.0F;
#endif
#if (IND & LIGHT_TWOSIDE)
	tnl_vb.c0_back[i][0] = mat_ems_back[0][0] + mat_amb_back[0][0] * ctx_light_model_ambient[0];
	tnl_vb.c0_back[i][1] = mat_ems_back[0][1] + mat_amb_back[0][1] * ctx_light_model_ambient[1];
	tnl_vb.c0_back[i][2] = mat_ems_back[0][2] + mat_amb_back[0][2] * ctx_light_model_ambient[2];
	tnl_vb.c0_back[i][3] = mat_diff_back[0][3];
#if (IND & LIGHT_SEPSPEC)
	tnl_vb.c1_back[i][0] = 0.0F;
	tnl_vb.c1_back[i][1] = 0.0F;
	tnl_vb.c1_back[i][2] = 0.0F;
#endif
#endif
	list_foreach (li, &ctx_light_list) {
	    /* attenuation factor */
	    if (li->position[3] == 0.0F) {
		l[0] = li->_posnorm[0];
		l[1] = li->_posnorm[1];
		l[2] = li->_posnorm[2];
		att = 1.0F;
	    } else {
		l[0] = li->position[0] - tnl_vb.veye[i][0];
		l[1] = li->position[1] - tnl_vb.veye[i][1];
		l[2] = li->position[2] - tnl_vb.veye[i][2];
		dp = SQLEN3(l);
		att = li->attenuation[2] * dp;
		dp = SQRT(dp);
		att += li->attenuation[1] * dp;
		att += li->attenuation[0];
		if (att > 1e3F) {
		    continue;
		}
		att = 1.0F / att;
		dp = 1.0F / dp;
		l[0] *= dp;
		l[1] *= dp;
		l[2] *= dp;
	    }
	    /* spotlight effect */
	    if (li->spot_cutoff < 180.0F) {
		GLfloat spot = DOT3(li->_spot_eye_norm, l);
		if (spot < li->_spot_cutcos) {
		    continue;
		}
		att *= POW(spot, li->spot_exponent);
	    }
	    /* ambient component */
	    c_front[0] = mat_amb_front[0][0] * li->ambient[0];
	    c_front[1] = mat_amb_front[0][1] * li->ambient[1];
	    c_front[2] = mat_amb_front[0][2] * li->ambient[2];
	    /* diffuse component */
	    dp = DOT3(l, tnl_vb.neye[i]);
#if (IND & LIGHT_TWOSIDE)
	    bp = dp;
#endif
	    if (!IS_NEGATIVE(dp)) {
		c_front[0] += dp * mat_diff_front[0][0] * li->diffuse[0];
		c_front[1] += dp * mat_diff_front[0][1] * li->diffuse[1];
		c_front[2] += dp * mat_diff_front[0][2] * li->diffuse[2];
		/* specular component */
		if (ctx_light_model_localv) {
		    s[0] = l[0] - tnl_vb.veyn[i][0];
		    s[1] = l[1] - tnl_vb.veyn[i][1];
		    s[2] = l[2] - tnl_vb.veyn[i][2];
		    dp = SQLEN3(s);
		    dp = DOT3(s, tnl_vb.neye[i]) / SQRT(dp);
		} else if (li->position[3] != 0.0F) {
		    /* FIXMEEEE TODO XXX apply M, Mt?!? normalize 001?!? */
		    s[0] = l[0];
		    s[1] = l[1];
		    s[2] = l[2] + 1.0F;
		    dp = SQLEN3(s);
		    dp = DOT3(s, tnl_vb.neye[i]) / SQRT(dp);
		} else {
		    dp = DOT3(li->_svector, tnl_vb.neye[i]);
		}
		if (!IS_NEGATIVE(dp)) {
		    dp = pow_sf(dp, mat_shi_front[0][0]);
#if (!(IND & LIGHT_SEPSPEC))
		    c_front[0] += dp * mat_spec_front[0][0] * li->specular[0];
		    c_front[1] += dp * mat_spec_front[0][1] * li->specular[1];
		    c_front[2] += dp * mat_spec_front[0][2] * li->specular[2];
#else
		    tnl_vb.c1_front[i][0] += att * dp * mat_spec_front[0][0] * li->specular[0];
		    tnl_vb.c1_front[i][1] += att * dp * mat_spec_front[0][1] * li->specular[1];
		    tnl_vb.c1_front[i][2] += att * dp * mat_spec_front[0][2] * li->specular[2];
#endif
		}
	    }
	    tnl_vb.c0_front[i][0] += c_front[0] * att;
	    tnl_vb.c0_front[i][1] += c_front[1] * att;
	    tnl_vb.c0_front[i][2] += c_front[2] * att;
#if (IND & LIGHT_TWOSIDE)
	    /* ambient component */
	    c_back[0] = mat_amb_back[0][0] * li->ambient[0];
	    c_back[1] = mat_amb_back[0][1] * li->ambient[1];
	    c_back[2] = mat_amb_back[0][2] * li->ambient[2];
	    /* diffuse component */
	    dp = -bp;
	    if (!IS_NEGATIVE(dp)) {
		c_back[0] += dp * mat_diff_back[0][0] * li->diffuse[0];
		c_back[1] += dp * mat_diff_back[0][1] * li->diffuse[1];
		c_back[2] += dp * mat_diff_back[0][2] * li->diffuse[2];
		/* specular component */
		if (ctx_light_model_localv) {
		    s[0] = l[0] - tnl_vb.veyn[i][0];
		    s[1] = l[1] - tnl_vb.veyn[i][1];
		    s[2] = l[2] - tnl_vb.veyn[i][2];
		    dp = SQLEN3(s);
		    dp = DOT3(s, tnl_vb.neye[i]) / SQRT(dp);
		} else if (li->position[3] != 0.0F) {
		    /* FIXMEEEE TODO XXX apply M, Mt?!? normalize 001?!? */
		    s[0] = l[0];
		    s[1] = l[1];
		    s[2] = l[2] + 1.0F;
		    dp = SQLEN3(s);
		    dp = DOT3(s, tnl_vb.neye[i]) / SQRT(dp);
		} else {
		    dp = DOT3(li->_svector, tnl_vb.neye[i]);
		}
		dp = -dp;
		if (!IS_NEGATIVE(dp)) {
		    dp = pow_sf(dp, mat_shi_back[0][0]);
#if (!(IND & LIGHT_SEPSPEC))
		    c_back[0] += dp * mat_spec_back[0][0] * li->specular[0];
		    c_back[1] += dp * mat_spec_back[0][1] * li->specular[1];
		    c_back[2] += dp * mat_spec_back[0][2] * li->specular[2];
#else
		    tnl_vb.c1_back[i][0] += att * dp * mat_spec_back[0][0] * li->specular[0];
		    tnl_vb.c1_back[i][1] += att * dp * mat_spec_back[0][1] * li->specular[1];
		    tnl_vb.c1_back[i][2] += att * dp * mat_spec_back[0][2] * li->specular[2];
#endif
		}
	    }
	    tnl_vb.c0_back[i][0] += c_back[0] * att;
	    tnl_vb.c0_back[i][1] += c_back[1] * att;
	    tnl_vb.c0_back[i][2] += c_back[2] * att;
#endif
	}

	mat_diff_front += mat_diff_front_stride;
	mat_amb_front += mat_amb_front_stride;
	mat_spec_front += mat_spec_front_stride;
	mat_shi_front += mat_shi_front_stride;
	mat_ems_front += mat_ems_front_stride;
#if (IND & LIGHT_TWOSIDE)
	mat_diff_back += mat_diff_back_stride;
	mat_amb_back += mat_amb_back_stride;
	mat_spec_back += mat_spec_back_stride;
	mat_shi_back += mat_shi_back_stride;
	mat_ems_back += mat_ems_back_stride;
#endif
    }
}


#undef TAG
#undef IND
