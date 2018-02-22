#ifndef MACROS_H_included
#define MACROS_H_included


#ifdef __DJGPP__
#define SQRT(x)   sqrt(x)
#define COS(x)    cos(x)
#define SIN(x)    sin(x)
#define FABS(x)   fabs(x)
#define EXP(x)    exp(x)
#define POW(x, y) pow(x, y)
#else
#define SQRT(x)   sqrtf(x)
#define COS(x)    cosf(x)
#define SIN(x)    sinf(x)
#define FABS(x)   fabsf(x)
#define EXP(x)    expf(x)
#define POW(x, y) powf(x, y)
#endif


#define LINTERP(T, OUT, IN) ((OUT) + (T) * ((IN) - (OUT)))


#define INTERP_UB( t, dstub, outub, inub )  \
do {                                        \
   GLfloat inf = inub;                      \
   GLfloat outf = outub;                    \
   GLfloat dstf = LINTERP( t, outf, inf );  \
   dstub = dstf;                            \
} while (0)


#define INTERP_F( t, dstf, outf, inf )      \
   dstf = LINTERP( t, outf, inf )


#define INTERP_4F( t, dst, out, in )            \
do {                                            \
   dst[0] = LINTERP( (t), (out)[0], (in)[0] );  \
   dst[1] = LINTERP( (t), (out)[1], (in)[1] );  \
   dst[2] = LINTERP( (t), (out)[2], (in)[2] );  \
   dst[3] = LINTERP( (t), (out)[3], (in)[3] );  \
} while (0)


#define MAX2(x, y) (((x) < (y)) ? (y) : (x))
#define MIN2(x, y) (((x) > (y)) ? (y) : (x))


typedef union {
    GLint i;
    GLfloat f;
} fi_type;


static __inline int
IS_NEGATIVE (GLfloat f)
{
    fi_type fi;
    fi.f = f;
    return (fi.i < 0);
}


static __inline int
DIFFERENT_SIGNS (GLfloat f1, GLfloat f2)
{
    fi_type fi1, fi2;
    fi1.f = f1;
    fi2.f = f2;
    return ((fi1.i < 0) ^ (fi2.i < 0));
}


static __inline GLfloat
CLAMPF (GLfloat f)
{
#define IEEE_0996 0x3f7f0000	/* 0.996 or so */
    fi_type fi;
    fi.f = f;
    if (fi.i < 0) {
	return 0.0F;
    } else if (fi.i > IEEE_0996) {
	return 1.0F;
    } else {
	return f;
    }
}


extern GLfloat ub_to_float[];


#define B_TO_FLOAT(b)  ((2.0F * (b) + 1.0F) * (1.0F/255.0F))
#define UB_TO_FLOAT(u) ub_to_float[(GLuint)u]/*((GLfloat)(u) / 255.0F)*/
#define S_TO_FLOAT(s)  ((2.0F * (s) + 1.0F) * (1.0F/65535.0F))
#define US_TO_FLOAT(u) ((GLfloat)(u) / 65535.0F)
#define I_TO_FLOAT(i)  ((2.0F * (i) + 1.0F) * (1.0F/4294967295.0F))
#define UI_TO_FLOAT(u) ((GLfloat)(u) / 4294967295.0F)


#define when(x) if (x)


#define DOT3(x, y) ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2])
#define DOT4(x, y) ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2] + (x)[3] * (y)[3])
#define SQLEN3(x) DOT3(x, x)

#define NORM3(u, S, v)	\
    do {			\
	GLfloat dp = SQLEN3(v);	\
	dp = S 1.0F / SQRT(dp);	\
	(u)[0] = (v)[0] * dp;	\
	(u)[1] = (v)[1] * dp;	\
	(u)[2] = (v)[2] * dp;	\
    } while (0)


static __inline GLushort
SWAP_SHORT (GLushort s)
{
    return (s >> 8) | (s << 8);
}


#define PTR_ADD_STRIDE(type, ptr, stride) ptr = (type *)((GLubyte *)ptr + stride)


#endif
