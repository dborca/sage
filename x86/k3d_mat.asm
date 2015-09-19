%include "xos.inc"


segment	TEXT


	align	16
proc	matrix_mul_vec4_k3d
	mov	edx, [esp + 4]
	mov	ecx, [esp + 8]
	mov	eax, [esp + 12]
	movq	mm0, [eax]		;   x   |   y
	movq	mm4, [eax + 8]		;   z   |   w
	movq	mm1, mm0		;   x   |   y
	movq	mm5, mm4		;   z   |   w
	punpckldq mm0, mm0		;   x   |   x
	punpckldq mm4, mm4		;   z   |   z
	movq	mm2, mm0		;   x   |   x
	movq	mm6, mm4		;   z   |   z
	punpckhdq mm1, mm1		;   y   |   y
	punpckhdq mm5, mm5		;   w   |   w
	movq	mm3, mm1		;   y   |   y
	movq	mm7, mm5		;   w   |   w
	pfmul	mm0, qword [ecx]	; a * x | b * x
	pfmul	mm2, qword [ecx + 8]	; c * x | d * x
	pfmul	mm1, qword [ecx + 16]	; e * y | f * y
	pfmul	mm3, qword [ecx + 24]	; g * y | h * y
	pfmul	mm4, qword [ecx + 32]	; i * z | j * z
	pfmul	mm6, qword [ecx + 40]	; k * z | l * z
	pfmul	mm5, qword [ecx + 48]	; m * w | n * w
	pfmul	mm7, qword [ecx + 56]	; o * w | p * w
	pfadd	mm0, mm1		; ax+ey | bx+fy
	pfadd	mm2, mm3		; cx+gy | dx+hy
	pfadd	mm4, mm5		; iz+mw | jz+nw
	pfadd	mm6, mm7		; kz+ow | lz+pw
	pfadd	mm0, mm4
	pfadd	mm2, mm6
	movq	[edx], mm0
	movq	[edx + 8], mm2
	femms
	ret
endp


	align	16
proc	matrix_mul_vec3_k3d
	mov	edx, [esp + 4]
	mov	ecx, [esp + 8]
	mov	eax, [esp + 12]
	movq	mm0, [eax]		;   x   |   y
	movd	mm4, [eax + 8]		;   z   |   0
	movq	mm1, mm0		;   x   |   y
	punpckldq mm0, mm0		;   x   |   x
	punpckldq mm4, mm4		;   z   |   z
	movq	mm2, mm0		;   x   |   x
	movq	mm6, mm4		;   z   |   z
	punpckhdq mm1, mm1		;   y   |   y
	movq	mm3, mm1		;   y   |   y
	pfmul	mm0, qword [ecx]	; a * x | b * x
	pfmul	mm2, qword [ecx + 8]	; c * x | d * x
	pfmul	mm1, qword [ecx + 16]	; e * y | f * y
	pfmul	mm3, qword [ecx + 24]	; g * y | h * y
	pfmul	mm4, qword [ecx + 32]	; i * z | j * z
	pfmul	mm6, qword [ecx + 40]	; k * z | l * z
	movq	mm5, qword [ecx + 48]	; m * 1 | n * 1
	movq	mm7, qword [ecx + 56]	; o * 1 | p * 1
	pfadd	mm0, mm1		; ax+ey | bx+fy
	pfadd	mm2, mm3		; cx+gy | dx+hy
	pfadd	mm4, mm5		; iz+m  | jz+n
	pfadd	mm6, mm7		; kz+o  | lz+p
	pfadd	mm0, mm4
	pfadd	mm2, mm6
	movq	[edx], mm0
	movq	[edx + 8], mm2
	femms
	ret
endp


	align	16
