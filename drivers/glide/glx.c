#include <dlfcn.h>
#include <stdlib.h>

/* new glx.h dependencies: */
#include <stddef.h>
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

#include <GL/gl.h>
#include <GL/glx.h>
#include "GL/sage.h"

#include "glinternal.h"
#include "main/context.h"


static int glx_use_gl = 0;
static int glx_rgba = 0;
static int glx_doublebuffer = 0;
static int glx_red_size = 0;
static int glx_green_size = 0;
static int glx_blue_size = 0;
static int glx_alpha_size = 0;
static int glx_depth_size = 0;
static int glx_stencil_size = 0;

static void *gc = NULL;


XVisualInfo *
glXChooseVisual (Display *dpy, int screen, int *attribList)
{
    static int colordepth = -1;
    XVisualInfo theTemplate;
    XVisualInfo *visuals, *rv;
    int numVisuals;
    long mask;
    int i;

    /* process user flags */
    while (*attribList != None) {
	switch (*attribList) {
	    case GLX_USE_GL:
		glx_use_gl = 1;
		break;
	    case GLX_RGBA:
		glx_rgba = 1;
		break;
	    case GLX_DOUBLEBUFFER:
		glx_doublebuffer = 1;
		break;
	    case GLX_RED_SIZE:
		glx_red_size = *++attribList;
		break;
	    case GLX_GREEN_SIZE:
		glx_green_size = *++attribList;
		break;
	    case GLX_BLUE_SIZE:
		glx_blue_size = *++attribList;
		break;
	    case GLX_ALPHA_SIZE:
		glx_alpha_size = *++attribList;
		break;
	    case GLX_DEPTH_SIZE:
		glx_depth_size = *++attribList;
		break;
	    case GLX_STENCIL_SIZE:
		glx_stencil_size = *++attribList;
		break;
	}
	attribList++;
    }

    if (!glx_rgba) {
	return NULL;
    }

    if (colordepth <= 0) {
	colordepth = sage_init();
	if (colordepth <= 0) {
	    return NULL;
	}
	atexit(sage_fini);
    }

    /* get list of all visuals on this screen */
    theTemplate.screen = screen;
    mask = VisualScreenMask;
    visuals = XGetVisualInfo(dpy, mask, &theTemplate, &numVisuals);

    for (i = 1; i < numVisuals; i++) {
	int c_class =
#if defined(__cplusplus) || defined(c_plusplus)
	visuals[i].c_class
#else
	visuals[i].class
#endif
	;
	if (c_class == DirectColor || c_class == TrueColor) {
	    break;
	}
    }

    if (i >= numVisuals) {
	rv = NULL;
    } else {
	rv = malloc(sizeof(XVisualInfo));
	if (rv != NULL) {
	    *rv = visuals[i];
	}
    }
    XFree(visuals);
    return rv;
}


GLXContext
glXCreateContext (Display *dpy, XVisualInfo *visInfo, GLXContext shareList, Bool direct)
{
    return (GLXContext)sage_open(glx_doublebuffer,
				 glx_red_size, glx_green_size,
				 glx_blue_size, glx_alpha_size,
				 glx_depth_size, glx_stencil_size);
}


Bool
glXMakeCurrent (Display *dpy, GLXDrawable drawable, GLXContext ctx)
{
    int width = 0, height = 0;
    XWindowAttributes attr;
    if (drawable != None) {
	XGetWindowAttributes(dpy, drawable, &attr);
	width = attr.width;
	height = attr.height;
    }
    if (sage_bind((sageContext *)ctx, (void *)drawable, width, height) == 0) {
	gc = ctx;
	return True;
    }
    return False;
}


void
glXSwapBuffers (Display *dpy, GLXDrawable drawable)
{
    sage_swap(1);
}


void
glXDestroyContext (Display *dpy, GLXContext ctx)
{
    sage_bind(NULL, NULL, 0, 0);
    sage_shut((sageContext *)ctx);
    sage_fini();
}


int
glXGetConfig (Display *dpy, XVisualInfo *vis, int attribute, int *value)
{
    return 0;
}


GLXContext
glXGetCurrentContext (void)
{
    return (GLXContext)gc;
}


GLXDrawable
glXGetCurrentDrawable (void)
{
    return None;
}


void (*glXGetProcAddress(const GLubyte *procname)) (void)
{
    return (void (*) (void))dlsym(NULL, (const char *)procname);
}


Bool
glXQueryVersion(Display *dpy, int *major, int *minor)
{
    *major = 1;
    *minor = 0;
    return False;
}


void
glXCopyContext (Display *dpy, GLXContext src, GLXContext dst, unsigned long mask)
{
}


Bool
glXQueryExtension (Display *dpy, int *errorBase, int *eventBase)
{
    int major_op, erb, evb;
    Bool rv;

    rv = XQueryExtension(dpy, "GLX", &major_op, &evb, &erb);
    if (rv) {
	if (errorBase) *errorBase = erb;
	if (eventBase) *eventBase = evb;
    }
    return rv;
}


Bool
glXIsDirect (Display *dpy, GLXContext ctx)
{
   return True;
}


const char *
glXQueryExtensionsString (Display *dpy, int screen)
{
    return
	"GLX_ARB_get_proc_address "
	;
}


void
glXWaitX (void)
{
}
