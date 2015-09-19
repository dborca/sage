#ifdef CMB_STEP1
static void
print_unit_combine (int unit)
{
    TEX_UNIT *texUnit = &ctx_texture[unit];
    gl_cry(__FILE__, __LINE__, __FUNCTION__, "%d\n"
	"\tCOMBINE_RGB = %x"
	"\tCOMBINE_ALPHA = %x\n"
	"\tSOURCE0_RGB = %x"
	"\tSOURCE0_ALPHA = %x\n"
	"\tSOURCE1_RGB = %x"
	"\tSOURCE1_ALPHA = %x\n"
	"\tSOURCE2_RGB = %x"
	"\tSOURCE2_ALPHA = %x\n"
	"\tOPERAND0_RGB = %x"
	"\tOPERAND0_ALPHA = %x\n"
	"\tOPERAND1_RGB = %x"
	"\tOPERAND1_ALPHA = %x\n"
	"\tOPERAND2_RGB = %x"
	"\tOPERAND2_ALPHA = %x\n"
	"\tRGB_SCALE = %x\t"
	"\tALPHA_SCALE = %x\n",
	unit,
	texUnit->combineRGB.mode,
	texUnit->combineAlpha.mode,
	texUnit->combineRGB.source[0],
	texUnit->combineAlpha.source[0],
	texUnit->combineRGB.source[1],
	texUnit->combineAlpha.source[1],
	texUnit->combineRGB.source[2],
	texUnit->combineAlpha.source[2],
	texUnit->combineRGB.operand[0],
	texUnit->combineAlpha.operand[0],
	texUnit->combineRGB.operand[1],
	texUnit->combineAlpha.operand[1],
	texUnit->combineRGB.operand[2],
	texUnit->combineAlpha.operand[2],
	texUnit->combineRGB.scale,
	texUnit->combineAlpha.scale);
}


#define CMB_INIT() GLint okRGB = 0, okAlp = 0

#define CMB_CHECK2(rgb, alp, all)			\
    if ((rgb) && !okRGB) {				\
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "\n"	\
	    "\tGL_COMBINE_RGB = %x\n",			\
	    texUnit->combineRGB.mode);			\
    }							\
    if ((alp) && !okAlp) {				\
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "\n"	\
	    "\tGL_COMBINE_ALPHA = %x\n",		\
	    texUnit->combineAlpha.mode);		\
    }							\
    if ((all) && (!okRGB || !okAlp)) {			\
	print_unit_combine(unit);			\
    }

#define CMB_CHECK3(rgb, alp, all)			\
    if ((rgb) && !okRGB) {				\
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "\n"	\
	    "\tGL_COMBINE_RGB = %x GL_COMBINE_RGB = %x\n",\
	    texUnit0->combineRGB.mode, texUnit1->combineRGB.mode);\
    }							\
    if ((alp) && !okAlp) {				\
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "\n"	\
	    "\tGL_COMBINE_ALPHA = %x GL_COMBINE_ALPHA = %x\n",\
	    texUnit0->combineAlpha.mode, texUnit1->combineAlpha.mode);\
    }							\
    if ((all) && (!okRGB || !okAlp)) {			\
	print_unit_combine(0);				\
	print_unit_combine(1);				\
    }

#define OK_RGB(d)					\
    do {						\
	okRGB = d;					\
	/*gl_cry(__FILE__, __LINE__, __FUNCTION__, "RGB+OK: %d\n", okRGB);*/\
    } while (0)

#define OK_ALP(d)					\
    do {						\
	okAlp = d;					\
	/*gl_cry(__FILE__, __LINE__, __FUNCTION__, "Alp+OK: %d\n", okAlp);*/\
    } while (0)
#endif


