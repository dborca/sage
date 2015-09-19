%include "xos.inc"

%include "x86.inc"


extrn	tnl_vb
extrn	tnl_general_flags
;extrn	tnl_flush, 4


segment	TEXT


	align	16
proc	x86_Vertex3fv, 4
	push	ebx
	mov	ebx, [esp + 8]		; const GLfloat *v
	mov	ecx, [tnl_vb + TNL_VB_NUM]
	mov	edx, ecx
	sal	edx, 4
	add	edx, [tnl_vb + TNL_VB_VERTEX_PTR]
	inc	ecx
	mov	[tnl_vb + TNL_VB_NUM], ecx
	mov	eax, [ebx]
	mov	[edx], eax
	mov	eax, [ebx + 4]
	mov	[edx + 4], eax
	mov	eax, [ebx + 8]
	mov	[edx + 8], eax
	mov	dword [edx + 12], 0x3f800000
	cmp	ecx, [tnl_vb + TNL_VB_MAX]
	je	.0
	pop	ebx
	ret
	align	16
.0:
;	mov	dword [esp + 8], 0	; XXX unused
	pop	ebx
;	jmp	tnl_flush
endp


	align	16
proc	x86_Color3fv, 4
	push	ebx
	mov	ecx, [esp + 8]		; const GLfloat *v
	mov	ebx, [tnl_vb + TNL_VB_NUM]
	mov	edx, ebx
	sal	edx, 4
	add	edx, [tnl_vb + TNL_VB_COLOR0_PTR]
	mov	eax, [ecx]
	mov	[edx], eax
	mov	eax, [ecx + 4]
	mov	[edx + 4], eax
	mov	eax, [ecx + 8]
	mov	[edx + 8], eax
	mov	dword [edx + 12], 0x3f800000
	mov	eax, [tnl_vb + TNL_VB_FLAGS]
	or	dword [eax + ebx * 4], TNL_COLOR0_BIT
	or	dword [tnl_general_flags], TNL_COLOR0_BIT
	pop	ebx
	ret
endp


	align	16
proc	x86_Color4fv, 4
	push	ebx
	mov	ecx, [esp + 8]		; const GLfloat *v
	mov	ebx, [tnl_vb + TNL_VB_NUM]
	mov	edx, ebx
	sal	edx, 4
	add	edx, [tnl_vb + TNL_VB_COLOR0_PTR]
	mov	eax, [ecx]
	mov	[edx], eax
	mov	eax, [ecx + 4]
	mov	[edx + 4], eax
	mov	eax, [ecx + 8]
	mov	[edx + 8], eax
	mov	eax, [ecx + 12]
	mov	[edx + 12], eax
	mov	eax, [tnl_vb + TNL_VB_FLAGS]
	or	dword [eax + ebx * 4], TNL_COLOR0_BIT
	or	dword [tnl_general_flags], TNL_COLOR0_BIT
	pop	ebx
	ret
endp


	align	16
proc	x86_TexCoord2fv, 4
	push	ebx
	mov	ebx, [esp + 8]		; const GLfloat *v
	mov	ecx, [tnl_vb + TNL_VB_NUM]
	mov	eax, ecx
	sal	eax, 4
	add	eax, [tnl_vb + TNL_VB_TEXCOORD0_PTR]
	mov	edx, [ebx]
	mov	[eax], edx
	mov	edx, [ebx + 4]
	mov	[eax + 4], edx
	mov	dword [eax + 8], 0x00000000
	mov	dword [eax + 12], 0x3f800000
	mov	eax, [tnl_vb + TNL_VB_FLAGS]
	or	dword [eax + ecx * 4], TNL_TEXCOORD0_BIT
	or	dword [tnl_general_flags], TNL_TEXCOORD0_BIT
	pop	ebx
	ret
endp


	align	16
proc	x86_MultiTexCoord2fv, 8
	push	esi
	push	ebx
	mov	ecx, [esp + 12]		; GLenum texture
	mov	esi, [esp + 16]		; const GLfloat *v
	mov	ebx, [tnl_vb + TNL_VB_NUM]
	lea	edx, [ecx + ecx * 2]	; EDX = ECX * 3
	mov	eax, ebx
	sal	eax, 4
	add	eax, [tnl_vb + edx * 4 - GL_TEXTURE0 * sizeof_TNL_ARRAY + TNL_VB_TEXCOORD0_PTR]
	mov	edx, [esi]
	mov	[eax], edx
	mov	edx, [esi + 4]
	mov	[eax + 4], edx
	mov	dword [eax + 8], 0x00000000
	mov	dword [eax + 12], 0x3f800000
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
