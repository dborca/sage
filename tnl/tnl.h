#ifndef TNL_H_included
#define TNL_H_included


typedef void (*RENDER_FUNC) ();

#define RENDER_FLAT	  (1<<0)
#define RENDER_OFFSET	  (1<<1)
#define RENDER_TWOSIDE	  (1<<2)
#define RENDER_UNFILLED	  (1<<3)
#define RENDER_SPECULAR	  (1<<4)
#define RENDER_TWOSTENCIL (1<<5)
#define RENDER_MAX	  (1<<6)


#define TNL_USERCLIP_PLANES 6


#define TNL_GEN_OBJECT  0
#define TNL_GEN_EYE     1
#define TNL_GEN_SPHERE  2
#define TNL_GEN_REFLECT 3
#define TNL_GEN_NORMAL  4


#define TNL_GEN_S_BIT (1<<0)
#define TNL_GEN_T_BIT (1<<1)
#define TNL_GEN_R_BIT (1<<2)
#define TNL_GEN_Q_BIT (1<<3)


typedef void (*TNL_DRAW_FUNC) (int start, int count, int mode);
extern const TNL_DRAW_FUNC* tnl_prim_tab;


#define TNL_PRIM_BEGIN  (1 << 16)
#define TNL_PRIM_END    (1 << 17)
#define TNL_PRIM_PARITY (1 << 18)
#define TNL_PRIM_WEAK   (1 << 19)

#define TNL_OUTSIDE_BE  ((GL_POLYGON + 1) | TNL_PRIM_BEGIN | TNL_PRIM_END)


#define TNL_CLIP_NEAR   (1 << 0)
#define TNL_CLIP_FAR    (1 << 1)
#define TNL_CLIP_TOP    (1 << 2)
#define TNL_CLIP_BOTTOM (1 << 3)
#define TNL_CLIP_LEFT   (1 << 4)
#define TNL_CLIP_RIGHT  (1 << 5)
#define TNL_CLIP_USER   (1 << 6)

#define TNL_CLIPPED_VERTS (2 * (6 + TNL_USERCLIP_PLANES) + 1)


#define TNL_VERTEXW_BIT     (1 <<  0)
#define TNL_VERTEX_BIT      (1 <<  0)
#define TNL_COLOR0_BIT      (1 <<  1)
#define TNL_COLOR1_BIT      (1 <<  2)
#define TNL_NORMAL_BIT      (1 <<  3)
#define TNL_TEXCOORD0_BIT   (1 <<  4)
#define TNL_TEXCOORD1_BIT   (1 <<  5)
#define TNL_TEXCOORD2_BIT   (1 <<  6)
#define TNL_TEXCOORD3_BIT   (1 <<  7)
#define TNL_TEXCOORD4_BIT   (1 <<  8)
#define TNL_TEXCOORD5_BIT   (1 <<  9)
#define TNL_TEXCOORD6_BIT   (1 << 10)
#define TNL_TEXCOORD7_BIT   (1 << 11)
#define TNL_AMBIENT_f_BIT   (1 << 12)
#define TNL_AMBIENT_b_BIT   (1 << 13)
#define TNL_DIFFUSE_f_BIT   (1 << 14)
#define TNL_DIFFUSE_b_BIT   (1 << 15)
#define TNL_SPECULAR_f_BIT  (1 << 16)
#define TNL_SPECULAR_b_BIT  (1 << 17)
#define TNL_EMISSION_f_BIT  (1 << 18)
#define TNL_EMISSION_b_BIT  (1 << 19)
#define TNL_SHININESS_f_BIT (1 << 20)
#define TNL_SHININESS_b_BIT (1 << 21)
#define TNL_FOGCOORD_BIT    (1 << 22)
#define TNL_EDGEFLAG_BIT    (1 << 23)
#define TNL_TEXCOORD0W_BIT  (1 << 24)
#define TNL_TEXCOORD1W_BIT  (1 << 25)
#define TNL_TEXCOORD2W_BIT  (1 << 26)
#define TNL_TEXCOORD3W_BIT  (1 << 27)
#define TNL_TEXCOORD4W_BIT  (1 << 28)
#define TNL_TEXCOORD5W_BIT  (1 << 29)
#define TNL_TEXCOORD6W_BIT  (1 << 30)
#define TNL_TEXCOORD7W_BIT  (1 << 31)

