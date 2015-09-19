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


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <GL/glut.h>

#include "base.h"
#include "blend.h"
#include "texenv.h"


GLint verbose = 0;


GLuint WIDTH = 640, HEIGHT = 480;
static GLfloat factor = 1.0F;
static GLfloat zplane[] = { 1.0F, 100.0F };


GLint colorBits[4];
GLfloat epsZero, epsColor[4];


GLuint flatTri = 0;


static InitProc testInit;
static DrawProc testDraw;


static float
calcEps (float eps, int bits)
{
   float e;

   if (bits == 0) {
      e = eps;
   } else {
      e = 1.0 / ((1 << bits) - 1) + eps;
      if (e > 1.0) {
         e = 1.0;
      }
   }
   return e;
}


void
readPixel (GLfloat color[], GLint x, GLint y)
{
   glutSwapBuffers();
   glutSwapBuffers();

   glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, color);
}


void
readPixels (GLfloat *buff, GLint x, GLint y, GLuint width, GLuint height)
{
   glutSwapBuffers();
   glutSwapBuffers();

   glReadPixels(x, y, width, height, GL_RGBA, GL_FLOAT, buff);
}


void
pickColor (GLfloat color[])
{
   GLfloat col[4];

   col[R_COMP] = (GLfloat)rand() / RAND_MAX;
   col[G_COMP] = (GLfloat)rand() / RAND_MAX;
   col[B_COMP] = (GLfloat)rand() / RAND_MAX;
   col[A_COMP] = (GLfloat)rand() / RAND_MAX;

   glClear(GL_COLOR_BUFFER_BIT);

   glColor4fv(col);
   glCallList(flatTri);

   readPixel(color, WIDTH/2, HEIGHT/2);
}


static void
Draw (void)
{
   if (!testDraw()) {
      /* XXX do something */
   }
}


static void
Idle (void)
{
   /*glutPostRedisplay();*/
}


static void
Reshape (int w, int h)
{
   GLfloat left, right, bottom, top;

   WIDTH = w;
   HEIGHT = h;

   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   left = -factor;
   right = factor;
   bottom = -factor;
   top = factor;
   zplane[0] = -1.0;
   zplane[1] = 1.0;
   if (w <= h) {
      GLfloat ratio = (GLfloat)h/(GLfloat)w;
      bottom *= ratio;
      top *= ratio;
   } else {
      GLfloat ratio = (GLfloat)w/(GLfloat)h;
      left *= ratio;
      right *= ratio;
   }
   glOrtho(left, right, bottom, top, zplane[0], zplane[1]);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}


static void
Key (unsigned char key, int x, int y)
{
   (void)(x && y);

   switch (key) {
      case 27:
         exit(0);
         break;
   }
   glutPostRedisplay();
}


static void
SpecialKey (int key, int x, int y)
{
   (void)(x && y);

   switch (key) {
      case GLUT_KEY_UP:
         break;
      case GLUT_KEY_DOWN:
         break;
      case GLUT_KEY_LEFT:
         break;
      case GLUT_KEY_RIGHT:
         break;
   }
   glutPostRedisplay();
}


static void
Init (int argc, char **argv)
{
   static const char *test = "texenv";
   GLfloat a = 0.1F;
   GLfloat a3 = a / sqrt(3.0);

   while (--argc) {
      char *p = *++argv;
      if (!strcmp(p, "-rand")) {
         srand(time(NULL));
      } else if (!strcmp(p, "-reset")) {
         srand(1);
      } else if (!strcmp(p, "-v")) {
         if (argc == 1) {
            fprintf(stderr, "error: argument to `%s' is missing\n", p);
            exit(-1);
         } else {
            --argc;
            p = *++argv;
            verbose = atoi(p);
         }
      } else if (!strcmp(p, "-t")) {
         if (argc == 1) {
            fprintf(stderr, "error: argument to `%s' is missing\n", p);
            exit(-1);
         } else {
            --argc;
            p = *++argv;
            test = p;
         }
      } else {
         fprintf(stderr, "error: unrecognized option `%s'\n", p);
         exit(-1);
      }
   }

   glGetIntegerv(GL_RED_BITS,   &colorBits[R_COMP]);
   glGetIntegerv(GL_GREEN_BITS, &colorBits[G_COMP]);
   glGetIntegerv(GL_BLUE_BITS,  &colorBits[B_COMP]);
   glGetIntegerv(GL_ALPHA_BITS, &colorBits[A_COMP]);

   epsZero = 1.0 / pow(2.0, 13.0);
   epsColor[R_COMP] = calcEps(epsZero, colorBits[R_COMP]);
   epsColor[G_COMP] = calcEps(epsZero, colorBits[G_COMP]);
   epsColor[B_COMP] = calcEps(epsZero, colorBits[B_COMP]);
   epsColor[A_COMP] = calcEps(epsZero, colorBits[A_COMP]);

   VERBOSE(1) {
      fprintf(stderr, "FRAMEBUFFER: %d, %d, %d, %d\n",
                      colorBits[R_COMP],
                      colorBits[G_COMP],
                      colorBits[B_COMP],
                      colorBits[A_COMP]);
      fprintf(stderr, "EPSILON: zero=%f, r=%f, g=%f, b=%f, a=%f\n", epsZero,
                      epsColor[R_COMP],
                      epsColor[G_COMP],
                      epsColor[B_COMP],
                      epsColor[A_COMP]);
   }

   flatTri = glGenLists(1);
   glNewList(flatTri, GL_COMPILE);
      glBegin(GL_TRIANGLES);
         glVertex2f(     0.0F,  a3);
         glVertex2f(-a / 2.0F, -a3 / 2.0F);
         glVertex2f( a / 2.0F, -a3 / 2.0F);
      glEnd();
   glEndList();

   if (!strcmp(test, "blend")) {
      testInit = blendInit;
   } else if (!strcmp(test, "texenv")) {
      testInit = texenvInit;
   }

   testDraw = testInit();
   if (!testDraw) {
      fprintf(stderr, "error: cannot initialize `%s'\n", test);
   }
}


int
main (int argc, char **argv)
{
   glutInit(&argc, argv);
   glutInitWindowPosition(0, 0);
   glutInitWindowSize(WIDTH, HEIGHT);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   if (!glutCreateWindow(argv[0])) {
      exit(-1);
   }
   glutReshapeFunc(Reshape);
   glutKeyboardFunc(Key);
   glutSpecialFunc(SpecialKey);
   glutDisplayFunc(Draw);
   glutIdleFunc(Idle);
   Init(argc, argv);
   glutMainLoop();
   return 0;
}
