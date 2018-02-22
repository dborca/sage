/*
 * OpenGL testsuite
 * Version:  0.1
 *
 * Copyright (C) 2005  Daniel Borca   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * DANIEL BORCA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


/*
 * Extension wrappers.
 */


#include <stdlib.h>
#include <GL/glut.h>

#include "extgl.h"


GLAPI void GLAPIENTRY
myBlendColorEXT (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
#ifdef _WIN32
   static PFNGLBLENDCOLOREXTPROC BlendColorEXT = NULL;
   if (BlendColorEXT == NULL) {
      BlendColorEXT = (PFNGLBLENDCOLOREXTPROC)wglGetProcAddress("glBlendColorEXT");
   }
   if (BlendColorEXT != NULL) {
      BlendColorEXT(red, green, blue, alpha);
   }
#else
   glBlendColorEXT(red, green, blue, alpha);
#endif
}


GLAPI void GLAPIENTRY
myBlendFuncSeparateEXT (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha)
{
#ifdef _WIN32
   static PFNGLBLENDFUNCSEPARATEEXTPROC BlendFuncSeparateEXT = NULL;
   if (BlendFuncSeparateEXT == NULL) {
      BlendFuncSeparateEXT = (PFNGLBLENDFUNCSEPARATEEXTPROC)wglGetProcAddress("glBlendFuncSeparateEXT");
   }
   if (BlendFuncSeparateEXT != NULL) {
      BlendFuncSeparateEXT(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
   }
#else
   glBlendFuncSeparateEXT(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
#endif
}


GLAPI void GLAPIENTRY
myBlendEquationEXT (GLenum mode)
{
#ifdef _WIN32
   static PFNGLBLENDEQUATIONEXTPROC BlendEquationEXT = NULL;
   if (BlendEquationEXT == NULL) {
      BlendEquationEXT = (PFNGLBLENDEQUATIONEXTPROC)wglGetProcAddress("glBlendEquationEXT");
   }
   if (BlendEquationEXT != NULL) {
      BlendEquationEXT(mode);
   }
#else
   glBlendEquationEXT(mode);
#endif
}


GLAPI void GLAPIENTRY
myBlendEquationSeparateEXT (GLenum modeRGB, GLenum modeAlpha)
{
#ifdef _WIN32
   static PFNGLBLENDEQUATIONSEPARATEEXTPROC BlendEquationSeparateEXT;
   if (BlendEquationSeparateEXT == NULL) {
      BlendEquationSeparateEXT = (PFNGLBLENDEQUATIONSEPARATEEXTPROC)wglGetProcAddress("glBlendEquationSeparateEXT");
   }
   if (BlendEquationSeparateEXT != NULL) {
      BlendEquationSeparateEXT(modeRGB, modeAlpha);
   }
#else
   /*glBlendEquationSeparateEXT(modeRGB, modeAlpha);*/
#endif
}


GLAPI void APIENTRY
myActiveTextureARB (GLenum texture)
{
#ifdef _WIN32
   static PFNGLACTIVETEXTUREARBPROC ActiveTextureARB;
   if (ActiveTextureARB == NULL) {
      ActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
   }
   if (ActiveTextureARB != NULL) {
      ActiveTextureARB(texture);
   }
#else
   glActiveTextureARB(texture);
#endif
}


GLAPI void APIENTRY
myMultiTexCoord2fARB (GLenum target, GLfloat s, GLfloat t)
{
#ifdef _WIN32
   static PFNGLMULTITEXCOORD2FARBPROC MultiTexCoord2fARB = NULL;
   if (MultiTexCoord2fARB == NULL) {
      MultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
   }
   if (MultiTexCoord2fARB != NULL) {
      MultiTexCoord2fARB(target, s, t);
   }
#else
   glMultiTexCoord2fARB(target, s, t);
#endif
}

