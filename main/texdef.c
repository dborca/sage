#include "GL/gl.h"

#include "glinternal.h"
#include "context.h"
#include "texdef.h"


const TEXDEF texdef_rgb_565[1] = {{
    GL_RGB,
    GL_UNSIGNED_SHORT_5_6_5,
    2,
    GL_FALSE,
    5, 6, 5, 0
}};

const TEXDEF texdef_bgra_4444_rev[1] = {{
    GL_BGRA,
    GL_UNSIGNED_SHORT_4_4_4_4_REV,
    2,
    GL_FALSE,
    4, 4, 4, 4
}};

const TEXDEF texdef_l_ubyte[1] = {{
    GL_LUMINANCE,
    GL_UNSIGNED_BYTE,
    1,
    GL_FALSE,
    8, 8, 8, 0
}};

const TEXDEF texdef_a_ubyte[1] = {{
    GL_ALPHA,
    GL_UNSIGNED_BYTE,
    1,
    GL_FALSE,
    0, 0, 0, 8
}};

const TEXDEF texdef_i_ubyte[1] = {{
    GL_INTENSITY,
    GL_UNSIGNED_BYTE,
    1,
    GL_FALSE,
    8, 8, 8, 8
}};

const TEXDEF texdef_la_ubyte[1] = {{
    GL_LUMINANCE_ALPHA,
    GL_UNSIGNED_BYTE,
    2,
    GL_FALSE,
    8, 8, 8, 8
}};

const TEXDEF texdef_bgra_1555_rev[1] = {{
    GL_BGRA,
    GL_UNSIGNED_SHORT_1_5_5_5_REV,
    2,
    GL_FALSE,
    5, 5, 5, 1
}};

const TEXDEF texdef_bgra_8888_rev[1] = {{
    GL_BGRA,
    GL_UNSIGNED_INT_8_8_8_8_REV,
    4,
    GL_FALSE,
    8, 8, 8, 8
}};

const TEXDEF texdef_bgr1_8888_rev[1] = {{
    GL_BGR,
    GL_UNSIGNED_INT_8_8_8_8_REV,
    4,
    GL_FALSE,
    8, 8, 8, 0
}};

const TEXDEF texdef_rgb_fxt1[1] = {{
    GL_COMPRESSED_RGB_FXT1_3DFX,
    GL_UNSIGNED_BYTE,
    1,
    GL_TRUE,
    4, 4, 4, 0
}};

const TEXDEF texdef_rgba_fxt1[1] = {{
    GL_COMPRESSED_RGBA_FXT1_3DFX,
    GL_UNSIGNED_BYTE,
    1,
    GL_TRUE,
    4, 4, 4, 4
}};

const TEXDEF texdef_rgb_dxt1[1] = {{
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
    GL_UNSIGNED_BYTE,
    1,
    GL_TRUE,
    4, 4, 4, 0
}};

const TEXDEF texdef_rgba_dxt1[1] = {{
    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    GL_UNSIGNED_BYTE,
    1,
    GL_TRUE,
    4, 4, 4, 4
}};

const TEXDEF texdef_rgba_dxt3[1] = {{
    GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    GL_UNSIGNED_BYTE,
    1,
    GL_TRUE,
    4, 4, 4, 4
}};

const TEXDEF texdef_rgba_dxt5[1] = {{
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
    GL_UNSIGNED_BYTE,
    1,
    GL_TRUE,
    4, 4, 4, 4
}};
