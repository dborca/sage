#ifndef GL_H_included
#define GL_H_included


#ifndef GLAPI
#ifdef _WIN32
#define GLAPI __declspec(dllexport)
#else
#define GLAPI
#endif
#endif

#ifndef GLAPIENTRY
#ifdef _WIN32
#define GLAPIENTRY __stdcall
#else
#define GLAPIENTRY
#endif
#endif

#ifndef APIENTRY
#define APIENTRY GLAPIENTRY
#endif


#ifdef __cplusplus
extern "C" {
#endif


/*
 * Datatypes
 */
typedef unsigned int	GLenum;
typedef unsigned char	GLboolean;
typedef unsigned int	GLbitfield;
typedef void		GLvoid;
typedef signed char	GLbyte;		/* 1-byte signed */
typedef short		GLshort;	/* 2-byte signed */
typedef int		GLint;		/* 4-byte signed */
typedef unsigned char	GLubyte;	/* 1-byte unsigned */
typedef unsigned short	GLushort;	/* 2-byte unsigned */
typedef unsigned int	GLuint;		/* 4-byte unsigned */
typedef int		GLsizei;	/* 4-byte signed */
typedef float		GLfloat;	/* single precision float */
typedef float		GLclampf;	/* single precision float in [0,1] */
typedef double		GLdouble;	/* double precision float */
typedef double		GLclampd;	/* double precision float in [0,1] */


/*
 * Constants
 */

/* Boolean values */
#define GL_FALSE				0x0
#define GL_TRUE					0x1

/* Data types */
#define GL_BYTE					0x1400
#define GL_UNSIGNED_BYTE			0x1401
#define GL_SHORT				0x1402
#define GL_UNSIGNED_SHORT			0x1403
#define GL_INT					0x1404
#define GL_UNSIGNED_INT				0x1405
#define GL_FLOAT				0x1406
#define GL_2_BYTES				0x1407
#define GL_3_BYTES				0x1408
#define GL_4_BYTES				0x1409
#define GL_DOUBLE				0x140A

/* Primitives */
#define GL_POINTS				0x0000
#define GL_LINES				0x0001
#define GL_LINE_LOOP				0x0002
#define GL_LINE_STRIP				0x0003
#define GL_TRIANGLES				0x0004
#define GL_TRIANGLE_STRIP			0x0005
#define GL_TRIANGLE_FAN				0x0006
#define GL_QUADS				0x0007
#define GL_QUAD_STRIP				0x0008
#define GL_POLYGON				0x0009

/* Vertex Arrays */
#define GL_VERTEX_ARRAY				0x8074
#define GL_NORMAL_ARRAY				0x8075
#define GL_COLOR_ARRAY				0x8076
#define GL_TEXTURE_COORD_ARRAY			0x8078
#define GL_EDGE_FLAG_ARRAY			0x8079
#define GL_V2F					0x2A20
#define GL_V3F					0x2A21
#define GL_C4UB_V2F				0x2A22
#define GL_C4UB_V3F				0x2A23
#define GL_C3F_V3F				0x2A24
#define GL_N3F_V3F				0x2A25
#define GL_C4F_N3F_V3F				0x2A26
#define GL_T2F_V3F				0x2A27
#define GL_T4F_V4F				0x2A28
#define GL_T2F_C4UB_V3F				0x2A29
#define GL_T2F_C3F_V3F				0x2A2A
#define GL_T2F_N3F_V3F				0x2A2B
#define GL_T2F_C4F_N3F_V3F			0x2A2C
#define GL_T4F_C4F_N3F_V4F			0x2A2D

/* Matrix Mode */
#define GL_MATRIX_MODE				0x0BA0
#define GL_MODELVIEW				0x1700
#define GL_PROJECTION				0x1701
#define GL_TEXTURE				0x1702

/* Polygons */
#define GL_POINT				0x1B00
#define GL_LINE					0x1B01
#define GL_FILL					0x1B02
#define GL_CW					0x0900
#define GL_CCW					0x0901
#define GL_FRONT				0x0404
#define GL_BACK					0x0405
#define GL_CULL_FACE				0x0B44
#define GL_CULL_FACE_MODE			0x0B45
#define GL_FRONT_FACE				0x0B46
#define GL_POLYGON_OFFSET_POINT			0x2A01
#define GL_POLYGON_OFFSET_LINE			0x2A02
#define GL_POLYGON_OFFSET_FILL			0x8037

/* Display Lists */
#define GL_COMPILE				0x1300
#define GL_COMPILE_AND_EXECUTE			0x1301
#define GL_LIST_BASE				0x0B32
#define GL_LIST_INDEX				0x0B33
#define GL_LIST_MODE				0x0B30

/* Depth buffer */
#define GL_NEVER				0x0200
#define GL_LESS					0x0201
#define GL_EQUAL				0x0202
#define GL_LEQUAL				0x0203
#define GL_GREATER				0x0204
#define GL_NOTEQUAL				0x0205
#define GL_GEQUAL				0x0206
#define GL_ALWAYS				0x0207
#define GL_DEPTH_TEST				0x0B71
#define GL_DEPTH_BITS				0x0D56

/* Lighting */
#define GL_LIGHTING				0x0B50
#define GL_LIGHT0				0x4000
#define GL_LIGHT1				0x4001
#define GL_LIGHT2				0x4002
#define GL_LIGHT3				0x4003
#define GL_LIGHT4				0x4004
#define GL_LIGHT5				0x4005
#define GL_LIGHT6				0x4006
#define GL_LIGHT7				0x4007
#define GL_SPOT_EXPONENT			0x1205
#define GL_SPOT_CUTOFF				0x1206
#define GL_CONSTANT_ATTENUATION			0x1207
#define GL_LINEAR_ATTENUATION			0x1208
#define GL_QUADRATIC_ATTENUATION		0x1209
#define GL_AMBIENT				0x1200
#define GL_DIFFUSE				0x1201
#define GL_SPECULAR				0x1202
#define GL_SHININESS				0x1601
#define GL_EMISSION				0x1600
#define GL_POSITION				0x1203
#define GL_SPOT_DIRECTION			0x1204
#define GL_AMBIENT_AND_DIFFUSE			0x1602
#define GL_LIGHT_MODEL_TWO_SIDE			0x0B52
#define GL_LIGHT_MODEL_LOCAL_VIEWER		0x0B51
#define GL_LIGHT_MODEL_AMBIENT			0x0B53
#define GL_FRONT_AND_BACK			0x0408
#define GL_FLAT					0x1D00
#define GL_SMOOTH				0x1D01
#define GL_COLOR_MATERIAL			0x0B57
#define GL_NORMALIZE				0x0BA1

/* User clipping planes */
#define GL_CLIP_PLANE0				0x3000
#define GL_CLIP_PLANE1				0x3001
#define GL_CLIP_PLANE2				0x3002
#define GL_CLIP_PLANE3				0x3003
#define GL_CLIP_PLANE4				0x3004
#define GL_CLIP_PLANE5				0x3005

/* Accumulation buffer */
#define GL_ADD					0x0104

/* Alpha testing */
#define GL_ALPHA_TEST				0x0BC0
#define GL_ALPHA_TEST_REF			0x0BC2
#define GL_ALPHA_TEST_FUNC			0x0BC1

/* Blending */
#define GL_BLEND				0x0BE2
#define GL_BLEND_SRC				0x0BE1
#define GL_BLEND_DST				0x0BE0
#define GL_ZERO					0x0
#define GL_ONE					0x1
#define GL_SRC_COLOR				0x0300
#define GL_ONE_MINUS_SRC_COLOR			0x0301
#define GL_SRC_ALPHA				0x0302
#define GL_ONE_MINUS_SRC_ALPHA			0x0303
#define GL_DST_ALPHA				0x0304
#define GL_ONE_MINUS_DST_ALPHA			0x0305
#define GL_DST_COLOR				0x0306
#define GL_ONE_MINUS_DST_COLOR			0x0307
#define GL_SRC_ALPHA_SATURATE			0x0308

/* Fog */
#define GL_FOG					0x0B60
#define GL_FOG_MODE				0x0B65
#define GL_FOG_DENSITY				0x0B62
#define GL_FOG_COLOR				0x0B66
#define GL_FOG_START				0x0B63
#define GL_FOG_END				0x0B64
#define GL_LINEAR				0x2601
#define GL_EXP					0x0800
#define GL_EXP2					0x0801

/* Logic Ops */
#define GL_INVERT				0x150A

/* Stencil */
#define GL_STENCIL_TEST				0x0B90
#define GL_KEEP					0x1E00
#define GL_REPLACE				0x1E01
#define GL_INCR					0x1E02
#define GL_DECR					0x1E03

/* Buffers, Pixel Drawing/Reading */
#define GL_NONE					0x0
#define GL_ALPHA				0x1906
#define GL_LUMINANCE				0x1909
#define GL_LUMINANCE_ALPHA			0x190A
#define GL_SUBPIXEL_BITS			0x0D50
#define GL_AUX_BUFFERS				0x0C00
#define GL_COLOR				0x1800
#define GL_DITHER				0x0BD0
#define GL_RGB					0x1907
#define GL_RGBA					0x1908

/* Implementation limits */
#define GL_MAX_MODELVIEW_STACK_DEPTH		0x0D36
#define GL_MAX_PROJECTION_STACK_DEPTH		0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH		0x0D39
#define GL_MAX_LIGHTS				0x0D31
#define GL_MAX_CLIP_PLANES			0x0D32
#define GL_MAX_TEXTURE_SIZE			0x0D33

/* Gets */
#define GL_INDEX_MODE				0x0C30
#define GL_MODELVIEW_MATRIX			0x0BA6
#define GL_PROJECTION_MATRIX			0x0BA7
#define GL_RGBA_MODE				0x0C31
#define GL_VIEWPORT				0x0BA2

/* Hints */
#define GL_FOG_HINT				0x0C54
#define GL_PERSPECTIVE_CORRECTION_HINT		0x0C50
#define GL_POLYGON_SMOOTH_HINT			0x0C53
#define GL_DONT_CARE				0x1100
#define GL_FASTEST				0x1101
#define GL_NICEST				0x1102

/* Scissor box */
#define GL_SCISSOR_TEST				0x0C11

/* Pixel Mode / Transfer */
#define GL_PACK_ALIGNMENT			0x0D05
#define GL_PACK_LSB_FIRST			0x0D01
#define GL_PACK_ROW_LENGTH			0x0D02
#define GL_PACK_SKIP_PIXELS			0x0D04
#define GL_PACK_SKIP_ROWS			0x0D03
#define GL_PACK_SWAP_BYTES			0x0D00
#define GL_UNPACK_ALIGNMENT			0x0CF5
#define GL_UNPACK_LSB_FIRST			0x0CF1
#define GL_UNPACK_ROW_LENGTH			0x0CF2
#define GL_UNPACK_SKIP_PIXELS			0x0CF4
#define GL_UNPACK_SKIP_ROWS			0x0CF3
#define GL_UNPACK_SWAP_BYTES			0x0CF0

/* Texture mapping */
#define GL_TEXTURE_ENV				0x2300
#define GL_TEXTURE_ENV_MODE			0x2200
#define GL_TEXTURE_1D				0x0DE0
#define GL_TEXTURE_2D				0x0DE1
#define GL_TEXTURE_WRAP_S			0x2802
#define GL_TEXTURE_WRAP_T			0x2803
#define GL_TEXTURE_MAG_FILTER			0x2800
#define GL_TEXTURE_MIN_FILTER			0x2801
#define GL_TEXTURE_ENV_COLOR			0x2201
#define GL_TEXTURE_GEN_S			0x0C60
#define GL_TEXTURE_GEN_T			0x0C61
#define GL_TEXTURE_GEN_MODE			0x2500
#define GL_TEXTURE_WIDTH			0x1000
#define GL_TEXTURE_HEIGHT			0x1001
#define GL_TEXTURE_GREEN_SIZE			0x805D
#define GL_NEAREST_MIPMAP_NEAREST		0x2700
#define GL_NEAREST_MIPMAP_LINEAR		0x2702
#define GL_LINEAR_MIPMAP_NEAREST		0x2701
#define GL_LINEAR_MIPMAP_LINEAR			0x2703
#define GL_OBJECT_LINEAR			0x2401
#define GL_OBJECT_PLANE				0x2501
#define GL_EYE_LINEAR				0x2400
#define GL_EYE_PLANE				0x2502
#define GL_SPHERE_MAP				0x2402
#define GL_DECAL				0x2101
#define GL_MODULATE				0x2100
#define GL_NEAREST				0x2600
#define GL_REPEAT				0x2901
#define GL_CLAMP				0x2900
#define GL_S					0x2000
#define GL_T					0x2001
#define GL_R					0x2002
#define GL_Q					0x2003
#define GL_TEXTURE_GEN_R			0x0C62
#define GL_TEXTURE_GEN_Q			0x0C63

/* Utility */
#define GL_VENDOR				0x1F00
#define GL_RENDERER				0x1F01
#define GL_VERSION				0x1F02
#define GL_EXTENSIONS				0x1F03

/* Errors */
#define GL_NO_ERROR 				0x0
#define GL_INVALID_VALUE			0x0501
#define GL_INVALID_ENUM				0x0500
#define GL_INVALID_OPERATION			0x0502
#define GL_STACK_OVERFLOW			0x0503
#define GL_STACK_UNDERFLOW			0x0504
#define GL_OUT_OF_MEMORY			0x0505

/* glPush/PopAttrib bits */
#define GL_FOG_BIT				0x00000080
#define GL_DEPTH_BUFFER_BIT			0x00000100
#define GL_STENCIL_BUFFER_BIT			0x00000400
#define GL_VIEWPORT_BIT				0x00000800
#define GL_COLOR_BUFFER_BIT			0x00004000


/* OpenGL 1.1 */
#define GL_PROXY_TEXTURE_1D			0x8063
#define GL_PROXY_TEXTURE_2D			0x8064
#define GL_ALPHA8				0x803C
#define GL_LUMINANCE8				0x8040
#define GL_LUMINANCE8_ALPHA8			0x8045
#define GL_INTENSITY				0x8049
#define GL_INTENSITY8				0x804B
#define GL_RGB4					0x804F
#define GL_RGB5					0x8050
#define GL_RGB8					0x8051
#define GL_RGBA4				0x8056
#define GL_RGB5_A1				0x8057
#define GL_RGBA8				0x8058
#define GL_CLIENT_VERTEX_ARRAY_BIT		0x00000002


/*
 * Miscellaneous
 */

GLAPI void GLAPIENTRY glClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );

