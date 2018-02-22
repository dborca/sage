#include <stddef.h>
#include <string.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "main/glapi.h"
#include "main/context.h"
#include "main/matrix.h"
#include "tnl/tnl.h"
#include "util/cfg.h"
#include "cpu.h"
#include "x86.h"


void x86_clipmask (void);
void GLAPIENTRY x86_Vertex3fv (const GLfloat *v);
void GLAPIENTRY x86_TexCoord2fv (const GLfloat *v);
void GLAPIENTRY x86_MultiTexCoord2fv (GLenum texture, const GLfloat *v);

void k3d_clipmask (void);
void k3d_calc_neye (void);
void k3d_calc_veyn3 (const GLfloat *mv);
void k3d_calc_veyn4 (const GLfloat *mv);
void k3d_reflect (void);
void k3d_reflect_mvec (void);
void matrix_mul_vec4_k3d (GLfloat4 dst, const GLfloat *m, const GLfloat4 src);
void matrix_mul_vec4_batch_k3d (GLfloat4 dst[], const GLfloat *m, const GLfloat4 src[], int n);
void matrix_mul_vec3_k3d (GLfloat4 dst, const GLfloat *m, const GLfloat4 src);
void matrix_mul_vec3_batch_k3d (GLfloat4 dst[], const GLfloat *m, const GLfloat4 src[], int n);
void matrix_mul_vec_rot_k3d (GLfloat4 dst, const GLfloat *m, const GLfloat4 src);

void sse_clipmask (void);
void sse_calc_neye (void);
void sse_calc_veyn3 (const GLfloat *mv);
void sse_calc_veyn4 (const GLfloat *mv);
void sse_reflect (void);
void sse_reflect_mvec (void);
void matrix_mul_vec4_sse (GLfloat4 dst, const GLfloat *m, const GLfloat4 src);
void matrix_mul_vec4_batch_sse (GLfloat4 dst[], const GLfloat *m, const GLfloat4 src[], int n);
void matrix_mul_vec3_sse (GLfloat4 dst, const GLfloat *m, const GLfloat4 src);
void matrix_mul_vec3_batch_sse (GLfloat4 dst[], const GLfloat *m, const GLfloat4 src[], int n);
void matrix_mul_vec_rot_sse (GLfloat4 dst, const GLfloat *m, const GLfloat4 src);
void GLAPIENTRY sse_Vertex3fv (const GLfloat *v);
void GLAPIENTRY sse_TexCoord2fv (const GLfloat *v);
void GLAPIENTRY sse_MultiTexCoord2fv (GLenum texture, const GLfloat *v);


int x86_cpu_bits;
char x86_cpu_name[13];

int x86_enable_sse;
int x86_enable_3dnow;


static int
sse_init (void)
{
    tnl_clipmask_tab[0] = sse_clipmask;
    tnl_calc_neye_tab[0] = sse_calc_neye;
    tnl_veyn_func[0] = sse_calc_veyn3;
    tnl_veyn_func[1] = sse_calc_veyn4;
    tnl_refl_func[0] = sse_reflect;
    tnl_refl_func[1] = sse_reflect_mvec;
    matrix_mul_vec4 = matrix_mul_vec4_sse;
    matrix_mul_vec4_batch = matrix_mul_vec4_batch_sse;
    matrix_mul_vec3 = matrix_mul_vec3_sse;
    matrix_mul_vec3_batch = matrix_mul_vec3_batch_sse;
    matrix_mul_vec_rot = matrix_mul_vec_rot_sse;
    /*ctx_imm_table.Vertex3fv = sse_Vertex3fv;*/
    ctx_imm_table.TexCoord2fv = sse_TexCoord2fv;
    ctx_imm_table.MultiTexCoord2fv = sse_MultiTexCoord2fv;
    return 0;
}


static int
k3d_init (void)
{
    tnl_clipmask_tab[0] = k3d_clipmask;
    tnl_calc_neye_tab[0] = k3d_calc_neye;
    tnl_veyn_func[0] = k3d_calc_veyn3;
    tnl_veyn_func[1] = k3d_calc_veyn4;
    tnl_refl_func[0] = k3d_reflect;
    tnl_refl_func[1] = k3d_reflect_mvec;
    matrix_mul_vec4 = matrix_mul_vec4_k3d;
    matrix_mul_vec4_batch = matrix_mul_vec4_batch_k3d;
    matrix_mul_vec3 = matrix_mul_vec3_k3d;
    matrix_mul_vec3_batch = matrix_mul_vec3_batch_k3d;
    matrix_mul_vec_rot = matrix_mul_vec_rot_k3d;
    return 0;
}


#define YES(v) !strcmp(cfg_get(v, "n"), "y")

int
x86_init (void)
{
    x86_cpu_bits = cpusoft(x86_cpu_name);

    x86_enable_3dnow = YES("x86.enable.3dnow");
    x86_enable_sse   = YES("x86.enable.sse");

    tnl_clipmask_tab[0] = x86_clipmask;
    /*ctx_imm_table.Vertex3fv = x86_Vertex3fv;*/
    ctx_imm_table.TexCoord2fv = x86_TexCoord2fv;
    ctx_imm_table.MultiTexCoord2fv = x86_MultiTexCoord2fv;

    if ((x86_cpu_bits & _CPU_FEATURE_SSE) && x86_enable_sse) {
	sse_init();
    }
    else if ((x86_cpu_bits & _CPU_FEATURE_3DNOW) && x86_enable_3dnow) {
	k3d_init();
    }
    return 0;
}

/* validate structure sizes and member offsets in x86.inc: */
#include "x86chk.h"
