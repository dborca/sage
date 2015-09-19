#include "GL/gl.h"

#include "glapi.h"


GLAPI void GLAPIENTRY
glActiveTextureARB (GLenum texture)
{
    GLCALL(ActiveTexture)(texture);
}


GLAPI void GLAPIENTRY
glMultiTexCoord1fARB (GLenum texture, GLfloat s)
{
    GLCALL(MultiTexCoord1f)(texture, s);
}


GLAPI void GLAPIENTRY
glMultiTexCoord1fvARB (GLenum texture, const GLfloat *v)
{
    GLCALL(MultiTexCoord1fv)(texture, v);
}


GLAPI void GLAPIENTRY
glMultiTexCoord2fARB (GLenum texture, GLfloat s, GLfloat t)
{
    GLCALL(MultiTexCoord2f)(texture, s, t);
}


GLAPI void GLAPIENTRY
glMultiTexCoord2fvARB (GLenum texture, const GLfloat *v)
{
    GLCALL(MultiTexCoord2fv)(texture, v);
}


GLAPI void GLAPIENTRY
glMultiTexCoord3fARB (GLenum texture, GLfloat s, GLfloat t, GLfloat r)
{
    GLCALL(MultiTexCoord3f)(texture, s, t, r);
}


GLAPI void GLAPIENTRY
glMultiTexCoord3fvARB (GLenum texture, const GLfloat *v)
{
    GLCALL(MultiTexCoord3fv)(texture, v);
}


GLAPI void GLAPIENTRY
glMultiTexCoord4fARB (GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    GLCALL(MultiTexCoord4f)(texture, s, t, r, q);
}


GLAPI void GLAPIENTRY
glMultiTexCoord4fvARB (GLenum texture, const GLfloat *v)
{
    GLCALL(MultiTexCoord4fv)(texture, v);
}


GLAPI void GLAPIENTRY
glClientActiveTextureARB (GLenum texture)
{
    GLCALL(ClientActiveTexture)(texture);
}


GLAPI void GLAPIENTRY
glBlendFuncSeparateEXT (GLenum sfactorRGB, GLenum dfactorRGB,
		        GLenum sfactorAlpha, GLenum dfactorAlpha)
{
    GLCALL(BlendFuncSeparate)(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}
