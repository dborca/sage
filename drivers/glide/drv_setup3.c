#define TEXENV_OPERAND_INVERTED(operand)		\
    ((operand) == GL_ONE_MINUS_SRC_ALPHA || (operand) == GL_ONE_MINUS_SRC_COLOR)

#define TEXENV_OPERAND_ALPHA(operand)			\
    ((operand) == GL_SRC_ALPHA || (operand) == GL_ONE_MINUS_SRC_ALPHA)

#define TEXENV_SETUP_ARG_A(param, source, operand, prevAlpha) \
    switch (source) {					\
	case GL_TEXTURE:				\
	    param = GR_CMBX_LOCAL_TEXTURE_ALPHA;	\
	    break;					\
	case GL_CONSTANT:				\
	    param = GR_CMBX_TMU_CALPHA;			\
	    break;					\
	case GL_PRIMARY_COLOR:				\
	    param = GR_CMBX_ITALPHA;			\
	    break;					\
	case GL_PREVIOUS:				\
	    param = prevAlpha;				\
	    break;					\
	default:					\
	    param = GR_CMBX_ZERO;			\
	    break;					\
    }

#define TEXENV_SETUP_ARG_RGB(param, source, operand, prevColor, prevAlpha) \
    if (!TEXENV_OPERAND_ALPHA(operand)) {		\
	switch (source) {				\
	    case GL_TEXTURE:				\
		param = GR_CMBX_LOCAL_TEXTURE_RGB;	\
		break;					\
	    case GL_CONSTANT:				\
		param = GR_CMBX_TMU_CCOLOR;		\
		break;					\
	    case GL_PRIMARY_COLOR:			\
		param = GR_CMBX_ITRGB;			\
		break;					\
	    case GL_PREVIOUS:				\
		param = prevColor;			\
		break;					\
	    default:					\
		param = GR_CMBX_ZERO;			\
		break;					\
	}						\
    } else {						\
	TEXENV_SETUP_ARG_A(param, source, operand, prevAlpha)\
    }

#define TEXENV_SETUP_MODE(operand, flip)	\
    (TEXENV_OPERAND_INVERTED(operand) ^ (flip))	\
	? GR_FUNC_MODE_ONE_MINUS_X		\
	: GR_FUNC_MODE_X

struct tdfx_combine_color_ext {
   GrCCUColor_t SourceA;
   GrCombineMode_t ModeA;
   GrCCUColor_t SourceB;
   GrCombineMode_t ModeB;
   GrCCUColor_t SourceC;
   FxBool InvertC;
   GrCCUColor_t SourceD;
   FxBool InvertD;
   FxU32 Shift;
   FxBool Invert;
};


