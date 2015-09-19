#include <stdlib.h>

#include "GL/gl.h"
#include "GL/sage.h"

#include "glinternal.h"
#include "main/context.h"
#include "tnl/tnl.h"
#include "main/glapi.h"
#include "util/cfg.h"
#include "driver.h"
#include "drv.h"


static sageContext *current;	/**< Current context */

static int hardware = -1;	/**< Hardware type */

SWvertex *vb;
int (*drv_multipass) (int pass);
void (*drv_emitvertices) (int n);
void (*drv_copypv) (int vdst, int vsrc);
void (*drv_interp) (float t, int vdst, int vout, int vin);


int
sage_init (void)
{
    if (hardware >= 0) {
	return 32;
    }

    if (cfg_load("sage.ini") != 0) {
	cfg_load("/etc/sage.ini");
    }

    hardware = 0;

    current = NULL;

    return 32;
}


sageContext *
sage_open (int db_flag,
	   int red_size, int green_size, int blue_size,
	   int alpha_size, int depth_size, int stencil_size)
{
    sageContext *ctx;

    if (hardware < 0) {
	goto exit_error;
    }

    /* Create context */
    ctx = malloc(sizeof(sageContext));
    if (ctx == NULL) {
	goto exit_error;
    }

    /* Initialize the core */
    if (ctx_init(db_flag,
		 red_size, green_size, blue_size,
		 alpha_size, depth_size, stencil_size) != 0) {
	goto exit_error1;
    }

    /* Finish driver setup */
    vb = malloc((tnl_vb.max + TNL_CLIPPED_VERTS) * sizeof(SWvertex));
    if (vb == NULL) {
	goto exit_error3;
    }

    drv_multipass = drv_multipass_none;

    return ctx;

  exit_error3:
    ctx_fini();
  exit_error1:
    free(ctx);
  exit_error:
    return NULL;
}


int
sage_bind (sageContext *ctx, void *win, int width, int height)
{
    if (hardware < 0) {
	return -1;
    }

    if (ctx == NULL || win == NULL) {
	if (current != NULL) {
	    /* unbind */
	    current = NULL;
	}
	return 0;
    }
    if (ctx == current) {
	if (win == current->drawable) {
	    /* nop */
	    return 0;
	}
	/* rebind */
    }

    ctx->drawable = win;
    current = ctx;

    GLCALL(Viewport)(0, 0, width, height);

    return 0;
}


void
sage_shut (sageContext *ctx)
{
    if (hardware < 0) {
	return;
    }
    if (ctx == NULL) {
	return;
    }

    sage_bind(NULL, NULL, 0, 0);

    free(vb);
    ctx_fini();
    free(ctx);
}


void
sage_fini (void)
{
    if (hardware < 0) {
	return;
    }

    if (current != NULL) {
	sage_shut(current);
    }

    cfg_kill();

    hardware = -1;
}


void
sage_swap (int interval)
{
    (void)interval;
    GLCALL(Flush)();
}
