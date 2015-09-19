#include <stdio.h>

#include <GL/gl.h>


/*#define NYI()					\
    do {					\
	printf(">>>%s:\n", __FUNCTION__);	\
	fflush(stdout);				\
    } while (0)
#endif*/
#define NYI() do {} while (0)


GLAPI void GLAPIENTRY
glAccum (GLenum op, GLfloat value)
{
    NYI();
}


GLAPI GLboolean GLAPIENTRY
glAreTexturesResident (GLsizei n, const GLuint *textures, GLboolean *residences)
{
    NYI();
    return 0;
}


GLAPI void GLAPIENTRY
glBitmap (GLsizei width, GLsizei height,
          GLfloat xorig, GLfloat yorig,
          GLfloat xmove, GLfloat ymove,
          const GLubyte *bitmap)
{
    NYI();
}


GLAPI void GLAPIENTRY
glClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    NYI();
}


GLAPI void GLAPIENTRY
glClearIndex (GLfloat c)
{
    NYI();
}


void GLAPIENTRY
glCompressedTexImage1D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data)
{
    NYI();
}


void GLAPIENTRY
glCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data)
{
    NYI();
}


void GLAPIENTRY
glCompressedTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    NYI();
}


void GLAPIENTRY
glCompressedTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data)
{
    NYI();
}


GLAPI void GLAPIENTRY
glCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
    NYI();
}


GLAPI void GLAPIENTRY
glCopyTexImage1D (GLenum target, GLint level,
                  GLenum internalformat,
                  GLint x, GLint y,
                  GLsizei width, GLint border)
{
    NYI();
}


GLAPI void GLAPIENTRY
glCopyTexImage2D (GLenum target, GLint level,
                  GLenum internalformat,
                  GLint x, GLint y,
                  GLsizei width, GLsizei height,
                  GLint border)
{
    NYI();
}


GLAPI void GLAPIENTRY
glCopyTexSubImage1D (GLenum target, GLint level,
                     GLint xoffset, GLint x, GLint y,
                     GLsizei width)
{
    NYI();
}


GLAPI void GLAPIENTRY
glDrawPixels (GLsizei width, GLsizei height,
              GLenum format, GLenum type,
              const GLvoid *pixels)
{
    NYI();
}


GLAPI void GLAPIENTRY
glEvalCoord1d (GLdouble u)
{
    NYI();
}


GLAPI void GLAPIENTRY
glEvalCoord1dv (const GLdouble *u)
{
    NYI();
}


GLAPI void GLAPIENTRY
glEvalCoord1f (GLfloat u)
{
    NYI();
}


GLAPI void GLAPIENTRY
glEvalCoord1fv (const GLfloat *u)
{
    NYI();
}


