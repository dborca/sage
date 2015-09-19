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
 */


#ifndef TEXENV_H_included
#define TEXENV_H_included

DrawProc texenvInit (void);

extern GLboolean
texEval (GLint stride, GLfloat *previous, GLfloat envcolor[],
         GLfloat *texture, GLenum type, GLenum e, GLint size,
         GLfloat *eval);

#endif