proc	matrix_mul_vec4_batch_k3d
	mov	edx, [esp + 4]
	mov	ecx, [esp + 8]
	mov	eax, [esp + 12]
	push	ebx
	mov	ebx, [esp + 20]
	align	16
    .0:
	movq	mm0, [eax]		;   x   |   y
	movq	mm4, [eax + 8]		;   z   |   w
	movq	mm1, mm0		;   x   |   y
	movq	mm5, mm4		;   z   |   w
	punpckldq mm0, mm0		;   x   |   x
	punpckldq mm4, mm4		;   z   |   z
	movq	mm2, mm0		;   x   |   x
	movq	mm6, mm4		;   z   |   z
	punpckhdq mm1, mm1		;   y   |   y
	punpckhdq mm5, mm5		;   w   |   w
	movq	mm3, mm1		;   y   |   y
	movq	mm7, mm5		;   w   |   w
	prefetchw [edx + 0x20]
	pfmul	mm0, qword [ecx]	; a * x | b * x
	pfmul	mm2, qword [ecx + 8]	; c * x | d * x
	pfmul	mm1, qword [ecx + 16]	; e * y | f * y
	pfmul	mm3, qword [ecx + 24]	; g * y | h * y
	pfmul	mm4, qword [ecx + 32]	; i * z | j * z
	pfmul	mm6, qword [ecx + 40]	; k * z | l * z
	pfmul	mm5, qword [ecx + 48]	; m * w | n * w
	pfmul	mm7, qword [ecx + 56]	; o * w | p * w
	pfadd	mm0, mm1		; ax+ey | bx+fy
	pfadd	mm2, mm3		; cx+gy | dx+hy
	pfadd	mm4, mm5		; iz+mw | jz+nw
	pfadd	mm6, mm7		; kz+ow | lz+pw
	pfadd	mm0, mm4
	pfadd	mm2, mm6
	prefetch [eax + 0x20]
	movq	[edx], mm0
	movq	[edx + 8], mm2
	add	eax, 16
	add	edx, 16
	dec	ebx
	jnz	.0
	pop	ebx
	femms
	ret
endp


	align	16
proc	matrix_mul_vec3_batch_k3d
	mov	edx, [esp + 4]
	mov	ecx, [esp + 8]
	mov	eax, [esp + 12]
	push	ebx
	mov	ebx, [esp + 20]
	movq	mm5, qword [ecx + 48]	; m * 1 | n * 1
	movq	mm7, qword [ecx + 56]	; o * 1 | p * 1
	align	16
    .0:
	movq	mm0, [eax]		;   x   |   y
	movd	mm4, [eax + 8]		;   z   |   0
	movq	mm1, mm0		;   x   |   y
	punpckldq mm0, mm0		;   x   |   x
	punpckldq mm4, mm4		;   z   |   z
	movq	mm2, mm0		;   x   |   x
	movq	mm6, mm4		;   z   |   z
	punpckhdq mm1, mm1		;   y   |   y
	movq	mm3, mm1		;   y   |   y
	prefetchw [edx + 0x20]
	pfmul	mm0, qword [ecx]	; a * x | b * x
	pfmul	mm2, qword [ecx + 8]	; c * x | d * x
	pfmul	mm1, qword [ecx + 16]	; e * y | f * y
	pfmul	mm3, qword [ecx + 24]	; g * y | h * y
	pfmul	mm4, qword [ecx + 32]	; i * z | j * z
	pfmul	mm6, qword [ecx + 40]	; k * z | l * z
	pfadd	mm0, mm1		; ax+ey | bx+fy
	pfadd	mm2, mm3		; cx+gy | dx+hy
	pfadd	mm4, mm5		; iz+m  | jz+n
	pfadd	mm6, mm7		; kz+o  | lz+p
	pfadd	mm0, mm4
	pfadd	mm2, mm6
	prefetch [eax + 0x20]
	movq	[edx], mm0
	movq	[edx + 8], mm2
	add	eax, 16
	add	edx, 16
	dec	ebx
	jnz	.0
	pop	ebx
	femms
	ret
endp


	align	16
proc	matrix_mul_vec_rot_k3d
	mov	edx, [esp + 4]
	mov	ecx, [esp + 8]
	mov	eax, [esp + 12]
	movq	mm0, [eax]		;   x   |   y
	movd	mm4, [eax + 8]		;   z   |   0
	movq	mm1, mm0		;   x   |   y
	punpckldq mm0, mm0		;   x   |   x
	punpckldq mm4, mm4		;   z   |   z
	movq	mm2, mm0		;   x   |   x
	movq	mm6, mm4		;   z   |   z
	punpckhdq mm1, mm1		;   y   |   y
	movq	mm3, mm1		;   y   |   y
	pfmul	mm0, qword [ecx]	; a * x | b * x
	pfmul	mm2, qword [ecx + 8]	; c * x | d * x
	pfmul	mm1, qword [ecx + 16]	; e * y | f * y
	pfmul	mm3, qword [ecx + 24]	; g * y | h * y
	pfmul	mm4, qword [ecx + 32]	; i * z | j * z
	pfmul	mm6, qword [ecx + 40]	; k * z | l * z
	pfadd	mm0, mm1		; ax+ey | bx+fy
	pfadd	mm2, mm3		; cx+gy | dx+hy
	pfadd	mm0, mm4
	pfadd	mm2, mm6
	movq	[edx], mm0
	movd	[edx + 8], mm2
	femms
	ret
endp
