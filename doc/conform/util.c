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
 * Miscellaneous functions.
 */


#include <assert.h>
#include <GL/glut.h>

#include "util.h"


const char *
blend_src_name (GLenum factor)
{
   switch (factor) {
      CASE_STRING(GL_ZERO);
      CASE_STRING(GL_ONE);
      CASE_STRING(GL_SRC_COLOR);
      CASE_STRING(GL_ONE_MINUS_SRC_COLOR);
      CASE_STRING(GL_DST_COLOR);
      CASE_STRING(GL_ONE_MINUS_DST_COLOR);
      CASE_STRING(GL_SRC_ALPHA);
      CASE_STRING(GL_ONE_MINUS_SRC_ALPHA);
      CASE_STRING(GL_DST_ALPHA);
      CASE_STRING(GL_ONE_MINUS_DST_ALPHA);
      CASE_STRING(GL_CONSTANT_COLOR);
      CASE_STRING(GL_ONE_MINUS_CONSTANT_COLOR);
      CASE_STRING(GL_CONSTANT_ALPHA);
      CASE_STRING(GL_ONE_MINUS_CONSTANT_ALPHA);
      CASE_STRING(GL_SRC_ALPHA_SATURATE);
      NODEFAULT;
   }
}


const char *
blend_dst_name (GLenum factor)
{
   switch (factor) {
      CASE_STRING(GL_ZERO);
      CASE_STRING(GL_ONE);
      CASE_STRING(GL_SRC_COLOR);
      CASE_STRING(GL_ONE_MINUS_SRC_COLOR);
      CASE_STRING(GL_DST_COLOR);
      CASE_STRING(GL_ONE_MINUS_DST_COLOR);
      CASE_STRING(GL_SRC_ALPHA);
      CASE_STRING(GL_ONE_MINUS_SRC_ALPHA);
      CASE_STRING(GL_DST_ALPHA);
      CASE_STRING(GL_ONE_MINUS_DST_ALPHA);
      CASE_STRING(GL_CONSTANT_COLOR);
      CASE_STRING(GL_ONE_MINUS_CONSTANT_COLOR);
      CASE_STRING(GL_CONSTANT_ALPHA);
      CASE_STRING(GL_ONE_MINUS_CONSTANT_ALPHA);
      NODEFAULT;
   }
}


const char *
blend_eq_name (GLenum eq)
{
   switch (eq) {
      CASE_STRING(GL_FUNC_ADD);
      CASE_STRING(GL_FUNC_SUBTRACT);
      CASE_STRING(GL_FUNC_REVERSE_SUBTRACT);
      CASE_STRING(GL_MIN);
      CASE_STRING(GL_MAX);
      NODEFAULT;
   }
}


const char *
tex_env_name (GLenum e)
{
   switch (e) {
      CASE_STRING(GL_REPLACE);
      CASE_STRING(GL_MODULATE);
      CASE_STRING(GL_DECAL);
      CASE_STRING(GL_BLEND);
      CASE_STRING(GL_ADD);
      NODEFAULT;
   }
}


const char *
tex_fmt_name (GLenum t)
{
   switch (t) {
      CASE_STRING(GL_RGBA);
      CASE_STRING(GL_RGB);
      CASE_STRING(GL_LUMINANCE_ALPHA);
      CASE_STRING(GL_LUMINANCE);
      CASE_STRING(GL_ALPHA);
      NODEFAULT;
   }
}
