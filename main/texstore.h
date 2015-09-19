#ifndef TEXSTORE_H_included
#define TEXSTORE_H_included

GLint
image_stride (const PACKING *packing, GLint width, GLint bytesPerPixel);

const GLvoid *
image_address (const PACKING *packing,
	       const GLubyte *src, GLsizei width, GLsizei height,
	       GLint bytesPerPixel,
	       GLint img, GLint row, GLint column);


GLvoid *
cvt2d (const PACKING *unpack, const PACKING *pack,
       int xoffset, int yoffset,
       int dst_width, int dst_height,
       GLvoid *dst,
       GLenum dst_format,
       GLenum dst_type,
       int src_width, int src_height,
       const GLvoid *src,
       GLenum src_format,
       GLenum src_type);

void *
avm2d (void *dst, int dst_width, int dst_height, int dstStrideInPixels,
       GLenum dst_format, GLenum dst_type,
       const void *src, int src_width, int src_height, int srcStrideInPixels,
       GLenum src_format, GLenum src_type);

#endif