static void
setupTexEnvNapalm (int unit, int tmu, int iterated)
{
    TEX_UNIT *texUnit = &ctx_texture[unit];
    TEX_OBJ *texObj = texUnit->object;
    GLenum baseFormat;

    struct tdfx_combine_color_ext colorComb, alphaComb;
    GrCombineLocal_t upstream_color, upstream_alpha;

    if (iterated) {
	/* we don't have upstream TMU */
	upstream_color = GR_CMBX_ITRGB;
	upstream_alpha = GR_CMBX_ITALPHA;
    } else {
	/* we have upstream TMU */
	upstream_color = GR_CMBX_OTHER_TEXTURE_RGB;
	upstream_alpha = GR_CMBX_OTHER_TEXTURE_ALPHA;
    }

    baseFormat = texObj->baseFormat;

    colorComb.InvertD = FXFALSE;
    colorComb.Shift   = 0;
    colorComb.Invert  = FXFALSE;
    alphaComb.InvertD = FXFALSE;
    alphaComb.Shift   = 0;
    alphaComb.Invert  = FXFALSE;

    switch (texUnit->texenv) {
	case GL_REPLACE:
	    if (baseFormat == GL_ALPHA) {
		/* (p + 0) * 1 + 0 */
		colorComb.SourceA = upstream_color;
	    } else {
		/* (C + 0) * 1 + 0 */
		colorComb.SourceA = GR_CMBX_LOCAL_TEXTURE_RGB;
	    }
	    colorComb.ModeA   = GR_FUNC_MODE_X;
	    colorComb.SourceB = GR_CMBX_ZERO;
	    colorComb.ModeB   = GR_FUNC_MODE_X;
	    colorComb.SourceC = GR_CMBX_ZERO;
	    colorComb.InvertC = FXTRUE;
	    colorComb.SourceD = GR_CMBX_ZERO;
	    if (baseFormat == GL_LUMINANCE || baseFormat == GL_RGB) {
		/* (p + 0) * 1 + 0 */
		alphaComb.SourceA = upstream_alpha;
	    } else {
		/* (A + 0) * 1 + 0 */
		alphaComb.SourceA = GR_CMBX_LOCAL_TEXTURE_ALPHA;
	    }
	    alphaComb.ModeA   = GR_FUNC_MODE_X;
	    alphaComb.SourceB = GR_CMBX_ZERO;
	    alphaComb.ModeB   = GR_FUNC_MODE_X;
	    alphaComb.SourceC = GR_CMBX_ZERO;
	    alphaComb.InvertC = FXTRUE;
	    alphaComb.SourceD = GR_CMBX_ZERO;
	    break;
	case GL_MODULATE:
	    if (baseFormat == GL_ALPHA) {
		/* (p + 0) * 1 + 0 */
		colorComb.SourceC = GR_CMBX_ZERO;
		colorComb.InvertC = FXTRUE;
	    } else {
		/* (p + 0) * C + 0 */
		colorComb.SourceC = GR_CMBX_LOCAL_TEXTURE_RGB;
		colorComb.InvertC = FXFALSE;
	    }
	    colorComb.SourceA = upstream_color;
	    colorComb.ModeA   = GR_FUNC_MODE_X;
	    colorComb.SourceB = GR_CMBX_ZERO;
	    colorComb.ModeB   = GR_FUNC_MODE_X;
	    colorComb.SourceD = GR_CMBX_ZERO;
	    /* (p + 0) * A + 0 */
	    alphaComb.SourceA = upstream_alpha;
	    alphaComb.ModeA   = GR_FUNC_MODE_X;
	    alphaComb.SourceB = GR_CMBX_ZERO;
	    alphaComb.ModeB   = GR_FUNC_MODE_X;
	    alphaComb.SourceC = GR_CMBX_LOCAL_TEXTURE_ALPHA;
	    alphaComb.InvertC = FXFALSE;
	    alphaComb.SourceD = GR_CMBX_ZERO;
	    break;
	case GL_DECAL:
	    /* p * (1 - A) + C * A = (C - p) * A + p */
	    colorComb.SourceA = GR_CMBX_LOCAL_TEXTURE_RGB;
	    colorComb.ModeA   = GR_FUNC_MODE_X;
	    colorComb.SourceB = upstream_color;
	    colorComb.ModeB   = GR_FUNC_MODE_NEGATIVE_X;
	    colorComb.SourceC = GR_CMBX_LOCAL_TEXTURE_ALPHA;
	    colorComb.InvertC = FXFALSE;
	    colorComb.SourceD = GR_CMBX_B;
	    /* (f + 0) * 1 + 0 */
	    alphaComb.SourceA = upstream_alpha;
	    alphaComb.ModeA   = GR_FUNC_MODE_X;
	    alphaComb.SourceB = GR_CMBX_ZERO;
	    alphaComb.ModeB   = GR_FUNC_MODE_X;
	    alphaComb.SourceC = GR_CMBX_ZERO;
	    alphaComb.InvertC = FXTRUE;
	    alphaComb.SourceD = GR_CMBX_ZERO;
	    break;
	case GL_BLEND:
	    if (baseFormat == GL_ALPHA) {
		/* (f + 0) * 1 + 0 */
		colorComb.SourceA = upstream_color;
		colorComb.ModeA   = GR_FUNC_MODE_X;
		colorComb.SourceB = GR_CMBX_ZERO;
		colorComb.ModeB   = GR_FUNC_MODE_X;
		colorComb.SourceC = GR_CMBX_ZERO;
		colorComb.InvertC = FXTRUE;
		colorComb.SourceD = GR_CMBX_ZERO;
	    } else {
		/* p * (1 - C) + Cc * C = (Cc - p) * C + p */
		colorComb.SourceA = GR_CMBX_TMU_CCOLOR;
		colorComb.ModeA   = GR_FUNC_MODE_X;
		colorComb.SourceB = upstream_color;
		colorComb.ModeB   = GR_FUNC_MODE_NEGATIVE_X;
		colorComb.SourceC = GR_CMBX_LOCAL_TEXTURE_RGB;
		colorComb.InvertC = FXFALSE;
		colorComb.SourceD = GR_CMBX_B;
	    }
	    if (baseFormat == GL_INTENSITY) {
		/* p * (1 - A) + Ac * A = (Ac - p) * A + p */
		alphaComb.SourceA = GR_CMBX_TMU_CALPHA;
		alphaComb.ModeA   = GR_FUNC_MODE_X;
		alphaComb.SourceB = upstream_alpha;
		alphaComb.ModeB   = GR_FUNC_MODE_NEGATIVE_X;
		alphaComb.SourceC = GR_CMBX_LOCAL_TEXTURE_ALPHA;
		alphaComb.InvertC = FXFALSE;
		alphaComb.SourceD = GR_CMBX_B;
	    } else {
		/* (f + 0) * A + 0 */
		alphaComb.SourceA = upstream_alpha;
		alphaComb.ModeA   = GR_FUNC_MODE_X;
		alphaComb.SourceB = GR_CMBX_ZERO;
		alphaComb.ModeB   = GR_FUNC_MODE_X;
		alphaComb.SourceC = GR_CMBX_LOCAL_TEXTURE_ALPHA;
		alphaComb.InvertC = FXFALSE;
		alphaComb.SourceD = GR_CMBX_ZERO;
	    }
	    gfConstantColorValueExt(tmu,
		(((GLuint)(texUnit->envcolor[0] * 255.0f))      ) |
		(((GLuint)(texUnit->envcolor[1] * 255.0f)) <<  8) |
		(((GLuint)(texUnit->envcolor[2] * 255.0f)) << 16) |
		(((GLuint)(texUnit->envcolor[3] * 255.0f)) << 24));
	    break;
	case GL_ADD:
	    if (baseFormat == GL_ALPHA) {
		/* (p + 0) * 1 + 0 */
		colorComb.SourceA = upstream_color;
		colorComb.SourceB = GR_CMBX_ZERO;
	    } else {
		/* (p + C) * 1 + 0 */
		colorComb.SourceA = upstream_color;
		colorComb.SourceB = GR_CMBX_LOCAL_TEXTURE_RGB;
	    }
	    colorComb.ModeA   = GR_FUNC_MODE_X;
	    colorComb.ModeB   = GR_FUNC_MODE_X;
	    colorComb.SourceC = GR_CMBX_ZERO;
	    colorComb.InvertC = FXTRUE;
	    colorComb.SourceD = GR_CMBX_ZERO;
	    if (baseFormat == GL_INTENSITY) {
		/* (p + A) * 1 + 0 */
		alphaComb.SourceA = upstream_alpha;
		alphaComb.ModeA   = GR_FUNC_MODE_X;
		alphaComb.SourceB = GR_CMBX_LOCAL_TEXTURE_ALPHA;
		alphaComb.ModeB   = GR_FUNC_MODE_X;
		alphaComb.SourceC = GR_CMBX_ZERO;
		alphaComb.InvertC = FXTRUE;
		alphaComb.SourceD = GR_CMBX_ZERO;
	    } else {
		/* (p + 0) * A + 0 */
		alphaComb.SourceA = upstream_alpha;
		alphaComb.ModeA   = GR_FUNC_MODE_X;
		alphaComb.SourceB = GR_CMBX_ZERO;
		alphaComb.ModeB   = GR_FUNC_MODE_X;
		alphaComb.SourceC = GR_CMBX_LOCAL_TEXTURE_ALPHA;
		alphaComb.InvertC = FXFALSE;
		alphaComb.SourceD = GR_CMBX_ZERO;
	    }
	    break;
	case GL_COMBINE:
	    colorComb.Shift = texUnit->combineRGB.scale / 2;
	    alphaComb.Shift = texUnit->combineAlpha.scale / 2;
	    switch (texUnit->combineRGB.mode) {
		case GL_REPLACE:
		    /* Arg0 = (Arg0 + 0) * 1 + 0 */
		    TEXENV_SETUP_ARG_RGB(colorComb.SourceA,
					 texUnit->combineRGB.source[0],
					 texUnit->combineRGB.operand[0],
					 upstream_color, upstream_alpha);
		    colorComb.ModeA = TEXENV_SETUP_MODE(
					 texUnit->combineRGB.operand[0],
					 FXFALSE);
		    colorComb.SourceB = GR_CMBX_ZERO;
		    colorComb.ModeB   = GR_FUNC_MODE_ZERO;
		    colorComb.SourceC = GR_CMBX_ZERO;
		    colorComb.InvertC = FXTRUE;
		    colorComb.SourceD = GR_CMBX_ZERO;
		    break;
		case GL_MODULATE:
		    /* Arg0 * Arg1 = (Arg0 + 0) * Arg1 + 0 */
		    TEXENV_SETUP_ARG_RGB(colorComb.SourceA,
					 texUnit->combineRGB.source[0],
					 texUnit->combineRGB.operand[0],
					 upstream_color, upstream_alpha);
		    colorComb.ModeA = TEXENV_SETUP_MODE(
					 texUnit->combineRGB.operand[0],
					 FXFALSE);
		    colorComb.SourceB = GR_CMBX_ZERO;
		    colorComb.ModeB   = GR_FUNC_MODE_ZERO;
		    TEXENV_SETUP_ARG_RGB(colorComb.SourceC,
					 texUnit->combineRGB.source[1],
					 texUnit->combineRGB.operand[1],
					 upstream_color, upstream_alpha);
		    colorComb.InvertC = TEXENV_OPERAND_INVERTED(
					 texUnit->combineRGB.operand[1]);
		    colorComb.SourceD = GR_CMBX_ZERO;
		    break;
		case GL_ADD:
		    /* Arg0 + Arg1 = (Arg0 + Arg1) * 1 + 0 */
		    TEXENV_SETUP_ARG_RGB(colorComb.SourceA,
					 texUnit->combineRGB.source[0],
					 texUnit->combineRGB.operand[0],
					 upstream_color, upstream_alpha);
		    colorComb.ModeA = TEXENV_SETUP_MODE(
					 texUnit->combineRGB.operand[0],
					 FXFALSE);
		    TEXENV_SETUP_ARG_RGB(colorComb.SourceB,
					 texUnit->combineRGB.source[1],
					 texUnit->combineRGB.operand[1],
					 upstream_color, upstream_alpha);
		    colorComb.ModeB = TEXENV_SETUP_MODE(
					 texUnit->combineRGB.operand[1],
					 FXFALSE);
		    colorComb.SourceC = GR_CMBX_ZERO;
		    colorComb.InvertC = FXTRUE;
		    colorComb.SourceD = GR_CMBX_ZERO;
		    break;
		case GL_INTERPOLATE:
		    /* Arg0 * Arg2 + Arg1 * (1 - Arg2) =
		     * = (Arg0 - Arg1) * Arg2 + Arg1
		     */
		    colorComb.Invert  = TEXENV_OPERAND_INVERTED(
					 texUnit->combineRGB.operand[1]);
		    TEXENV_SETUP_ARG_RGB(colorComb.SourceA,
					 texUnit->combineRGB.source[0],
					 texUnit->combineRGB.operand[0],
					 upstream_color, upstream_alpha);
		    colorComb.ModeA = TEXENV_SETUP_MODE(
					 texUnit->combineRGB.operand[0],
					 colorComb.Invert);
		    TEXENV_SETUP_ARG_RGB(colorComb.SourceB,
					 texUnit->combineRGB.source[1],
					 texUnit->combineRGB.operand[1],
					 upstream_color, upstream_alpha);
		    colorComb.ModeB = GR_FUNC_MODE_NEGATIVE_X;
		    TEXENV_SETUP_ARG_RGB(colorComb.SourceC,
					 texUnit->combineRGB.source[2],
					 texUnit->combineRGB.operand[2],
					 upstream_color, upstream_alpha);
		    colorComb.InvertC = TEXENV_OPERAND_INVERTED(
					 texUnit->combineRGB.operand[2]);
		    colorComb.SourceD = GR_CMBX_B;
		    break;
		default:
		    gl_error(NULL, GL_INVALID_ENUM, "%s: COMBINE_color %x\n", __FUNCTION__, texUnit->combineRGB.mode);
	    }
	    switch (texUnit->combineAlpha.mode) {
		case GL_REPLACE:
		    /* Arg0 = (Arg0 + 0) * 1 + 0 */
		    TEXENV_SETUP_ARG_A(alphaComb.SourceA,
					texUnit->combineAlpha.source[0],
					texUnit->combineAlpha.operand[0],
					upstream_alpha);
		    alphaComb.ModeA = TEXENV_SETUP_MODE(
					texUnit->combineAlpha.operand[0],
					FXFALSE);
		    alphaComb.SourceB = GR_CMBX_ZERO;
		    alphaComb.ModeB   = GR_FUNC_MODE_ZERO;
		    alphaComb.SourceC = GR_CMBX_ZERO;
		    alphaComb.InvertC = FXTRUE;
		    alphaComb.SourceD = GR_CMBX_ZERO;
		    break;
		case GL_MODULATE:
		    /* Arg0 * Arg1 = (Arg0 + 0) * Arg1 + 0 */
		    TEXENV_SETUP_ARG_A(alphaComb.SourceA,
					texUnit->combineAlpha.source[0],
					texUnit->combineAlpha.operand[0],
					upstream_alpha);
		    alphaComb.ModeA = TEXENV_SETUP_MODE(
					texUnit->combineAlpha.operand[0],
					FXFALSE);
		    alphaComb.SourceB = GR_CMBX_ZERO;
		    alphaComb.ModeB   = GR_FUNC_MODE_ZERO;
		    TEXENV_SETUP_ARG_A(alphaComb.SourceC,
					texUnit->combineAlpha.source[1],
					texUnit->combineAlpha.operand[1],
					upstream_alpha);
		    alphaComb.InvertC = TEXENV_OPERAND_INVERTED(
					texUnit->combineAlpha.operand[1]);
		    alphaComb.SourceD = GR_CMBX_ZERO;
		    break;
		case GL_ADD:
		    /* Arg0 + Arg1 = (Arg0 + Arg1) * 1 + 0 */
		    TEXENV_SETUP_ARG_A(alphaComb.SourceA,
					texUnit->combineAlpha.source[0],
					texUnit->combineAlpha.operand[0],
					upstream_alpha);
		    alphaComb.ModeA = TEXENV_SETUP_MODE(
					texUnit->combineAlpha.operand[0],
					FXFALSE);
		    TEXENV_SETUP_ARG_A(alphaComb.SourceB,
					texUnit->combineAlpha.source[1],
					texUnit->combineAlpha.operand[1],
					upstream_alpha);
		    alphaComb.ModeB = TEXENV_SETUP_MODE(
					texUnit->combineAlpha.operand[1],
					FXFALSE);
		    alphaComb.SourceC = GR_CMBX_ZERO;
		    alphaComb.InvertC = FXTRUE;
		    alphaComb.SourceD = GR_CMBX_ZERO;
		    break;
		default:
		    gl_error(NULL, GL_INVALID_ENUM, "%s: COMBINE_color %x\n", __FUNCTION__, texUnit->combineAlpha.mode);
	    }
	    gfConstantColorValueExt(tmu,
		(((GLuint)(texUnit->envcolor[0] * 255.0f))      ) |
		(((GLuint)(texUnit->envcolor[1] * 255.0f)) <<  8) |
		(((GLuint)(texUnit->envcolor[2] * 255.0f)) << 16) |
		(((GLuint)(texUnit->envcolor[3] * 255.0f)) << 24));
	    break;
	default:
	    gl_error(NULL, GL_INVALID_ENUM, "%s: %x\n", __FUNCTION__, texUnit->texenv);
    }

    /* On Napalm we simply put the color combine unit into passthrough mode
     * and do everything we need with the texture combine units.
     */
    gfColorCombineExt(GR_CMBX_TEXTURE_RGB,
		      GR_FUNC_MODE_X,
		      GR_CMBX_ZERO,
		      GR_FUNC_MODE_X,
		      GR_CMBX_ZERO,
		      FXTRUE,
		      GR_CMBX_ZERO,
		      FXFALSE,
		      0,
		      FXFALSE);
    gfAlphaCombineExt(GR_CMBX_TEXTURE_ALPHA,
		      GR_FUNC_MODE_X,
		      GR_CMBX_ZERO,
		      GR_FUNC_MODE_X,
		      GR_CMBX_ZERO,
		      FXTRUE,
		      GR_CMBX_ZERO,
		      FXFALSE,
		      0,
		      FXFALSE);

    gfTexColorCombineExt(tmu,
			 colorComb.SourceA,
			 colorComb.ModeA,
			 colorComb.SourceB,
			 colorComb.ModeB,
			 colorComb.SourceC,
			 colorComb.InvertC,
			 colorComb.SourceD,
			 colorComb.InvertD,
			 colorComb.Shift,
			 colorComb.Invert);
    gfTexAlphaCombineExt(tmu,
			 alphaComb.SourceA,
			 alphaComb.ModeA,
			 alphaComb.SourceB,
			 alphaComb.ModeB,
			 alphaComb.SourceC,
			 alphaComb.InvertC,
			 alphaComb.SourceD,
			 alphaComb.InvertD,
			 alphaComb.Shift,
			 alphaComb.Invert);
}