#ifdef CMB_STEP2
    case GL_COMBINE: {
	TEX_UNIT *texUnit = &ctx_texture[unit];
	CMB_INIT();
	if (texUnit->combineRGB.mode == GL_REPLACE &&
	    texUnit->combineRGB.source[0] == GL_TEXTURE &&
	    texUnit->combineRGB.operand[0] == GL_SRC_COLOR) {
	    /* XXX we cannot handle scale */
	    CC = f_texture;
	    OK_RGB(21);
	} else if (texUnit->combineRGB.mode == GL_REPLACE &&
	    texUnit->combineRGB.source[0] == GL_CONSTANT &&
	    texUnit->combineRGB.operand[0] == GL_SRC_COLOR) {
	    /* XXX we cannot handle scale */
	    CC = f_constant;
	    grConstantColorValue(
		(((GLuint)(texUnit->envcolor[0] * 255.0f))      ) |
		(((GLuint)(texUnit->envcolor[1] * 255.0f)) <<  8) |
		(((GLuint)(texUnit->envcolor[2] * 255.0f)) << 16) |
		(((GLuint)(texUnit->envcolor[3] * 255.0f)) << 24));
	    OK_RGB(22);
	} else if (texUnit->combineRGB.mode == GL_REPLACE &&
	    texUnit->combineRGB.source[0] == GL_PRIMARY_COLOR &&
	    texUnit->combineRGB.operand[0] == GL_SRC_COLOR) {
	    /* XXX we cannot handle scale */
	    CC = f_local;
	    OK_RGB(23);
	} else if (texUnit->combineRGB.mode == GL_MODULATE &&
	    texUnit->combineRGB.source[0] == GL_CONSTANT &&
	    texUnit->combineRGB.source[1] == GL_PRIMARY_COLOR &&
	    texUnit->combineRGB.operand[0] == GL_SRC_COLOR &&
	    texUnit->combineRGB.operand[1] == GL_SRC_COLOR) {
	    /* XXX we cannot handle scale */
	    CC = f_mul_constant;
	    grConstantColorValue(
		(((GLuint)(texUnit->envcolor[0] * 255.0f))      ) |
		(((GLuint)(texUnit->envcolor[1] * 255.0f)) <<  8) |
		(((GLuint)(texUnit->envcolor[2] * 255.0f)) << 16) |
		(((GLuint)(texUnit->envcolor[3] * 255.0f)) << 24));
	    OK_RGB(24);
	} else if (texUnit->combineRGB.mode == GL_MODULATE &&
	    texUnit->combineRGB.source[0] == GL_TEXTURE &&
	    (texUnit->combineRGB.source[1] == GL_PRIMARY_COLOR || texUnit->combineRGB.source[1] == GL_PREVIOUS) &&
	    texUnit->combineRGB.operand[0] == GL_SRC_COLOR &&
	    texUnit->combineRGB.operand[1] == GL_SRC_COLOR) {
	    /* XXX we cannot handle scale */
	    CC = f_mul_texture;
	    OK_RGB(25);
	}
	if (texUnit->combineAlpha.mode == GL_REPLACE &&
	    texUnit->combineAlpha.source[0] == GL_TEXTURE &&
	    texUnit->combineAlpha.operand[0] == GL_SRC_ALPHA) {
	    /* XXX we cannot handle scale */
	    AC = f_texture;
	    OK_ALP(21);
	} else if (texUnit->combineAlpha.mode == GL_REPLACE &&
	    texUnit->combineAlpha.source[0] == GL_CONSTANT &&
	    texUnit->combineAlpha.operand[0] == GL_SRC_ALPHA) {
	    /* XXX we cannot handle scale */
	    AC = f_constant;
	    grConstantColorValue(
		(((GLuint)(texUnit->envcolor[0] * 255.0f))      ) |
		(((GLuint)(texUnit->envcolor[1] * 255.0f)) <<  8) |
		(((GLuint)(texUnit->envcolor[2] * 255.0f)) << 16) |
		(((GLuint)(texUnit->envcolor[3] * 255.0f)) << 24));
	    OK_ALP(22);
	} else if (texUnit->combineAlpha.mode == GL_REPLACE &&
	    texUnit->combineAlpha.source[0] == GL_PRIMARY_COLOR &&
	    texUnit->combineAlpha.operand[0] == GL_SRC_ALPHA) {
	    /* XXX we cannot handle scale */
	    AC = f_local;
	    OK_ALP(23);
	} else if (texUnit->combineAlpha.mode == GL_MODULATE &&
	    texUnit->combineAlpha.source[0] == GL_CONSTANT &&
	    texUnit->combineAlpha.source[1] == GL_PRIMARY_COLOR &&
	    texUnit->combineAlpha.operand[0] == GL_SRC_ALPHA &&
	    texUnit->combineAlpha.operand[1] == GL_SRC_ALPHA) {
	    /* XXX we cannot handle scale */
	    AC = f_mul_constant;
	    grConstantColorValue(
		(((GLuint)(texUnit->envcolor[0] * 255.0f))      ) |
		(((GLuint)(texUnit->envcolor[1] * 255.0f)) <<  8) |
		(((GLuint)(texUnit->envcolor[2] * 255.0f)) << 16) |
		(((GLuint)(texUnit->envcolor[3] * 255.0f)) << 24));
	    OK_ALP(24);
	} else if (texUnit->combineAlpha.mode == GL_MODULATE &&
	    texUnit->combineAlpha.source[0] == GL_TEXTURE &&
	    (texUnit->combineAlpha.source[1] == GL_PRIMARY_COLOR || texUnit->combineAlpha.source[1] == GL_PREVIOUS) &&
	    texUnit->combineAlpha.operand[0] == GL_SRC_ALPHA &&
	    texUnit->combineAlpha.operand[1] == GL_SRC_ALPHA) {
	    /* XXX we cannot handle scale */
	    AC = f_mul_texture;
	    OK_ALP(25);
	}
	CMB_CHECK2(0, 0, 0);
	break;
    }
