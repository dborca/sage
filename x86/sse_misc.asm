%include "xos.inc"

%include "x86.inc"


%macro	movt	2
    ; dst[3] = src[0]
	shufps	%2, %1, SHUF(X, Y, Z, W)
	shufps	%1, %2, SHUF(X, Y, Z, X)
%endmacro


extrn	tnl_vb
extrn	get_imv


segment	TEXT


	align	16
proc	sse_calc_veyn4
	mov	ecx, [esp + 4]
	push	edi
	mov	edi, [tnl_vb + TNL_VB_VEYN]
	mov	edx, [tnl_vb + TNL_VB_VEYE]
	movups	xmm4, [ecx]
	movups	xmm5, [ecx+0x10]
	movups	xmm6, [ecx+0x20]
	movups	xmm7, [ecx+0x30]
	mov	eax, [tnl_vb + TNL_VB_VERTEX_DATA]
	mov	ecx, [tnl_vb + TNL_VB_LEN]
	align	16
    .0:
	movaps	xmm0, [eax]
	movaps	xmm1, xmm0
	movaps	xmm2, xmm0
	movaps	xmm3, xmm0
	shufps	xmm0, xmm0, SHUF(X, X, X, X)
	shufps	xmm1, xmm1, SHUF(Y, Y, Y, Y)
	shufps	xmm2, xmm2, SHUF(Z, Z, Z, Z)
	shufps	xmm3, xmm3, SHUF(W, W, W, W)
	mulps	xmm0, xmm4
	mulps	xmm1, xmm5
	mulps	xmm2, xmm6
	mulps	xmm3, xmm7
	addps	xmm1, xmm0
	addps	xmm2, xmm3
	addps	xmm1, xmm2
	movaps	[edx], xmm1			;  x  |  y  |  z  |  w

	movaps	xmm0, xmm1			;  x  |  y  |  z  |  w
	mulps	xmm1, xmm1			; x*x | y*y | z*z | w*w
	movaps	xmm2, xmm1
	movaps	xmm3, xmm1
	shufps	xmm2, xmm2, SHUF(Y, Z, X, W)	; y*y | z*z | x*x | w*w
	shufps	xmm3, xmm3, SHUF(Z, X, Y, W)	; z*z | x*x | y*y | w*w
	addps	xmm1, xmm2
	addps	xmm1, xmm3
	rsqrtps	xmm1, xmm1
	mulps	xmm0, xmm1
	movaps	[edi], xmm0

	add	eax, 16
	add	edx, 16
	add	edi, 16
	dec	ecx
	jnz	.0

	pop	edi
	ret
endp


	align	16
proc	sse_calc_veyn3
	mov	ecx, [esp + 4]
	push	edi
	mov	edi, [tnl_vb + TNL_VB_VEYN]
	mov	edx, [tnl_vb + TNL_VB_VEYE]
	movups	xmm4, [ecx]
	movups	xmm5, [ecx+0x10]
	movups	xmm6, [ecx+0x20]
	movups	xmm7, [ecx+0x30]
	mov	eax, [tnl_vb + TNL_VB_VERTEX_DATA]
	mov	ecx, [tnl_vb + TNL_VB_LEN]
	align	16
    .0:
	movaps	xmm0, [eax]
	movaps	xmm1, xmm0
	movaps	xmm2, xmm0
	shufps	xmm0, xmm0, SHUF(X, X, X, X)
	shufps	xmm1, xmm1, SHUF(Y, Y, Y, Y)
	shufps	xmm2, xmm2, SHUF(Z, Z, Z, Z)
	mulps	xmm0, xmm4
	mulps	xmm1, xmm5
	mulps	xmm2, xmm6
	addps	xmm1, xmm0
	addps	xmm2, xmm7
	addps	xmm1, xmm2
	movaps	[edx], xmm1			;  x  |  y  |  z  |  w

	movaps	xmm0, xmm1			;  x  |  y  |  z  |  w
	mulps	xmm1, xmm1			; x*x | y*y | z*z | w*w
	movaps	xmm2, xmm1
	movaps	xmm3, xmm1
	shufps	xmm2, xmm2, SHUF(Y, Z, X, W)	; y*y | z*z | x*x | w*w
	shufps	xmm3, xmm3, SHUF(Z, X, Y, W)	; z*z | x*x | y*y | w*w
	addps	xmm1, xmm2
	addps	xmm1, xmm3
	rsqrtps	xmm1, xmm1
	mulps	xmm0, xmm1
	movaps	[edi], xmm0

	add	eax, 16
	add	edx, 16
	add	edi, 16
	dec	ecx
	jnz	.0

	pop	edi
	ret