GLAPI void GLAPIENTRY glClear( GLbitfield mask );

GLAPI void GLAPIENTRY glColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );

GLAPI void GLAPIENTRY glAlphaFunc( GLenum func, GLclampf ref );

GLAPI void GLAPIENTRY glBlendFunc( GLenum sfactor, GLenum dfactor );

GLAPI void GLAPIENTRY glCullFace( GLenum mode );

GLAPI void GLAPIENTRY glFrontFace( GLenum mode );

GLAPI void GLAPIENTRY glPolygonMode( GLenum face, GLenum mode );

GLAPI void GLAPIENTRY glPolygonOffset( GLfloat factor, GLfloat units );

GLAPI void GLAPIENTRY glEdgeFlag( GLboolean flag );

GLAPI void GLAPIENTRY glEdgeFlagv( const GLboolean *flag );

GLAPI void GLAPIENTRY glScissor( GLint x, GLint y, GLsizei width, GLsizei height);

GLAPI void GLAPIENTRY glClipPlane( GLenum plane, const GLdouble *equation );

GLAPI void GLAPIENTRY glDrawBuffer( GLenum mode );

GLAPI void GLAPIENTRY glReadBuffer( GLenum mode );

GLAPI void GLAPIENTRY glEnable( GLenum cap );

