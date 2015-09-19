%include "xos.inc"

%include "x86.inc"


extrn	tnl_vb
extrn	tnl_general_flags
;extrn	tnl_flush, 4


segment	TEXT


	align	16
f_0001	dd	0.0, 0.0, 0.0, 1.0


	align	16
proc	sse_Vertex3fv, 4
	mov	eax, [esp + 4]		; const GLfloat *v
	mov	ecx, [tnl_vb + TNL_VB_NUM]
	mov	edx, ecx
	sal	edx, 4
	add	edx, [tnl_vb + TNL_VB_VERTEX_PTR]
	inc	ecx
	mov	[tnl_vb + TNL_VB_NUM], ecx
	movss	xmm0, [eax + 8]
	shufps	xmm0, [f_0001], SHUF(X, Y, Z, W)
	shufps	xmm0, xmm0, SHUF(Y, Z, X, W)
	movlps	xmm0, [eax]
	movaps	[edx], xmm0
	cmp	ecx, [tnl_vb + TNL_VB_MAX]
	je	.0
	ret
	align	16
.0:
;	mov	dword [esp + 8], 0	; XXX unused
;	jmp	tnl_flush
endp


	align	16
proc	sse_Color3fv, 4
	mov	edx, [esp + 4]		; const GLfloat *v
	mov	ecx, [tnl_vb + TNL_VB_NUM]
	mov	eax, ecx
	sal	eax, 4
	add	eax, [tnl_vb + TNL_VB_COLOR0_PTR]
	movss	xmm0, [edx + 8]
	shufps	xmm0, [f_0001], SHUF(X, Y, Z, W)
	shufps	xmm0, xmm0, SHUF(Y, Z, X, W)
	movlps	xmm0, [edx]
	movaps	[eax], xmm0
	mov	eax, [tnl_vb + TNL_VB_FLAGS]
	or	dword [eax + ecx * 4], TNL_COLOR0_BIT
	or	dword [tnl_general_flags], TNL_COLOR0_BIT
	ret
endp


	align	16
proc	sse_Color4fv, 4
	mov	edx, [esp + 4]		; const GLfloat *v
	mov	ecx, [tnl_vb + TNL_VB_NUM]
	mov	eax, ecx
	sal	eax, 4
	add	eax, [tnl_vb + TNL_VB_COLOR0_PTR]
	movups	xmm0, [edx]
	movaps	[eax], xmm0
	mov	eax, [tnl_vb + TNL_VB_FLAGS]
	or	dword [eax + ecx * 4], TNL_COLOR0_BIT
	or	dword [tnl_general_flags], TNL_COLOR0_BIT
	ret
endp


	align	16
proc	sse_TexCoord2fv, 4
	mov	edx, [esp + 4]		; const GLfloat *v
	mov	ecx, [tnl_vb + TNL_VB_NUM]
	mov	eax, ecx
	sal	eax, 4
	add	eax, [tnl_vb + TNL_VB_TEXCOORD0_PTR]
	movaps	xmm0, [f_0001]
	movlps	xmm0, [edx]
	movaps	[eax], xmm0
	mov	eax, [tnl_vb + TNL_VB_FLAGS]
	or	dword [eax + ecx * 4], TNL_TEXCOORD0_BIT
	or	dword [tnl_general_flags], TNL_TEXCOORD0_BIT
	ret
endp


	align	16
proc	sse_MultiTexCoord2fv, 8
	push	esi
	push	ebx
	mov	ecx, [esp + 12]		; GLenum texture
	mov	esi, [esp + 16]		; const GLfloat *v
	mov	ebx, [tnl_vb + TNL_VB_NUM]
	lea	edx, [ecx + ecx * 2]	; EDX = ECX * 3
	mov	eax, ebx
	sal	eax, 4
	add	eax, [tnl_vb + edx * 4 - GL_TEXTURE0 * sizeof_TNL_ARRAY + TNL_VB_TEXCOORD0_PTR]
	movaps	xmm0, [f_0001]
	movlps	xmm0, [esi]
	movaps	[eax], xmm0
	mov	edx, [tnl_vb + TNL_VB_FLAGS]
	sub	ecx, GL_TEXTURE0
	mov	eax, TNL_TEXCOORD0_BIT
	sal	eax, cl
	or	[edx + ebx * 4], eax
	or	[tnl_general_flags], eax
	pop	ebx
	pop	esi
	ret
endp
