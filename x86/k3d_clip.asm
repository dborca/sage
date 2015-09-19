%include "xos.inc"

%include "x86.inc"


extrn	tnl_vb
extrn	tnl_prim
extrn	tnl_prim_num
extrn	x86_clip_lut


segment	TEXT


	align	16
proc	k3d_clipmask
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
	movd	mm0, [esi + 12]
	pfrcp	mm1, mm0
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
	punpckldq mm0, mm0
	mov	[ebx], cl
	or	eax, ecx
	test	ecx, ecx
	jnz	.next_j
	pfrcpit1 mm0, mm1
	movq	mm3, [esi + 8]
	pfrcpit2 mm0, mm1
	movq	mm2, [esi]
	pfmul	mm3, mm0
	pfmul	mm2, mm0
	punpckldq mm3, mm0
	movq	[ebp], mm2
	movq	[ebp + 8], mm3
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
	femms
	align	16
    .done:
	pop	ebp
	pop	edi
	pop	esi
	pop	ebx
	ret
endp