static void
setupTextureSingleNapalm (int unit)
{
    TEX_OBJ *texObj = ctx_texture[unit].object;
    TFX_OBJ *fxObj = (TFX_OBJ *)texObj->driverData;
    int tmu;

    if (!setupTexture(texObj)) {
	return;
    }

    if (!fxObj->in) {
	tmu = tm_best_tmu(unit, texObj);
	tmu_movein(texObj, tmu);
    } else {
	tmu = fxObj->tmu;
	if (fxObj->tmu == GR_TMU_BOTH) {
	    tmu = unit;
	}
    }

    grTexClampMode(tmu, fxObj->sclamp, fxObj->tclamp);
    grTexMipMapMode(tmu, fxObj->mipmap, FXFALSE);
    grTexFilterMode(tmu, fxObj->minFilt, fxObj->magFilt);
    grTexLodBiasValue(tmu, texObj->bias + ctx_texture[unit].bias); /* XXX clamp */
    grTexSource(tmu,
                fxObj->range[tmu]->start,
                GR_MIPMAPLEVELMASK_BOTH,
                (GrTexInfo *)fxObj);


    setupTexEnvNapalm(unit, tmu, GL_TRUE);

    if (tmu == unit) {
	tmu0_source = GR_TMU0;
	tmu1_source = GR_TMU1;
    } else {
	tmu0_source = GR_TMU1;
	tmu1_source = GR_TMU0;
    }

    if (tmu == GR_TMU0) {
	if (allow_multitex) {
	    /* disable TMU1 */
	    gfTexAlphaCombineExt(GR_TMU1,
				 GR_CMBX_ZERO,
				 GR_FUNC_MODE_ZERO,
				 GR_CMBX_ZERO,
				 GR_FUNC_MODE_ZERO,
				 GR_CMBX_ZERO,
				 FXTRUE,
				 GR_CMBX_ZERO,
				 FXFALSE,
				 0,
				 FXFALSE);
	    gfTexColorCombineExt(GR_TMU1,
				 GR_CMBX_ZERO,
				 GR_FUNC_MODE_ZERO,
				 GR_CMBX_ZERO,
				 GR_FUNC_MODE_ZERO,
				 GR_CMBX_ZERO,
				 FXTRUE,
				 GR_CMBX_ZERO,
				 FXFALSE,
				 0,
				 FXFALSE);
	}
	/* TMU0 is already set */
    } else {
	if (allow_multitex) {
	    /* TMU1 is already set */
	}
	/* TMU0 in passthrough mode */
#if !FX_PASSTMUHACK
	grTexCombine(GR_TMU0,
		     GR_COMBINE_FUNCTION_SCALE_OTHER,
		     GR_COMBINE_FACTOR_ONE,
		     GR_COMBINE_FUNCTION_SCALE_OTHER,
		     GR_COMBINE_FACTOR_ONE,
		     FXFALSE,
		     FXFALSE);
#else
	grTexCombine(GR_TMU0,
		     GR_COMBINE_FUNCTION_BLEND,
		     GR_COMBINE_FACTOR_ONE,
		     GR_COMBINE_FUNCTION_BLEND,
		     GR_COMBINE_FACTOR_ONE,
		     FXFALSE,
		     FXFALSE);
#endif
	/* XXX does it work?
	gfTexColorCombineExt(GR_TMU0,
			     GR_CMBX_OTHER_TEXTURE_RGB,
			     GR_FUNC_MODE_X,
			     GR_CMBX_ZERO,
			     GR_FUNC_MODE_X,
			     GR_CMBX_ZERO,
			     FXTRUE,
			     GR_CMBX_ZERO,
			     FXFALSE,
			     0,
			     FXFALSE);
	gfTexAlphaCombineExt(GR_TMU0,
			     GR_CMBX_OTHER_TEXTURE_ALPHA,
			     GR_FUNC_MODE_X,
			     GR_CMBX_ZERO,
			     GR_FUNC_MODE_X,
			     GR_CMBX_ZERO,
			     FXTRUE,
			     GR_CMBX_ZERO,
			     FXFALSE,
			     0,
			     FXFALSE);
	*/
    }

#if FX_PASSTMUHACK
    if (0 == unit) {
	tmu0_source = GR_TMU0;
	tmu1_source = GR_TMU1;
    } else {
	tmu0_source = GR_TMU1;
	tmu1_source = GR_TMU0;
    }
#endif
}