#endif


#ifdef CMB_STEP3
    } else if (texEnv0 == GL_COMBINE && texEnv1 == GL_COMBINE) {
	TEX_UNIT *texUnit0 = &ctx_texture[0];
	TEX_UNIT *texUnit1 = &ctx_texture[1];
	CMB_INIT();
	if (texUnit1->combineRGB.mode == GL_REPLACE &&
	    texUnit1->combineRGB.source[0] == GL_PREVIOUS &&
	    texUnit1->combineRGB.operand[0] == GL_SRC_COLOR) {
	    if (texUnit0->combineRGB.mode == GL_MODULATE &&
		texUnit0->combineRGB.source[0] == GL_TEXTURE &&
		texUnit0->combineRGB.source[1] == GL_PRIMARY_COLOR &&
		texUnit0->combineRGB.operand[0] == GL_SRC_COLOR &&
		texUnit0->combineRGB.operand[1] == GL_SRC_COLOR) {
		/* XXX we cannot handle scale */
		TC1C = t_local;
		TC0C = t_other;
		CC = f_mul_texture;
		OK_RGB(31);
	    } else if (texUnit0->combineRGB.mode == GL_REPLACE &&
		texUnit0->combineRGB.source[0] == GL_TEXTURE &&
		texUnit0->combineRGB.operand[0] == GL_SRC_COLOR) {
		/* XXX we cannot handle scale */
		TC1C = t_local;
		TC0C = t_other;
		CC = f_texture;
		OK_RGB(32);
	    }
	} else if (texUnit1->combineRGB.mode == GL_MODULATE &&
	    texUnit1->combineRGB.source[0] == GL_TEXTURE &&
	    texUnit1->combineRGB.source[1] == GL_PREVIOUS &&
	    texUnit1->combineRGB.operand[0] == GL_SRC_COLOR &&
	    texUnit1->combineRGB.operand[1] == GL_SRC_COLOR) {
	    if (texUnit0->combineRGB.mode == GL_MODULATE &&
		texUnit0->combineRGB.source[0] == GL_TEXTURE &&
		texUnit0->combineRGB.source[1] == GL_PRIMARY_COLOR &&
		texUnit0->combineRGB.operand[0] == GL_SRC_COLOR &&
		texUnit0->combineRGB.operand[1] == GL_SRC_COLOR) {
		/* XXX we cannot handle scale */
		TC1C = t_local;
		TC0C = t_mul_other;
		CC = f_mul_texture;
		OK_RGB(33);
	    } else if (texUnit0->combineRGB.mode == GL_REPLACE &&
		texUnit0->combineRGB.source[0] == GL_TEXTURE &&
		texUnit0->combineRGB.operand[0] == GL_SRC_COLOR) {
		/* XXX we cannot handle scale */
		TC1C = t_local;
		TC0C = t_mul_other;
		CC = f_texture;
		OK_RGB(34);
	    }
#if 0
	} else if (texUnit1->combineRGB.mode == GL_INTERPOLATE &&
	    texUnit1->combineRGB.source[0] == GL_TEXTURE &&
	    texUnit1->combineRGB.source[1] == GL_PREVIOUS &&
	    texUnit1->combineRGB.source[2] == GL_PREVIOUS &&
	    texUnit1->combineRGB.operand[0] == GL_SRC_COLOR &&
	    texUnit1->combineRGB.operand[1] == GL_SRC_COLOR &&
	    texUnit1->combineRGB.operand[2] == GL_SRC_ALPHA) {
	    if (texUnit0->combineRGB.mode == GL_MODULATE &&
		texUnit0->combineRGB.source[0] == GL_TEXTURE &&
		texUnit0->combineRGB.source[1] == GL_PRIMARY_COLOR &&
		texUnit0->combineRGB.operand[0] == GL_SRC_COLOR) {
		/* XXX we cannot handle scale */
		/* XXX not correct, depends on OTHER_ALPHA */
		TC1C = t_local;
		TC0C = t_custom4;
		CC = f_mul_texture;
		OK_RGB(666);
	    }
#endif
	}
	if (texUnit1->combineAlpha.mode == GL_REPLACE &&
	    texUnit1->combineAlpha.source[0] == GL_PREVIOUS &&
	    texUnit1->combineAlpha.operand[0] == GL_SRC_ALPHA) {
	    if (texUnit0->combineAlpha.mode == GL_MODULATE &&
		texUnit0->combineAlpha.source[0] == GL_TEXTURE &&
		texUnit0->combineAlpha.source[1] == GL_PRIMARY_COLOR &&
		texUnit0->combineAlpha.operand[0] == GL_SRC_ALPHA &&
		texUnit0->combineAlpha.operand[1] == GL_SRC_ALPHA) {
		/* XXX we cannot handle scale */
		TC1A = t_local;
		TC0A = t_other;
		AC = f_mul_texture;
		OK_ALP(31);
	    } else if (texUnit0->combineAlpha.mode == GL_REPLACE &&
		texUnit0->combineAlpha.source[0] == GL_TEXTURE &&
		texUnit0->combineAlpha.operand[0] == GL_SRC_ALPHA) {
		/* XXX we cannot handle scale */
		TC1A = t_local;
		TC0A = t_other;
		AC = f_texture;
		OK_ALP(32);
	    }
	} else if (texUnit1->combineAlpha.mode == GL_REPLACE &&
	    texUnit1->combineAlpha.source[0] == GL_TEXTURE &&
	    texUnit1->combineAlpha.operand[0] == GL_SRC_ALPHA) {
		/* XXX we cannot handle scale */
		TC1A = t_zero;
		TC0A = t_local;
		AC = f_texture;
		OK_ALP(33);
	}
	CMB_CHECK3(0, 0, 0);
    } else if (texEnv0 == GL_MODULATE && texEnv1 == GL_COMBINE) {
	/* ala SeriousSam2 */
	TEX_UNIT *texUnit0 = &ctx_texture[0];
	TEX_UNIT *texUnit1 = &ctx_texture[1];
	CMB_INIT();
	if (texUnit1->combineRGB.mode == GL_MODULATE &&
	    texUnit1->combineRGB.source[0] == GL_TEXTURE &&
	    texUnit1->combineRGB.source[1] == GL_PREVIOUS &&
	    texUnit1->combineRGB.operand[0] == GL_SRC_COLOR &&
	    texUnit1->combineRGB.operand[1] == GL_SRC_COLOR) {
	    /* XXX we cannot handle scale */
	    TC1C = t_local;
	    TC0C = t_mul_other;
	    CC = f_mul_texture;
	    if (baseFormat0 == GL_ALPHA) {
		TC1C = t_one;
	    }
	    OK_RGB(10);
	}
	if (texUnit1->combineAlpha.mode == GL_MODULATE &&
	    texUnit1->combineAlpha.source[0] == GL_TEXTURE &&
	    texUnit1->combineAlpha.source[1] == GL_PREVIOUS &&
	    texUnit1->combineAlpha.operand[0] == GL_SRC_ALPHA &&
	    texUnit1->combineAlpha.operand[1] == GL_SRC_ALPHA) {
	    /* XXX we cannot handle scale */
	    TC1A = t_local;
	    TC0A = t_mul_other;
	    AC = f_mul_texture;
	    OK_ALP(10);
	}
	CMB_CHECK3(0, 0, 0);
    } else if (texEnv0 == GL_COMBINE && texEnv1 == GL_MODULATE) {
	/* ala Doom3 */
	TEX_UNIT *texUnit0 = &ctx_texture[0];
	TEX_UNIT *texUnit1 = &ctx_texture[1];
	CMB_INIT();
	if (texUnit0->combineRGB.mode == GL_MODULATE &&
	    texUnit0->combineRGB.source[0] == GL_TEXTURE &&
	    texUnit0->combineRGB.source[1] == GL_PRIMARY_COLOR &&
	    texUnit0->combineRGB.operand[0] == GL_SRC_COLOR &&
	    texUnit0->combineRGB.operand[1] == GL_ONE_MINUS_SRC_COLOR) {
	    /* XXX we cannot handle scale */
	    TC1C = t_local;
	    TC0C = t_mul_other;
	    CC = f_custom4;
	    if (baseFormat0 == GL_ALPHA) {
		TC0C = t_other;
	    }
	    OK_RGB(11);
	}
	if (texUnit0->combineAlpha.mode == GL_MODULATE &&
	    texUnit0->combineAlpha.source[0] == GL_TEXTURE &&
	    texUnit0->combineAlpha.source[1] == GL_PREVIOUS &&
	    texUnit0->combineAlpha.operand[0] == GL_SRC_ALPHA &&
	    texUnit0->combineAlpha.operand[1] == GL_SRC_ALPHA) {
	    /* XXX we cannot handle scale */
	    TC1A = t_local;
	    TC0A = t_mul_other;
	    AC = f_mul_texture;
	    OK_ALP(11);
	}
	CMB_CHECK3(0, 0, 0);
#endif


#undef CMB_STEP3
#undef CMB_STEP2
#undef CMB_STEP1
