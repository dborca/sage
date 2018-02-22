#ifndef DRV_H_included
#define DRV_H_included


#ifndef FX_PACKEDCOLOR
#define FX_PACKEDCOLOR 1
#endif
#ifndef FX_PASSTMUHACK
#define FX_PASSTMUHACK 1
#endif
#ifndef FX_RESCALEHACK
#define FX_RESCALEHACK 1
#endif


enum {
      GR_SSTTYPE_VOODOO  = 0,
      GR_SSTTYPE_SST96   = 1,
      GR_SSTTYPE_AT3D    = 2,
      GR_SSTTYPE_Voodoo2 = 3,
      GR_SSTTYPE_Banshee = 4,
      GR_SSTTYPE_Voodoo3 = 5,
      GR_SSTTYPE_Voodoo4 = 6,
      GR_SSTTYPE_Voodoo5 = 7
};


#define GrState                 void


typedef struct {
    float sow;		/* s texture ordinate (s over w) */
    float tow;		/* t texture ordinate (t over w) */
    float oow;		/* 1/w (used mipmapping - really 0xfff/w) */
} GrTmuVertex;

#if FX_PACKEDCOLOR
typedef struct {
    float x, y;		/* X and Y in screen space */
    float ooz;		/* 65535/Z (used for Z-buffering) */
    float oow;		/* 1/W (used for W-buffering, texturing) */
    unsigned char pargb[4];	/* B, G, R, A [0..255] */
    GrTmuVertex tmuvtx[GLIDE_NUM_TMU];
    float fog;		/* fog coordinate */
    float psize;		/* point size */
    unsigned char pspec[4];	/* B, G, R, A [0..255] */
    int pad[16 - 14];	/* ensure 64b structure */
} GrVertex;

#define GR_VERTEX_X_OFFSET              0
#define GR_VERTEX_Y_OFFSET              1
#define GR_VERTEX_OOZ_OFFSET            2
#define GR_VERTEX_OOW_OFFSET            3
#define GR_VERTEX_PARGB_OFFSET          4
#define GR_VERTEX_SOW_TMU0_OFFSET       5
#define GR_VERTEX_TOW_TMU0_OFFSET       6
#define GR_VERTEX_OOW_TMU0_OFFSET       7
#define GR_VERTEX_SOW_TMU1_OFFSET       8
#define GR_VERTEX_TOW_TMU1_OFFSET       9
#define GR_VERTEX_OOW_TMU1_OFFSET       10
#define GR_VERTEX_FOG_OFFSET            11
#define GR_VERTEX_PSPEC_OFFSET          13
#else  /* !FX_PACKEDCOLOR */
typedef struct {
    float x, y;		/* X and Y in screen space */
    float ooz;		/* 65535/Z (used for Z-buffering) */
    float oow;		/* 1/W (used for W-buffering, texturing) */
    float r, g, b, a;	/* R, G, B, A [0..255] */
    GrTmuVertex tmuvtx[GLIDE_NUM_TMU];
    float fog;		/* fog coordinate */
    float psize;		/* point size */
    float r1, g1, b1;	/* R, G, B [0..255] */
    int pad[20 - 19];	/* ensure multiple of 16 */
} GrVertex;

#define GR_VERTEX_X_OFFSET              0
#define GR_VERTEX_Y_OFFSET              1
#define GR_VERTEX_OOZ_OFFSET            2
#define GR_VERTEX_OOW_OFFSET            3
#define GR_VERTEX_RGB_OFFSET            4
#define GR_VERTEX_A_OFFSET              7
#define GR_VERTEX_SOW_TMU0_OFFSET       8
#define GR_VERTEX_TOW_TMU0_OFFSET       9
#define GR_VERTEX_OOW_TMU0_OFFSET       10
#define GR_VERTEX_SOW_TMU1_OFFSET       11
#define GR_VERTEX_TOW_TMU1_OFFSET       12
#define GR_VERTEX_OOW_TMU1_OFFSET       13
#define GR_VERTEX_FOG_OFFSET            14
#define GR_VERTEX_SPEC_OFFSET           16
#endif /* !FX_PACKEDCOLOR */


typedef FxU32 GrHint_t;
#define GR_HINTTYPE_MIN         0
#define GR_HINT_STWHINT         0

typedef FxU32 GrSTWHint_t;
#define GR_STWHINT_W_DIFF_FBI   FXBIT(0)
#define GR_STWHINT_W_DIFF_TMU0  FXBIT(1)
#define GR_STWHINT_ST_DIFF_TMU0 FXBIT(2)
#define GR_STWHINT_W_DIFF_TMU1  FXBIT(3)
#define GR_STWHINT_ST_DIFF_TMU1 FXBIT(4)
#define GR_STWHINT_W_DIFF_TMU2  FXBIT(5)
#define GR_STWHINT_ST_DIFF_TMU2 FXBIT(6)


typedef struct MEM_RANGE {
    struct MEM_RANGE *next;
    unsigned int start, end;
} MEM_RANGE;


typedef struct {
    GrTexInfo info;
    TEX_OBJ *parent;
    GLfloat sscale, tscale;
    GLuint tmu;
    GLboolean in;
    GLint minLevel, maxLevel;
    GLboolean validated;
    MEM_RANGE *range[2];
    GrTextureClampMode_t sclamp, tclamp;
    GLboolean mipmap;
    GrTextureFilterMode_t minFilt, magFilt;
} TFX_OBJ;


