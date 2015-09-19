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


#ifndef BLEND_H_included
#define BLEND_H_included

DrawProc blendInit (void);

extern void
blendEval (GLfloat color0[], GLfloat color1[], GLfloat color2[],
           GLenum sfactorRGB, GLenum dfactorRGB,
           GLenum sfactorAlpha, GLenum dfactorAlpha,
           GLenum modeRGB, GLenum modeAlpha,
           GLfloat result[]);

#endif
