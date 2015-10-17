#ifndef CONTEXT_H_included
#define CONTEXT_H_included


#define POLYGON_FILL  (1<<0)
#define POLYGON_LINE  (1<<1)
#define POLYGON_POINT (1<<2)


#define SUB_PIXEL_BITS 4


#define NEW_BLEND    (1<<0)
#define NEW_ALPHA    (1<<1)
#define NEW_COLOR    (1<<2)
#define NEW_DEPTH    (1<<3)
#define NEW_CULL     (1<<4)
#define NEW_LIGHT    (1<<5)
#define NEW_TEXTURE  (1<<6)
#define NEW_FOG      (1<<7)
#define NEW_SCISSOR  (1<<8)
#define NEW_USERCLIP (1<<9)
#define NEW_STENCIL  (1<<10)


#define MAX_TEXTURE_LEVELS 12
#define MAX_TEXTURE_UNITS   8
#define MAX_TEXTURE_SIZE   (1<<(MAX_TEXTURE_LEVELS-1))

#define MAX_LIGHTS 8

#define MAX_CLIP_PLANES 6


#define TEXTURE_1D   (1<<0)
#define TEXTURE_2D   (1<<1)


typedef struct LIGHT {
    struct LIGHT *next, *prev;
    GLboolean enabled;
    GLfloat4 ambient;
    GLfloat4 diffuse;
    GLfloat4 specular;
    GLfloat4 position;
    GLfloat4 spot_direction;
    GLfloat spot_exponent;
    GLfloat spot_cutoff;
    GLfloat4 attenuation;
    GLfloat4 _posnorm;
    GLfloat4 _svector;
    GLfloat4 _spot_eye_norm;
    GLfloat _spot_cutcos;
} LIGHT;


typedef struct DLIST {
    struct DLIST *next, *prev;
    GLuint name;
    GLvoid *block;
} DLIST;


typedef struct {
    GLenum iFormat;
    GLenum iType;
    GLuint texelBytes;
    GLboolean isCompressed;
    GLint redBits, greenBits, blueBits, alphaBits;
} TEXDEF;


typedef struct TEX_IMG {
    GLvoid *data;
    GLuint width, height, depth;
    GLsizei compressedSize;
    GLvoid *driverData;
} TEX_IMG;


typedef struct TEX_OBJ {
    struct TEX_OBJ *next, *prev;
    GLuint name;
    GLenum target;
    GLenum baseFormat;
    GLint minLevel, maxLevel;
    GLenum minFilter, magFilter;
    GLenum wrapS, wrapT, wrapR;
    TEX_IMG image[6][MAX_TEXTURE_LEVELS];
    GLvoid *driverData;
    GLfloat bias;
    const TEXDEF *t;
} TEX_OBJ;


typedef struct {
    GLenum mode;
    GLenum source[3];
    GLenum operand[3];
    GLint scale;
} COMBINE;


typedef struct {
    GLenum enabled;
    GLenum texenv;
    TEX_OBJ *object;
    TEX_OBJ *obj1d, *obj2d;
    GLfloat4 envcolor;

    GLbitfield texgen;

    GLenum genmode[4];
    GLfloat4 objplane[4];
    GLfloat4 eyeplane[4];

    COMBINE combineRGB, combineAlpha;

    GLfloat bias;
} TEX_UNIT;


typedef struct {
    GLboolean swap_bytes;
    GLboolean lsb_first;
    GLint row_length;
    GLint image_height;
    GLint skip_rows;
    GLint skip_pixels;
    GLint skip_images;
    GLint alignment;
} PACKING;


typedef struct {
    /* alpha */
    GLenum alpha_test;
    GLenum alpha_func;
    GLfloat alpha_ref;
    /* blend */
    GLboolean blending;
    GLenum blend_src_rgb;
    GLenum blend_dst_rgb;
    GLenum blend_src_alpha;
    GLenum blend_dst_alpha;
    /* color */
    GLfloat4 clear_color;
    GLboolean color_mask_r;
    GLboolean color_mask_g;
    GLboolean color_mask_b;
    GLboolean color_mask_a;
    /* buffer */
    GLenum draw_buffer;
} gl_color_attrib;


typedef struct {
    /* color */
    GLboolean secondary_color;
    /* fog */
    GLboolean fogging;
    GLenum mode;
    GLfloat density;
    GLfloat start;
    GLfloat end;
    GLfloat4 color;
    GLenum source;
} gl_fog_attrib;


