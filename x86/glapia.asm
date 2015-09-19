%include "xos.inc"


extern	ctx_gl_table


struc	glapi
%define ENTRY_NR(z, x, y, w, k) x resd 1
%define ENTRY_RV(z, x, y, w, k) x resd 1
%include "glapit.h"
%undef ENTRY_NR
%undef ENTRY_RV
endstruc

%macro	JUMP	2
	align	16
proc	gl%1, %2
	mov	eax, [ctx_gl_table]
	jmp	[eax + %1]
endp
dllexp	gl%1
%endmacro


%define ENTRY_NR(z, x, y, w, k) JUMP x, k
%define ENTRY_RV(z, x, y, w, k) JUMP x, k
%include "glapit.h"
%undef ENTRY_NR
%undef ENTRY_RV