GLAPI void GLAPIENTRY glDisable( GLenum cap );

GLAPI GLboolean GLAPIENTRY glIsEnabled( GLenum cap );


GLAPI void GLAPIENTRY glEnableClientState( GLenum cap );  /* 1.1 */

GLAPI void GLAPIENTRY glDisableClientState( GLenum cap );  /* 1.1 */


GLAPI void GLAPIENTRY glGetBooleanv( GLenum pname, GLboolean *params );

GLAPI void GLAPIENTRY glGetDoublev( GLenum pname, GLdouble *params );

GLAPI void GLAPIENTRY glGetFloatv( GLenum pname, GLfloat *params );

GLAPI void GLAPIENTRY glGetIntegerv( GLenum pname, GLint *params );


GLAPI void GLAPIENTRY glPushAttrib( GLbitfield mask );

GLAPI void GLAPIENTRY glPopAttrib( void );


GLAPI void GLAPIENTRY glPushClientAttrib( GLbitfield mask );  /* 1.1 */

GLAPI void GLAPIENTRY glPopClientAttrib( void );  /* 1.1 */


GLAPI GLenum GLAPIENTRY glGetError( void );

GLAPI const GLubyte * GLAPIENTRY glGetString( GLenum name );

GLAPI void GLAPIENTRY glFinish( void );

GLAPI void GLAPIENTRY glFlush( void );

GLAPI void GLAPIENTRY glHint( GLenum target, GLenum mode );


/*
 * Depth Buffer
 */

GLAPI void GLAPIENTRY glClearDepth( GLclampd depth );

GLAPI void GLAPIENTRY glDepthFunc( GLenum func );

GLAPI void GLAPIENTRY glDepthMask( GLboolean flag );

GLAPI void GLAPIENTRY glDepthRange( GLclampd near_val, GLclampd far_val );


