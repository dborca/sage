#include "cpu.h"

/* Testing code:
 * TEST_SSE       = xorps xmm0, xmm0
 * TEST_SSE2      = xorpd xmm0, xmm0
 * TEST_3DNOW     = femms
 * TEST_MMX       = emms
 * TEST_3DNOWPLUS = femms | pswapd mm0, mm0 | femms
 * TEST_MMXPLUS   = emms | pminsw mm0, mm0 | emms
 */
#ifdef __GNUC__

#define USE_SIGNALS 1

#define TEST_CPUID(f)    __asm __volatile ("pushl %%ebx; cpuid; popl %%ebx"::"a"(f):"%ecx", "%edx")
#define TEST_SSE()       __asm __volatile (".byte 0x0f, 0x57, 0xc0")
#define TEST_SSE2()      __asm __volatile (".byte 0x66, 0x0f, 0x57, 0xc0")
#define TEST_3DNOW()     __asm __volatile (".byte 0x0f, 0x0e")
#define TEST_MMX()       __asm __volatile (".byte 0x0f, 0x77")
#define TEST_3DNOWPLUS() __asm __volatile (".byte 0x0f, 0x0e, 0x0f, 0x0f, 0xc0, 0xbb, 0x0f, 0x0e")
#define TEST_MMXPLUS()   __asm __volatile (".byte 0x0f, 0x77, 0x0f, 0xea, 0xc0, 0x0f, 0x77")

#else  /* !__GNUC__ */

#define USE_SIGNALS 0

#define TEST_CPUID(f)    __asm { _asm mov eax, f _asm cpuid }
#define TEST_SSE()       __asm { _asm _emit 0x0f _asm _emit 0x57 _asm _emit 0xc0 }
#define TEST_SSE2()      __asm { _asm _emit 0x66 _asm _emit 0x0f _asm _emit 0x57 _asm _emit 0xc0 }
#define TEST_3DNOW()     __asm { _asm _emit 0x0f _asm _emit 0x0e }
#define TEST_MMX()       __asm { _asm _emit 0x0f _asm _emit 0x77 }
#define TEST_3DNOWPLUS() __asm { _asm _emit 0x0f _asm _emit 0x0e _asm _emit 0x0f _asm _emit 0x0f _asm _emit 0xc0 _asm _emit 0xbb _asm _emit 0x0f _asm _emit 0x0e }
#define TEST_MMXPLUS()   __asm { _asm _emit 0x0f _asm _emit 0x77 _asm _emit 0x0f _asm _emit 0xea _asm _emit 0xc0 _asm _emit 0x0f _asm _emit 0x77 }

#endif /* !__GNUC__ */


#if USE_SIGNALS

#include <setjmp.h>
#include <signal.h>

#define __try if (!setjmp(j))
#define __except(x) else

#define EXC_INIT() ((old_sigill = signal(SIGILL, handler)) != SIG_ERR)
#define EXC_FINI() signal(SIGILL, old_sigill)

static jmp_buf j;
static void (*old_sigill) (int);


static void
handler (int signal)
{
    longjmp(j, signal + 1); /* so we can tell... also ensure we don't pass 0 */
}

#else  /* !USE_SIGNALS */

#include <excpt.h>

#define EXC_INIT() !0
#define EXC_FINI()

#endif /* !USE_SIGNALS */


static int
check_feature (int feature)
{
    __try {
	/* we have signals and jump buffer set */
	switch (feature) {
	    case _CPU_HAS_CPUID:
		TEST_CPUID(0);
		break;
	    case _CPU_FEATURE_SSE:
		TEST_SSE();
		break;
	    case _CPU_FEATURE_SSE2:
		TEST_SSE2();
		break;
	    case _CPU_FEATURE_3DNOW:
		TEST_3DNOW();
		break;
	    case _CPU_FEATURE_MMX:
		TEST_MMX();
		break;
	    case _CPU_FEATURE_3DNOWPLUS:
		TEST_3DNOWPLUS();
		break;
	    case _CPU_FEATURE_MMXPLUS:
		TEST_MMXPLUS();
		break;
	    default:
		return 0;
	}
	return feature;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
	/* we got here only when `longjmp'ed by signal handlers */
	return 0;
    }
}


static int
has_feature (int feature)
{
    int rv;

    if (!EXC_INIT()) {
	return 0;
    }

    rv = check_feature(feature);

    EXC_FINI();
    return rv;
}


int
cpuhard (void)
{
    int rv = has_feature(_CPU_HAS_CPUID);
    if (rv) {
	rv |= has_feature(_CPU_FEATURE_MMX);
	rv |= has_feature(_CPU_FEATURE_SSE);
	rv |= has_feature(_CPU_FEATURE_SSE2);
	rv |= has_feature(_CPU_FEATURE_3DNOW);
	rv |= has_feature(_CPU_FEATURE_3DNOWPLUS);
	rv |= has_feature(_CPU_FEATURE_MMXPLUS);
    }
    return rv;
}
