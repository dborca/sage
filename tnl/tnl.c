#include <stdlib.h>
#include <string.h>

#include "GL/gl.h"
#include "glinternal.h"
#include "util/macros.h"
#include "util/alloc.h"
#include "util/list.h"
#include "util/cfg.h"
#include "tnl.h"

#define LOGGING 0
#include "log.h"


GLfloat4 tnl_init_attr[TNL_BITS] = {
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* vertex */
    { 1.0F, 1.0F, 1.0F, 1.0F }, /* color0 */
    { 0.0F, 0.0F, 0.0F, 1.0F }, /* color1 */
    { 0.0F, 0.0F, 1.0F, 0.0F }, /* normal */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* tex0 */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* tex1 */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* tex2 */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* tex3 */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* tex4 */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* tex5 */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* tex6 */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* tex7 */
    { 0.2F, 0.2F, 0.2F, 1.0F },	/* ambient front */
    { 0.2F, 0.2F, 0.2F, 1.0F },	/* ambient back */
    { 0.8F, 0.8F, 0.8F, 1.0F },	/* diffuse front */
    { 0.8F, 0.8F, 0.8F, 1.0F },	/* diffuse back */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* specular front */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* specular back */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* emission front */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* emission back */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* shininess front */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* shininess back */
    { 0.0F, 0.0F, 0.0F, 0.0F },	/* fog */
    { 1.0F, 0.0F, 0.0F, 0.0F }	/* edgeflag */
};

GLuint tnl_general_flags;

TNL_VERTEXBUFFER tnl_vb;

GLenum tnl_primitive;
GLuint tnl_prim_size;

TNL_PRIMITIVE *tnl_prim;
GLuint tnl_prim_num;


TNL_CLIENT tnl_client;


GLbitfield tnl_extra_flags;


GLbitfield tnl_light_state;
GLbitfield tnl_texgen_state;


int *tnl_vb_elt;


static void
_tnl_playback (void)
{
    int i, j;
    GLfloat4 *last_attr[TNL_BITS];
    for (j = 1; j < TNL_BITS; j++) {
	last_attr[j] = &tnl_init_attr[j];
    }
    for (j = 1; j < TNL_BITS; j++) {
	int attr = 1 << j;
	if (tnl_general_flags & attr) {
	    for (i = 0; i < tnl_vb.num; i++) {
		if (tnl_vb.flags[i] & attr) {
		    last_attr[j] = &tnl_vb.attr[j].ptr[i];
		} else {
		    tnl_vb.attr[j].ptr[i][0] = last_attr[j][0][0];
		    tnl_vb.attr[j].ptr[i][1] = last_attr[j][0][1];
		    tnl_vb.attr[j].ptr[i][2] = last_attr[j][0][2];
		    tnl_vb.attr[j].ptr[i][3] = last_attr[j][0][3];
		}
	    }
	}
    }
    /* check dangling attributes */
    if (tnl_vb.num != tnl_vb.max) {
	for (j = 1; j < TNL_BITS; j++) {
	    int attr = 1 << j;
	    if (tnl_vb.flags[tnl_vb.num] & attr) {
		last_attr[j] = &tnl_vb.attr[j].ptr[tnl_vb.num];
	    }
	}
    }
    /* copy default values back and fix strides */
    for (j = 1; j < TNL_BITS; j++) {
	int attr = 1 << j;
	if (tnl_general_flags & attr) {
	    tnl_init_attr[j][0] = last_attr[j][0][0];
	    tnl_init_attr[j][1] = last_attr[j][0][1];
	    tnl_init_attr[j][2] = last_attr[j][0][2];
	    tnl_init_attr[j][3] = last_attr[j][0][3];
	    tnl_vb.attr[j].data = tnl_vb.attr[j].ptr;
	    tnl_vb.attr[j].stride = 1;
	} else {
	    tnl_vb.attr[j].data = &tnl_init_attr[j];
	    tnl_vb.attr[j].stride = 0;
	}
    }
}


/**
 * Trim the last primitive.
 *
 * We want to avoid sending degenerate primitives to the driver.
 */