/*
 * Transformation
 */

GLAPI void GLAPIENTRY glMatrixMode( GLenum mode );

GLAPI void GLAPIENTRY glOrtho( GLdouble left, GLdouble right,
                                 GLdouble bottom, GLdouble top,
                                 GLdouble near_val, GLdouble far_val );

GLAPI void GLAPIENTRY glFrustum( GLdouble left, GLdouble right,
                                   GLdouble bottom, GLdouble top,
                                   GLdouble near_val, GLdouble far_val );

GLAPI void GLAPIENTRY glViewport( GLint x, GLint y,
                                    GLsizei width, GLsizei height );

GLAPI void GLAPIENTRY glPushMatrix( void );

GLAPI void GLAPIENTRY glPopMatrix( void );

GLAPI void GLAPIENTRY glLoadIdentity( void );

GLAPI void GLAPIENTRY glLoadMatrixd( const GLdouble *m );
GLAPI void GLAPIENTRY glLoadMatrixf( const GLfloat *m );

GLAPI void GLAPIENTRY glMultMatrixd( const GLdouble *m );
GLAPI void GLAPIENTRY glMultMatrixf( const GLfloat *m );

GLAPI void GLAPIENTRY glRotated( GLdouble angle,
                                   GLdouble x, GLdouble y, GLdouble z );
GLAPI void GLAPIENTRY glRotatef( GLfloat angle,
                                   GLfloat x, GLfloat y, GLfloat z );

GLAPI void GLAPIENTRY glScaled( GLdouble x, GLdouble y, GLdouble z );
GLAPI void GLAPIENTRY glScalef( GLfloat x, GLfloat y, GLfloat z );

GLAPI void GLAPIENTRY glTranslated( GLdouble x, GLdouble y, GLdouble z );
GLAPI void GLAPIENTRY glTranslatef( GLfloat x, GLfloat y, GLfloat z );


/*
 * Display Lists
 */

GLAPI GLboolean GLAPIENTRY glIsList( GLuint list );

GLAPI void GLAPIENTRY glDeleteLists( GLuint list, GLsizei range );

GLAPI GLuint GLAPIENTRY glGenLists( GLsizei range );

GLAPI void GLAPIENTRY glNewList( GLuint list, GLenum mode );

GLAPI void GLAPIENTRY glEndList( void );

GLAPI void GLAPIENTRY glCallList( GLuint list );

GLAPI void GLAPIENTRY glCallLists( GLsizei n, GLenum type,
                                     const GLvoid *lists );

GLAPI void GLAPIENTRY glListBase( GLuint base );


/*
 * Drawing Functions
 */

GLAPI void GLAPIENTRY glBegin( GLenum mode );

GLAPI void GLAPIENTRY glEnd( void );


GLAPI void GLAPIENTRY glVertex2d( GLdouble x, GLdouble y );
GLAPI void GLAPIENTRY glVertex2f( GLfloat x, GLfloat y );
GLAPI void GLAPIENTRY glVertex2i( GLint x, GLint y );
GLAPI void GLAPIENTRY glVertex2s( GLshort x, GLshort y );

GLAPI void GLAPIENTRY glVertex3d( GLdouble x, GLdouble y, GLdouble z );
GLAPI void GLAPIENTRY glVertex3f( GLfloat x, GLfloat y, GLfloat z );
GLAPI void GLAPIENTRY glVertex3i( GLint x, GLint y, GLint z );
GLAPI void GLAPIENTRY glVertex3s( GLshort x, GLshort y, GLshort z );

GLAPI void GLAPIENTRY glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
GLAPI void GLAPIENTRY glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
GLAPI void GLAPIENTRY glVertex4i( GLint x, GLint y, GLint z, GLint w );
GLAPI void GLAPIENTRY glVertex4s( GLshort x, GLshort y, GLshort z, GLshort w );

GLAPI void GLAPIENTRY glVertex2dv( const GLdouble *v );
GLAPI void GLAPIENTRY glVertex2fv( const GLfloat *v );
GLAPI void GLAPIENTRY glVertex2iv( const GLint *v );
GLAPI void GLAPIENTRY glVertex2sv( const GLshort *v );

GLAPI void GLAPIENTRY glVertex3dv( const GLdouble *v );
GLAPI void GLAPIENTRY glVertex3fv( const GLfloat *v );
GLAPI void GLAPIENTRY glVertex3iv( const GLint *v );
GLAPI void GLAPIENTRY glVertex3sv( const GLshort *v );

GLAPI void GLAPIENTRY glVertex4dv( const GLdouble *v );
GLAPI void GLAPIENTRY glVertex4fv( const GLfloat *v );
GLAPI void GLAPIENTRY glVertex4iv( const GLint *v );
GLAPI void GLAPIENTRY glVertex4sv( const GLshort *v );


GLAPI void GLAPIENTRY glNormal3b( GLbyte nx, GLbyte ny, GLbyte nz );
GLAPI void GLAPIENTRY glNormal3d( GLdouble nx, GLdouble ny, GLdouble nz );
GLAPI void GLAPIENTRY glNormal3f( GLfloat nx, GLfloat ny, GLfloat nz );
GLAPI void GLAPIENTRY glNormal3i( GLint nx, GLint ny, GLint nz );
GLAPI void GLAPIENTRY glNormal3s( GLshort nx, GLshort ny, GLshort nz );

GLAPI void GLAPIENTRY glNormal3bv( const GLbyte *v );
GLAPI void GLAPIENTRY glNormal3dv( const GLdouble *v );
GLAPI void GLAPIENTRY glNormal3fv( const GLfloat *v );
GLAPI void GLAPIENTRY glNormal3iv( const GLint *v );
GLAPI void GLAPIENTRY glNormal3sv( const GLshort *v );


