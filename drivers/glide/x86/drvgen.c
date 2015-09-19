#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <GL/gl.h>
#include <glide.h>
#include <g3ext.h>

#include "glinternal.h"
#include "main/context.h"
#include "drivers/glide/drv.h"


int
main (int argc, char **argv)
{
    const char *myname = argv[0];
    char *out = NULL;
    FILE *f = stdout;

    while (--argc) {
	char *p = *++argv;
	if (!strcmp(p, "-o")) {
	    if (argc <= 1) {
		fprintf(stderr, "%s: argument to `%s' is missing\n", myname, p);
		return -1;
	    }
	    out = *++argv;
	    argc--;
	}
    }

    if (out != NULL) {
	f = fopen(out, "wt");
	if (f == NULL) {
	    fprintf(stderr, "%s: cannot open `%s' for writing\n", myname, out);
	    return -2;
	}
    }

    fprintf(f, "%%define FX_PACKEDCOLOR\t\t\t%d\n", FX_PACKEDCOLOR);
    fprintf(f, "\n");
    fprintf(f, "%%define GR_VERTEX_X_OFFSET\t\t%d\n", offsetof(GrVertex, x));
    fprintf(f, "%%define GR_VERTEX_Y_OFFSET\t\t%d\n", offsetof(GrVertex, y));
    fprintf(f, "%%define GR_VERTEX_OOZ_OFFSET\t\t%d\n", offsetof(GrVertex, ooz));
    fprintf(f, "%%define GR_VERTEX_OOW_OFFSET\t\t%d\n", offsetof(GrVertex, oow));
#if FX_PACKEDCOLOR
    fprintf(f, "%%define GR_VERTEX_PARGB_OFFSET\t\t%d\n", offsetof(GrVertex, pargb));
#else
    fprintf(f, "%%define GR_VERTEX_RGB_OFFSET\t\t%d\n", offsetof(GrVertex, r));
    fprintf(f, "%%define GR_VERTEX_A_OFFSET\t\t%d\n", offsetof(GrVertex, a));
#endif
    fprintf(f, "%%define GR_VERTEX_SOW_TMU0_OFFSET\t%d\n", offsetof(GrVertex, tmuvtx[0].sow));
    fprintf(f, "%%define GR_VERTEX_TOW_TMU0_OFFSET\t%d\n", offsetof(GrVertex, tmuvtx[0].tow));
    fprintf(f, "%%define GR_VERTEX_OOW_TMU0_OFFSET\t%d\n", offsetof(GrVertex, tmuvtx[0].oow));
    fprintf(f, "%%define GR_VERTEX_SOW_TMU1_OFFSET\t%d\n", offsetof(GrVertex, tmuvtx[1].sow));
    fprintf(f, "%%define GR_VERTEX_TOW_TMU1_OFFSET\t%d\n", offsetof(GrVertex, tmuvtx[1].tow));
    fprintf(f, "%%define GR_VERTEX_OOW_TMU1_OFFSET\t%d\n", offsetof(GrVertex, tmuvtx[1].oow));
    fprintf(f, "%%define GR_VERTEX_FOG_OFFSET\t\t%d\n", offsetof(GrVertex, fog));
#if FX_PACKEDCOLOR
    fprintf(f, "%%define GR_VERTEX_PSPEC_OFFSET\t\t%d\n", offsetof(GrVertex, pspec));
#else
    fprintf(f, "%%define GR_VERTEX_SPEC_OFFSET\t\t%d\n", offsetof(GrVertex, r1));
#endif
    /*fprintf(f, "%%define GR_VERTEX_PAD_OFFSET\t\t%d\n", offsetof(GrVertex, pad));*/
    fprintf(f, "%%define sizeof_GrVertex\t\t\t%d\n", sizeof(GrVertex));
    fprintf(f, "\n");
    fprintf(f, "%%define TFX_OBJ_SSCALE\t\t\t%d\n", offsetof(TFX_OBJ, sscale));
    fprintf(f, "\n");
    fprintf(f, "%%define SETUP_TEX0\t\t\t(1<<0)\n");
    fprintf(f, "%%define SETUP_TEX1\t\t\t(1<<1)\n");
    fprintf(f, "%%define SETUP_PTX0\t\t\t(1<<2)\n");
    fprintf(f, "%%define SETUP_PTX1\t\t\t(1<<3)\n");
    fprintf(f, "%%define SETUP_FOGC\t\t\t(1<<4)\n");
    fprintf(f, "%%define SETUP_SPEC\t\t\t(1<<5)\n");
    fprintf(f, "%%define SETUP_MAX\t\t\t(1<<6)\n");
    fprintf(f, "%%define SETUP_PSIZ\t\t\t(1<<6)\n");

    if (out != NULL) {
	fclose(f);
    }
    return 0;
}