static void
_tnl_trim_current (void)
{
    int trim = 0;
    int prim_count = tnl_prim[tnl_prim_num - 1].count;

    switch (tnl_prim[tnl_prim_num - 1].name & 0xffff) {
	case GL_POINTS:
	    break;
	case GL_LINES:
	    trim = (prim_count & 1);
	    break;
	case GL_LINE_LOOP:
	    if (prim_count < 2) {
		trim = prim_count;
	    }
	    break;
	case GL_LINE_STRIP:
	    if (prim_count < 2) {
		trim = prim_count;
	    }
	    break;
	case GL_TRIANGLES:
	    trim = (prim_count % 3);
	    break;
	case GL_TRIANGLE_STRIP:
	    if (prim_count < 3) {
		trim = prim_count;
	    }
	    break;
	case GL_TRIANGLE_FAN:
	case GL_POLYGON:
	    if (prim_count < 3) {
		trim = prim_count;
	    }
	    break;
	case GL_QUADS:
	    trim = (prim_count & 3);
	    break;
	case GL_QUAD_STRIP:
	    if (prim_count < 4) {
		trim = prim_count;
	    } else {
		trim = (prim_count & 1);
	    }
	    break;
    }

    LOG(("%s: trimming %d\n", __FUNCTION__, trim));
    tnl_prim_size = tnl_prim[tnl_prim_num - 1].count;
    tnl_prim[tnl_prim_num - 1].count -= trim;
    tnl_vb.len = tnl_vb.num - trim;
}


/**
 * Copy vertex data from one location to another.
 */
static void
_tnl_copy_vertex_data (int dst, int src)
{
    int j;
    LOG(("%s: copying from %d to %d\n", __FUNCTION__, src, dst));
    tnl_vb.flags[dst] = tnl_general_flags;
    if (dst != src) {
	for (j = 0; j < TNL_BITS; j++) {
	    int attr = 1 << j;
	    if ((tnl_general_flags | TNL_VERTEX_BIT) & attr) {
		tnl_vb.attr[j].ptr[dst][0] = tnl_vb.attr[j].ptr[src][0];
		tnl_vb.attr[j].ptr[dst][1] = tnl_vb.attr[j].ptr[src][1];
		tnl_vb.attr[j].ptr[dst][2] = tnl_vb.attr[j].ptr[src][2];
		tnl_vb.attr[j].ptr[dst][3] = tnl_vb.attr[j].ptr[src][3];
	    }
	}
    }
}


/**
 * Copy the required vertices to continue last primitive.
 *
 * At this point, we know the last primitive was not closed.
 * The vertex buffer state is consistent, `tnl_vb.num' being
 * the last emitted vertex.
 */
static int
_tnl_copy_to_current (void)
{
    int i, copy = 0;
    int prim_start = tnl_prim[tnl_prim_num - 1].start;

    switch (tnl_prim[tnl_prim_num - 1].name & 0xffff) {
	case GL_POINTS:
	    break;
	case GL_LINES:
	    copy = (tnl_prim_size & 1);
	    for (i = 0; i < copy; i++) {
		_tnl_copy_vertex_data(i, tnl_vb.num - copy + i);
	    }
	    break;
	case GL_LINE_LOOP:
	    if (tnl_prim_size == 1) {
		copy = 1;
		_tnl_copy_vertex_data(0, prim_start);
	    } else if (tnl_prim_size >= 2) {
		copy = 2;
		_tnl_copy_vertex_data(0, prim_start);
		_tnl_copy_vertex_data(1, tnl_vb.num - 1);
	    }
	    break;
	case GL_LINE_STRIP:
	    if (tnl_prim_size) {
		copy = 1;
		for (i = 0; i < copy; i++) {
		    _tnl_copy_vertex_data(i, tnl_vb.num - copy + i);
		}
	    }
	    break;
	case GL_TRIANGLES:
	    copy = (tnl_prim_size % 3);
	    for (i = 0; i < copy; i++) {
		_tnl_copy_vertex_data(i, tnl_vb.num - copy + i);
	    }
	    break;
	case GL_TRIANGLE_STRIP:
	    copy = MIN2(2, tnl_prim_size);
	    for (i = 0; i < copy; i++) {
		_tnl_copy_vertex_data(i, tnl_vb.num - copy + i);
	    }
	    break;
	case GL_TRIANGLE_FAN:
	case GL_POLYGON:
	    if (tnl_prim_size == 1) {
		copy = 1;
		_tnl_copy_vertex_data(0, prim_start);
	    } else if (tnl_prim_size >= 2) {
		copy = 2;
		_tnl_copy_vertex_data(0, prim_start);
		_tnl_copy_vertex_data(1, tnl_vb.num - 1);
	    }
	    break;
	case GL_QUADS:
	    copy = (tnl_prim_size & 3);
	    for (i = 0; i < copy; i++) {
		_tnl_copy_vertex_data(i, tnl_vb.num - copy + i);
	    }
	    break;
	case GL_QUAD_STRIP:
	    if (tnl_prim_size < 2) {
		copy = tnl_prim_size;
	    } else {
		copy = 2 + (tnl_prim_size & 1);
	    }
	    for (i = 0; i < copy; i++) {
		_tnl_copy_vertex_data(i, tnl_vb.num - copy + i);
	    }
	    break;
    }

    LOG(("%s: copied %d\n", __FUNCTION__, copy));
    if ((tnl_prim_size > 1) && (tnl_prim_size & 1)) {
	tnl_prim[tnl_prim_num - 1].name ^= TNL_PRIM_PARITY;
    }
    return copy;
}