GLAPI void GLAPIENTRY glColor3b( GLbyte red, GLbyte green, GLbyte blue );
GLAPI void GLAPIENTRY glColor3d( GLdouble red, GLdouble green, GLdouble blue );
GLAPI void GLAPIENTRY glColor3f( GLfloat red, GLfloat green, GLfloat blue );
GLAPI void GLAPIENTRY glColor3i( GLint red, GLint green, GLint blue );
GLAPI void GLAPIENTRY glColor3s( GLshort red, GLshort green, GLshort blue );
GLAPI void GLAPIENTRY glColor3ub( GLubyte red, GLubyte green, GLubyte blue );
GLAPI void GLAPIENTRY glColor3ui( GLuint red, GLuint green, GLuint blue );
GLAPI void GLAPIENTRY glColor3us( GLushort red, GLushort green, GLushort blue );

GLAPI void GLAPIENTRY glColor4b( GLbyte red, GLbyte green,
                                   GLbyte blue, GLbyte alpha );
GLAPI void GLAPIENTRY glColor4d( GLdouble red, GLdouble green,
                                   GLdouble blue, GLdouble alpha );
GLAPI void GLAPIENTRY glColor4f( GLfloat red, GLfloat green,
                                   GLfloat blue, GLfloat alpha );
GLAPI void GLAPIENTRY glColor4i( GLint red, GLint green,
                                   GLint blue, GLint alpha );
GLAPI void GLAPIENTRY glColor4s( GLshort red, GLshort green,
                                   GLshort blue, GLshort alpha );
GLAPI void GLAPIENTRY glColor4ub( GLubyte red, GLubyte green,
                                    GLubyte blue, GLubyte alpha );
GLAPI void GLAPIENTRY glColor4ui( GLuint red, GLuint green,
                                    GLuint blue, GLuint alpha );
GLAPI void GLAPIENTRY glColor4us( GLushort red, GLushort green,
                                    GLushort blue, GLushort alpha );


GLAPI void GLAPIENTRY glColor3bv( const GLbyte *v );
GLAPI void GLAPIENTRY glColor3dv( const GLdouble *v );
GLAPI void GLAPIENTRY glColor3fv( const GLfloat *v );
GLAPI void GLAPIENTRY glColor3iv( const GLint *v );
GLAPI void GLAPIENTRY glColor3sv( const GLshort *v );
GLAPI void GLAPIENTRY glColor3ubv( const GLubyte *v );
GLAPI void GLAPIENTRY glColor3uiv( const GLuint *v );
GLAPI void GLAPIENTRY glColor3usv( const GLushort *v );

GLAPI void GLAPIENTRY glColor4bv( const GLbyte *v );
GLAPI void GLAPIENTRY glColor4dv( const GLdouble *v );
GLAPI void GLAPIENTRY glColor4fv( const GLfloat *v );
GLAPI void GLAPIENTRY glColor4iv( const GLint *v );
GLAPI void GLAPIENTRY glColor4sv( const GLshort *v );
GLAPI void GLAPIENTRY glColor4ubv( const GLubyte *v );
GLAPI void GLAPIENTRY glColor4uiv( const GLuint *v );
GLAPI void GLAPIENTRY glColor4usv( const GLushort *v );


GLAPI void GLAPIENTRY glTexCoord1d( GLdouble s );
GLAPI void GLAPIENTRY glTexCoord1f( GLfloat s );
GLAPI void GLAPIENTRY glTexCoord1i( GLint s );
GLAPI void GLAPIENTRY glTexCoord1s( GLshort s );

GLAPI void GLAPIENTRY glTexCoord2d( GLdouble s, GLdouble t );
GLAPI void GLAPIENTRY glTexCoord2f( GLfloat s, GLfloat t );
GLAPI void GLAPIENTRY glTexCoord2i( GLint s, GLint t );
GLAPI void GLAPIENTRY glTexCoord2s( GLshort s, GLshort t );

GLAPI void GLAPIENTRY glTexCoord3d( GLdouble s, GLdouble t, GLdouble r );
GLAPI void GLAPIENTRY glTexCoord3f( GLfloat s, GLfloat t, GLfloat r );
GLAPI void GLAPIENTRY glTexCoord3i( GLint s, GLint t, GLint r );
GLAPI void GLAPIENTRY glTexCoord3s( GLshort s, GLshort t, GLshort r );

GLAPI void GLAPIENTRY glTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
GLAPI void GLAPIENTRY glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q );
GLAPI void GLAPIENTRY glTexCoord4i( GLint s, GLint t, GLint r, GLint q );
GLAPI void GLAPIENTRY glTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q );

GLAPI void GLAPIENTRY glTexCoord1dv( const GLdouble *v );
GLAPI void GLAPIENTRY glTexCoord1fv( const GLfloat *v );
GLAPI void GLAPIENTRY glTexCoord1iv( const GLint *v );
GLAPI void GLAPIENTRY glTexCoord1sv( const GLshort *v );

GLAPI void GLAPIENTRY glTexCoord2dv( const GLdouble *v );
GLAPI void GLAPIENTRY glTexCoord2fv( const GLfloat *v );
GLAPI void GLAPIENTRY glTexCoord2iv( const GLint *v );
GLAPI void GLAPIENTRY glTexCoord2sv( const GLshort *v );

GLAPI void GLAPIENTRY glTexCoord3dv( const GLdouble *v );
GLAPI void GLAPIENTRY glTexCoord3fv( const GLfloat *v );
GLAPI void GLAPIENTRY glTexCoord3iv( const GLint *v );
GLAPI void GLAPIENTRY glTexCoord3sv( const GLshort *v );

GLAPI void GLAPIENTRY glTexCoord4dv( const GLdouble *v );
GLAPI void GLAPIENTRY glTexCoord4fv( const GLfloat *v );
GLAPI void GLAPIENTRY glTexCoord4iv( const GLint *v );
GLAPI void GLAPIENTRY glTexCoord4sv( const GLshort *v );


GLAPI void GLAPIENTRY glRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
GLAPI void GLAPIENTRY glRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
GLAPI void GLAPIENTRY glRecti( GLint x1, GLint y1, GLint x2, GLint y2 );
GLAPI void GLAPIENTRY glRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );


