#include "GL/gl.h"

#include "glinternal.h"
#include "context.h"
#include "glapi.h"


typedef struct {
    GLfloat4 position;
    GLfloat distance;
    GLboolean valid;
    GLfloat4 color0;
    GLfloat4 color1;
    GLfloat4 texture[MAX_TEXTURE_UNITS];
} RASTER;