static void
setupTextureMultiNapalm (void)
{
    TEX_OBJ *texObj0 = ctx_texture[0].object;
    TEX_OBJ *texObj1 = ctx_texture[1].object;
    TFX_OBJ *fxObj0 = texObj0->driverData;
    TFX_OBJ *fxObj1 = texObj1->driverData;

    /* OpenGL: 0 -> 1
     * Glide : 1 -> 0
     */

    if (!setupTexture(texObj0)) {
	return;
    }
    if (!setupTexture(texObj1)) {
	return;
    }

    if (fxObj0->in) {
	if (fxObj0->tmu != GR_TMU1 && fxObj0->tmu != GR_TMU_BOTH) {
	    if (fxObj0->tmu == GR_TMU_SPLIT) {
		tmu_moveout(texObj0);
	    }
	    tmu_movein(texObj0, GR_TMU1);
	}
    } else {
	tmu_movein(texObj0, GR_TMU1);
    }
    if (fxObj1->in) {
	if (fxObj1->tmu != GR_TMU0 && fxObj1->tmu != GR_TMU_BOTH) {
	    if (fxObj1->tmu == GR_TMU_SPLIT) {
		tmu_moveout(texObj1);
	    }
	    tmu_movein(texObj1, GR_TMU0);
	}
    } else {
	tmu_movein(texObj1, GR_TMU0);
    }

    grTexClampMode(GR_TMU0, fxObj1->sclamp, fxObj1->tclamp);
    grTexMipMapMode(GR_TMU0, fxObj1->mipmap, FXFALSE);
    grTexFilterMode(GR_TMU0, fxObj1->minFilt, fxObj1->magFilt);
    grTexLodBiasValue(GR_TMU0, texObj1->bias + ctx_texture[1].bias); /* XXX clamp */
    grTexSource(GR_TMU0,
                fxObj1->range[GR_TMU0]->start,
                GR_MIPMAPLEVELMASK_BOTH,
                (GrTexInfo *)fxObj1);
    grTexClampMode(GR_TMU1, fxObj0->sclamp, fxObj0->tclamp);
    grTexMipMapMode(GR_TMU1, fxObj0->mipmap, FXFALSE);
    grTexFilterMode(GR_TMU1, fxObj0->minFilt, fxObj0->magFilt);
    grTexLodBiasValue(GR_TMU1, texObj0->bias + ctx_texture[0].bias); /* XXX clamp */
    grTexSource(GR_TMU1,
                fxObj0->range[GR_TMU1]->start,
                GR_MIPMAPLEVELMASK_BOTH,
                (GrTexInfo *)fxObj0);

    tmu0_source = GR_TMU1;
    tmu1_source = GR_TMU0;

    setupTexEnvNapalm(0, GR_TMU1, GL_TRUE);
    setupTexEnvNapalm(1, GR_TMU0, GL_FALSE);
}


static void
setupTextureNoneNapalm (void)
{
    /* the combiner formula is: (A + B) * C + D
     *
     * a = tc_otherselect
     * a_mode = tc_invert_other
     * b = tc_localselect
     * b_mode = tc_invert_local
     * c = (tc_mselect, tc_mselect_7)
     * d = (tc_add_clocal, tc_add_alocal)
     * shift = tc_outshift
     * invert = tc_invert_output
     */
    gfColorCombineExt(GR_CMBX_ITRGB,
		      GR_FUNC_MODE_X,
		      GR_CMBX_ZERO,
		      GR_FUNC_MODE_ZERO,
		      GR_CMBX_ZERO,
		      FXTRUE,
		      GR_CMBX_ZERO,
		      FXFALSE,
		      0,
		      FXFALSE);
    gfAlphaCombineExt(GR_CMBX_ITALPHA,
		      GR_FUNC_MODE_X,
		      GR_CMBX_ZERO,
		      GR_FUNC_MODE_ZERO,
		      GR_CMBX_ZERO,
		      FXTRUE,
		      GR_CMBX_ZERO,
		      FXFALSE,
		      0,
		      FXFALSE);
}
