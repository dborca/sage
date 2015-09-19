%include "xos.inc"

%include "x86.inc"


extrn	tnl_vb
extrn	tnl_prim
extrn	tnl_prim_num


segment	TEXT


	align	16
proc	x86_clipmask
	push	ebx
	push	esi
	push	edi
	push	ebp
	mov	ebx, [tnl_vb + TNL_VB_CLIPMASK]
	mov	ecx, [tnl_prim_num]
	mov	esi, [tnl_vb + TNL_VB_CLIP]
	mov	edi, [tnl_prim]
	mov	ebp, [tnl_vb + TNL_VB_NDC]
	test	ecx, ecx
	jz	.done
	align	16
    .k_loop:
	push	ecx
	push	edi
	mov	edi, [edi + PRIM_COUNT]
	xor	eax, eax
	test	edi, edi
	jz	.next_k
	align	16
    .j_loop:
	fld1
	fdiv	dword [esi + 12]	; 1/w
	push	ebp

	xor	ecx, ecx		; ecx = <       >
	mov	ebp, [esi + 12]		; ebp = w
	add	ebp, ebp		; ebp = 2*|w|,	carry = (w < 0)
	adc	ecx, ecx		; ecx = <      q>
	mov	edx, [esi + 8]		; edx = z
	add	edx, edx		; edx = 2*|z|,	carry = (z < 0)
	adc	ecx, ecx		; ecx = <     qf>
	cmp	ebp, edx		; carry = 2*|w| < 2*|z|
	adc	ecx, ecx		; ecx = <    qfe>
	mov	edx, [esi + 4]		; edx = y
	add	edx, edx		; edx = 2*|y|,	carry = (y < 0)
	adc	ecx, ecx		; ecx = <   qfed>
	cmp	ebp, edx		; carry = 2*|w| < 2*|y|
	adc	ecx, ecx		; ecx = <  qfedc>
	mov	edx, [esi]		; edx = x
	add	edx, edx		; edx = 2*|x|,	carry = (x < 0)
	adc	ecx, ecx		; ecx = < qfedcb>
	cmp	ebp, edx		; carry = 2*|w| < 2*|x|
	adc	ecx, ecx		; ecx = <qfedcba>
	mov	cl, [x86_clip_lut + ecx]

	pop	ebp
	mov	[ebx], cl
	or	eax, ecx
	test	ecx, ecx
	jnz	.next_j
	fld	dword [esi]		;  x    1/w
	fmul	st1			; x/w   1/w
	fld	dword [esi + 4]		;  y    x/w   1/w
	fmul	st2			; y/w   x/w   1/w
	fld	dword [esi + 8]		;  z    y/w   x/w   1/w
	fmul	st3			; z/w   y/w   x/w   1/w
	fxch	st2			; x/w   y/w   z/w   1/w
	fstp	dword [ebp]		; y/w   z/w   1/w
	fstp	dword [ebp + 4]		; z/w   1/w
	fstp	dword [ebp + 8]		; 1/w
	align	16
    .next_j:
	fstp	dword [ebp + 12]
	add	esi, 16
	add	ebp, 16
	inc	ebx
	dec	edi
	jnz	.j_loop
	align	16
    .next_k:
	pop	edi
	pop	ecx
	mov	[edi + PRIM_ORMASK], eax
	add	edi, sizeof_TNL_PRIMITIVE
	dec	ecx
	jnz	.k_loop
	align	16
    .done:
	pop	ebp
	pop	edi
	pop	esi
	pop	ebx
	ret
endp


	align	16
	global	x86_clip_lut