GLAPI void GLAPIENTRY glRectdv( const GLdouble *v1, const GLdouble *v2 );
GLAPI void GLAPIENTRY glRectfv( const GLfloat *v1, const GLfloat *v2 );
GLAPI void GLAPIENTRY glRectiv( const GLint *v1, const GLint *v2 );
GLAPI void GLAPIENTRY glRectsv( const GLshort *v1, const GLshort *v2 );


/*
 * Vertex Arrays  (1.1)
 */

GLAPI void GLAPIENTRY glVertexPointer( GLint size, GLenum type,
                                       GLsizei stride, const GLvoid *ptr );

GLAPI void GLAPIENTRY glNormalPointer( GLenum type, GLsizei stride,
                                       const GLvoid *ptr );

GLAPI void GLAPIENTRY glColorPointer( GLint size, GLenum type,
                                      GLsizei stride, const GLvoid *ptr );

GLAPI void GLAPIENTRY glTexCoordPointer( GLint size, GLenum type,
                                         GLsizei stride, const GLvoid *ptr );

GLAPI void GLAPIENTRY glEdgeFlagPointer( GLsizei stride, const GLvoid *ptr );

GLAPI void GLAPIENTRY glArrayElement( GLint i );

GLAPI void GLAPIENTRY glDrawArrays( GLenum mode, GLint first, GLsizei count );

GLAPI void GLAPIENTRY glDrawElements( GLenum mode, GLsizei count,
                                      GLenum type, const GLvoid *indices );

GLAPI void GLAPIENTRY glInterleavedArrays( GLenum format, GLsizei stride,
                                           const GLvoid *pointer );

/*
 * Lighting
 */

GLAPI void GLAPIENTRY glShadeModel( GLenum mode );

