#include <stdlib.h>
#include <string.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "context.h"


typedef struct {
    GLboolean enabled;
    const char *name;
} gl_ext;


typedef struct {
    gl_ext EXT_texture;			/* 1.1 */
    gl_ext EXT_subtexture;		/* 1.1 */
    gl_ext EXT_texture_object;		/* 1.1 */
    gl_ext EXT_vertex_array;		/* 1.1 */
    gl_ext EXT_rescale_normal;		/* 1.2 */
    gl_ext EXT_draw_range_elements;	/* 1.2 */
    gl_ext EXT_bgra;			/* 1.2 */
    gl_ext EXT_separate_specular_color;	/* 1.2 */
    gl_ext EXT_texture_env_combine;	/* 1.3 */
    gl_ext EXT_texture_env_add;		/* 1.3 */
    gl_ext ARB_multitexture;		/* 1.3 (1.2.1 as SGIS) */
    gl_ext EXT_fog_coord;		/* 1.4 */
    gl_ext ARB_texture_mirrored_repeat;	/* 1.4 */
    gl_ext EXT_blend_func_separate;	/* 1.4 */
    gl_ext EXT_secondary_color;		/* 1.4 */
    gl_ext EXT_texture_lod_bias;	/* 1.4 (1.2 as SGIS) */
    gl_ext EXT_texture_edge_clamp;	/*     (1.2 as SGIS) */
    gl_ext EXT_compiled_vertex_array;
    gl_ext EXT_polygon_offset;
    gl_ext NV_texgen_reflection;
    gl_ext EXT_clip_volume_hint;
    gl_ext ARB_texture_compression;
    gl_ext EXT_texture_compression_s3tc;
    gl_ext TDFX_texture_compression_FXT1;
    gl_ext S3_s3tc;
    gl_ext ARB_texture_env_combine;
    gl_ext ARB_texture_cube_map;
    gl_ext ARB_texture_env_dot3;
    gl_ext EXT_stencil_wrap;
    gl_ext NV_blend_square;
} gl_extensions;


gl_extensions ctx_extensions = {
    { GL_TRUE,  "GL_EXT_texture" },			/* XXX no proxies */
    { GL_TRUE,  "GL_EXT_subtexture" },			/* XXX no proxies */
    { GL_TRUE,  "GL_EXT_texture_object" },
    { GL_TRUE,  "GL_EXT_vertex_array" },
    { GL_TRUE,  "GL_EXT_rescale_normal" },
    { GL_TRUE,  "GL_EXT_draw_range_elements" },
    { GL_TRUE,  "GL_EXT_bgra" },
    { GL_FALSE, "GL_EXT_separate_specular_color" },
    { GL_FALSE, "GL_EXT_texture_env_combine" },		/* XXX not all modes */
    { GL_FALSE, "GL_EXT_texture_env_add" },
    { GL_FALSE, "GL_ARB_multitexture" },
    { GL_FALSE, "GL_EXT_fog_coord" },
    { GL_FALSE, "GL_ARB_texture_mirrored_repeat" },
    { GL_FALSE, "GL_EXT_blend_func_separate" },
    { GL_FALSE, "GL_EXT_secondary_color" },
    { GL_FALSE, "GL_EXT_texture_lod_bias" },
    { GL_FALSE, "GL_EXT_texture_edge_clamp" },
    { GL_FALSE, "GL_EXT_compiled_vertex_array" },
    { GL_TRUE,  "GL_EXT_polygon_offset" },
    { GL_TRUE,  "GL_NV_texgen_reflection" },
    { GL_TRUE,  "GL_EXT_clip_volume_hint" },

    { GL_FALSE, "GL_ARB_texture_compression" },
    { GL_FALSE, "GL_EXT_texture_compression_s3tc" },
    { GL_FALSE, "GL_3DFX_texture_compression_FXT1" },
    { GL_FALSE, "GL_S3_s3tc" },

    { GL_FALSE,  "GL_ARB_texture_env_combine" },
    { GL_FALSE,  "GL_ARB_texture_cube_map" },		/* YYY Doom3 */
    { GL_FALSE,  "GL_ARB_texture_env_dot3" },		/* YYY Doom3 */
    { GL_FALSE,  "GL_EXT_stencil_wrap" },
    { GL_FALSE,  "GL_NV_blend_square" }
};


char *
ext_create_string (void)
{
    char *ptr;
    int i, n, len;
    gl_ext *array;

    if (ctx_ext_string != NULL) {
	return ctx_ext_string;
    }

    len = 0;
    array = (gl_ext *)&ctx_extensions;
    n = sizeof(gl_extensions) / sizeof(gl_ext);

    for (i = 0; i < n; i++) {
	if (array[i].enabled) {
	    len += strlen(array[i].name) + 1;
	}
    }

    ptr = malloc(len * sizeof(char));
    if (ptr == NULL) {
	return NULL;
    }

    strcpy(ptr, array[0].name);
    for (i = 1; i < n; i++) {
	if (array[i].enabled) {
	    strcat(ptr, " ");
	    strcat(ptr, array[i].name);
	}
    }

    ctx_ext_string = ptr;
    return ptr;
}


void
ext_set (const char *name, int flag)
{
    int i, n;
    gl_ext *array;

    if (ctx_ext_string != NULL) {
	return;
    }

    array = (gl_ext *)&ctx_extensions;
    n = sizeof(gl_extensions) / sizeof(gl_ext);

    for (i = 0; i < n; i++) {
	if (!strcmp(array[i].name, name)) {
	    array[i].enabled = flag;
	}
    }
}
