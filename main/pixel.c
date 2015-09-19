#include "GL/gl.h"

#include "glinternal.h"
#include "context.h"


void GLAPIENTRY
imm_PixelStoref (GLenum pname, GLfloat param)
{
    switch (pname) {
	case GL_UNPACK_SWAP_BYTES:
	    ctx_unpack.swap_bytes = param;
	    break;
	case GL_UNPACK_LSB_FIRST:
	    ctx_unpack.lsb_first = param;
	    break;
	case GL_UNPACK_ROW_LENGTH:
	    ctx_unpack.row_length = param;
	    break;
	case GL_UNPACK_SKIP_ROWS:
	    ctx_unpack.skip_rows = param;
	    break;
	case GL_UNPACK_SKIP_PIXELS:
	    ctx_unpack.skip_pixels = param;
	    break;
	case GL_UNPACK_ALIGNMENT:
	    ctx_unpack.alignment = param;
	    break;
	case GL_PACK_SWAP_BYTES:
	    ctx_pack.swap_bytes = param;
	    break;
	case GL_PACK_LSB_FIRST:
	    ctx_pack.lsb_first = param;
	    break;
	case GL_PACK_ROW_LENGTH:
	    ctx_pack.row_length = param;
	    break;
	case GL_PACK_SKIP_ROWS:
	    ctx_pack.skip_rows = param;
	    break;
	case GL_PACK_SKIP_PIXELS:
	    ctx_pack.skip_pixels = param;
	    break;
	case GL_PACK_ALIGNMENT:
	    ctx_pack.alignment = param;
	    break;
    }
}


/* variations */


void GLAPIENTRY
imm_PixelStorei (GLenum pname, GLint param)
{
    switch (pname) {
	case GL_UNPACK_SWAP_BYTES:
	    ctx_unpack.swap_bytes = param;
	    break;
	case GL_UNPACK_LSB_FIRST:
	    ctx_unpack.lsb_first = param;
	    break;
	case GL_UNPACK_ROW_LENGTH:
	    ctx_unpack.row_length = param;
	    break;
	case GL_UNPACK_SKIP_ROWS:
	    ctx_unpack.skip_rows = param;
	    break;
	case GL_UNPACK_SKIP_PIXELS:
	    ctx_unpack.skip_pixels = param;
	    break;
	case GL_UNPACK_ALIGNMENT:
	    ctx_unpack.alignment = param;
	    break;
	case GL_PACK_SWAP_BYTES:
	    ctx_pack.swap_bytes = param;
	    break;
	case GL_PACK_LSB_FIRST:
	    ctx_pack.lsb_first = param;
	    break;
	case GL_PACK_ROW_LENGTH:
	    ctx_pack.row_length = param;
	    break;
	case GL_PACK_SKIP_ROWS:
	    ctx_pack.skip_rows = param;
	    break;
	case GL_PACK_SKIP_PIXELS:
	    ctx_pack.skip_pixels = param;
	    break;
	case GL_PACK_ALIGNMENT:
	    ctx_pack.alignment = param;
	    break;
    }
}
