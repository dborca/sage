#include <assert.h>
#include <math.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "glapi.h"
#include "context.h"
#include "util/macros.h"
#include "matrix.h"


void GLAPIENTRY
imm_ColorMaterial (GLenum face, GLenum mode)
{
    switch (face) {
    case GL_FRONT:
    case GL_BACK:
    case GL_FRONT_AND_BACK:
	break;
    default:
	gl_assert(0);
	return;
    }
    switch (mode) {
    case GL_EMISSION:
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_AMBIENT_AND_DIFFUSE:
	break;
    default:
	gl_assert(0);
	return;
    }
    /* XXX must be outside begin/end */

    FLUSH_VERTICES();

    ctx_colormat_face = face;
    ctx_colormat_mode = mode;

    ctx_gl_state |= NEW_LIGHT;
}


void GLAPIENTRY
imm_ShadeModel (GLenum mode)
{
    switch (mode) {
    case GL_SMOOTH:
    case GL_FLAT:	break;
    default: gl_assert(0);
	return;
    }

    FLUSH_VERTICES();

    ctx_shade_model = mode;

    ctx_gl_state |= 0; /* XXX GULP!?! */
}


void GLAPIENTRY
imm_Lightfv (GLenum light, GLenum pname, const GLfloat *params)
{
    LIGHT *li = &ctx_light[light - GL_LIGHT0];

    FLUSH_VERTICES();

    switch (pname) {
	case GL_AMBIENT:
	    li->ambient[0] = params[0];
	    li->ambient[1] = params[1];
	    li->ambient[2] = params[2];
	    li->ambient[3] = params[3];
	    break;
	case GL_DIFFUSE:
	    li->diffuse[0] = params[0];
	    li->diffuse[1] = params[1];
	    li->diffuse[2] = params[2];
	    li->diffuse[3] = params[3];
	    break;
	case GL_SPECULAR:
	    li->specular[0] = params[0];
	    li->specular[1] = params[1];
	    li->specular[2] = params[2];
	    li->specular[3] = params[3];
	    break;
	case GL_POSITION:
	    matrix_mul_vec4(li->position, ctx_mx_modelview_top->mat, params);
	    NORM3(li->_posnorm, +, li->position);
	    break;
	case GL_SPOT_DIRECTION:
	    li->spot_direction[0] = params[0];
	    li->spot_direction[1] = params[1];
	    li->spot_direction[2] = params[2];
	    li->spot_direction[3] = 1.0F;
	    matrix_mul_vec4(li->_spot_eye_norm, get_imv(), li->spot_direction);
	    NORM3(li->_spot_eye_norm, -, li->_spot_eye_norm);
	    break;
	case GL_SPOT_EXPONENT:
	    li->spot_exponent = params[0];
	    break;
	case GL_SPOT_CUTOFF:
	    li->spot_cutoff = params[0];
	    li->_spot_cutcos = COS(params[0] * M_PI / 180.0F);
	    break;
	case GL_CONSTANT_ATTENUATION:
	    li->attenuation[0] = params[0];
	    break;
	case GL_LINEAR_ATTENUATION:
	    li->attenuation[1] = params[0];
	    break;
	case GL_QUADRATIC_ATTENUATION:
	    li->attenuation[2] = params[0];
	    break;
    }

    ctx_gl_state |= NEW_LIGHT;
}


void GLAPIENTRY
imm_Lightf (GLenum light, GLenum pname, GLfloat param)
{
    imm_Lightfv(light, pname, &param);
}


void GLAPIENTRY
imm_LightModelfv (GLenum pname, const GLfloat *params)
{

    FLUSH_VERTICES();

    switch (pname) {
	case GL_LIGHT_MODEL_AMBIENT:
	    ctx_light_model_ambient[0] = params[0];
	    ctx_light_model_ambient[1] = params[1];
	    ctx_light_model_ambient[2] = params[2];
	    ctx_light_model_ambient[3] = params[3];
	    break;
	case GL_LIGHT_MODEL_LOCAL_VIEWER:
	    ctx_light_model_localv = params[0];
	    break;
	case GL_LIGHT_MODEL_TWO_SIDE:
	    ctx_light_model_twoside = params[0];
	    break;
	case GL_LIGHT_MODEL_COLOR_CONTROL:
	    ctx_light_model_colctrl = params[0];
	    break;
    }

    ctx_gl_state |= NEW_LIGHT;
}


