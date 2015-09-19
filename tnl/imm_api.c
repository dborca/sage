#include <assert.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "main/glapi.h"
#include "main/context.h"
#include "util/macros.h"
#include "tnl.h"


void GLAPIENTRY
imm_Begin (GLenum mode)
{
    if (tnl_primitive != TNL_OUTSIDE_BE) gl_assert(0);

    gl_assert(tnl_prim_num < TNL_PRIM_MAX);

    /* start building a new primitive */
    tnl_prim[tnl_prim_num].start = tnl_vb.num;
    tnl_prim[tnl_prim_num].count = -tnl_vb.num;
    tnl_prim[tnl_prim_num].name = mode | TNL_PRIM_BEGIN;
    tnl_prim_num++;

    /* track the current primitive name */
    tnl_primitive = mode;
}


void GLAPIENTRY
imm_End (void)
{
    if (tnl_primitive == TNL_OUTSIDE_BE) gl_assert(0);

    /* close the primitive */
    tnl_prim[tnl_prim_num - 1].name |= TNL_PRIM_END;
    tnl_prim[tnl_prim_num - 1].count += tnl_vb.num;

    /* see if we can handle more primitives */
    if (tnl_prim_num == TNL_PRIM_MAX) {
	/* flush the vertices */
	tnl_flush();
    }

    /* track the current primitive name: we're outside Begin/End */
    tnl_primitive = TNL_OUTSIDE_BE;
}


void GLAPIENTRY
imm_Finish (void)
{
    /* block until all GL execution is complete */
    tnl_flush();
}


void GLAPIENTRY
imm_Flush (void)
{
    /* force execution of GL commands in finite time */
    tnl_flush();
}


#define RECT_F(x1, y1, x2, y2)				\
    do {						\
	imm_Begin(GL_POLYGON);				\
	imm_Vertex2f(x1, y1);				\
	imm_Vertex2f(x2, y1);				\
	imm_Vertex2f(x2, y2);				\
	imm_Vertex2f(x1, y2);				\
	imm_End();					\
    } while (0)


void GLAPIENTRY
imm_Rectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    RECT_F(x1, y1, x2, y2);
}


void GLAPIENTRY
imm_Rectfv (const GLfloat *v1, const GLfloat *v2)
{
    RECT_F(v1[0], v1[1], v2[0], v2[1]);
}


/* variations */


void GLAPIENTRY
imm_Rectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
    RECT_F(x1, y1, x2, y2);
}


void GLAPIENTRY
imm_Rectdv (const GLdouble *v1, const GLdouble *v2)
{
    RECT_F(v1[0], v1[1], v2[0], v2[1]);
}


void GLAPIENTRY
imm_Recti (GLint x1, GLint y1, GLint x2, GLint y2)
{
    RECT_F(I_TO_FLOAT(x1), I_TO_FLOAT(y1), I_TO_FLOAT(x2), I_TO_FLOAT(y2));
}


void GLAPIENTRY
imm_Rectiv (const GLint *v1, const GLint *v2)
{
    RECT_F(I_TO_FLOAT(v1[0]), I_TO_FLOAT(v1[1]), I_TO_FLOAT(v2[0]), I_TO_FLOAT(v2[1]));
}


void GLAPIENTRY
imm_Rects (GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
    RECT_F(S_TO_FLOAT(x1), S_TO_FLOAT(y1), S_TO_FLOAT(x2), S_TO_FLOAT(y2));
}


void GLAPIENTRY
imm_Rectsv (const GLshort *v1, const GLshort *v2)
{
    RECT_F(S_TO_FLOAT(v1[0]), S_TO_FLOAT(v1[1]), S_TO_FLOAT(v2[0]), S_TO_FLOAT(v2[1]));
}