typedef struct {
    GLboolean enabled;
    GLenum func;
    GLint ref;
    GLuint valMask;
    GLenum fail, zfail, zpass;
    GLuint writeMask;
    GLint clear;
} gl_stencil_attrib;


extern GLenum ctx_error_code;
extern GLbitfield ctx_gl_state;
extern GLuint ctx_depthbits;
extern GLuint ctx_depthmaxi;
extern GLfloat ctx_depthmaxf;
extern GLuint ctx_stencilmaxi;
extern GLuint ctx_redbits, ctx_greenbits, ctx_bluebits, ctx_alphabits, ctx_stencilbits;

extern GLboolean ctx_texcodec;


/* constants */
extern GLuint ctx_const_max_texture_size;
extern GLuint ctx_const_max_texture_units;
extern GLfloat ctx_const_max_lod_bias;
extern GLfloat ctx_const_depth_resolution;


/* extensions */
extern char *ctx_ext_string;


/* color */
extern gl_color_attrib ctx_color;


/* depth */
extern GLfloat ctx_clear_depth;
extern GLenum ctx_depth_func;
extern GLboolean ctx_depth_mask;
extern GLboolean ctx_depth_test;


/* culling */
extern GLboolean ctx_culling;
extern GLenum ctx_cull_face;
extern GLenum ctx_front_face;

extern GLbitfield ctx_polygon_offset_mode;
extern GLfloat ctx_polygon_offset_factor;
extern GLfloat ctx_polygon_offset_bias;

extern GLenum ctx_polygon_mode[2];


/* lighting */
extern GLboolean ctx_lighting;
extern GLboolean ctx_colormat;
extern GLenum ctx_colormat_face;
extern GLenum ctx_colormat_mode;
extern LIGHT ctx_light[MAX_LIGHTS];
extern LIGHT ctx_light_list;
extern GLfloat4 ctx_light_model_ambient;
extern GLboolean ctx_light_model_localv;
extern GLboolean ctx_light_model_twoside;
extern GLenum ctx_light_model_colctrl;
extern GLbitfield ctx_normalize;

extern GLenum ctx_shade_model;


/* texture */
extern TEX_OBJ *ctx_tex1d_obj;
extern TEX_OBJ *ctx_tex2d_obj;
extern TEX_OBJ *ctx_proxy1d_obj;
extern TEX_OBJ *ctx_proxy2d_obj;
extern TEX_OBJ *ctx_tex_obj_list;
extern GLuint ctx_tex_hinumber;
extern TEX_UNIT ctx_texture[MAX_TEXTURE_UNITS];
extern TEX_UNIT *ctx_active_tex;


/* fog */
extern gl_fog_attrib ctx_fog;


/* scissor */
extern GLboolean ctx_scissor;
extern GLint ctx_scissor_x1;
extern GLint ctx_scissor_y1;
extern GLint ctx_scissor_x2;
extern GLint ctx_scissor_y2;


/* userclip */
extern GLbitfield ctx_userclip;
extern GLfloat4 ctx_userplanes[2][MAX_CLIP_PLANES];


/* hints */
extern GLenum ctx_hint_clip_volume;


/* packing */
extern PACKING ctx_unpack, ctx_pack;
extern const PACKING ctx_string_packing;


/* buffer */
extern GLenum ctx_read_buffer;


/* stencil */
extern gl_stencil_attrib ctx_stencil;


/* display list */
extern DLIST ctx_dlist_list;
extern GLuint ctx_dlist_base;
extern GLenum ctx_dlist_mode;


int
ctx_init (int db_flag,
          int red_size, int green_size, int blue_size,
	  int alpha_size, int depth_size, int stencil_size);
void ctx_fini (void);
void ctx_validate_state (GLuint which);


const char *gl_enum (GLenum e);
void gl_error (void *ctx, int code, const char *fmt, ...);
void gl_warning (void *ctx, int code, const char *fmt, ...);
void gl_cry (const char *file, int line, const char *func, const char *fmt, ...);


char *ext_create_string (void);
void ext_set (const char *name, int flag);


#define NEED_SECONDARY_COLOR()	\
    (ctx_fog.secondary_color ||	\
     (ctx_lighting && ctx_light_model_colctrl == GL_SEPARATE_SPECULAR_COLOR))


/*#define FLUSH_VERTICES() GLCALL(Flush)()*/
#define FLUSH_VERTICES() imm_Flush()


#define gl_assert(e) assert(e)
/*#define gl_assert(e) gl_cry(__FILE__, __LINE__, __FUNCTION__, "assertion failed: %s\n", #e)*/


#endif