GLAPI void GLAPIENTRY
glEvalCoord2d (GLdouble u, GLdouble v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glEvalCoord2dv (const GLdouble *u)
{
    NYI();
}


GLAPI void GLAPIENTRY
glEvalCoord2f (GLfloat u, GLfloat v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glEvalCoord2fv (const GLfloat *u)
{
    NYI();
}


GLAPI void GLAPIENTRY
glEvalMesh1 (GLenum mode, GLint i1, GLint i2)
{
    NYI();
}


GLAPI void GLAPIENTRY
glEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2)
{
    NYI();
}


GLAPI void GLAPIENTRY
glEvalPoint1 (GLint i)
{
    NYI();
}


GLAPI void GLAPIENTRY
glEvalPoint2 (GLint i, GLint j)
{
    NYI();
}


GLAPI void GLAPIENTRY
glFeedbackBuffer (GLsizei size, GLenum type, GLfloat *buffer)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetClipPlane (GLenum plane, GLdouble *equation)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetLightfv (GLenum light, GLenum pname, GLfloat *params)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetLightiv (GLenum light, GLenum pname, GLint *params)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetMapdv (GLenum target, GLenum query, GLdouble *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetMapfv (GLenum target, GLenum query, GLfloat *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetMapiv (GLenum target, GLenum query, GLint *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetMaterialiv (GLenum face, GLenum pname, GLint *params)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetPixelMapfv (GLenum map, GLfloat *values)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetPixelMapuiv (GLenum map, GLuint *values)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetPixelMapusv (GLenum map, GLushort *values)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetPointerv (GLenum pname, GLvoid **params)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetPolygonStipple (GLubyte *mask)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetTexEnvfv (GLenum target, GLenum pname, GLfloat *params)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetTexEnviv (GLenum target, GLenum pname, GLint *params)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetTexGendv (GLenum coord, GLenum pname, GLdouble *params)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetTexGenfv (GLenum coord, GLenum pname, GLfloat *params)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetTexGeniv (GLenum coord, GLenum pname, GLint *params)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
    NYI();
}


GLAPI void GLAPIENTRY
glGetTexParameteriv (GLenum target, GLenum pname, GLint *params)
{
    NYI();
}


GLAPI void GLAPIENTRY
glIndexMask (GLuint mask)
{
    NYI();
}


GLAPI void GLAPIENTRY
glIndexPointer (GLenum type, GLsizei stride, const GLvoid *ptr)
{
    NYI();
}


GLAPI void GLAPIENTRY
glIndexd (GLdouble c)
{
    NYI();
}


GLAPI void GLAPIENTRY
glIndexdv (const GLdouble *c)
{
    NYI();
}


GLAPI void GLAPIENTRY
glIndexf (GLfloat c)
{
    NYI();
}


GLAPI void GLAPIENTRY
glIndexfv (const GLfloat *c)
{
    NYI();
}


GLAPI void GLAPIENTRY
glIndexi (GLint c)
{
    NYI();
}


GLAPI void GLAPIENTRY
glIndexiv (const GLint *c)
{
    NYI();
}


GLAPI void GLAPIENTRY
glIndexs (GLshort c)
{
    NYI();
}


GLAPI void GLAPIENTRY
glIndexsv (const GLshort *c)
{
    NYI();
}


GLAPI void GLAPIENTRY
glIndexub (GLubyte c)
{
    NYI();
}


GLAPI void GLAPIENTRY
glIndexubv (const GLubyte *c)
{
    NYI();
}


GLAPI void GLAPIENTRY
glInitNames (void)
{
    NYI();
}


GLAPI void GLAPIENTRY
glLineStipple (GLint factor, GLushort pattern)
{
    NYI();
}


GLAPI void GLAPIENTRY
glLineWidth (GLfloat width)
{
    NYI();
}


GLAPI void GLAPIENTRY
glLoadName (GLuint name)
{
    NYI();
}


GLAPI void GLAPIENTRY
glLogicOp (GLenum opcode)
{
    NYI();
}


GLAPI void GLAPIENTRY
glMap1d (GLenum target, GLdouble u1, GLdouble u2,
         GLint stride,
         GLint order, const GLdouble *points)
{
    NYI();
}


GLAPI void GLAPIENTRY
glMap1f (GLenum target, GLfloat u1, GLfloat u2,
         GLint stride,
         GLint order, const GLfloat *points)
{
    NYI();
}


GLAPI void GLAPIENTRY
glMap2d (GLenum target,
	 GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
	 GLdouble v1, GLdouble v2, GLint vstride, GLint vorder,
	 const GLdouble *points)
{
    NYI();
}


GLAPI void GLAPIENTRY
glMap2f (GLenum target,
	 GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
	 GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
	 const GLfloat *points)
{
    NYI();
}


GLAPI void GLAPIENTRY
glMapGrid1d (GLint un, GLdouble u1, GLdouble u2)
{
    NYI();
}


GLAPI void GLAPIENTRY
glMapGrid1f (GLint un, GLfloat u1, GLfloat u2)
{
    NYI();
}


GLAPI void GLAPIENTRY
glMapGrid2d (GLint un, GLdouble u1, GLdouble u2,
             GLint vn, GLdouble v1, GLdouble v2)
{
    NYI();
}


GLAPI void GLAPIENTRY
glMapGrid2f (GLint un, GLfloat u1, GLfloat u2,
             GLint vn, GLfloat v1, GLfloat v2)
{
    NYI();
}


GLAPI void GLAPIENTRY
glPassThrough (GLfloat token)
{
    NYI();
}


GLAPI void GLAPIENTRY
glPixelMapfv (GLenum map, GLsizei mapsize, const GLfloat *values)
{
    NYI();
}


GLAPI void GLAPIENTRY
glPixelMapuiv (GLenum map, GLsizei mapsize, const GLuint *values)
{
    NYI();
}


GLAPI void GLAPIENTRY
glPixelMapusv (GLenum map, GLsizei mapsize, const GLushort *values)
{
    NYI();
}


GLAPI void GLAPIENTRY
glPixelTransferf (GLenum pname, GLfloat param)
{
    NYI();
}


GLAPI void GLAPIENTRY
glPixelTransferi (GLenum pname, GLint param)
{
    NYI();
}


GLAPI void GLAPIENTRY
glPixelZoom (GLfloat xfactor, GLfloat yfactor)
{
    NYI();
}


GLAPI void GLAPIENTRY
glPointSize (GLfloat size)
{
    NYI();
}


GLAPI void GLAPIENTRY
glPolygonStipple (const GLubyte *mask)
{
    NYI();
}


GLAPI void GLAPIENTRY
glPopName (void)
{
    NYI();
}


GLAPI void GLAPIENTRY
glPrioritizeTextures (GLsizei n,
                      const GLuint *textures,
                      const GLclampf *priorities)
{
    NYI();
}


GLAPI void GLAPIENTRY
glPushName (GLuint name)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos2d (GLdouble x, GLdouble y)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos2dv (const GLdouble *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos2f (GLfloat x, GLfloat y)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos2fv (const GLfloat *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos2i (GLint x, GLint y)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos2iv (const GLint *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos2s (GLshort x, GLshort y)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos2sv (const GLshort *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos3d (GLdouble x, GLdouble y, GLdouble z)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos3dv (const GLdouble *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos3f (GLfloat x, GLfloat y, GLfloat z)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos3fv (const GLfloat *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos3i (GLint x, GLint y, GLint z)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos3iv (const GLint *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos3s (GLshort x, GLshort y, GLshort z)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos3sv (const GLshort *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos4dv (const GLdouble *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos4fv (const GLfloat *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos4i (GLint x, GLint y, GLint z, GLint w)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos4iv (const GLint *v)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w)
{
    NYI();
}


GLAPI void GLAPIENTRY
glRasterPos4sv (const GLshort *v)
{
    NYI();
}


GLAPI GLint GLAPIENTRY
glRenderMode (GLenum mode)
{
    NYI();
    return 0;
}


GLAPI void GLAPIENTRY
glSelectBuffer (GLsizei size, GLuint *buffer)
{
    NYI();
}
