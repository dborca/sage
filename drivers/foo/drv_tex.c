#include "GL/gl.h"

#include "glinternal.h"
#include "main/context.h"
#include "tnl/tnl.h"
#include "driver.h"
#include "drv.h"


void
drv_TexImage2D (TEX_OBJ *texObj, GLint level, GLint internalFormat,
		GLsizei width, GLsizei height, GLenum format, GLenum type,
		const GLvoid *pixels, const PACKING *unpack)
{
}


void
drv_TexSubImage2D (TEX_OBJ *texObj, GLint level, GLint xoffset, GLint yoffset,
		   GLsizei width, GLsizei height, GLenum format, GLenum type,
		   const GLvoid *pixels, const PACKING *unpack)
{
}


void
drv_CompressedTexImage2D (TEX_OBJ *texObj, GLint level, GLenum internalformat,
			  GLsizei width, GLsizei height, GLsizei imageSize,
			  const GLvoid *data)
{
}


void
drv_CompressedTexSubImage2D (TEX_OBJ *texObj, GLint level,
			     GLint xoffset, GLint yoffset,
			     GLsizei width, GLsizei height,
			     GLenum format, GLsizei imageSize,
			     const GLvoid *data)
{
}


void
drv_GetTexImage (TEX_OBJ *texObj, GLint level, GLenum format, GLenum type, GLvoid *pixels)
{
}


void
drv_GetCompressedTexImage (TEX_OBJ *texObj, GLint lod, GLvoid *img)
{
}


void
drv_DeleteTexture (TEX_OBJ *texObj)
{
}