GLAPI void GLAPIENTRY glLightf( GLenum light, GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glLighti( GLenum light, GLenum pname, GLint param );
GLAPI void GLAPIENTRY glLightfv( GLenum light, GLenum pname,
                                 const GLfloat *params );
GLAPI void GLAPIENTRY glLightiv( GLenum light, GLenum pname,
                                 const GLint *params );

GLAPI void GLAPIENTRY glLightModelf( GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glLightModeli( GLenum pname, GLint param );
GLAPI void GLAPIENTRY glLightModelfv( GLenum pname, const GLfloat *params );
GLAPI void GLAPIENTRY glLightModeliv( GLenum pname, const GLint *params );

GLAPI void GLAPIENTRY glMaterialf( GLenum face, GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glMateriali( GLenum face, GLenum pname, GLint param );
GLAPI void GLAPIENTRY glMaterialfv( GLenum face, GLenum pname, const GLfloat *params );
GLAPI void GLAPIENTRY glMaterialiv( GLenum face, GLenum pname, const GLint *params );

GLAPI void GLAPIENTRY glColorMaterial( GLenum face, GLenum mode );


/*
 * Raster functions
 */

GLAPI void GLAPIENTRY glPixelStoref( GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glPixelStorei( GLenum pname, GLint param );

GLAPI void GLAPIENTRY glReadPixels( GLint x, GLint y,
                                    GLsizei width, GLsizei height,
                                    GLenum format, GLenum type,
                                    GLvoid *pixels );

/*
 * Stenciling
 */

GLAPI void GLAPIENTRY glStencilFunc( GLenum func, GLint ref, GLuint mask );

GLAPI void GLAPIENTRY glStencilMask( GLuint mask );

GLAPI void GLAPIENTRY glStencilOp( GLenum fail, GLenum zfail, GLenum zpass );

GLAPI void GLAPIENTRY glClearStencil( GLint s );



/*
 * Texture mapping
 */

GLAPI void GLAPIENTRY glTexGend( GLenum coord, GLenum pname, GLdouble param );
GLAPI void GLAPIENTRY glTexGenf( GLenum coord, GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glTexGeni( GLenum coord, GLenum pname, GLint param );

GLAPI void GLAPIENTRY glTexGendv( GLenum coord, GLenum pname, const GLdouble *params );
GLAPI void GLAPIENTRY glTexGenfv( GLenum coord, GLenum pname, const GLfloat *params );
GLAPI void GLAPIENTRY glTexGeniv( GLenum coord, GLenum pname, const GLint *params );

GLAPI void GLAPIENTRY glTexEnvf( GLenum target, GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glTexEnvi( GLenum target, GLenum pname, GLint param );

GLAPI void GLAPIENTRY glTexEnvfv( GLenum target, GLenum pname, const GLfloat *params );
GLAPI void GLAPIENTRY glTexEnviv( GLenum target, GLenum pname, const GLint *params );

GLAPI void GLAPIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param );
GLAPI void GLAPIENTRY glTexParameteri( GLenum target, GLenum pname, GLint param );

GLAPI void GLAPIENTRY glTexParameterfv( GLenum target, GLenum pname,
                                          const GLfloat *params );
GLAPI void GLAPIENTRY glTexParameteriv( GLenum target, GLenum pname,
                                          const GLint *params );

GLAPI void GLAPIENTRY glGetTexLevelParameterfv( GLenum target, GLint level,
                                                GLenum pname, GLfloat *params );
GLAPI void GLAPIENTRY glGetTexLevelParameteriv( GLenum target, GLint level,
                                                GLenum pname, GLint *params );


GLAPI void GLAPIENTRY glTexImage1D( GLenum target, GLint level,
                                    GLint internalFormat,
                                    GLsizei width, GLint border,
                                    GLenum format, GLenum type,
                                    const GLvoid *pixels );

GLAPI void GLAPIENTRY glTexImage2D( GLenum target, GLint level,
                                    GLint internalFormat,
                                    GLsizei width, GLsizei height,
                                    GLint border, GLenum format, GLenum type,
                                    const GLvoid *pixels );

GLAPI void GLAPIENTRY glGetTexImage( GLenum target, GLint level,
                                     GLenum format, GLenum type,
                                     GLvoid *pixels );


/* 1.1 functions */

GLAPI void GLAPIENTRY glGenTextures( GLsizei n, GLuint *textures );

GLAPI void GLAPIENTRY glDeleteTextures( GLsizei n, const GLuint *textures);

GLAPI void GLAPIENTRY glBindTexture( GLenum target, GLuint texture );

GLAPI GLboolean GLAPIENTRY glIsTexture( GLuint texture );


GLAPI void GLAPIENTRY glTexSubImage1D( GLenum target, GLint level,
                                       GLint xoffset,
                                       GLsizei width, GLenum format,
                                       GLenum type, const GLvoid *pixels );


GLAPI void GLAPIENTRY glTexSubImage2D( GLenum target, GLint level,
                                       GLint xoffset, GLint yoffset,
                                       GLsizei width, GLsizei height,
                                       GLenum format, GLenum type,
                                       const GLvoid *pixels );


GLAPI void GLAPIENTRY glCopyTexSubImage2D( GLenum target, GLint level,
                                           GLint xoffset, GLint yoffset,
                                           GLint x, GLint y,
                                           GLsizei width, GLsizei height );


/*
 * Fog
 */

GLAPI void GLAPIENTRY glFogf( GLenum pname, GLfloat param );

GLAPI void GLAPIENTRY glFogi( GLenum pname, GLint param );

GLAPI void GLAPIENTRY glFogfv( GLenum pname, const GLfloat *params );

GLAPI void GLAPIENTRY glFogiv( GLenum pname, const GLint *params );


/*
 * OpenGL 1.2
 */

#define GL_RESCALE_NORMAL			0x803A
#define GL_CLAMP_TO_EDGE			0x812F
#define GL_BGR					0x80E0
#define GL_BGRA					0x80E1
#define GL_UNSIGNED_SHORT_5_6_5			0x8363
#define GL_UNSIGNED_SHORT_4_4_4_4_REV		0x8365
#define GL_UNSIGNED_SHORT_1_5_5_5_REV		0x8366
#define GL_UNSIGNED_INT_8_8_8_8_REV		0x8367
#define GL_LIGHT_MODEL_COLOR_CONTROL		0x81F8
#define GL_SINGLE_COLOR				0x81F9
#define GL_SEPARATE_SPECULAR_COLOR		0x81FA
#define GL_TEXTURE_MAX_LEVEL			0x813D

GLAPI void GLAPIENTRY glDrawRangeElements( GLenum mode, GLuint start,
	GLuint end, GLsizei count, GLenum type, const GLvoid *indices );



/*
 * OpenGL 1.3
 */

/* multitexture */
#define GL_TEXTURE0				0x84C0
#define GL_TEXTURE1				0x84C1
#define GL_TEXTURE2				0x84C2
#define GL_TEXTURE3				0x84C3
#define GL_TEXTURE4				0x84C4
#define GL_TEXTURE5				0x84C5
#define GL_TEXTURE6				0x84C6
#define GL_TEXTURE7				0x84C7
#define GL_TEXTURE8				0x84C8
#define GL_TEXTURE9				0x84C9
#define GL_TEXTURE10				0x84CA
#define GL_TEXTURE11				0x84CB
#define GL_TEXTURE12				0x84CC
#define GL_TEXTURE13				0x84CD
#define GL_TEXTURE14				0x84CE
#define GL_TEXTURE15				0x84CF
#define GL_TEXTURE16				0x84D0
#define GL_TEXTURE17				0x84D1
#define GL_TEXTURE18				0x84D2
#define GL_TEXTURE19				0x84D3
#define GL_TEXTURE20				0x84D4
#define GL_TEXTURE21				0x84D5
#define GL_TEXTURE22				0x84D6
#define GL_TEXTURE23				0x84D7
#define GL_TEXTURE24				0x84D8
#define GL_TEXTURE25				0x84D9
#define GL_TEXTURE26				0x84DA
#define GL_TEXTURE27				0x84DB
#define GL_TEXTURE28				0x84DC
#define GL_TEXTURE29				0x84DD
#define GL_TEXTURE30				0x84DE
#define GL_TEXTURE31				0x84DF
#define GL_MAX_TEXTURE_UNITS			0x84E2
/* texture_cube_map */
#define GL_NORMAL_MAP				0x8511
#define GL_REFLECTION_MAP			0x8512
/* texture_compression */
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE	0x86A0
#define GL_TEXTURE_COMPRESSED			0x86A1
/* texture_env_combine */
#define GL_COMBINE				0x8570
/* texture_border_clamp */
#define GL_CLAMP_TO_BORDER			0x812D

GLAPI void GLAPIENTRY glActiveTexture( GLenum texture );

GLAPI void GLAPIENTRY glClientActiveTexture( GLenum texture );

GLAPI void GLAPIENTRY glMultiTexCoord1d( GLenum target, GLdouble s );

GLAPI void GLAPIENTRY glMultiTexCoord1dv( GLenum target, const GLdouble *v );

GLAPI void GLAPIENTRY glMultiTexCoord1f( GLenum target, GLfloat s );

GLAPI void GLAPIENTRY glMultiTexCoord1fv( GLenum target, const GLfloat *v );

GLAPI void GLAPIENTRY glMultiTexCoord1i( GLenum target, GLint s );

GLAPI void GLAPIENTRY glMultiTexCoord1iv( GLenum target, const GLint *v );

GLAPI void GLAPIENTRY glMultiTexCoord1s( GLenum target, GLshort s );

GLAPI void GLAPIENTRY glMultiTexCoord1sv( GLenum target, const GLshort *v );

GLAPI void GLAPIENTRY glMultiTexCoord2d( GLenum target, GLdouble s, GLdouble t );

GLAPI void GLAPIENTRY glMultiTexCoord2dv( GLenum target, const GLdouble *v );

GLAPI void GLAPIENTRY glMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t );

GLAPI void GLAPIENTRY glMultiTexCoord2fv( GLenum target, const GLfloat *v );

GLAPI void GLAPIENTRY glMultiTexCoord2i( GLenum target, GLint s, GLint t );

GLAPI void GLAPIENTRY glMultiTexCoord2iv( GLenum target, const GLint *v );

GLAPI void GLAPIENTRY glMultiTexCoord2s( GLenum target, GLshort s, GLshort t );

GLAPI void GLAPIENTRY glMultiTexCoord2sv( GLenum target, const GLshort *v );

GLAPI void GLAPIENTRY glMultiTexCoord3d( GLenum target, GLdouble s, GLdouble t, GLdouble r );

GLAPI void GLAPIENTRY glMultiTexCoord3dv( GLenum target, const GLdouble *v );

GLAPI void GLAPIENTRY glMultiTexCoord3f( GLenum target, GLfloat s, GLfloat t, GLfloat r );

GLAPI void GLAPIENTRY glMultiTexCoord3fv( GLenum target, const GLfloat *v );

GLAPI void GLAPIENTRY glMultiTexCoord3i( GLenum target, GLint s, GLint t, GLint r );

GLAPI void GLAPIENTRY glMultiTexCoord3iv( GLenum target, const GLint *v );

GLAPI void GLAPIENTRY glMultiTexCoord3s( GLenum target, GLshort s, GLshort t, GLshort r );

GLAPI void GLAPIENTRY glMultiTexCoord3sv( GLenum target, const GLshort *v );

GLAPI void GLAPIENTRY glMultiTexCoord4d( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q );

GLAPI void GLAPIENTRY glMultiTexCoord4dv( GLenum target, const GLdouble *v );

GLAPI void GLAPIENTRY glMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q );

GLAPI void GLAPIENTRY glMultiTexCoord4fv( GLenum target, const GLfloat *v );

GLAPI void GLAPIENTRY glMultiTexCoord4i( GLenum target, GLint s, GLint t, GLint r, GLint q );

GLAPI void GLAPIENTRY glMultiTexCoord4iv( GLenum target, const GLint *v );

GLAPI void GLAPIENTRY glMultiTexCoord4s( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q );

GLAPI void GLAPIENTRY glMultiTexCoord4sv( GLenum target, const GLshort *v );


/*
 * More extensions
 */
#define GL_MIRRORED_REPEAT			0x8370
#define GL_FOG_COORDINATE_SOURCE		0x8450
#define GL_FOG_COORDINATE			0x8451
#define GL_FRAGMENT_DEPTH			0x8452
#define GL_FOG_COORDINATE_ARRAY			0x8457
#define GL_COLOR_SUM				0x8458
#define GL_SECONDARY_COLOR_ARRAY		0x845E
#define GL_MAX_TEXTURE_LOD_BIAS			0x84FD
#define GL_TEXTURE_FILTER_CONTROL		0x8500
#define GL_TEXTURE_LOD_BIAS			0x8501
#define GL_CLIP_VOLUME_CLIPPING_HINT_EXT	0x80F0
#define GL_INCR_WRAP_EXT			0x8507
#define GL_DECR_WRAP_EXT			0x8508

GLAPI void APIENTRY glFogCoordf (GLfloat);
GLAPI void APIENTRY glFogCoordfv (const GLfloat *);
GLAPI void APIENTRY glFogCoordd (GLdouble);
GLAPI void APIENTRY glFogCoorddv (const GLdouble *);
GLAPI void APIENTRY glFogCoordPointer (GLenum, GLsizei, const GLvoid *);

GLAPI void APIENTRY glSecondaryColor3b (GLbyte, GLbyte, GLbyte);
GLAPI void APIENTRY glSecondaryColor3bv (const GLbyte *);
GLAPI void APIENTRY glSecondaryColor3d (GLdouble, GLdouble, GLdouble);
GLAPI void APIENTRY glSecondaryColor3dv (const GLdouble *);
GLAPI void APIENTRY glSecondaryColor3f (GLfloat, GLfloat, GLfloat);
GLAPI void APIENTRY glSecondaryColor3fv (const GLfloat *);
GLAPI void APIENTRY glSecondaryColor3i (GLint, GLint, GLint);
GLAPI void APIENTRY glSecondaryColor3iv (const GLint *);
GLAPI void APIENTRY glSecondaryColor3s (GLshort, GLshort, GLshort);
GLAPI void APIENTRY glSecondaryColor3sv (const GLshort *);
GLAPI void APIENTRY glSecondaryColor3ub (GLubyte, GLubyte, GLubyte);
GLAPI void APIENTRY glSecondaryColor3ubv (const GLubyte *);
GLAPI void APIENTRY glSecondaryColor3ui (GLuint, GLuint, GLuint);
GLAPI void APIENTRY glSecondaryColor3uiv (const GLuint *);
GLAPI void APIENTRY glSecondaryColor3us (GLushort, GLushort, GLushort);
GLAPI void APIENTRY glSecondaryColor3usv (const GLushort *);
GLAPI void APIENTRY glSecondaryColorPointer (GLint, GLenum, GLsizei, const GLvoid *);

GLAPI void APIENTRY glBlendFuncSeparate (GLenum, GLenum, GLenum, GLenum);

GLAPI void APIENTRY glLockArraysEXT (GLint, GLsizei);
GLAPI void APIENTRY glUnlockArraysEXT (void);

GLAPI void APIENTRY glPolygonOffsetEXT (GLfloat, GLfloat);

GLAPI void GLAPIENTRY glCompressedTexImage1D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data );
GLAPI void GLAPIENTRY glCompressedTexImage2D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data );
GLAPI void GLAPIENTRY glCompressedTexImage3D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data );
GLAPI void GLAPIENTRY glCompressedTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data );
GLAPI void GLAPIENTRY glCompressedTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data );
GLAPI void GLAPIENTRY glCompressedTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data );
GLAPI void GLAPIENTRY glGetCompressedTexImage( GLenum target, GLint lod, GLvoid *img );

#ifdef __cplusplus
}
#endif

#endif
