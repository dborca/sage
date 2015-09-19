%include "xos.inc"

%include "x86.inc"


segment	TEXT


	align	16
proc	matrix_mul_vec4_sse
	mov	edx, [esp+4]
	mov	ecx, [esp+8]
	mov	eax, [esp+12]
	movups	xmm4, [ecx]
	movups	xmm5, [ecx+0x10]
	movups	xmm6, [ecx+0x20]
	movups	xmm7, [ecx+0x30]
	movups	xmm0, [eax]
	movups	xmm1, xmm0
	movups	xmm2, xmm0
	movups	xmm3, xmm0
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
	movups	[edx], xmm1
	ret
endp


	align	16
proc	matrix_mul_vec3_sse
	mov	edx, [esp+4]
	mov	ecx, [esp+8]
	mov	eax, [esp+12]
	movups	xmm4, [ecx]
	movups	xmm5, [ecx+0x10]
	movups	xmm6, [ecx+0x20]
	movups	xmm7, [ecx+0x30]
	movups	xmm0, [eax]
	movups	xmm1, xmm0
	movups	xmm2, xmm0
	shufps	xmm0, xmm0, SHUF(X, X, X, X)
	shufps	xmm1, xmm1, SHUF(Y, Y, Y, Y)
	shufps	xmm2, xmm2, SHUF(Z, Z, Z, Z)
	mulps	xmm0, xmm4
	mulps	xmm1, xmm5
	mulps	xmm2, xmm6
	addps	xmm1, xmm0
	addps	xmm2, xmm7
	addps	xmm1, xmm2
	movups	[edx], xmm1
	ret
endp


	align	16
proc	matrix_mul_vec4_batch_sse
	mov	edx, [esp+4]
	mov	ecx, [esp+8]
	mov	eax, [esp+12]
	movups	xmm4, [ecx]
	movups	xmm5, [ecx+0x10]
	movups	xmm6, [ecx+0x20]
	movups	xmm7, [ecx+0x30]
	mov	ecx, [esp+16]
	align	16
    .0:
	movaps	xmm0, [eax]
	prefetchnta	[eax+0x30]
	movaps	xmm1, xmm0
	add	eax, 16
	movaps	xmm2, xmm0
	add	edx, 16
	movaps	xmm3, xmm0
	prefetchnta	[edx+0x20]
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
	movaps	[edx-16], xmm1
	dec	ecx
	jnz	.0
	ret
endp


	align	16
proc	matrix_mul_vec3_batch_sse
	mov	edx, [esp+4]
	mov	ecx, [esp+8]
	mov	eax, [esp+12]
	movups	xmm4, [ecx]
	movups	xmm5, [ecx+0x10]
	movups	xmm6, [ecx+0x20]
	movups	xmm7, [ecx+0x30]
	mov	ecx, [esp+16]
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
	addps	xmm2, xmm7
	addps	xmm1, xmm2
	movaps	[edx-16], xmm1
	dec	ecx
	jnz	.0
	ret
endp


	align	16
proc	matrix_mul_vec_rot_sse
	mov	edx, [esp+4]
	mov	ecx, [esp+8]
	mov	eax, [esp+12]
	movups	xmm4, [ecx]
	movups	xmm5, [ecx+0x10]
	movups	xmm6, [ecx+0x20]
	movups	xmm0, [eax]
	movups	xmm1, xmm0
	movups	xmm2, xmm0
	shufps	xmm0, xmm0, SHUF(X, X, X, X)
	shufps	xmm1, xmm1, SHUF(Y, Y, Y, Y)
	shufps	xmm2, xmm2, SHUF(Z, Z, Z, Z)
	mulps	xmm0, xmm4
	mulps	xmm1, xmm5
	mulps	xmm2, xmm6
	addps	xmm1, xmm0
	addps	xmm1, xmm2
	movups	[edx], xmm1
	ret
endp
