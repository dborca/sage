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
 * GLUT-based mini-framework.
 */


#ifndef BASE_H_included
#define BASE_H_included

#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#define MID(x,y,z)   MAX((x), MIN((y), (z)))

#define VERBOSE(n) if (verbose >= (n))
extern GLint verbose;

extern GLuint WIDTH, HEIGHT;

#define R_COMP 0/*(GL_RED   - GL_RED)*/
#define G_COMP 1/*(GL_GREEN - GL_RED)*/
#define B_COMP 2/*(GL_BLUE  - GL_RED)*/
#define A_COMP 3/*(GL_ALPHA - GL_RED)*/
extern GLint colorBits[4];
extern GLfloat epsZero, epsColor[4];

extern GLuint flatTri;

void readPixel (GLfloat color[], GLint x, GLint y);
void readPixels (GLfloat *buff, GLint x, GLint y, GLuint width, GLuint height);
void pickColor (GLfloat color[]);

typedef GLboolean (*DrawProc) (void);
typedef DrawProc (*InitProc) (void);

#endif
