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
 * Test OpenGL blending accuracy.  This demo uses
 *    destination alpha
 *    GL_NV_blend_square
 *    GL_EXT_blend_color
 *    GL_EXT_blend_equation_separate
 *    GL_EXT_blend_func_separate
 *    GL_EXT_blend_minmax
 *    GL_EXT_blend_subtract
 * for maximum number of combinations:
 * sfactor = 15 dfactor = 14 mode = 5
 * When all the above extensions are present,
 * we have 15 * 15 * 14 * 14 * 5 * 5 = 1,102,500 tries
 */


#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <GL/glut.h>

#include "base.h"
#include "util.h"
#include "extgl.h"
#include "blend.h"


static GLuint blendCount, blendError;


static GLboolean blendSquare;
static GLboolean blendColor;
static GLboolean blendEqSeparate;
static GLboolean blendFuncSeparate;
static GLboolean blendMinMax;
static GLboolean blendSubtract;


static GLenum src_fact[32];
static GLenum dst_fact[32];
static GLenum blend_eq[32];


static void
blend (GLfloat color0[], GLfloat color1[],
       GLenum sfactorRGB, GLenum dfactorRGB,
       GLenum sfactorAlpha, GLenum dfactorAlpha,
       GLenum modeRGB, GLenum modeAlpha)
{
   glClear(GL_COLOR_BUFFER_BIT);

   glDisable(GL_BLEND);

   glColor4fv(color0);
   glCallList(flatTri);

   glEnable(GL_BLEND);

   if (blendFuncSeparate) {
      myBlendFuncSeparateEXT(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
   } else {
      assert(sfactorRGB == sfactorAlpha);
      assert(dfactorRGB == dfactorAlpha);
      glBlendFunc(sfactorRGB, dfactorRGB);
   }
   if (blendEqSeparate) {
      myBlendEquationSeparateEXT(modeRGB, modeAlpha);
   } else if (blendMinMax | blendSubtract) {
      assert(modeRGB == modeAlpha);
      myBlendEquationEXT(modeRGB);
   } else {
      assert(modeRGB == modeAlpha);
      assert(modeRGB == GL_FUNC_ADD);
   }

   glColor4fv(color1);
   glCallList(flatTri);
}


void
blendEval (GLfloat color0[], GLfloat color1[], GLfloat color2[],
           GLenum sfactorRGB, GLenum dfactorRGB,
           GLenum sfactorAlpha, GLenum dfactorAlpha,
           GLenum modeRGB, GLenum modeAlpha,
           GLfloat result[])
{
   GLfloat src[4], dst[4];

   switch (sfactorRGB) {
      case GL_ZERO:
         src[R_COMP] = 0.0F;
         src[G_COMP] = 0.0F;
         src[B_COMP] = 0.0F;
         break;
      case GL_ONE:
         src[R_COMP] = 1.0F;
         src[G_COMP] = 1.0F;
         src[B_COMP] = 1.0F;
         break;
      case GL_SRC_COLOR:
         src[R_COMP] = color0[R_COMP];
         src[G_COMP] = color0[G_COMP];
         src[B_COMP] = color0[B_COMP];
         break;
      case GL_ONE_MINUS_SRC_COLOR:
         src[R_COMP] = 1.0F - color0[R_COMP];
         src[G_COMP] = 1.0F - color0[G_COMP];
         src[B_COMP] = 1.0F - color0[B_COMP];
         break;
      case GL_DST_COLOR:
         src[R_COMP] = color1[R_COMP];
         src[G_COMP] = color1[G_COMP];
         src[B_COMP] = color1[B_COMP];
         break;
      case GL_ONE_MINUS_DST_COLOR:
         src[R_COMP] = 1.0F - color1[R_COMP];
         src[G_COMP] = 1.0F - color1[G_COMP];
         src[B_COMP] = 1.0F - color1[B_COMP];
         break;
      case GL_SRC_ALPHA:
         src[R_COMP] = color0[A_COMP];
         src[G_COMP] = color0[A_COMP];
         src[B_COMP] = color0[A_COMP];
         break;
      case GL_ONE_MINUS_SRC_ALPHA:
         src[R_COMP] = 1.0F - color0[A_COMP];
         src[G_COMP] = 1.0F - color0[A_COMP];
         src[B_COMP] = 1.0F - color0[A_COMP];
         break;
      case GL_DST_ALPHA:
         src[R_COMP] = color1[A_COMP];
         src[G_COMP] = color1[A_COMP];
         src[B_COMP] = color1[A_COMP];
         break;
      case GL_ONE_MINUS_DST_ALPHA:
         src[R_COMP] = 1.0F - color1[A_COMP];
         src[G_COMP] = 1.0F - color1[A_COMP];
         src[B_COMP] = 1.0F - color1[A_COMP];
         break;
      case GL_CONSTANT_COLOR:
         src[R_COMP] = color2[R_COMP];
         src[G_COMP] = color2[G_COMP];
         src[B_COMP] = color2[B_COMP];
         break;
      case GL_ONE_MINUS_CONSTANT_COLOR:
         src[R_COMP] = 1.0F - color2[R_COMP];
         src[G_COMP] = 1.0F - color2[G_COMP];
         src[B_COMP] = 1.0F - color2[B_COMP];
         break;
      case GL_CONSTANT_ALPHA:
         src[R_COMP] = color2[A_COMP];
         src[G_COMP] = color2[A_COMP];
         src[B_COMP] = color2[A_COMP];
         break;
      case GL_ONE_MINUS_CONSTANT_ALPHA:
         src[R_COMP] = 1.0F - color2[A_COMP];
         src[G_COMP] = 1.0F - color2[A_COMP];
         src[B_COMP] = 1.0F - color2[A_COMP];
         break;
      case GL_SRC_ALPHA_SATURATE:
         src[R_COMP] = MIN(color0[A_COMP], 1.0F - color1[A_COMP]);
         src[G_COMP] = MIN(color0[A_COMP], 1.0F - color1[A_COMP]);
         src[B_COMP] = MIN(color0[A_COMP], 1.0F - color1[A_COMP]);
         break;
      NODEFAULT;
   }

   switch (sfactorAlpha) {
      case GL_ZERO:
         src[A_COMP] = 0.0F;
         break;
      case GL_ONE:
         src[A_COMP] = 1.0F;
         break;
      case GL_SRC_COLOR:
         src[A_COMP] = color0[A_COMP];
         break;
      case GL_ONE_MINUS_SRC_COLOR:
         src[A_COMP] = 1.0F - color0[A_COMP];
         break;
      case GL_DST_COLOR:
         src[A_COMP] = color1[A_COMP];
         break;
      case GL_ONE_MINUS_DST_COLOR:
         src[A_COMP] = 1.0F - color1[A_COMP];
         break;
      case GL_SRC_ALPHA:
         src[A_COMP] = color0[A_COMP];
         break;
      case GL_ONE_MINUS_SRC_ALPHA:
         src[A_COMP] = 1.0F - color0[A_COMP];
         break;
      case GL_DST_ALPHA:
         src[A_COMP] = color1[A_COMP];
         break;
      case GL_ONE_MINUS_DST_ALPHA:
         src[A_COMP] = 1.0F - color1[A_COMP];
         break;
      case GL_CONSTANT_COLOR:
         src[A_COMP] = color2[A_COMP];
         break;
      case GL_ONE_MINUS_CONSTANT_COLOR:
         src[A_COMP] = 1.0F - color2[A_COMP];
         break;
      case GL_CONSTANT_ALPHA:
         src[A_COMP] = color2[A_COMP];
         break;
      case GL_ONE_MINUS_CONSTANT_ALPHA:
         src[A_COMP] = 1.0F - color2[A_COMP];
         break;
      case GL_SRC_ALPHA_SATURATE:
         src[A_COMP] = 1.0F;
         break;
      NODEFAULT;
   }

   switch (dfactorRGB) {
      case GL_ZERO:
         dst[R_COMP] = 0.0F;
         dst[G_COMP] = 0.0F;
         dst[B_COMP] = 0.0F;
         break;
      case GL_ONE:
         dst[R_COMP] = 1.0F;
         dst[G_COMP] = 1.0F;
         dst[B_COMP] = 1.0F;
         break;
      case GL_SRC_COLOR:
         dst[R_COMP] = color0[R_COMP];
         dst[G_COMP] = color0[G_COMP];
         dst[B_COMP] = color0[B_COMP];
         break;
      case GL_ONE_MINUS_SRC_COLOR:
         dst[R_COMP] = 1.0F - color0[R_COMP];
         dst[G_COMP] = 1.0F - color0[G_COMP];
         dst[B_COMP] = 1.0F - color0[B_COMP];
         break;
      case GL_DST_COLOR:
         dst[R_COMP] = color1[R_COMP];
         dst[G_COMP] = color1[G_COMP];
         dst[B_COMP] = color1[B_COMP];
         break;
      case GL_ONE_MINUS_DST_COLOR:
         dst[R_COMP] = 1.0F - color1[R_COMP];
         dst[G_COMP] = 1.0F - color1[G_COMP];
         dst[B_COMP] = 1.0F - color1[B_COMP];
         break;
      case GL_SRC_ALPHA:
         dst[R_COMP] = color0[A_COMP];
         dst[G_COMP] = color0[A_COMP];
         dst[B_COMP] = color0[A_COMP];
         break;
      case GL_ONE_MINUS_SRC_ALPHA:
         dst[R_COMP] = 1.0F - color0[A_COMP];
         dst[G_COMP] = 1.0F - color0[A_COMP];
         dst[B_COMP] = 1.0F - color0[A_COMP];
         break;
      case GL_DST_ALPHA:
         dst[R_COMP] = color1[A_COMP];
         dst[G_COMP] = color1[A_COMP];
         dst[B_COMP] = color1[A_COMP];
         break;
      case GL_ONE_MINUS_DST_ALPHA:
         dst[R_COMP] = 1.0F - color1[A_COMP];
         dst[G_COMP] = 1.0F - color1[A_COMP];
         dst[B_COMP] = 1.0F - color1[A_COMP];
         break;
      case GL_CONSTANT_COLOR:
         dst[R_COMP] = color2[R_COMP];
         dst[G_COMP] = color2[G_COMP];
         dst[B_COMP] = color2[B_COMP];
         break;
      case GL_ONE_MINUS_CONSTANT_COLOR:
         dst[R_COMP] = 1.0F - color2[R_COMP];
         dst[G_COMP] = 1.0F - color2[G_COMP];
         dst[B_COMP] = 1.0F - color2[B_COMP];
         break;
      case GL_CONSTANT_ALPHA:
         dst[R_COMP] = color2[A_COMP];
         dst[G_COMP] = color2[A_COMP];
         dst[B_COMP] = color2[A_COMP];
         break;
      case GL_ONE_MINUS_CONSTANT_ALPHA:
         dst[R_COMP] = 1.0F - color2[A_COMP];
         dst[G_COMP] = 1.0F - color2[A_COMP];
         dst[B_COMP] = 1.0F - color2[A_COMP];
         break;
      NODEFAULT;
   }

   switch (dfactorAlpha) {
      case GL_ZERO:
         dst[A_COMP] = 0.0F;
         break;
      case GL_ONE:
         dst[A_COMP] = 1.0F;
         break;
      case GL_SRC_COLOR:
         dst[A_COMP] = color0[A_COMP];
         break;
      case GL_ONE_MINUS_SRC_COLOR:
         dst[A_COMP] = 1.0F - color0[A_COMP];
         break;
      case GL_DST_COLOR:
         dst[A_COMP] = color1[A_COMP];
         break;
      case GL_ONE_MINUS_DST_COLOR:
         dst[A_COMP] = 1.0F - color1[A_COMP];
         break;
      case GL_SRC_ALPHA:
         dst[A_COMP] = color0[A_COMP];
         break;
      case GL_ONE_MINUS_SRC_ALPHA:
         dst[A_COMP] = 1.0F - color0[A_COMP];
         break;
      case GL_DST_ALPHA:
         dst[A_COMP] = color1[A_COMP];
         break;
      case GL_ONE_MINUS_DST_ALPHA:
         dst[A_COMP] = 1.0F - color1[A_COMP];
         break;
      case GL_CONSTANT_COLOR:
         dst[A_COMP] = color2[A_COMP];
         break;
      case GL_ONE_MINUS_CONSTANT_COLOR:
         dst[A_COMP] = 1.0F - color2[A_COMP];
         break;
      case GL_CONSTANT_ALPHA:
         dst[A_COMP] = color2[A_COMP];
         break;
      case GL_ONE_MINUS_CONSTANT_ALPHA:
         dst[A_COMP] = 1.0F - color2[A_COMP];
         break;
      NODEFAULT;
   }

   switch (modeRGB) {
      case GL_FUNC_ADD:
         result[R_COMP] = color0[R_COMP] * src[R_COMP] + color1[R_COMP] * dst[R_COMP];
         result[G_COMP] = color0[G_COMP] * src[G_COMP] + color1[G_COMP] * dst[G_COMP];
         result[B_COMP] = color0[B_COMP] * src[B_COMP] + color1[B_COMP] * dst[B_COMP];
         break;
      case GL_FUNC_SUBTRACT:
         result[R_COMP] = color0[R_COMP] * src[R_COMP] - color1[R_COMP] * dst[R_COMP];
         result[G_COMP] = color0[G_COMP] * src[G_COMP] - color1[G_COMP] * dst[G_COMP];
         result[B_COMP] = color0[B_COMP] * src[B_COMP] - color1[B_COMP] * dst[B_COMP];
         break;
      case GL_FUNC_REVERSE_SUBTRACT:
         result[R_COMP] = -(color0[R_COMP] * src[R_COMP] - color1[R_COMP] * dst[R_COMP]);
         result[G_COMP] = -(color0[G_COMP] * src[G_COMP] - color1[G_COMP] * dst[G_COMP]);
         result[B_COMP] = -(color0[B_COMP] * src[B_COMP] - color1[B_COMP] * dst[B_COMP]);
         break;
      case GL_MIN:
         result[R_COMP] = MIN(color0[R_COMP], color1[R_COMP]);
         result[G_COMP] = MIN(color0[G_COMP], color1[G_COMP]);
         result[B_COMP] = MIN(color0[B_COMP], color1[B_COMP]);
         break;
      case GL_MAX:
         result[R_COMP] = MAX(color0[R_COMP], color1[R_COMP]);
         result[G_COMP] = MAX(color0[G_COMP], color1[G_COMP]);
         result[B_COMP] = MAX(color0[B_COMP], color1[B_COMP]);
         break;
      NODEFAULT;
   }

   switch (modeAlpha) {
      case GL_FUNC_ADD:
         result[A_COMP] = color0[A_COMP] * src[A_COMP] + color1[A_COMP] * dst[A_COMP];
         break;
      case GL_FUNC_SUBTRACT:
         result[A_COMP] = color0[A_COMP] * src[A_COMP] - color1[A_COMP] * dst[A_COMP];
         break;
      case GL_FUNC_REVERSE_SUBTRACT:
         result[A_COMP] = -(color0[A_COMP] * src[A_COMP] - color1[A_COMP] * dst[A_COMP]);
         break;
      case GL_MIN:
         result[A_COMP] = MIN(color0[A_COMP], color1[A_COMP]);
         break;
      case GL_MAX:
         result[A_COMP] = MAX(color0[A_COMP], color1[A_COMP]);
         break;
      NODEFAULT;
   }

   result[R_COMP] = MID(0.0F, result[R_COMP], 1.0F);
   result[G_COMP] = MID(0.0F, result[G_COMP], 1.0F);
   result[B_COMP] = MID(0.0F, result[B_COMP], 1.0F);
   result[A_COMP] = MID(0.0F, result[A_COMP], 1.0F);
}


static GLboolean
blendDraw (void)
{
   GLuint i, j, k, ii, jj, kk;
   GLfloat diff[4], real[4], eval[4], color0[4], color1[4], color2[4];

   pickColor(color0);
   pickColor(color1);
   if (blendColor) {
      pickColor(color2);
      myBlendColorEXT(color2[R_COMP], color2[G_COMP], color2[B_COMP], color2[A_COMP]);
      VERBOSE(1) {
         fprintf(stderr, " CT: r=%f, g=%f, b=%f, a=%f\n",
                         color2[R_COMP], color2[G_COMP], color2[B_COMP], color2[A_COMP]);
      }
   }
   VERBOSE(1) {
      fprintf(stderr, "SRC: r=%f, g=%f, b=%f, a=%f\n",
                      color1[R_COMP], color1[G_COMP], color1[B_COMP], color1[A_COMP]);
      fprintf(stderr, "DST: r=%f, g=%f, b=%f, a=%f\n",
                      color0[R_COMP], color0[G_COMP], color0[B_COMP], color0[A_COMP]);
   }

   blendCount = blendError = 0;

   for (i = 0; src_fact[i] != (GLenum)-1; i++) {
      for (ii = 0; src_fact[ii] != (GLenum)-1; ii++) {
         if (!blendFuncSeparate) ii = i;
         for (j = 0; dst_fact[j] != (GLenum)-1; j++) {
            for (jj = 0; dst_fact[jj] != (GLenum)-1; jj++) {
               if (!blendFuncSeparate) jj = j;
               for (k = 0; blend_eq[k] != (GLenum)-1; k++) {
                  for (kk = 0; blend_eq[kk] != (GLenum)-1; kk++) {
                     if (!blendEqSeparate) kk = k;
                     blend(color0, color1,
                          src_fact[i], dst_fact[j],
                          src_fact[ii], dst_fact[jj],
                          blend_eq[k], blend_eq[kk]);

                     readPixel(real, WIDTH/2, HEIGHT/2);

                     blendEval(color1, color0, color2,
                               src_fact[i], dst_fact[j],
                               src_fact[ii], dst_fact[jj],
                               blend_eq[k], blend_eq[kk],
                               eval);

                     diff[R_COMP] = fabs(real[R_COMP] - eval[R_COMP]);
                     diff[G_COMP] = fabs(real[G_COMP] - eval[G_COMP]);
                     diff[B_COMP] = fabs(real[B_COMP] - eval[B_COMP]);
                     if (colorBits[A_COMP]) {
                        diff[A_COMP] = fabs(real[A_COMP] - eval[A_COMP]);
                     } else {
                        diff[A_COMP] = 0.0F;
                     }

                     blendCount++;
                     if ((diff[R_COMP] > epsColor[R_COMP]) ||
                         (diff[G_COMP] > epsColor[G_COMP]) ||
                         (diff[B_COMP] > epsColor[B_COMP]) ||
                         (diff[A_COMP] > epsColor[A_COMP])) {
                        blendError++;
                        VERBOSE(1) {
                           fprintf(stderr, "%s <%s> %s, %s <%s> %s\n",
                                           blend_src_name(src_fact[i]),
                                           blend_eq_name(blend_eq[k]),
                                           blend_dst_name(dst_fact[j]),
                                           blend_src_name(src_fact[ii]),
                                           blend_eq_name(blend_eq[kk]),
                                           blend_dst_name(dst_fact[jj]));
                           fprintf(stderr, "\tReal(%.4f, %.4f, %.4f, %.4f) ~ Eval(%.4f, %.4f, %.4f, %.4f)\n",
                                           real[R_COMP], real[G_COMP], real[B_COMP], real[A_COMP],
                                           eval[R_COMP], eval[G_COMP], eval[B_COMP], eval[A_COMP]);
                        }
                     } else {
                        VERBOSE(2) {
                           fprintf(stderr, "%s <%s> %s, %s <%s> %s\n",
                                           blend_src_name(src_fact[i]),
                                           blend_eq_name(blend_eq[k]),
                                           blend_dst_name(dst_fact[j]),
                                           blend_src_name(src_fact[ii]),
                                           blend_eq_name(blend_eq[kk]),
                                           blend_dst_name(dst_fact[jj]));
                           fprintf(stderr, "\t+OK\n");
                        }
                     }
                     if (!blendEqSeparate) break;
                  }
               }
               if (!blendFuncSeparate) break;
            }
         }
         if (!blendFuncSeparate) break;
      }
   }

   if (blendError) {
      fprintf(stderr, "BLEND failed %d out of %d tests\n", blendError, blendCount);
   } else {
      fprintf(stderr, "BLEND passed %d tests\n", blendCount);
   }

   return GL_TRUE;
}


DrawProc
blendInit (void)
{
   GLuint i;

   blendSquare = glutExtensionSupported("GL_NV_blend_square");
   blendColor = glutExtensionSupported("GL_EXT_blend_color");
   blendEqSeparate = glutExtensionSupported("GL_EXT_blend_equation_separate");
   blendFuncSeparate = glutExtensionSupported("GL_EXT_blend_func_separate");
   blendMinMax = glutExtensionSupported("GL_EXT_blend_minmax");
   blendSubtract = glutExtensionSupported("GL_EXT_blend_subtract");

   i = 0;
   src_fact[i++] = GL_ZERO;
   src_fact[i++] = GL_ONE;
   if (blendSquare) {
      src_fact[i++] = GL_SRC_COLOR;
      src_fact[i++] = GL_ONE_MINUS_SRC_COLOR;
   }
   src_fact[i++] = GL_DST_COLOR;
   src_fact[i++] = GL_ONE_MINUS_DST_COLOR;
   src_fact[i++] = GL_SRC_ALPHA;
   src_fact[i++] = GL_ONE_MINUS_SRC_ALPHA;
   if (colorBits[A_COMP]) {
      src_fact[i++] = GL_DST_ALPHA;
      src_fact[i++] = GL_ONE_MINUS_DST_ALPHA;
   }
   if (blendColor) {
      src_fact[i++] = GL_CONSTANT_COLOR;
      src_fact[i++] = GL_ONE_MINUS_CONSTANT_COLOR;
      src_fact[i++] = GL_CONSTANT_ALPHA;
      src_fact[i++] = GL_ONE_MINUS_CONSTANT_ALPHA;
   }
   if (colorBits[A_COMP]) {
      /* XXX Ad = 1.0F */
      src_fact[i++] = GL_SRC_ALPHA_SATURATE;
   }
   src_fact[i] = -1;

   i = 0;
   dst_fact[i++] = GL_ZERO;
   dst_fact[i++] = GL_ONE;
   dst_fact[i++] = GL_SRC_COLOR;
   dst_fact[i++] = GL_ONE_MINUS_SRC_COLOR;
   if (blendSquare) {
      dst_fact[i++] = GL_DST_COLOR;
      dst_fact[i++] = GL_ONE_MINUS_DST_COLOR;
   }
   dst_fact[i++] = GL_SRC_ALPHA;
   dst_fact[i++] = GL_ONE_MINUS_SRC_ALPHA;
   if (colorBits[A_COMP]) {
      dst_fact[i++] = GL_DST_ALPHA;
      dst_fact[i++] = GL_ONE_MINUS_DST_ALPHA;
   }
   if (blendColor) {
      dst_fact[i++] = GL_CONSTANT_COLOR;
      dst_fact[i++] = GL_ONE_MINUS_CONSTANT_COLOR;
      dst_fact[i++] = GL_CONSTANT_ALPHA;
      dst_fact[i++] = GL_ONE_MINUS_CONSTANT_ALPHA;
   }
   dst_fact[i] = -1;

   i = 0;
   blend_eq[i++] = GL_FUNC_ADD;
   if (blendSubtract) {
      blend_eq[i++] = GL_FUNC_SUBTRACT;
      blend_eq[i++] = GL_FUNC_REVERSE_SUBTRACT;
   }
   if (blendMinMax) {
      blend_eq[i++] = GL_MIN;
      blend_eq[i++] = GL_MAX;
   }
   blend_eq[i] = -1;

   glDisable(GL_DITHER);
   glDisable(GL_DEPTH_TEST);
   glShadeModel(GL_FLAT);

   return blendDraw;
}