void
tnl_fini (void)
{
    int i;

    if (tnl_prim != NULL) {
	free(tnl_prim);
	tnl_prim = NULL;
    }

    if (tnl_vb.clipmask != NULL) {
	free(tnl_vb.clipmask);
	tnl_vb.clipmask = NULL;
    }

    if (tnl_vb.c1_back != NULL) {
	free(tnl_vb.c1_back);
	tnl_vb.c1_back = NULL;
    }

    if (tnl_vb.c0_back != NULL) {
	free(tnl_vb.c0_back);
	tnl_vb.c0_back = NULL;
    }

    if (tnl_vb.c1_front != NULL) {
	free(tnl_vb.c1_front);
	tnl_vb.c1_front = NULL;
    }

    if (tnl_vb.c0_front != NULL) {
	free_a(tnl_vb.c0_front);
	tnl_vb.c0_front = NULL;
    }

    if (tnl_vb.veyn != NULL) {
	free_a(tnl_vb.veyn);
	tnl_vb.veyn = NULL;
    }

    if (tnl_vb.neye != NULL) {
	free_a(tnl_vb.neye);
	tnl_vb.neye = NULL;
    }

    if (tnl_vb.veye != NULL) {
	free_a(tnl_vb.veye);
	tnl_vb.veye = NULL;
    }

    if (tnl_vb.fogcoord != NULL) {
	free(tnl_vb.fogcoord);
	tnl_vb.fogcoord = NULL;
    }

    if (tnl_vb.refl != NULL) {
	free_a(tnl_vb.refl);
	tnl_vb.refl = NULL;
    }

    if (tnl_vb.ndc != NULL) {
	free_a(tnl_vb.ndc);
	tnl_vb.ndc = NULL;
    }

    if (tnl_vb.clip != NULL) {
	free_a(tnl_vb.clip);
	tnl_vb.clip = NULL;
    }

    for (i = 0; i < TNL_MAX_TEXCOORD; i++) {
	if (tnl_vb.texcoord[i] != NULL) {
	    free_a(tnl_vb.texcoord[i]);
	    tnl_vb.texcoord[i] = NULL;
	}
    }

    for (i = 0; i < TNL_MAX_TEXCOORD; i++) {
	if (tnl_vb.texgen[i] != NULL) {
	    free_a(tnl_vb.texgen[i]);
	    tnl_vb.texgen[i] = NULL;
	}
    }

    for (i = 0; i < TNL_BITS; i++) {
	if (tnl_vb.attr[i].ptr != NULL) {
	    free_a(tnl_vb.attr[i].ptr);
	    tnl_vb.attr[i].ptr = NULL;
	}
    }

    if (tnl_vb.flags != NULL) {
	free(tnl_vb.flags);
	tnl_vb.flags = NULL;
    }
}


