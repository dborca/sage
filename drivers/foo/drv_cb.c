#include <string.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "main/context.h"
#include "tnl/tnl.h"
#include "driver.h"
#include "drv.h"


void
drv_Clear (GLbitfield mask)
{
}


void
drv_ClearColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
}


void
drv_ClearDepth (GLclampd depth)
{
}


const char *
drv_GetString (int name)
{
    if (name == GL_RENDERER) {
	return "SAGE foo";
    }
    return NULL;
}


void
drv_DrawBuffer (GLenum mode)
{
}


void
drv_ReadPixels (GLint x, GLint y,
                GLsizei width, GLsizei height,
                GLenum format, GLenum type,
                GLvoid *pixels)
{
}
