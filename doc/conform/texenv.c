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
 * Test OpenGL texture environment.  This demo uses
 *    destination alpha
 *    GL_ARB_texture_env_add
 *    GL_ARB_multitexture
 *
 * In order to test output ALPHA without ALPHABUFFER,
 * we blend with GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA
 * over a known color.  But we must have BLEND test ok!
 *
 * The epsilon is not quite framebuffer epsilon, but
 * is the minimum of fbEps and texEps. XXX
 *
 * for n = 1 to numTexUnits
 *    back(n)
 *
 * after using a texture unit, can we put it in bypass mode?
 * yes, by using:
 *    myActiveTextureARB(unit);
 *    glDisable(GL_TEXTURE_2D);
 */


#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

#include "base.h"
#include "util.h"
#include "extgl.h"
#include "blend.h"
#include "texenv.h"


#define TEX_RGBA            0
#define TEX_RGB             1
#define TEX_LUMINANCE_ALPHA 2
#define TEX_LUMINANCE       3
#define TEX_ALPHA           4
#define MAX_TEXTYPE         5

#define MAX_TEXUNITS        2


static GLuint texenvCount, texenvError;


static GLint units;
static GLuint flatQuad = 0;
static GLuint tex_obj[MAX_TEXTYPE];
static const GLuint tex_width = 8, tex_height = 8;
static GLfloat *tex_data[MAX_TEXTYPE];
static GLboolean blend_over;
static GLfloat fragment[4], envcolor[MAX_TEXUNITS][4], backgrnd[4];
static GLfloat *real, *eval;


static GLenum tex_env[32];
static GLenum tex_fmt[] = {
   GL_RGBA,
   GL_RGB,
   GL_LUMINANCE_ALPHA,
   GL_LUMINANCE,
   GL_ALPHA,
   -1
};


