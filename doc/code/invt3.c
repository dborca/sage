#include <stdio.h>
#include <string.h>


#define GL_FALSE 0
#define GL_TRUE !GL_FALSE

typedef float GLfloat;
typedef int GLboolean;


static GLboolean
matrix_invt3 (GLfloat *inv, const GLfloat *src)
{
    GLfloat x00, x01, x02;
    GLfloat x10, x11, x12;
    GLfloat x20, x21, x22;
    GLfloat rcp;

    GLfloat z00, z01, z02;
    GLfloat z10, z11, z12;
    GLfloat z20, z21, z22;

    /* read 3x3 matrix into registers */
    x00 = src[0];
    x01 = src[1];
    x02 = src[2];
    x10 = src[4];
    x11 = src[5];
    x12 = src[6];
    x20 = src[8];
    x21 = src[9];
    x22 = src[10];

    /* compute first three 2x2 cofactors */
    z20 = x01 * x12 - x11 * x02;
    z10 = x21 * x02 - x01 * x22;
    z00 = x11 * x22 - x12 * x21;

    /* compute 3x3 determinant & its reciprocal */
    rcp = x20 * z20 + x10 * z10 + x00 * z00;
    if (rcp == 0.0F)
	return GL_FALSE;
    rcp = 1.0F / rcp;

    /* compute other six 2x2 cofactors */
    z01 = x20 * x12 - x10 * x22;
    z02 = x10 * x21 - x20 * x11;
    z11 = x00 * x22 - x20 * x02;
    z12 = x20 * x01 - x00 * x21;
    z21 = x10 * x02 - x00 * x12;
    z22 = x00 * x11 - x10 * x01;

    /* multiply all cofactors by reciprocal */
    inv[0]  = z00 * rcp;
    inv[1]  = z01 * rcp;
    inv[2]  = z02 * rcp;
    inv[4]  = z10 * rcp;
    inv[5]  = z11 * rcp;
    inv[6]  = z12 * rcp;
    inv[8]  = z20 * rcp;
    inv[9]  = z21 * rcp;
    inv[10] = z22 * rcp;

    return GL_TRUE;
}


void
matrix_mul (GLfloat *dst, const GLfloat *a, const GLfloat *b)
{
    /*
     *	a0  a4  a8  a12		b0  b4  b8  b12
     *	a1  a5  a9  a13	   *	b1  b5  b9  b13
     *	a2  a6  a10 a14		b2  b6  b10 b14
     *	a3  a7  a11 a15		b3  b7  b11 b15
     */
    dst[0]  = a[0] * b[0]  + a[4] * b[1]  + a[8]  * b[2]  + a[12] * b[3];
    dst[1]  = a[1] * b[0]  + a[5] * b[1]  + a[9]  * b[2]  + a[13] * b[3];
    dst[2]  = a[2] * b[0]  + a[6] * b[1]  + a[10] * b[2]  + a[14] * b[3];
    dst[3]  = a[3] * b[0]  + a[7] * b[1]  + a[11] * b[2]  + a[15] * b[3];

    dst[4]  = a[0] * b[4]  + a[4] * b[5]  + a[8]  * b[6]  + a[12] * b[7];
    dst[5]  = a[1] * b[4]  + a[5] * b[5]  + a[9]  * b[6]  + a[13] * b[7];
    dst[6]  = a[2] * b[4]  + a[6] * b[5]  + a[10] * b[6]  + a[14] * b[7];
    dst[7]  = a[3] * b[4]  + a[7] * b[5]  + a[11] * b[6]  + a[15] * b[7];

    dst[8]  = a[0] * b[8]  + a[4] * b[9]  + a[8]  * b[10] + a[12] * b[11];
    dst[9]  = a[1] * b[8]  + a[5] * b[9]  + a[9]  * b[10] + a[13] * b[11];
    dst[10] = a[2] * b[8]  + a[6] * b[9]  + a[10] * b[10] + a[14] * b[11];
    dst[11] = a[3] * b[8]  + a[7] * b[9]  + a[11] * b[10] + a[15] * b[11];

    dst[12] = a[0] * b[12] + a[4] * b[13] + a[8]  * b[14] + a[12] * b[15];
    dst[13] = a[1] * b[12] + a[5] * b[13] + a[9]  * b[14] + a[13] * b[15];
    dst[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
    dst[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
}


void
matrix_transpose (GLfloat *transpose, const GLfloat *src)
{
    transpose[0]  = src[0];
    transpose[1]  = src[4];
    transpose[2]  = src[8];
    transpose[3]  = src[12];
    transpose[4]  = src[1];
    transpose[5]  = src[5];
    transpose[6]  = src[9];
    transpose[7]  = src[13];
    transpose[8]  = src[2];
    transpose[9]  = src[6];
    transpose[10] = src[10];
    transpose[11] = src[14];
    transpose[12] = src[3];
    transpose[13] = src[7];
    transpose[14] = src[11];
    transpose[15] = src[15];
}


static void
matrix_print (const GLfloat *m)
{
    printf("%6.2f, %6.2f, %6.2f, %6.2f\n", m[0], m[4], m[8], m[12]);
    printf("%6.2f, %6.2f, %6.2f, %6.2f\n", m[1], m[5], m[9], m[13]);
    printf("%6.2f, %6.2f, %6.2f, %6.2f\n", m[2], m[6], m[10], m[14]);
    printf("%6.2f, %6.2f, %6.2f, %6.2f\n", m[3], m[7], m[11], m[15]);
    printf("\n");
}


int
main (void)
{
    GLfloat ini[] = {
	1, 2, 3, 0,
	4, 1, 6, 0,
	7, 8, 9, 0,
	0, 0, 0, 0
    };
    GLfloat inv[16];
    GLfloat mat[16];
    GLfloat tmp[16];
    memset(inv, 0, sizeof(inv));
    memset(mat, 0, sizeof(mat));
    memset(tmp, 0, sizeof(tmp));
    if (matrix_invt3(inv, ini)) {
	matrix_print(ini);
	matrix_transpose(tmp, inv);
	matrix_print(tmp);
	matrix_mul(mat, ini, tmp);
	matrix_print(mat);
	matrix_mul(mat, tmp, ini);
	matrix_print(mat);
    }
    return 0;
}
