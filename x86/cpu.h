#ifndef CPU_H_included
#define CPU_H_included

#define _CPU_HAS_CPUID		0x8000
#define _CPU_FEATURE_MMX	0x0001
#define _CPU_FEATURE_SSE	0x0002
#define _CPU_FEATURE_SSE2	0x0004
#define _CPU_FEATURE_3DNOW	0x0008
#define _CPU_FEATURE_3DNOWPLUS	0x0010
#define _CPU_FEATURE_MMXPLUS	0x0020

int cpuhard (void);
int cpusoft (char *name);

#endif