typedef struct {
    TEX_IMG *parent;
    GLuint width, height;
} TFX_IMG;


extern GrVertex *vb;
extern char renderer_name[64];
extern const char *opengl_ver;
extern GrFog_t *fogtable;
extern int screen_width, screen_height;
extern int tmu0_source, tmu1_source;
extern GLuint tex_bound_mask;
extern int fb_color, fb_alpha;
extern int max_texture_levels;
extern GrBuffer_t render_buffer;


extern int allow_texuma;
extern int allow_texmirror;
extern int allow_fogcoord;
extern int allow_texfmt;
extern int allow_32bpt;
extern int allow_combine;
extern int allow_blendsquare;
extern int allow_compressed;
extern int allow_multitex;


typedef char * (FX_CALL *gfGetRegistryOrEnvironmentStringExt_f) (char *theEntry);
typedef GrContext_t (FX_CALL *gfSstWinOpenExt_f) (FxU32 hWnd, GrScreenResolution_t resolution, GrScreenRefresh_t refresh, GrColorFormat_t format, GrOriginLocation_t origin, GrPixelFormat_t pixelformat, int nColBuffers, int nAuxBuffers);
typedef void (FX_CALL *gfBufferClearExt_f) (GrColor_t color, GrAlpha_t alpha, FxU32 depth, GrStencil_t stencil);
typedef void (FX_CALL *gfColorMaskExt_f) (FxBool r, FxBool g, FxBool b, FxBool a);
typedef void (FX_CALL *gfStencilFuncExt_f) (GrCmpFnc_t fnc, GrStencil_t ref, GrStencil_t mask);
typedef void (FX_CALL *gfStencilMaskExt_f) (GrStencil_t value);
typedef void (FX_CALL *gfStencilOpExt_f) (GrStencilOp_t stencil_fail, GrStencilOp_t depth_fail, GrStencilOp_t depth_pass);
typedef void (FX_CALL *gfColorCombineExt_f) (GrCCUColor_t a, GrCombineMode_t a_mode, GrCCUColor_t b, GrCombineMode_t b_mode, GrCCUColor_t c, FxBool c_invert, GrCCUColor_t d, FxBool d_invert, FxU32 shift, FxBool invert);
typedef void (FX_CALL *gfAlphaCombineExt_f) (GrACUColor_t a, GrCombineMode_t a_mode, GrACUColor_t b, GrCombineMode_t b_mode, GrACUColor_t c, FxBool c_invert, GrACUColor_t d, FxBool d_invert, FxU32 shift, FxBool invert);
typedef void (FX_CALL *gfTexColorCombineExt_f) (GrChipID_t tmu, GrTCCUColor_t a, GrCombineMode_t a_mode, GrTCCUColor_t b, GrCombineMode_t b_mode, GrTCCUColor_t c, FxBool c_invert, GrTCCUColor_t d, FxBool d_invert, FxU32 shift, FxBool invert);
typedef void (FX_CALL *gfTexAlphaCombineExt_f) (GrChipID_t tmu, GrTACUColor_t a, GrCombineMode_t a_mode, GrTACUColor_t b, GrCombineMode_t b_mode, GrTACUColor_t c, FxBool c_invert, GrTACUColor_t d, FxBool d_invert, FxU32 shift, FxBool invert);
typedef void (FX_CALL *gfConstantColorValueExt_f) (GrChipID_t tmu, GrColor_t value);

extern gfGetRegistryOrEnvironmentStringExt_f gfGetRegistryOrEnvironmentStringExt;
extern gfSstWinOpenExt_f gfSstWinOpenExt;
extern gfBufferClearExt_f gfBufferClearExt;
extern gfColorMaskExt_f gfColorMaskExt;
extern gfStencilFuncExt_f gfStencilFuncExt;
extern gfStencilMaskExt_f gfStencilMaskExt;
extern gfStencilOpExt_f gfStencilOpExt;
extern gfColorCombineExt_f gfColorCombineExt;
extern gfAlphaCombineExt_f gfAlphaCombineExt;
extern gfTexColorCombineExt_f gfTexColorCombineExt;
extern gfTexAlphaCombineExt_f gfTexAlphaCombineExt;
extern gfConstantColorValueExt_f gfConstantColorValueExt;


#define GR_TMU_SPLIT 99
#define GR_TMU_BOTH 100
#define GR_TMU_NONE 101


void
tex_info (int width, int height,
	  int *maxlod, int *ar, float *ss, float *ts,
	  int *rwidth, int *rheight);

int tm_init (void);
void tm_fini (void);
void tmu_movein (TEX_OBJ *obj, GLuint where);
void tmu_moveout (TEX_OBJ *obj);
GLuint tm_best_tmu (GLuint tmu, TEX_OBJ *texObj);

int vb_init (void);


int drv_multipass_none (int pass);
int drv_multipass_colorsum (int pass);


void init_tri_pointers (void);
void setup_tri_pointers (void);


struct sageContext {
    GrContext_t gr_ctx;		/**< Hardware context */
    GrPixelFormat_t fmt;	/**< Hardware pixel format */
    void *drawable;		/**< Current drawable */
};


#endif