endp


	align	16
proc	sse_calc_neye
	call	get_imv
	mov	edx, [tnl_vb + TNL_VB_NORMAL_STRIDE]
	mov	ecx, eax
	test	edx, edx
	movups	xmm4, [ecx]
	movups	xmm5, [ecx+0x10]
	movups	xmm6, [ecx+0x20]
	mov	edx, [tnl_vb + TNL_VB_NEYE]
	mov	eax, [tnl_vb + TNL_VB_NORMAL_DATA]
	mov	ecx, [tnl_vb + TNL_VB_LEN]
	jz	.2
%if 0
	align	16
    .0:
	movaps	xmm0, [eax]
	prefetchnta	[eax+0x30]
	movaps	xmm1, xmm0
	add	eax, 16
	movaps	xmm2, xmm0
	add	edx, 16
	prefetchnta	[edx+0x20]
	shufps	xmm0, xmm0, SHUF(X, X, X, X)
	shufps	xmm1, xmm1, SHUF(Y, Y, Y, Y)
	shufps	xmm2, xmm2, SHUF(Z, Z, Z, Z)
	mulps	xmm0, xmm4
	mulps	xmm1, xmm5
	mulps	xmm2, xmm6
	addps	xmm1, xmm0
	addps	xmm1, xmm2
	movaps	[edx-16], xmm1
	dec	ecx
	jnz	.0
%else
	push	esi
	push	edi
	mov	esi, [tnl_vb + TNL_VB_FLAGS]
	mov	edi, TNL_NORMAL_BIT
	align	16
    .0:
	test	edi, TNL_NORMAL_BIT
	jz	.1
	movaps	xmm0, [eax]
	prefetchnta	[eax+0x30]
	movaps	xmm1, xmm0
	movaps	xmm2, xmm0
	prefetchnta	[edx+0x30]
	shufps	xmm0, xmm0, SHUF(X, X, X, X)
	shufps	xmm1, xmm1, SHUF(Y, Y, Y, Y)
	shufps	xmm2, xmm2, SHUF(Z, Z, Z, Z)
	mulps	xmm0, xmm4
	mulps	xmm1, xmm5
	mulps	xmm2, xmm6
	addps	xmm1, xmm0
	addps	xmm1, xmm2
	align	16
    .1:
	movaps	[edx], xmm1
	add	esi, 4
	add	eax, 16
	add	edx, 16
	mov	edi, [esi]
	dec	ecx
	jnz	.0
	pop	edi
	pop	esi
%endif
	ret
	align	16
    .2:
	movaps	xmm0, [eax]
	movaps	xmm1, xmm0
	movaps	xmm2, xmm0
	shufps	xmm0, xmm0, SHUF(X, X, X, X)
	shufps	xmm1, xmm1, SHUF(Y, Y, Y, Y)
	shufps	xmm2, xmm2, SHUF(Z, Z, Z, Z)
	mulps	xmm0, xmm4
	mulps	xmm1, xmm5
	mulps	xmm2, xmm6
	addps	xmm1, xmm0
	addps	xmm1, xmm2
	align	16
    .3:
	movaps	[edx], xmm1
	add	edx, 16
	dec	ecx
	jnz	.3
	ret
endp


	align	16
f_0010	dd	0.0, 0.0, 1.0, 0.0
f_2222	dd	2.0, 2.0, 2.0, 2.0
f_h000	dd	0.5, 0.0, 0.0, 0.0


	align	16
