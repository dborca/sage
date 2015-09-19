%include "xos.inc"

%include "x86.inc"


extrn	tnl_vb
extrn	get_imv


segment	TEXT


	align	16
proc	k3d_calc_veyn4
	push	ebp
	push	edi
	mov	edi, [tnl_vb + TNL_VB_VEYN]
	mov	edx, [tnl_vb + TNL_VB_VEYE]
	mov	ecx, [esp + 12]
	mov	eax, [tnl_vb + TNL_VB_VERTEX_DATA]
	mov	ebp, [tnl_vb + TNL_VB_LEN]
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

	pxor	mm3, mm3
	punpckldq mm2, mm3		;     Z     |     0
	movq	mm1, mm0
	movq	mm3, mm2
	pfmul	mm0, mm0		;    X*X    |    Y*Y
	pfmul	mm2, mm2		;    Z*Z    |     0
	pfacc	mm0, mm2		; X*X + Y*Y |    Z*Z
	pfacc	mm0, mm0		;    dot    |    dot
	pfrsqrt	mm2, mm0
	movq	mm4, mm2
	pfmul	mm2, mm2
	pfrsqit1 mm2, mm0
	pfrcpit2 mm2, mm4
	pfmul	mm1, mm2
	pfmul	mm3, mm2
	movq	[edi], mm1
	movd	[edi + 8], mm3

	add	eax, 16
	add	edx, 16
	add	edi, 16
	dec	ebp
	jnz	.0
	femms
	pop	edi
	pop	ebp
	ret
endp


	align	16
proc	k3d_calc_veyn3
	push	ebp
	push	edi
	mov	edi, [tnl_vb + TNL_VB_VEYN]
	mov	edx, [tnl_vb + TNL_VB_VEYE]
	mov	ecx, [esp + 12]
	mov	eax, [tnl_vb + TNL_VB_VERTEX_DATA]
	mov	ebp, [tnl_vb + TNL_VB_LEN]
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
	movq	[edx], mm0
	movq	[edx + 8], mm2

	pxor	mm3, mm3
	punpckldq mm2, mm3		;     Z     |     0
	movq	mm1, mm0
	movq	mm3, mm2
	pfmul	mm0, mm0		;    X*X    |    Y*Y
	pfmul	mm2, mm2		;    Z*Z    |     0
	pfacc	mm0, mm2		; X*X + Y*Y |    Z*Z
	pfacc	mm0, mm0		;    dot    |    dot
	pfrsqrt	mm2, mm0
	movq	mm4, mm2
	pfmul	mm2, mm2
	pfrsqit1 mm2, mm0
	pfrcpit2 mm2, mm4
	pfmul	mm1, mm2
	pfmul	mm3, mm2
	movq	[edi], mm1
	movd	[edi + 8], mm3

	add	eax, 16
	add	edx, 16
	add	edi, 16
	dec	ebp
	jnz	.0
	femms
	pop	edi
	pop	ebp
	ret
endp


	align	16
proc	k3d_calc_neye
	call	get_imv
	push	ebp
	mov	edx, [tnl_vb + TNL_VB_NORMAL_STRIDE]
	mov	ecx, eax
	test	edx, edx
	movq	mm5, qword [ecx + 32]	;   i   |   j
	movq	mm7, qword [ecx + 40]	;   k   |   l
	mov	edx, [tnl_vb + TNL_VB_NEYE]
	mov	eax, [tnl_vb + TNL_VB_NORMAL_DATA]
	mov	ebp, [tnl_vb + TNL_VB_LEN]
	jz	.2
%if 0
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
	pfmul	mm0, qword [ecx]	; a * x | b * x
	pfmul	mm2, qword [ecx + 8]	; c * x | d * x
	pfmul	mm1, qword [ecx + 16]	; e * y | f * y
	pfmul	mm3, qword [ecx + 24]	; g * y | h * y
	pfmul	mm4, mm5		; i * z | j * z
	pfmul	mm6, mm7		; k * z | l * z
	pfadd	mm0, mm1		; ax+ey | bx+fy
	pfadd	mm2, mm3		; cx+gy | dx+hy
	pfadd	mm0, mm4
	pfadd	mm2, mm6
	movq	[edx], mm0
	movd	[edx + 8], mm2

	add	eax, 16
	add	edx, 16
	dec	ebp
	jnz	.0
	femms
%else
	push	esi
	push	edi
	mov	esi, [tnl_vb + TNL_VB_FLAGS]
	mov	edi, TNL_NORMAL_BIT
	align	16
    .0:
	test	edi, TNL_NORMAL_BIT
	jz	.1
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
	pfmul	mm4, mm5		; i * z | j * z
	pfmul	mm6, mm7		; k * z | l * z
	pfadd	mm0, mm1		; ax+ey | bx+fy
	pfadd	mm2, mm3		; cx+gy | dx+hy
	pfadd	mm0, mm4
	pfadd	mm2, mm6
	align	16
    .1:
	movq	[edx], mm0
	movd	[edx + 8], mm2
	add	esi, 4
	add	eax, 16
	add	edx, 16
	mov	edi, [esi]
	dec	ebp
	jnz	.0
	femms
	pop	edi
	pop	esi