#define TNL_VERTEX        0
#define TNL_COLOR0        1
#define TNL_COLOR1        2
#define TNL_NORMAL        3
#define TNL_TEXCOORD0     4
#define TNL_TEXCOORD1     5
#define TNL_TEXCOORD2     6
#define TNL_TEXCOORD3     7
#define TNL_TEXCOORD4     8
#define TNL_TEXCOORD5     9
#define TNL_TEXCOORD6    10
#define TNL_TEXCOORD7    11
#define TNL_AMBIENT_f    12
#define TNL_AMBIENT_b    13
#define TNL_DIFFUSE_f    14
#define TNL_DIFFUSE_b    15
#define TNL_SPECULAR_f   16
#define TNL_SPECULAR_b   17
#define TNL_EMISSION_f   18
#define TNL_EMISSION_b   19
#define TNL_SHININESS_f  20
#define TNL_SHININESS_b  21
#define TNL_FOGCOORD     22
#define TNL_EDGEFLAG     23
#define TNL_BITS         24


#define TNL_MAX_TEXCOORD (TNL_TEXCOORD7 - TNL_TEXCOORD0 + 1)


typedef struct {
    int stride;		/* in elements (1 element == 4 floats) */
    GLfloat4 *data;	/* pointer to actual data */
    GLfloat4 *ptr;	/* storage array */
} TNL_ARRAY;

typedef void (GLAPIENTRY *attr_func1) (const void *);
typedef void (GLAPIENTRY *attr_func2) (int, const void *);

typedef struct CLIENT_ARRAY {
    struct CLIENT_ARRAY *next, *prev;
    GLint size;		/* 2f, 3f, 4f */
    GLenum type;	/* GL_UNSIGNED_BYTE, GL_FLOAT */
    GLsizei stride;	/* if zero, it's set to size of type (in bytes) */
    const GLvoid *ptr;
    attr_func1 func1;
    attr_func2 func2;
    int index;		/* for func2 */
} CLIENT_ARRAY;


typedef struct {
    CLIENT_ARRAY arrays[TNL_BITS];	/* data info */
    GLbitfield flags;			/* enabled arrays */
    GLuint texunit;			/* active texture unit */
    CLIENT_ARRAY list1, list2;
} TNL_CLIENT;


#define TNL_PRIM_MAX     32

typedef struct {
    int start;
    int count;
    GLenum name;
    GLbitfield ormask;
} TNL_PRIMITIVE;

extern TNL_PRIMITIVE *tnl_prim;
extern GLuint tnl_prim_num;
extern GLenum tnl_primitive;
extern GLuint tnl_prim_size;


extern GLfloat4 tnl_init_attr[TNL_BITS];


typedef struct {
    int num, max, len;
    GLbitfield *flags;
    GLubyte *clipmask;
    TNL_ARRAY attr[TNL_BITS];
    GLfloat4 *clip;
    GLfloat4 *ndc;
    GLfloat4 *c0_front, *c1_front;
    GLfloat4 *c0_back, *c1_back;
    GLfloat4 *texgen[TNL_MAX_TEXCOORD];
    GLfloat4 *texcoord[TNL_MAX_TEXCOORD];
    GLfloat4 *veye;
    GLfloat4 *neye;
    GLfloat4 *veyn;
    GLfloat4 *refl;
    GLfloat4 *fogcoord;
} TNL_VERTEXBUFFER;


extern GLuint tnl_general_flags;
extern TNL_VERTEXBUFFER tnl_vb;


extern TNL_CLIENT tnl_client;


extern GLbitfield tnl_extra_flags;


#define D_NEED_NEYE (1<<0)
#define D_NEED_VEYE (1<<1)
#define D_NEED_MVEC (1<<2)
#define D_NEED_REFL (1<<3)
#define D_NEED_VEYN (1<<4)	/* XXX alien flag, don't index with it!!! */
extern GLbitfield tnl_light_state;
extern GLbitfield tnl_texgen_state;
void tnl_setup_init (void);
typedef void (*TEXGEN_FUNC) (int unit);
extern TEXGEN_FUNC tnl_texgen_tab[16];
void tnl_reflect (void);
void tnl_reflect_mvec (void);
void tnl_light_oneside (void);
void tnl_light_twoside (void);
void tnl_light_oneside_sepspec (void);
void tnl_light_twoside_sepspec (void);
#define NORM_RESCALE   (1<<0)
#define NORM_NORMALIZE (1<<1)
typedef void (*FUNC) (void);
extern FUNC tnl_calc_neye_tab[];
#define V_USERCLIP 1 /* a better name?!? */
#define V_NOCLIP   2 /* a better name?!? */
extern FUNC tnl_clipmask_tab[];
typedef void (*VEYN_FUNC) (const GLfloat *);
extern VEYN_FUNC tnl_veyn_func[];
extern FUNC tnl_refl_func[];


int tnl_init (void);
void GLAPIENTRY tnl_flush (void);
void tnl_fini (void);


void tnl_setupUserClip (void);
void tnl_setupLight (void);
void tnl_setupTexture (void);


void tnl_run_pipeline (void);

extern int *tnl_vb_elt;

#endif