proc	sse_reflect
	push	ebp
	mov	ebp, [tnl_vb + TNL_VB_LEN]
	mov	eax, [tnl_vb + TNL_VB_VEYN]
	mov	ecx, [tnl_vb + TNL_VB_NEYE]
	mov	edx, [tnl_vb + TNL_VB_REFL]
	movaps	xmm5, [f_0010]
	movaps	xmm6, [f_2222]
	align	16
    .0:
	movaps	xmm1, [eax]		;    ux     |    uy     |    uz     |    uw
	movaps	xmm2, [ecx]		;    nx     |    ny     |    nz     |    nw
	movaps	xmm0, xmm1		;    ux     |    uy     |    uz     |    uw
	mulps	xmm1, xmm2		;   ux*nx   |   uy*ny   |   uz*nz   |   uw*nw
	movaps	xmm3, xmm1		;   ux*nx   |   uy*ny   |   uz*nz   |   uw*nw
	movaps	xmm4, xmm1		;   ux*nx   |   uy*ny   |   uz*nz   |   uw*nw
	shufps	xmm3, xmm3, SHUF(Y, Z, X, W)
	shufps	xmm4, xmm4, SHUF(Z, X, Y, W)
	addps	xmm1, xmm3
	addps	xmm1, xmm4		;    dot    |    dot    |    dot    |     ?

	mulps	xmm1, xmm6
	mulps	xmm1, xmm2
	subps	xmm1, xmm5
	subps	xmm0, xmm1
	movaps	[edx], xmm0

	add	eax, 16
	add	ecx, 16
	add	edx, 16
	dec	ebp
	jnz	.0
	pop	ebp
	ret
endp


	align	16
proc	sse_reflect_mvec
	push	ebp
	mov	ebp, [tnl_vb + TNL_VB_LEN]
	mov	eax, [tnl_vb + TNL_VB_VEYN]
	mov	ecx, [tnl_vb + TNL_VB_NEYE]
	mov	edx, [tnl_vb + TNL_VB_REFL]
	movaps	xmm5, [f_0010]
	movaps	xmm6, [f_2222]
	movaps	xmm7, [f_h000]
	align	16
    .0:
	movaps	xmm1, [eax]		;    ux     |    uy     |    uz     |    uw
	movaps	xmm2, [ecx]		;    nx     |    ny     |    nz     |    nw
	movaps	xmm0, xmm1		;    ux     |    uy     |    uz     |    uw
	mulps	xmm1, xmm2		;   ux*nx   |   uy*ny   |   uz*nz   |   uw*nw
	movaps	xmm3, xmm1		;   ux*nx   |   uy*ny   |   uz*nz   |   uw*nw
	movaps	xmm4, xmm1		;   ux*nx   |   uy*ny   |   uz*nz   |   uw*nw
	shufps	xmm3, xmm3, SHUF(Y, Z, X, W)
	shufps	xmm4, xmm4, SHUF(Z, X, Y, W)
	addps	xmm1, xmm3
	addps	xmm1, xmm4		;    dot    |    dot    |    dot    |     ?

	mulps	xmm1, xmm6
	mulps	xmm1, xmm2
	subps	xmm1, xmm5
	subps	xmm0, xmm1

	movaps	xmm1, xmm0
	mulps	xmm1, xmm1
	movaps	xmm2, xmm1
	movaps	xmm3, xmm1
	shufps	xmm2, xmm2, SHUF(Y, Z, X, W)
	shufps	xmm3, xmm3, SHUF(Z, X, Y, W)
	addps	xmm1, xmm2
	addps	xmm1, xmm3
	rsqrtps	xmm1, xmm1
	mulps	xmm1, xmm7

	movt	xmm0, xmm1
	movaps	[edx], xmm0

	add	eax, 16
	add	ecx, 16
	add	edx, 16
	dec	ebp
	jnz	.0
	pop	ebp
	ret
endp