%endif
	pop	ebp
	ret
	align	16
    .2:
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
	pfmul	mm4, mm5		; i * z | j * z
	pfmul	mm6, mm7		; k * z | l * z
	pfadd	mm0, mm1		; ax+ey | bx+fy
	pfadd	mm2, mm3		; cx+gy | dx+hy
	pfadd	mm0, mm4
	pfadd	mm2, mm6
	align	16
    .3:
	movq	[edx], mm0
	movd	[edx + 8], mm2
	add	edx, 16
	dec	ebp
	jnz	.3
	femms
	pop	ebp
	ret
endp


	align	16
f_0010	dd	0.0, 0.0, 1.0, 0.0
f_2222	dd	2.0, 2.0, 2.0, 2.0
f_h000	dd	0.5, 0.0, 0.0, 0.0


	align	16
proc	k3d_reflect
	push	ebp
	mov	ebp, [tnl_vb + TNL_VB_LEN]
	mov	eax, [tnl_vb + TNL_VB_VEYN]
	mov	ecx, [tnl_vb + TNL_VB_NEYE]
	mov	edx, [tnl_vb + TNL_VB_REFL]
	movq	mm6, [f_2222]		;     2     |     2
	align	16
    .0:
	movq	mm4, [eax]		;    ux     |    uy
	movd	mm5, [eax + 8]		;    uz     |     0
	movq	mm2, [ecx]		;    nx     |    ny
	movd	mm3, [ecx + 8]		;    nz     |     0
	movq	mm0, mm4		;    ux     |    uy
	movq	mm1, mm5		;    uz     |     0
	pfmul	mm4, mm2		;   ux*nx   |   uy*ny
	pfmul	mm5, mm3		;   uz*nz   |     0
	pfacc	mm4, mm5		;ux*nx+uy*ny|   uz*nz
	pfacc	mm4, mm4		;    dot    |    dot

	pfmul	mm4, mm6		;  2 * dot  |  2 * dot
	pfmul	mm2, mm4
	pfmul	mm3, mm4
	pfsub	mm0, mm2
	pfsub	mm3, [f_0010 + 8]
	movq	[edx], mm0
	pfsub	mm1, mm3
	movd	[edx + 8], mm1

	add	eax, 16
	add	ecx, 16
	add	edx, 16
	dec	ebp
	jnz	.0
	femms
	pop	ebp
	ret
endp


	align	16
proc	k3d_reflect_mvec
	push	ebp
	mov	ebp, [tnl_vb + TNL_VB_LEN]
	mov	eax, [tnl_vb + TNL_VB_VEYN]
	mov	ecx, [tnl_vb + TNL_VB_NEYE]
	mov	edx, [tnl_vb + TNL_VB_REFL]
	movq	mm6, [f_2222]		;     2     |     2
	movq	mm7, [f_h000]		;    0.5    |     0
	align	16
    .0:
	movq	mm4, [eax]		;    ux     |    uy
	movd	mm5, [eax + 8]		;    uz     |     0
	movq	mm2, [ecx]		;    nx     |    ny
	movd	mm3, [ecx + 8]		;    nz     |     0
	movq	mm0, mm4		;    ux     |    uy
	movq	mm1, mm5		;    uz     |     0
	pfmul	mm4, mm2		;   ux*nx   |   uy*ny
	pfmul	mm5, mm3		;   uz*nz   |     0
	pfacc	mm4, mm5		;ux*nx+uy*ny|   uz*nz
	pfacc	mm4, mm4		;    dot    |    dot

	pfmul	mm4, mm6		;  2 * dot  |  2 * dot
	pfmul	mm2, mm4
	pfmul	mm3, mm4
	pfsub	mm0, mm2
	pfsub	mm3, [f_0010 + 8]
	movq	[edx], mm0
	pfsub	mm1, mm3
    ; XXX Could split MM1 and get rid of MM3 here
	movq	mm3, mm1
	pfmul	mm0, mm0		;    X*X    |    Y*Y
	pfmul	mm1, mm1		;    Z*Z    |     0
	pfacc	mm0, mm1		; X*X + Y*Y |    Z*Z
	pfacc	mm0, mm0		;    dot    |    dot
	pfrsqrt	mm1, mm0
	movq	mm4, mm1
	pfmul	mm1, mm1
	pfrsqit1 mm1, mm0
	pfrcpit2 mm1, mm4
	pfmul	mm1, mm7
	punpckldq mm3, mm1
	movq	[edx + 8], mm3

	add	eax, 16
	add	ecx, 16
	add	edx, 16
	dec	ebp
	jnz	.0
	femms
	pop	ebp
	ret
endp
