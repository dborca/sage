%include "xos.inc"

%include "x86.inc"
%include "drv.inc"


extrn	tnl_vb
extrn	ctx_mx_viewport
extrn	ctx_texture

extrn	vb
extrn	tmu0_source
extrn	tmu1_source


segment	TEXT


	align	16
f_0010	dd	0.0, 0.0, 1.0, 0.0
f_bbbb	dd	255.0, 255.0, 255.0, 255.0
i_xxx0	dd	-1, -1, -1, 0


%define TAG(x) x %+ _g
%define IND 0
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t0
%define IND SETUP_TEX0
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q0
%define IND (SETUP_TEX0|SETUP_PTX0)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t1
%define IND SETUP_TEX1
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q1
%define IND (SETUP_TEX1|SETUP_PTX1)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t0t1
%define IND (SETUP_TEX0|SETUP_TEX1)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q0t1
%define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t0q1
%define IND (SETUP_TEX0|SETUP_TEX1|SETUP_PTX1)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q0q1
%define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _f
%define IND (SETUP_FOGC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t0f
%define IND (SETUP_TEX0|SETUP_FOGC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q0f
%define IND (SETUP_TEX0|SETUP_PTX0|SETUP_FOGC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t1f
%define IND (SETUP_TEX1|SETUP_FOGC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q1f
%define IND (SETUP_TEX1|SETUP_PTX1|SETUP_FOGC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t0t1f
%define IND (SETUP_TEX0|SETUP_TEX1|SETUP_FOGC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q0t1f
%define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_FOGC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t0q1f
%define IND (SETUP_TEX0|SETUP_TEX1|SETUP_PTX1|SETUP_FOGC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q0q1f
%define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1|SETUP_FOGC)
%include "sse_vbtmp.asm"


%define TAG(x) x %+ _s
%define IND (SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t0s
%define IND (SETUP_TEX0|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q0s
%define IND (SETUP_TEX0|SETUP_PTX0|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t1s
%define IND (SETUP_TEX1|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q1s
%define IND (SETUP_TEX1|SETUP_PTX1|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t0t1s
%define IND (SETUP_TEX0|SETUP_TEX1|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q0t1s
%define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t0q1s
%define IND (SETUP_TEX0|SETUP_TEX1|SETUP_PTX1|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q0q1s
%define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _fs
%define IND (SETUP_FOGC|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t0fs
%define IND (SETUP_TEX0|SETUP_FOGC|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q0fs
%define IND (SETUP_TEX0|SETUP_PTX0|SETUP_FOGC|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t1fs
%define IND (SETUP_TEX1|SETUP_FOGC|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q1fs
%define IND (SETUP_TEX1|SETUP_PTX1|SETUP_FOGC|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t0t1fs
%define IND (SETUP_TEX0|SETUP_TEX1|SETUP_FOGC|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q0t1fs
%define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_FOGC|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _t0q1fs
%define IND (SETUP_TEX0|SETUP_TEX1|SETUP_PTX1|SETUP_FOGC|SETUP_SPEC)
%include "sse_vbtmp.asm"

%define TAG(x) x %+ _q0q1fs
%define IND (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1|SETUP_FOGC|SETUP_SPEC)
%include "sse_vbtmp.asm"
