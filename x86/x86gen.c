#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include <GL/gl.h>

#include "../glinternal.h"
#include "../main/context.h"
#include "../main/matrix.h"
#include "../tnl/tnl.h"
#include "cpu.h"


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

    fprintf(f, ";---------------------------------------\n");
    fprintf(f, "; SSE\n");
    fprintf(f, ";---------------------------------------\n");
    fprintf(f, "%%define X\t\t\t0\n");
    fprintf(f, "%%define Y\t\t\t1\n");
    fprintf(f, "%%define Z\t\t\t2\n");
    fprintf(f, "%%define W\t\t\t3\n");
    fprintf(f, "%%define SHUF(_x, _y, _z, _w)\t(((_x)<<0) | ((_y)<<2) | ((_z)<<4) | ((_w)<<6))\n");
    fprintf(f, "\n\n;---------------------------------------\n");
    fprintf(f, "; CPU\n");
    fprintf(f, ";---------------------------------------\n");
    fprintf(f, "%%define _CPU_HAS_CPUID\t\t%d\n", _CPU_HAS_CPUID);
    fprintf(f, "%%define _CPU_FEATURE_MMX\t%d\n", _CPU_FEATURE_MMX);
    fprintf(f, "%%define _CPU_FEATURE_SSE\t%d\n", _CPU_FEATURE_SSE);
    fprintf(f, "%%define _CPU_FEATURE_SSE2\t%d\n", _CPU_FEATURE_SSE2);
    fprintf(f, "%%define _CPU_FEATURE_3DNOW\t%d\n", _CPU_FEATURE_3DNOW);
    fprintf(f, "%%define _CPU_FEATURE_3DNOWPLUS\t%d\n", _CPU_FEATURE_3DNOWPLUS);
    fprintf(f, "%%define _CPU_FEATURE_MMXPLUS\t%d\n", _CPU_FEATURE_MMXPLUS);
    fprintf(f, "\n\n;---------------------------------------\n");
    fprintf(f, "; OpenGL\n");
    fprintf(f, ";---------------------------------------\n");
    fprintf(f, "%%define GL_TEXTURE0\t\t%d\n", GL_TEXTURE0);
    fprintf(f, "\n\n;---------------------------------------\n");
    fprintf(f, "; Core\n");
    fprintf(f, ";---------------------------------------\n");
    fprintf(f, "%%define MAT_MAT\t\t\t%d\n", offsetof(MATRIX, mat));
    fprintf(f, "\n");
    fprintf(f, "%%define TEX_UNIT_OBJECT\t\t%d\n", offsetof(TEX_UNIT, object));
    fprintf(f, "%%define sizeof_TEX_UNIT\t\t%d\n", sizeof(TEX_UNIT));
    fprintf(f, "%%define TEX_OBJ_DRIVERDATA\t%d\n", offsetof(TEX_OBJ, driverData));
    fprintf(f, "\n\n;---------------------------------------\n");
    fprintf(f, "; TNL\n");
    fprintf(f, ";---------------------------------------\n");
    fprintf(f, "%%define TNL_COLOR0_BIT\t\t%d\n", TNL_COLOR0_BIT);
    fprintf(f, "%%define TNL_NORMAL_BIT\t\t%d\n", TNL_NORMAL_BIT);
    fprintf(f, "%%define TNL_TEXCOORD0_BIT\t%d\n", TNL_TEXCOORD0_BIT);
    fprintf(f, "\n");
    fprintf(f, "%%define sizeof_TNL_ARRAY\t%d\n", sizeof(TNL_ARRAY));
    fprintf(f, "\n");
    fprintf(f, "%%define PRIM_COUNT\t\t%d\n", offsetof(TNL_PRIMITIVE, count));
    fprintf(f, "%%define PRIM_ORMASK\t\t%d\n", offsetof(TNL_PRIMITIVE, ormask));
    fprintf(f, "%%define sizeof_TNL_PRIMITIVE\t%d\n", sizeof(TNL_PRIMITIVE));
    fprintf(f, "\n");
    fprintf(f, "%%define TNL_VB_NUM\t\t%d\n", offsetof(TNL_VERTEXBUFFER, num));
    fprintf(f, "%%define TNL_VB_MAX\t\t%d\n", offsetof(TNL_VERTEXBUFFER, max));
    fprintf(f, "%%define TNL_VB_LEN\t\t%d\n", offsetof(TNL_VERTEXBUFFER, len));
    fprintf(f, "%%define TNL_VB_FLAGS\t\t%d\n", offsetof(TNL_VERTEXBUFFER, flags));
    fprintf(f, "%%define TNL_VB_CLIPMASK\t\t%d\n", offsetof(TNL_VERTEXBUFFER, clipmask));
    fprintf(f, "%%define TNL_VB_VERTEX_STRIDE\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_VERTEX].stride));
    fprintf(f, "%%define TNL_VB_VERTEX_DATA\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_VERTEX].data));
    fprintf(f, "%%define TNL_VB_VERTEX_PTR\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_VERTEX].ptr));
    fprintf(f, "%%define TNL_VB_COLOR0_STRIDE\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_COLOR0].stride));
    fprintf(f, "%%define TNL_VB_COLOR0_DATA\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_COLOR0].data));
    fprintf(f, "%%define TNL_VB_COLOR0_PTR\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_COLOR0].ptr));
    fprintf(f, "%%define TNL_VB_COLOR1_STRIDE\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_COLOR1].stride));
    fprintf(f, "%%define TNL_VB_COLOR1_DATA\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_COLOR1].data));
    fprintf(f, "%%define TNL_VB_COLOR1_PTR\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_COLOR1].ptr));
    fprintf(f, "%%define TNL_VB_NORMAL_STRIDE\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_NORMAL].stride));
    fprintf(f, "%%define TNL_VB_NORMAL_DATA\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_NORMAL].data));
    fprintf(f, "%%define TNL_VB_NORMAL_PTR\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_NORMAL].ptr));
    fprintf(f, "%%define TNL_VB_TEXCOORD0_STRIDE\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_TEXCOORD0].stride));
    fprintf(f, "%%define TNL_VB_TEXCOORD0_DATA\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_TEXCOORD0].data));
    fprintf(f, "%%define TNL_VB_TEXCOORD0_PTR\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_TEXCOORD0].ptr));
    fprintf(f, "%%define TNL_VB_FOGCOORD_STRIDE\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_FOGCOORD].stride));
    fprintf(f, "%%define TNL_VB_FOGCOORD_DATA\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_FOGCOORD].data));
    fprintf(f, "%%define TNL_VB_FOGCOORD_PTR\t%d\n", offsetof(TNL_VERTEXBUFFER, attr[TNL_FOGCOORD].ptr));
    fprintf(f, "%%define TNL_VB_CLIP\t\t%d\n", offsetof(TNL_VERTEXBUFFER, clip));
    fprintf(f, "%%define TNL_VB_NDC\t\t%d\n", offsetof(TNL_VERTEXBUFFER, ndc));
    fprintf(f, "%%define TNL_VB_VEYE\t\t%d\n", offsetof(TNL_VERTEXBUFFER, veye));
    fprintf(f, "%%define TNL_VB_NEYE\t\t%d\n", offsetof(TNL_VERTEXBUFFER, neye));
    fprintf(f, "%%define TNL_VB_VEYN\t\t%d\n", offsetof(TNL_VERTEXBUFFER, veyn));
    fprintf(f, "%%define TNL_VB_REFL\t\t%d\n", offsetof(TNL_VERTEXBUFFER, refl));
    fprintf(f, "\n");
    fprintf(f, "%%define qN\t\t\t%d\n", TNL_CLIP_NEAR);
    fprintf(f, "%%define qF\t\t\t%d\n", TNL_CLIP_FAR);
    fprintf(f, "%%define qT\t\t\t%d\n", TNL_CLIP_TOP);
    fprintf(f, "%%define qB\t\t\t%d\n", TNL_CLIP_BOTTOM);
    fprintf(f, "%%define qL\t\t\t%d\n", TNL_CLIP_LEFT);
    fprintf(f, "%%define qR\t\t\t%d\n", TNL_CLIP_RIGHT);

    if (out != NULL) {
	fclose(f);
    }
    return 0;
}
