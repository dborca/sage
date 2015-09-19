%include "xos.inc"

%include "x86.inc"


%macro	rcppsit	2
    ; same operands as rcpps
    ; result = a * (2 - x * a)
	mulps	%2, %1
	mulps	%2, %1
	addps	%1, %1
	subps	%1, %2
%endmacro


%macro	movt	2
    ; dst[3] = src[0]
	shufps	%2, %1, SHUF(X, Y, Z, W)
	shufps	%1, %2, SHUF(X, Y, Z, X)
%endmacro


extrn	tnl_vb
extrn	tnl_prim
extrn	tnl_prim_num
extrn	x86_clip_lut


segment	TEXT


	align	16
proc	sse_clipmask
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
	movaps	xmm2, [esi]
	rcpps	xmm0, xmm2
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
	movaps	xmm1, xmm2
	mov	[ebx], cl
	or	eax, ecx
	test	ecx, ecx
	jnz	.next_j
	rcppsit	xmm0, xmm1
	shufps	xmm0, xmm0, SHUF(W, W, W, W)
	mulps	xmm2, xmm0
	movt	xmm2, xmm0
	movaps	[ebp], xmm2
	align	16
    .next_j:
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
