#include "GL/gl.h"

#include "glinternal.h"
#include "main/glapi.h"
#include "util/macros.h"
#include "main/dlist.h"
#include "tnl.h"


void GLAPIENTRY
sav_Begin (GLenum mode)
{
    DL_NODE *node = dlist_new_operation(OP_BEGIN);
    (node + 1)->i = mode | TNL_PRIM_WEAK;
}


void GLAPIENTRY
sav_End (void)
{
    DL_NODE *node = dlist_new_operation(OP_END);
}


void GLAPIENTRY
sav_Finish (void)
{
    /* block until all GL execution is complete */
    imm_Finish();
}


void GLAPIENTRY
sav_Flush (void)
{
    /* force execution of GL commands in finite time */
    imm_Flush();
}


#define RECT_F(x1, y1, x2, y2)				\
    do {						\
	DL_NODE *node = dlist_new_operation(OP_RECTF);	\
	(node + 1)->f = x1;				\
	(node + 2)->f = y1;				\
	(node + 3)->f = x2;				\
	(node + 4)->f = y2;				\
    } while (0)


void GLAPIENTRY
sav_Rectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    RECT_F(x1, y1, x2, y2);
}


void GLAPIENTRY
sav_Rectfv (const GLfloat *v1, const GLfloat *v2)
{
    RECT_F(v1[0], v1[1], v2[0], v2[1]);
}


/* variations */


void GLAPIENTRY
sav_Rectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
    RECT_F(x1, y1, x2, y2);
}


void GLAPIENTRY
sav_Rectdv (const GLdouble *v1, const GLdouble *v2)
{
    RECT_F(v1[0], v1[1], v2[0], v2[1]);
}


void GLAPIENTRY
sav_Recti (GLint x1, GLint y1, GLint x2, GLint y2)
{
    RECT_F(I_TO_FLOAT(x1), I_TO_FLOAT(y1), I_TO_FLOAT(x2), I_TO_FLOAT(y2));
}


void GLAPIENTRY
sav_Rectiv (const GLint *v1, const GLint *v2)
{
    RECT_F(I_TO_FLOAT(v1[0]), I_TO_FLOAT(v1[1]), I_TO_FLOAT(v2[0]), I_TO_FLOAT(v2[1]));
}


void GLAPIENTRY
sav_Rects (GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
    RECT_F(S_TO_FLOAT(x1), S_TO_FLOAT(y1), S_TO_FLOAT(x2), S_TO_FLOAT(y2));
}


void GLAPIENTRY
sav_Rectsv (const GLshort *v1, const GLshort *v2)
{
    RECT_F(S_TO_FLOAT(v1[0]), S_TO_FLOAT(v1[1]), S_TO_FLOAT(v2[0]), S_TO_FLOAT(v2[1]));
}
