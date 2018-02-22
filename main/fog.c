#include <assert.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "glapi.h"
#include "context.h"
#include "util/macros.h"


void GLAPIENTRY
imm_Fogfv (GLenum pname, const GLfloat *params)
{
    FLUSH_VERTICES();

    switch (pname) {
	case GL_FOG_MODE:
	    ctx_fog.mode = params[0];
	    break;
	case GL_FOG_DENSITY:
	    ctx_fog.density = params[0];
	    break;
	case GL_FOG_START:
	    ctx_fog.start = params[0];
	    break;
	case GL_FOG_END:
	    ctx_fog.end = params[0];
	    break;
	case GL_FOG_COORDINATE_SOURCE:
	    ctx_fog.source = params[0];
	    break;
	case GL_FOG_COLOR:
	    ctx_fog.color[0] = params[0];
	    ctx_fog.color[1] = params[1];
	    ctx_fog.color[2] = params[2];
	    ctx_fog.color[3] = params[3];
	    break;
	default:
	    gl_assert(0);
	    return;
    }

    ctx_gl_state |= NEW_FOG;
}


void GLAPIENTRY
imm_Fogf (GLenum pname, GLfloat param)
{
    imm_Fogfv(pname, &param);
}


/* variations */


void GLAPIENTRY
imm_Fogiv (GLenum pname, const GLint *params)
{
    FLUSH_VERTICES();

    switch (pname) {
	case GL_FOG_MODE:
	    ctx_fog.mode = params[0];
	    break;
	case GL_FOG_DENSITY:
	    ctx_fog.density = params[0];
	    break;
	case GL_FOG_START:
	    ctx_fog.start = params[0];
	    break;
	case GL_FOG_END:
	    ctx_fog.end = params[0];
	    break;
	case GL_FOG_COORDINATE_SOURCE:
	    ctx_fog.source = params[0];
	    break;
	case GL_FOG_COLOR:
	    ctx_fog.color[0] = I_TO_FLOAT(params[0]);
	    ctx_fog.color[1] = I_TO_FLOAT(params[1]);
	    ctx_fog.color[2] = I_TO_FLOAT(params[2]);
	    ctx_fog.color[3] = I_TO_FLOAT(params[3]);
	    break;
	default:
	    gl_assert(0);
	    return;
    }

    ctx_gl_state |= NEW_FOG;
}


void GLAPIENTRY
imm_Fogi (GLenum pname, GLint param)
{
    imm_Fogiv(pname, &param);
}