GLboolean
texEval (GLint stride, GLfloat *previous, GLfloat envcolor[],
         GLfloat *texture, GLenum type, GLenum e, GLint size,
         GLfloat *eval)
{
   GLint i;
   GLfloat *texel, *outtx;

   texel = texture;
   outtx = eval;

   if (type == GL_RGBA) {
      switch (e) {
         case GL_REPLACE:
            memcpy(eval, texture, size * 4 * sizeof(GLfloat));
            return GL_TRUE;
         case GL_MODULATE:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] * texel[R_COMP];
               outtx[G_COMP] = previous[G_COMP] * texel[G_COMP];
               outtx[B_COMP] = previous[B_COMP] * texel[B_COMP];
               outtx[A_COMP] = previous[A_COMP] * texel[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_DECAL:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] * (1.0F - texel[A_COMP]) + texel[R_COMP] * texel[A_COMP];
               outtx[G_COMP] = previous[G_COMP] * (1.0F - texel[A_COMP]) + texel[G_COMP] * texel[A_COMP];
               outtx[B_COMP] = previous[B_COMP] * (1.0F - texel[A_COMP]) + texel[B_COMP] * texel[A_COMP];
               outtx[A_COMP] = previous[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_BLEND:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] * (1.0F - texel[R_COMP]) + envcolor[R_COMP] * texel[R_COMP];
               outtx[G_COMP] = previous[G_COMP] * (1.0F - texel[G_COMP]) + envcolor[G_COMP] * texel[G_COMP];
               outtx[B_COMP] = previous[B_COMP] * (1.0F - texel[B_COMP]) + envcolor[B_COMP] * texel[B_COMP];
               outtx[A_COMP] = previous[A_COMP] * texel[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_ADD:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] + texel[R_COMP];
               outtx[G_COMP] = previous[G_COMP] + texel[G_COMP];
               outtx[B_COMP] = previous[B_COMP] + texel[B_COMP];
               outtx[A_COMP] = previous[A_COMP] * texel[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         NODEFAULT;
      }
   } else if (type == GL_RGB) {
      switch (e) {
         case GL_REPLACE:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = texel[R_COMP];
               outtx[G_COMP] = texel[G_COMP];
               outtx[B_COMP] = texel[B_COMP];
               outtx[A_COMP] = previous[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_MODULATE:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] * texel[R_COMP];
               outtx[G_COMP] = previous[G_COMP] * texel[G_COMP];
               outtx[B_COMP] = previous[B_COMP] * texel[B_COMP];
               outtx[A_COMP] = previous[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_DECAL:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = texel[R_COMP];
               outtx[G_COMP] = texel[G_COMP];
               outtx[B_COMP] = texel[B_COMP];
               outtx[A_COMP] = previous[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_BLEND:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] * (1.0F - texel[R_COMP]) + envcolor[R_COMP] * texel[R_COMP];
               outtx[G_COMP] = previous[G_COMP] * (1.0F - texel[G_COMP]) + envcolor[G_COMP] * texel[G_COMP];
               outtx[B_COMP] = previous[B_COMP] * (1.0F - texel[B_COMP]) + envcolor[B_COMP] * texel[B_COMP];
               outtx[A_COMP] = previous[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_ADD:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] + texel[R_COMP];
               outtx[G_COMP] = previous[G_COMP] + texel[G_COMP];
               outtx[B_COMP] = previous[B_COMP] + texel[B_COMP];
               outtx[A_COMP] = previous[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         NODEFAULT;
      }
   } else if (type == GL_INTENSITY) {
      switch (e) {
         case GL_REPLACE:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = texel[0];
               outtx[G_COMP] = texel[0];
               outtx[B_COMP] = texel[0];
               outtx[A_COMP] = texel[0];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_MODULATE:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] * texel[0];
               outtx[G_COMP] = previous[G_COMP] * texel[0];
               outtx[B_COMP] = previous[B_COMP] * texel[0];
               outtx[A_COMP] = previous[A_COMP] * texel[0];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_DECAL:
            return GL_FALSE;
         case GL_BLEND:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] * (1.0F - texel[0]) + envcolor[R_COMP] * texel[0];
               outtx[G_COMP] = previous[G_COMP] * (1.0F - texel[0]) + envcolor[G_COMP] * texel[0];
               outtx[B_COMP] = previous[B_COMP] * (1.0F - texel[0]) + envcolor[B_COMP] * texel[0];
               outtx[A_COMP] = previous[A_COMP] * (1.0F - texel[0]) + envcolor[A_COMP] * texel[0];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_ADD:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] + texel[0];
               outtx[G_COMP] = previous[G_COMP] + texel[0];
               outtx[B_COMP] = previous[B_COMP] + texel[0];
               outtx[A_COMP] = previous[A_COMP] + texel[0];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         NODEFAULT;
      }
   } else if (type == GL_LUMINANCE_ALPHA) {
      switch (e) {
         case GL_REPLACE:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = texel[0];
               outtx[G_COMP] = texel[0];
               outtx[B_COMP] = texel[0];
               outtx[A_COMP] = texel[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_MODULATE:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] * texel[0];
               outtx[G_COMP] = previous[G_COMP] * texel[0];
               outtx[B_COMP] = previous[B_COMP] * texel[0];
               outtx[A_COMP] = previous[A_COMP] * texel[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_DECAL:
            return GL_FALSE;
         case GL_BLEND:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] * (1.0F - texel[0]) + envcolor[R_COMP] * texel[0];
               outtx[G_COMP] = previous[G_COMP] * (1.0F - texel[0]) + envcolor[G_COMP] * texel[0];
               outtx[B_COMP] = previous[B_COMP] * (1.0F - texel[0]) + envcolor[B_COMP] * texel[0];
               outtx[A_COMP] = previous[A_COMP] * texel[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_ADD:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] + texel[0];
               outtx[G_COMP] = previous[G_COMP] + texel[0];
               outtx[B_COMP] = previous[B_COMP] + texel[0];
               outtx[A_COMP] = previous[A_COMP] * texel[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         NODEFAULT;
      }
   } else if (type == GL_LUMINANCE) {
      switch (e) {
         case GL_REPLACE:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = texel[0];
               outtx[G_COMP] = texel[0];
               outtx[B_COMP] = texel[0];
               outtx[A_COMP] = previous[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_MODULATE:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] * texel[0];
               outtx[G_COMP] = previous[G_COMP] * texel[0];
               outtx[B_COMP] = previous[B_COMP] * texel[0];
               outtx[A_COMP] = previous[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_DECAL:
            return GL_FALSE;
         case GL_BLEND:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] * (1.0F - texel[0]) + envcolor[R_COMP] * texel[0];
               outtx[G_COMP] = previous[G_COMP] * (1.0F - texel[0]) + envcolor[G_COMP] * texel[0];
               outtx[B_COMP] = previous[B_COMP] * (1.0F - texel[0]) + envcolor[B_COMP] * texel[0];
               outtx[A_COMP] = previous[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_ADD:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP] + texel[0];
               outtx[G_COMP] = previous[G_COMP] + texel[0];
               outtx[B_COMP] = previous[B_COMP] + texel[0];
               outtx[A_COMP] = previous[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         NODEFAULT;
      }
   } else if (type == GL_ALPHA) {
      switch (e) {
         case GL_REPLACE:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP];
               outtx[G_COMP] = previous[G_COMP];
               outtx[B_COMP] = previous[B_COMP];
               outtx[A_COMP] = texel[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_MODULATE:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP];
               outtx[G_COMP] = previous[G_COMP];
               outtx[B_COMP] = previous[B_COMP];
               outtx[A_COMP] = previous[A_COMP] * texel[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_DECAL:
            return GL_FALSE;
         case GL_BLEND:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP];
               outtx[G_COMP] = previous[G_COMP];
               outtx[B_COMP] = previous[B_COMP];
               outtx[A_COMP] = previous[A_COMP] * texel[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         case GL_ADD:
            for (i = 0; i < size; i++) {
               outtx[R_COMP] = previous[R_COMP];
               outtx[G_COMP] = previous[G_COMP];
               outtx[B_COMP] = previous[B_COMP];
               outtx[A_COMP] = previous[A_COMP] * texel[A_COMP];
               texel += 4;
               outtx += 4;
               previous += stride;
            }
            break;
         NODEFAULT;
      }
   } else {
      assert(0);
   }

   i = size * 4 * sizeof(GLfloat) - 1;
   while (i >= 0) {
      eval[i] = MID(0.0F, eval[i], 1.0F);
      i--;
   }

   return GL_TRUE;
}


static void
retsol (GLuint n, GLuint sol[])
{
   GLuint u, ii, jj, err;
   GLfloat *prev;
   GLint stride;

   /* reject unsupported cases */
   for (u = 0; u < n; u++) {
      GLuint i = sol[u] >> 16;
      GLuint j = sol[u] & 0xFFFF;
      if ((tex_env[j] == GL_DECAL) && (tex_fmt[i] != GL_RGBA) && (tex_fmt[i] != GL_RGB)) {
         return;
      }
   }

#if 1/*VOODOO*/
   /* Voodoo multitex cases */
   if (n == 2) {
      GLuint j1 = sol[0] & 0xFFFF;
      GLuint j2 = sol[1] & 0xFFFF;
      if (((tex_env[j1] != GL_MODULATE) || (tex_env[j2] != GL_MODULATE)) &&
          ((tex_env[j1] != GL_REPLACE) || (tex_env[j2] != GL_ADD)) &&
          ((tex_env[j1] != GL_REPLACE) || (tex_env[j2] != GL_MODULATE)) &&
          ((tex_env[j1] != GL_MODULATE) || (tex_env[j2] != GL_REPLACE)) &&
	  /* problematic */
          ((tex_env[j1] != GL_MODULATE) || (tex_env[j2] != GL_DECAL)) &&
          ((tex_env[j1] != GL_MODULATE) || (tex_env[j2] != GL_BLEND)) &&
          ((tex_env[j1] != GL_DECAL) || (tex_env[j2] != GL_MODULATE))
         ) {
         return;
      }
   }
#endif

   /* clear background */
   glClear(GL_COLOR_BUFFER_BIT);
   if (blend_over) {
      glDisable(GL_BLEND);
      glColor4fv(backgrnd);
      glCallList(flatQuad);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   }

   /* load textures */
   for (u = 0; u < n; u++) {
      GLuint i = sol[u] >> 16;
      GLuint j = sol[u] & 0xFFFF;
      if (n > 1) {
         myActiveTextureARB(GL_TEXTURE0 + u);
      }
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, tex_obj[i]);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, tex_env[j]);
      glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, envcolor[u]);
   }

   /* draw */
   glColor4fv(fragment);
   glBegin(GL_QUADS);
   if (n > 1) {
      for (u = 0; u < n; u++) myMultiTexCoord2fARB(GL_TEXTURE0 + u, 0.0F, 0.0F);
      glVertex2i(0, 0);
      for (u = 0; u < n; u++) myMultiTexCoord2fARB(GL_TEXTURE0 + u, 0.0F, 1.0F);
      glVertex2i(tex_width, 0);
      for (u = 0; u < n; u++) myMultiTexCoord2fARB(GL_TEXTURE0 + u, 1.0F, 1.0F);
      glVertex2i(tex_width, tex_height);
      for (u = 0; u < n; u++) myMultiTexCoord2fARB(GL_TEXTURE0 + u, 1.0F, 0.0F);
      glVertex2i(0, tex_height);
   } else {
      glTexCoord2f(0.0F, 0.0F);
      glVertex2i(0, 0);
      glTexCoord2f(0.0F, 1.0F);
      glVertex2i(tex_width, 0);
      glTexCoord2f(1.0F, 1.0F);
      glVertex2i(tex_width, tex_height);
      glTexCoord2f(1.0F, 0.0F);
      glVertex2i(0, tex_height);
   }
   glEnd();

   /* read back */
   readPixels(real, 0, 0, tex_width, tex_height);

   /* calculate expected output */
   prev = fragment;
   stride = 0;
   for (u = 0; u < n; u++) {
      GLuint i = sol[u] >> 16;
      GLuint j = sol[u] & 0xFF;

      if (n > 1) {
         myActiveTextureARB(GL_TEXTURE0 + u);
      }
      glDisable(GL_TEXTURE_2D);

      texEval(stride, prev, envcolor[u], tex_data[i], tex_fmt[i], tex_env[j], tex_width * tex_height, eval);
      prev = eval;
      stride = 4;
   }

   if (blend_over) {
      GLfloat *textured = eval;
      for (jj = 0; jj < tex_height; jj++) {
         for (ii = 0; ii < tex_width; ii++) {
            blendEval(textured, backgrnd, NULL,
                      GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                      GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                      GL_FUNC_ADD, GL_FUNC_ADD,
                      textured);
            textured += 4;
         }
      }
   }

   texenvCount++;

   /* test */
   err = 0;
   for (jj = 0; jj < tex_height - 1; jj++) {
      for (ii = 0; ii < tex_width - 1; ii++) {
         GLfloat *pr = &real[(jj * tex_width + ii) * 4];
         GLfloat *pe = &eval[(jj * tex_width + ii) * 4];
         GLfloat diff[4];
         diff[R_COMP] = fabs(pr[R_COMP] - pe[R_COMP]);
         diff[G_COMP] = fabs(pr[G_COMP] - pe[G_COMP]);
         diff[B_COMP] = fabs(pr[B_COMP] - pe[B_COMP]);
         if (colorBits[A_COMP]) {
            diff[A_COMP] = fabs(pr[A_COMP] - pe[A_COMP]);
         } else {
            diff[A_COMP] = 0.0F;
         }
         if ((diff[R_COMP] > epsColor[R_COMP]) ||
             (diff[G_COMP] > epsColor[G_COMP]) ||
             (diff[B_COMP] > epsColor[B_COMP]) ||
             (diff[A_COMP] > epsColor[A_COMP])) {
            VERBOSE(1) {
               for (u = 0; u < n; u++) {
                  GLuint i = sol[u] >> 16;
                  GLuint j = sol[u] & 0xFF;
                  fprintf(stderr, "%s(%s) ", tex_env_name(tex_env[j]), tex_fmt_name(tex_fmt[i]));
               }
               fprintf(stderr, "at [%d, %d]\n", ii, jj);
            }
            VERBOSE(2) {
               fprintf(stderr, "\tReal(%.4f, %.4f, %.4f, %.4f) ~ Eval(%.4f, %.4f, %.4f, %.4f)\n",
                               pr[R_COMP], pr[G_COMP], pr[B_COMP], pr[A_COMP],
                               pe[R_COMP], pe[G_COMP], pe[B_COMP], pe[A_COMP]);
            }
            err++;
            break;
         }
      }
      if (err) {
         texenvError++;
         break;
      }
   }
}


