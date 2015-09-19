#ifndef LOG_H_included
#define LOG_H_included

#undef LOG

#ifndef LOGGING
#define LOGGING 0
#endif

#if LOGGING
#include <stdio.h>
#define LOG(x) printf x
#else
#define LOG(x)
#endif

#endif
