#ifndef X86_H_included
#define X86_H_included

#ifdef FAST_MATH
#define DEFAULT_X86_FPU    0x037f /* exc masked, extdbl prec, nearest */
#define FAST_X86_FPU	   0x003f /* exc masked, single prec, nearest */

#ifdef __GNUC__
#define START_FAST_MATH(x) 				\
    do {						\
	static GLuint mask = FAST_X86_FPU;		\
	__asm__("fnstcw %0" : "=m" (*&(x)));		\
	__asm__("fldcw %0" : : "m" (mask));		\
    } while (0)
#define END_FAST_MATH(x) 				\
    do {						\
	__asm__("fnclex ; fldcw %0" : : "m" (*&(x)));	\
    } while (0)
#else  /* !__GNUC__ */
#define START_FAST_MATH(x) (void)(x)
#define END_FAST_MATH(x)   (void)(x)
#warning FAST_MATH disabled for this compiler
#endif /* !__GNUC__ */
#else  /* !FAST_MATH */
#define START_FAST_MATH(x) (void)(x)
#define END_FAST_MATH(x)   (void)(x)
#endif /* !FAST_MATH */

extern int x86_cpu_bits;
extern char x86_cpu_name[];

extern int x86_enable_sse;
extern int x86_enable_3dnow;

int x86_init (void);

#endif