int
tnl_init (void)
{
    int i;

    tnl_vb.max = atoi(cfg_get("tnl.vb.size", "64")) + TNL_CLIPPED_VERTS;

    tnl_vb.flags = malloc(tnl_vb.max * sizeof(GLbitfield));
    if (tnl_vb.flags == NULL) {
	tnl_fini();
	return -1;
    }

    for (i = 0; i < TNL_BITS; i++) {
	tnl_vb.attr[i].ptr = malloc_a(tnl_vb.max * sizeof(GLfloat4), 4);
	if (tnl_vb.attr[i].ptr == NULL) {
	    tnl_fini();
	    return -1;
	}
	tnl_vb.attr[i].data = tnl_vb.attr[i].ptr;
    }

    for (i = 0; i < TNL_MAX_TEXCOORD; i++) {
	tnl_vb.texgen[i] = malloc_a(tnl_vb.max * sizeof(GLfloat4), 4);
	if (tnl_vb.texgen[i] == NULL) {
	    tnl_fini();
	    return -1;
	}
    }

    for (i = 0; i < TNL_MAX_TEXCOORD; i++) {
	tnl_vb.texcoord[i] = malloc_a(tnl_vb.max * sizeof(GLfloat4), 4);
	if (tnl_vb.texcoord[i] == NULL) {
	    tnl_fini();
	    return -1;
	}
    }

    tnl_vb.clip = malloc_a(tnl_vb.max * sizeof(GLfloat4), 4);
    if (tnl_vb.clip == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_vb.ndc = malloc_a(tnl_vb.max * sizeof(GLfloat4), 4);
    if (tnl_vb.ndc == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_vb.veye = malloc_a(tnl_vb.max * sizeof(GLfloat4), 4);
    if (tnl_vb.veye == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_vb.neye = malloc_a(tnl_vb.max * sizeof(GLfloat4), 4);
    if (tnl_vb.neye == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_vb.veyn = malloc_a(tnl_vb.max * sizeof(GLfloat4), 4);
    if (tnl_vb.veyn == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_vb.refl = malloc_a(tnl_vb.max * sizeof(GLfloat4), 4);
    if (tnl_vb.refl == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_vb.fogcoord = malloc(tnl_vb.max * sizeof(GLfloat4));
    if (tnl_vb.fogcoord == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_vb.c0_front = malloc_a(tnl_vb.max * sizeof(GLfloat4), 4);
    if (tnl_vb.c0_front == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_vb.c1_front = malloc(tnl_vb.max * sizeof(GLfloat4));
    if (tnl_vb.c1_front == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_vb.c0_back = malloc(tnl_vb.max * sizeof(GLfloat4));
    if (tnl_vb.c0_back == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_vb.c1_back = malloc(tnl_vb.max * sizeof(GLfloat4));
    if (tnl_vb.c1_back == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_vb.clipmask = malloc(tnl_vb.max * sizeof(GLubyte));
    if (tnl_vb.clipmask == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_prim = malloc(TNL_PRIM_MAX * sizeof(TNL_PRIMITIVE));
    if (tnl_prim == NULL) {
	tnl_fini();
	return -1;
    }

    tnl_vb.max -= TNL_CLIPPED_VERTS;
    tnl_vb.num = 0;
    tnl_general_flags = 0;
    memset(tnl_vb.flags, 0, tnl_vb.max * sizeof(GLbitfield));
    tnl_prim_num = 0;
    tnl_primitive = TNL_OUTSIDE_BE;

    tnl_client.flags = 0;
    tnl_client.texunit = 0;
    memset(tnl_client.arrays, 0, sizeof(tnl_client.arrays));
    list_create(&tnl_client.list1);
    list_create(&tnl_client.list2);

    tnl_extra_flags = 0;

    tnl_light_state = 0;
    tnl_texgen_state = 0;
    tnl_setup_init();

    return 0;
}


void GLAPIENTRY
tnl_flush (void)
{
    if (tnl_prim_num) {
	int old_num = tnl_vb.num;
	if (tnl_vb.num != tnl_vb.max) old_num++;

	if (!(tnl_prim[tnl_prim_num - 1].name & TNL_PRIM_END)) {
	    /* if wasn't fixed by End(), fix it now */
	    tnl_prim[tnl_prim_num - 1].count += tnl_vb.num;
	}

	tnl_extra_flags = tnl_general_flags & ~((1 << TNL_BITS) - 2);

	_tnl_trim_current();
	_tnl_playback();
	tnl_run_pipeline();

	if (!(tnl_prim[tnl_prim_num - 1].name & TNL_PRIM_END)) {
	    /* copy leftovers from previous buffer */
	    tnl_vb.num = _tnl_copy_to_current();

	    tnl_prim[0].start = 0;
	    tnl_prim[0].count = 0;
	    tnl_prim[0].name = tnl_prim[tnl_prim_num - 1].name & ~TNL_PRIM_BEGIN;

	    tnl_prim_num = 1;
	} else {
	    /* brand new buffer */
	    tnl_vb.num = 0;
	    tnl_general_flags = 0;
	    tnl_prim_num = 0;
	}

	memset(tnl_vb.flags + tnl_vb.num, 0, (old_num - tnl_vb.num) * sizeof(GLbitfield));
    }
}