x86_clip_lut:
	db	0
	db	qR
	db	0
	db	qL
	db	qT
	db	qR | qT
	db	qT
	db	qL | qT
	db	0
	db	qR
	db	0
	db	qL
	db	qB
	db	qR | qB
	db	qB
	db	qL | qB
	db	qF
	db	qR | qF
	db	qF
	db	qL | qF
	db	qT | qF
	db	qR | qT | qF
	db	qT | qF
	db	qL | qT | qF
	db	qF
	db	qR | qF
	db	qF
	db	qL | qF
	db	qB | qF
	db	qR | qB | qF
	db	qB | qF
	db	qL | qB | qF
	db	0
	db	qR
	db	0
	db	qL
	db	qT
	db	qR | qT
	db	qT
	db	qL | qT
	db	0
	db	qR
	db	0
	db	qL
	db	qB
	db	qR | qB
	db	qB
	db	qL | qB
	db	qN
	db	qR | qN
	db	qN
	db	qL | qN
	db	qT | qN
	db	qR | qT | qN
	db	qT | qN
	db	qL | qT | qN
	db	qN
	db	qR | qN
	db	qN
	db	qL | qN
	db	qB | qN
	db	qR | qB | qN
	db	qB | qN
	db	qL | qB | qN
	db	qL | qR | qB | qT | qN | qF
	db	qR | qB | qT | qN | qF
	db	qL | qR | qB | qT | qN | qF
	db	qL | qB | qT | qN | qF
	db	qL | qR | qT | qN | qF
	db	qR | qT | qN | qF
	db	qL | qR | qT | qN | qF
	db	qL | qT | qN | qF
	db	qL | qR | qB | qT | qN | qF
	db	qR | qB | qT | qN | qF
	db	qL | qR | qB | qT | qN | qF
	db	qL | qB | qT | qN | qF
	db	qL | qR | qB | qN | qF
	db	qR | qB | qN | qF
	db	qL | qR | qB | qN | qF
	db	qL | qB | qN | qF
	db	qL | qR | qB | qT | qF
	db	qR | qB | qT | qF
	db	qL | qR | qB | qT | qF
	db	qL | qB | qT | qF
	db	qL | qR | qT | qF
	db	qR | qT | qF
	db	qL | qR | qT | qF
	db	qL | qT | qF
	db	qL | qR | qB | qT | qF
	db	qR | qB | qT | qF
	db	qL | qR | qB | qT | qF
	db	qL | qB | qT | qF
	db	qL | qR | qB | qF
	db	qR | qB | qF
	db	qL | qR | qB | qF
	db	qL | qB | qF
	db	qL | qR | qB | qT | qN | qF
	db	qR | qB | qT | qN | qF
	db	qL | qR | qB | qT | qN | qF
	db	qL | qB | qT | qN | qF
	db	qL | qR | qT | qN | qF
	db	qR | qT | qN | qF
	db	qL | qR | qT | qN | qF
	db	qL | qT | qN | qF
	db	qL | qR | qB | qT | qN | qF
	db	qR | qB | qT | qN | qF
	db	qL | qR | qB | qT | qN | qF
	db	qL | qB | qT | qN | qF
	db	qL | qR | qB | qN | qF
	db	qR | qB | qN | qF
	db	qL | qR | qB | qN | qF
	db	qL | qB | qN | qF
	db	qL | qR | qB | qT | qN
	db	qR | qB | qT | qN
	db	qL | qR | qB | qT | qN
	db	qL | qB | qT | qN
	db	qL | qR | qT | qN
	db	qR | qT | qN
	db	qL | qR | qT | qN
	db	qL | qT | qN
	db	qL | qR | qB | qT | qN
	db	qR | qB | qT | qN
	db	qL | qR | qB | qT | qN
	db	qL | qB | qT | qN
	db	qL | qR | qB | qN
	db	qR | qB | qN
	db	qL | qR | qB | qN
	db	qL | qB | qN


;	if (x < -w) {
;	    code |= qL;
;	}
;	if (x > w) {
;	    code |= qR;
;	}
;
;   LEFT = (w < 0) ? ((|x| < |w|) | (x < 0))
;		   : ((|x| > |w|) & (x < 0))
;
;   RIGHT= (w < 0) ? ((|x| < |w|) | (x > 0))
;		   : ((|x| > |w|) & (x > 0))
;
;   q = w < 0
;   a = |x| > |w|, b = x < 0
;   c = |y| > |w|, d = y < 0
;   e = |z| > |w|, f = z < 0
;
;   L = q ? (!a |  b) : (a &  b)
;   R = q ? (!a | !b) : (a & !b)
;   B = q ? (!c |  d) : (c &  d)
;   T = q ? (!c | !d) : (c & !d)
;   N = q ? (!e |  f) : (e &  f)
;   F = q ? (!e | !f) : (e & !f)


;#include <stdio.h>
;#define CCODE(p) if (p) { if (v) v = 0; else printf(" | "); printf("q" #p); }
;int
;main (void)
;{
;    int a, b, c, d, e, f, q;
;    for (q = 0; q < 2; q++) {
;	for (f = 0; f < 2; f++) { for (e = 0; e < 2; e++) {
;	    for (d = 0; d < 2; d++) { for (c = 0; c < 2; c++) {
;		for (b = 0; b < 2; b++) { for (a = 0; a < 2; a++) {
;		    int L = q ? (!a |  b) : (a &  b);
;		    int R = q ? (!a | !b) : (a & !b);
;		    int B = q ? (!c |  d) : (c &  d);
;		    int T = q ? (!c | !d) : (c & !d);
;		    int N = q ? (!e |  f) : (e &  f);
;		    int F = q ? (!e | !f) : (e & !f);
;		    int v = !0;
;		    printf("\tdb\t");
;		    CCODE(L);
;		    CCODE(R);
;		    CCODE(B);
;		    CCODE(T);
;		    CCODE(N);
;		    CCODE(F);
;		    if (v) {
;			printf("0");
;		    }
;		    printf("\n");
;		}}
;	    }}
;	}}
;    }
;    return 0;
;}
