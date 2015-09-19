#include "GL/gl.h"

#include "glinternal.h"
#include "glapi.h"
#include "context.h"
#include "matrix.h"


void GLAPIENTRY
imm_CullFace (GLenum mode)
{
    FLUSH_VERTICES();

    ctx_cull_face = mode;

    ctx_gl_state |= NEW_CULL;
}


void GLAPIENTRY
imm_FrontFace (GLenum mode)
{
    FLUSH_VERTICES();

    ctx_front_face = mode;

    ctx_gl_state |= NEW_CULL;
}


void GLAPIENTRY
imm_PolygonMode (GLenum face, GLenum mode)
{
    FLUSH_VERTICES();

    /* face: GL_FRONT, GL_BACK, GL_FRONT_AND_BACK */
    /* mode: GL_FILL, GL_LINE, GL_POINT */
    if (face != GL_BACK) { /* front */
	switch (mode) {
	    case GL_FILL:
		ctx_polygon_mode[0] = POLYGON_FILL;
		break;
	    case GL_LINE:
		ctx_polygon_mode[0] = POLYGON_LINE;
		break;
	    case GL_POINT:
		ctx_polygon_mode[0] = POLYGON_POINT;
		break;
	}
    }
    if (face != GL_FRONT) { /* back */
	switch (mode) {
	    case GL_FILL:
		ctx_polygon_mode[1] = POLYGON_FILL;
		break;
	    case GL_LINE:
		ctx_polygon_mode[1] = POLYGON_LINE;
		break;
	    case GL_POINT:
		ctx_polygon_mode[1] = POLYGON_POINT;
		break;
	}
    }
}


void GLAPIENTRY
imm_ClipPlane (GLenum plane, const GLdouble *equation)
{
    GLfloat4 tmp;

    FLUSH_VERTICES();

    tmp[0] = equation[0];
    tmp[1] = equation[1];
    tmp[2] = equation[2];
    tmp[3] = equation[3];
    matrix_mul_vec4(ctx_userplanes[0][plane - GL_CLIP_PLANE0], get_imv(), tmp);

    ctx_gl_state |= NEW_USERCLIP;
}


void GLAPIENTRY
imm_PolygonOffset (GLfloat factor, GLfloat units)
{
    FLUSH_VERTICES();

    ctx_polygon_offset_factor = factor;
    ctx_polygon_offset_bias = units * ctx_const_depth_resolution;
    ctx_gl_state |= 0; /* XXX GULP!?! */
}


void GLAPIENTRY
imm_PolygonOffsetEXT (GLfloat factor, GLfloat bias)
{
    FLUSH_VERTICES();

    ctx_polygon_offset_factor = factor;
    ctx_polygon_offset_bias = bias;
    ctx_gl_state |= 0; /* XXX GULP!?! */
}