void GLAPIENTRY
imm_LightModelf (GLenum pname, GLfloat param)
{
    imm_LightModelfv(pname, &param);
}


/* variations */


void GLAPIENTRY
imm_Lightiv (GLenum light, GLenum pname, const GLint *params)
{
    LIGHT *li = &ctx_light[light - GL_LIGHT0];

    FLUSH_VERTICES();

    switch (pname) {
	case GL_AMBIENT:
	    li->ambient[0] = I_TO_FLOAT(params[0]);
	    li->ambient[1] = I_TO_FLOAT(params[1]);
	    li->ambient[2] = I_TO_FLOAT(params[2]);
	    li->ambient[3] = I_TO_FLOAT(params[3]);
	    break;
	case GL_DIFFUSE:
	    li->diffuse[0] = I_TO_FLOAT(params[0]);
	    li->diffuse[1] = I_TO_FLOAT(params[1]);
	    li->diffuse[2] = I_TO_FLOAT(params[2]);
	    li->diffuse[3] = I_TO_FLOAT(params[3]);
	    break;
	case GL_SPECULAR:
	    li->specular[0] = I_TO_FLOAT(params[0]);
	    li->specular[1] = I_TO_FLOAT(params[1]);
	    li->specular[2] = I_TO_FLOAT(params[2]);
	    li->specular[3] = I_TO_FLOAT(params[3]);
	    break;
	case GL_POSITION: {
	    GLfloat4 tmp;
	    tmp[0] = params[0];
	    tmp[1] = params[1];
	    tmp[2] = params[2];
	    tmp[3] = params[3];
	    matrix_mul_vec4(li->position, ctx_mx_modelview_top->mat, tmp);
	    NORM3(li->_posnorm, +, li->position);
	    break;
	}
	case GL_SPOT_DIRECTION:
	    li->spot_direction[0] = params[0];
	    li->spot_direction[1] = params[1];
	    li->spot_direction[2] = params[2];
	    li->spot_direction[3] = 1.0F;
	    matrix_mul_vec4(li->_spot_eye_norm, get_imv(), li->spot_direction);
	    NORM3(li->_spot_eye_norm, -, li->_spot_eye_norm);
	    break;
	case GL_SPOT_EXPONENT:
	    li->spot_exponent = params[0];
	    break;
	case GL_SPOT_CUTOFF:
	    li->spot_cutoff = params[0];
	    li->_spot_cutcos = COS(params[0] * M_PI / 180.0F);
	    break;
	case GL_CONSTANT_ATTENUATION:
	    li->attenuation[0] = params[0];
	    break;
	case GL_LINEAR_ATTENUATION:
	    li->attenuation[1] = params[0];
	    break;
	case GL_QUADRATIC_ATTENUATION:
	    li->attenuation[2] = params[0];
	    break;
    }

    ctx_gl_state |= NEW_LIGHT;
}


void GLAPIENTRY
imm_Lighti (GLenum light, GLenum pname, GLint param)
{
    imm_Lightiv(light, pname, &param);
}


void GLAPIENTRY
imm_LightModeliv (GLenum pname, const GLint *params)
{

    FLUSH_VERTICES();

    switch (pname) {
	case GL_LIGHT_MODEL_AMBIENT:
	    ctx_light_model_ambient[0] = I_TO_FLOAT(params[0]);
	    ctx_light_model_ambient[1] = I_TO_FLOAT(params[1]);
	    ctx_light_model_ambient[2] = I_TO_FLOAT(params[2]);
	    ctx_light_model_ambient[3] = I_TO_FLOAT(params[3]);
	    break;
	case GL_LIGHT_MODEL_LOCAL_VIEWER:
	    ctx_light_model_localv = params[0];
	    break;
	case GL_LIGHT_MODEL_TWO_SIDE:
	    ctx_light_model_twoside = params[0];
	    break;
	case GL_LIGHT_MODEL_COLOR_CONTROL:
	    ctx_light_model_colctrl = params[0];
	    break;
    }

    ctx_gl_state |= NEW_LIGHT;
}


void GLAPIENTRY
imm_LightModeli (GLenum pname, GLint param)
{
    imm_LightModeliv(pname, &param);
}