static void
back (GLuint k, GLuint n, GLuint sol[])
{
   if (k == n) {
      retsol(n, sol);
   } else {
      int i, j;
      for (i = 0; tex_fmt[i] != (GLenum)-1; i++) {
         for (j = 0; tex_env[j] != (GLenum)-1; j++) {
            sol[k] = (i << 16) | j;
            back(k + 1, n, sol);
         }
      }
   }
}


static GLboolean
texenvDraw (void)
{
   GLint i, u, size = tex_width * tex_height;
   GLuint sol[MAX_TEXUNITS];

   /* allocate work area */
   real = malloc(size * 4 * sizeof(GLfloat));
   if (real == NULL) {
      return GL_FALSE;
   }
   eval = malloc(size * 4 * sizeof(GLfloat));
   if (eval == NULL) {
      free(real);
      return GL_FALSE;
   }

   /* pick random working colors */
   pickColor(fragment);
   for (u = 0; u < units; u++) {
      pickColor(envcolor[u]);
   }
   if (blend_over) {
      pickColor(backgrnd);
   }
   VERBOSE(1) {
      fprintf(stderr, "FRAGMENT: r=%f, g=%f, b=%f, a=%f\n",
                      fragment[R_COMP], fragment[G_COMP], fragment[B_COMP], fragment[A_COMP]);
      for (u = 0; u < units; u++) {
         fprintf(stderr, "ENVCOLR%d: r=%f, g=%f, b=%f, a=%f\n", u,
                         envcolor[u][R_COMP], envcolor[u][G_COMP], envcolor[u][B_COMP], envcolor[u][A_COMP]);
      }
      if (blend_over) {
         fprintf(stderr, "BACKGRND: r=%f, g=%f, b=%f, a=%f\n",
                         backgrnd[R_COMP], backgrnd[G_COMP], backgrnd[B_COMP], backgrnd[A_COMP]);
      }
   }

   /* pick random textures */
   glGenTextures(MAX_TEXTYPE, tex_obj);
   for (i = 0; i < MAX_TEXTYPE; i++) {
      GLfloat col[4], *texel;
      GLint k;

      col[R_COMP] = (GLfloat)rand() / RAND_MAX;
      col[G_COMP] = (GLfloat)rand() / RAND_MAX;
      col[B_COMP] = (GLfloat)rand() / RAND_MAX;
      col[A_COMP] = (GLfloat)rand() / RAND_MAX;

      tex_data[i] = malloc(size * 4 * sizeof(GLfloat));
      if (tex_data[i] == NULL) {
         while (i) {
            free(tex_data[--i]);
         }
         free(eval);
         free(real);
         return GL_FALSE;
      }

      glBindTexture(GL_TEXTURE_2D, tex_obj[i]);

      texel = real;
      for (k = 0; k < size; k++) {
         texel[R_COMP] = col[R_COMP];
         texel[G_COMP] = col[G_COMP];
         texel[B_COMP] = col[B_COMP];
         texel[A_COMP] = col[A_COMP];
         texel += 4;
      }

      glTexImage2D(GL_TEXTURE_2D, 0, tex_fmt[i], tex_width, tex_height, 0,
                   GL_RGBA, GL_FLOAT, real);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, tex_data[i]);

      /* XXX */
      texel = tex_data[i];
      for (k = 0; k < size; k++) {
         if (texel[A_COMP] > 1.0F) {
            texel[A_COMP] = 1.0F;
         }
         texel += 4;
      }
      /* XXX */

      VERBOSE(1) {
         fprintf(stderr, "TEXTURE%d: r=%f, g=%f, b=%f, a=%f\n", i,
                         tex_data[i][R_COMP], tex_data[i][G_COMP], tex_data[i][B_COMP], tex_data[i][A_COMP]);
      }
   }

   texenvCount = texenvError = 0;

   /* set up 1:1 ratio */
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(0, WIDTH, 0, HEIGHT, -1.0F, 1.0F);
   glMatrixMode(GL_MODELVIEW);

   /* backtracking */
   back(0, units, sol);

   /* restore projection */
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

   /* free work area */
   glDeleteTextures(MAX_TEXTYPE, tex_obj);
   for (i = 0; i < MAX_TEXTYPE; i++) {
      free(tex_data[i]);
   }
   free(eval);
   free(real);

   /* report */
   if (texenvError) {
      fprintf(stderr, "TEXENV failed %d out of %d tests\n", texenvError, texenvCount);
   } else {
      fprintf(stderr, "TEXENV passed %d tests\n", texenvCount);
   }

   return GL_TRUE;
}


