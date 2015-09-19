#ifndef GLAPI_H_included
#define GLAPI_H_included

#define GLCALL(x) ctx_gl_table->x


#define ENTRY_NR(z, x, y, w, k)	\
    z GLAPIENTRY imm_##x y;
#define ENTRY_RV(z, x, y, w, k)	\
    z GLAPIENTRY imm_##x y;
#include "glapit.h"
#undef ENTRY_NR
#undef ENTRY_RV

#define ENTRY_NR(z, x, y, w, k)	\
    z GLAPIENTRY sav_##x y;
#define ENTRY_RV(z, x, y, w, k)	\
    z GLAPIENTRY sav_##x y;
#include "glapit.h"
#undef ENTRY_NR
#undef ENTRY_RV


typedef struct {
#define ENTRY_NR(z, x, y, w, k)	\
    z (GLAPIENTRY *x) y;
#define ENTRY_RV(z, x, y, w, k)	\
    z (GLAPIENTRY *x) y;
#include "glapit.h"
#undef ENTRY_NR
#undef ENTRY_RV
} GLAPITABLE;


extern GLAPITABLE *ctx_gl_table;
extern GLAPITABLE ctx_imm_table;


void gl_switch_nop (void);
void gl_switch_imm (void);
void gl_switch_sav (void);
void gl_init (void);
void gl_fini (void);

#endif
