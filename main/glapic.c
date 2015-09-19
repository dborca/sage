#include "GL/gl.h"

#include "glapi.h"


#ifndef X86
#define ENTRY_NR(z, x, y, w, k)	\
GLAPI z GLAPIENTRY gl##x y	\
{				\
    GLCALL(x) w;		\
}
#define ENTRY_RV(z, x, y, w, k)	\
GLAPI z GLAPIENTRY gl##x y	\
{				\
    return GLCALL(x) w;		\
}


#include "glapit.h"
#undef ENTRY_NR
#undef ENTRY_RV
#endif /* X86 */