DrawProc
texenvInit (void)
{
   GLuint i;
   GLboolean texenvAdd = glutExtensionSupported("GL_EXT_texture_env_add");

   units = 1;
   if (glutExtensionSupported("GL_ARB_multitexture")) {
      glGetIntegerv(GL_MAX_TEXTURE_UNITS, &units);
      if (units > MAX_TEXUNITS) {
         units = MAX_TEXUNITS;
      }
   }

   i = 0;
   tex_env[i++] = GL_REPLACE;
   tex_env[i++] = GL_MODULATE;
   tex_env[i++] = GL_DECAL;
   tex_env[i++] = GL_BLEND;
   if (texenvAdd) {
      tex_env[i++] = GL_ADD;
   }
   tex_env[i] = -1;

   blend_over = (colorBits[A_COMP] == 0);

   flatQuad = glGenLists(1);
   glNewList(flatQuad, GL_COMPILE);
      glBegin(GL_QUADS);
         glVertex2i(0, 0);
         glVertex2i(tex_width, 0);
         glVertex2i(tex_width, tex_height);
         glVertex2i(0, tex_height);
      glEnd();
   glEndList();

   glDisable(GL_DITHER);
   glDisable(GL_DEPTH_TEST);
   glShadeModel(GL_FLAT);

   return texenvDraw;
}
