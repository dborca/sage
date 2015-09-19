#include "GL/gl.h"

#include "glapi.h"


#define ENTRY_NR(z, x, y, w, k)	\
static z GLAPIENTRY nop_##x y	\
{				\
}
#define ENTRY_RV(z, x, y, w, k)	\
static z GLAPIENTRY nop_##x y	\
{				\
    return 0;			\
}
#include "glapit.h"
#undef ENTRY_NR
#undef ENTRY_RV


static GLAPITABLE ctx_nop_table = {
#define ENTRY_NR(z, x, y, w, k)	\
    nop_##x,
#define ENTRY_RV(z, x, y, w, k)	\
    nop_##x,
#include "glapit.h"
#undef ENTRY_NR
#undef ENTRY_RV
};

GLAPITABLE ctx_imm_table = {
#define ENTRY_NR(z, x, y, w, k)	\
    imm_##x,
#define ENTRY_RV(z, x, y, w, k)	\
    imm_##x,
#include "glapit.h"
#undef ENTRY_NR
#undef ENTRY_RV
};

static GLAPITABLE ctx_sav_table = {
#define ENTRY_NR(z, x, y, w, k)	\
    sav_##x,
#define ENTRY_RV(z, x, y, w, k)	\
    sav_##x,
#include "glapit.h"
#undef ENTRY_NR
#undef ENTRY_RV
};

GLAPITABLE *ctx_gl_table = &ctx_nop_table;


void
gl_switch_nop (void)
{
    ctx_gl_table = &ctx_nop_table;
}


void
gl_switch_imm (void)
{
    ctx_gl_table = &ctx_imm_table;
}


void
gl_switch_sav (void)
{
    ctx_gl_table = &ctx_sav_table;
}


static void
gl_init_imm (void)
{
    /* XXX necessary? */
}


static void
gl_init_sav (void)
{
    /* XXX necessary? */
}


void
gl_init (void)
{
    gl_init_imm();
    gl_init_sav();
    gl_switch_imm();
}


void
gl_fini (void)
{
    gl_switch_nop();
}
