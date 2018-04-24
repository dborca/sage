	align	16
proc	TAG(sse_emitvertices)
	push	ebx
	push	esi
	push	edi
	push	ebp
	mov	ebp, [esp + 20]		; n
	mov	edi, [vb]		; vertex

	push	ebp
	push	edi

    ; Viewport transformation
	mov	ebx, [tnl_vb + TNL_VB_CLIPMASK]
	mov	esi, [tnl_vb + TNL_VB_NDC]
	movaps	xmm7, [i_xxx0]
	movss	xmm2, [ctx_mx_viewport + MAT_MAT + 00 * 4]
	movss	xmm1, [ctx_mx_viewport + MAT_MAT + 05 * 4]
	movss	xmm3, [ctx_mx_viewport + MAT_MAT + 10 * 4]
	movaps	xmm5, [f_0010]
	shufps	xmm2, xmm1, SHUF(X, Y, X, Y)
	orps	xmm3, xmm5
	movups	xmm4, [ctx_mx_viewport + MAT_MAT + 12 * 4]
	shufps	xmm2, xmm3, SHUF(X, Z, X, Z)
	andps	xmm4, xmm7
	align	16
    .0:
	mov	al, [ebx]
	mov	dword [edi + GR_VERTEX_OOW_OFFSET], 0x3f800000
	test	al, al
	jnz	.1
	movaps	xmm0, [esi]
	mulps	xmm0, xmm2
	addps	xmm0, xmm4
	movaps	[edi + GR_VERTEX_X_OFFSET], xmm0
	align	16
    .1:
	inc	ebx
	add	esi, 16
	add	edi, sizeof_GrVertex
	dec	ebp
	jnz	.0

	pop	edi
	pop	ebp
	push	ebp
	push	edi

    ; Color0
	mov	esi, [tnl_vb + TNL_VB_COLOR0_DATA]
	mov	ebx, [tnl_vb + TNL_VB_COLOR0_STRIDE]
	xorps	xmm6, xmm6
	movaps	xmm7, [f_bbbb]
	shl	ebx, 4
	align	16
    .2:
	movaps	xmm0, [esi]
	mulps	xmm0, xmm7
%if FX_PACKEDCOLOR
	shufps	xmm0, xmm0, SHUF(Z, Y, X, W)
	cvtps2pi mm0, xmm0
	movhlps	xmm0, xmm0
	cvtps2pi mm1, xmm0
	packssdw mm0, mm1
	packuswb mm0, mm0
	movd	eax, mm0		; b g r a
	mov	[edi + GR_VERTEX_PARGB_OFFSET], eax
%else  ; !FX_PACKEDCOLOR
	minps	xmm0, xmm7
	maxps	xmm0, xmm6
	movaps	[edi + GR_VERTEX_RGB_OFFSET], xmm0
%endif ; !FX_PACKEDCOLOR
	add	esi, ebx
	add	edi, sizeof_GrVertex
	dec	ebp
	jnz	.2

%if (IND & SETUP_TEX0)
	pop	edi
	pop	ebp
	push	ebp
	push	edi

    ; Texture0
	mov	edx, [tmu0_source]
	mov	eax, edx
	imul	eax, sizeof_TNL_ARRAY
	mov	esi, [tnl_vb + eax + TNL_VB_TEXCOORD0_DATA]
	mov	ebx, [tnl_vb + eax + TNL_VB_TEXCOORD0_STRIDE]
	xorps	xmm7, xmm7
	shl	ebx, 4
	mov	eax, edx
	imul	eax, sizeof_TEX_UNIT
	mov	eax, [ctx_texture + eax + TEX_UNIT_OBJECT]
	mov	eax, [eax + TEX_OBJ_DRIVERDATA]
	test	eax, eax
	jz	.3
	shufps	xmm7, xmm5, SHUF(Z, Z, Z, Z)
	movlps	xmm7, [eax + TFX_OBJ_SSCALE]
	align	16
    .3:
	movss	xmm6, [edi + GR_VERTEX_OOW_OFFSET]
	movaps	xmm0, [esi]
	shufps	xmm6, xmm6, SHUF(X, X, X, X)
	mulps	xmm0, xmm7
	mulps	xmm0, xmm6
	movlps	[edi + GR_VERTEX_SOW_TMU0_OFFSET], xmm0
%if (IND & SETUP_PTX0)
	shufps	xmm0, xmm0, SHUF(W, W, W, W)
	movss	[edi + GR_VERTEX_OOW_TMU0_OFFSET], xmm0
%endif
	add	esi, ebx
	add	edi, sizeof_GrVertex
	dec	ebp
	jnz	.3
%endif

%if (IND & SETUP_TEX1)
	pop	edi
	pop	ebp
	push	ebp
	push	edi

    ; Texture1
	mov	edx, [tmu1_source]
	mov	eax, edx
	imul	eax, sizeof_TNL_ARRAY
	mov	esi, [tnl_vb + eax + TNL_VB_TEXCOORD0_DATA]
	mov	ebx, [tnl_vb + eax + TNL_VB_TEXCOORD0_STRIDE]
	xorps	xmm7, xmm7
	shl	ebx, 4
	mov	eax, edx
	imul	eax, sizeof_TEX_UNIT
	mov	eax, [ctx_texture + eax + TEX_UNIT_OBJECT]
	mov	eax, [eax + TEX_OBJ_DRIVERDATA]
	test	eax, eax
	jz	.4
	shufps	xmm7, xmm5, SHUF(Z, Z, Z, Z)
	movlps	xmm7, [eax + TFX_OBJ_SSCALE]
	align	16
    .4:
	movss	xmm6, [edi + GR_VERTEX_OOW_OFFSET]
	movaps	xmm0, [esi]
	shufps	xmm6, xmm6, SHUF(X, X, X, X)
	mulps	xmm0, xmm7
	mulps	xmm0, xmm6
	movlps	[edi + GR_VERTEX_SOW_TMU1_OFFSET], xmm0
%if (IND & SETUP_PTX1)
	shufps	xmm0, xmm0, SHUF(W, W, W, W)
	movss	[edi + GR_VERTEX_OOW_TMU1_OFFSET], xmm0
%endif
%if ((IND & (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1)) == (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1))
	; tex0 projected + tex1 unprojected = emit fake q1
	movss	[edi + GR_VERTEX_OOW_TMU1_OFFSET], xmm6
%endif
	add	esi, ebx
	add	edi, sizeof_GrVertex
	dec	ebp
	jnz	.4
%endif

%if (IND & SETUP_SPEC)
    ; Color1
	mov	esi, [tnl_vb + TNL_VB_COLOR1_DATA]
	mov	ebx, [tnl_vb + TNL_VB_COLOR1_STRIDE]
	xorps	xmm6, xmm6
	movaps	xmm7, [f_bbbb]
	shl	ebx, 4
	align	16
    .5:
	movaps	xmm0, [esi]
	mulps	xmm0, xmm7
%if FX_PACKEDCOLOR
	shufps	xmm0, xmm0, SHUF(Z, Y, X, W)
	cvtps2pi mm0, xmm0
	movhlps	xmm0, xmm0
	cvtps2pi mm1, xmm0
	packssdw mm0, mm1
	packuswb mm0, mm0
	movd	eax, mm0		; b g r a
	mov	[edi + GR_VERTEX_PSPEC_OFFSET], eax
%else  ; !FX_PACKEDCOLOR
	minps	xmm0, xmm7
	maxps	xmm0, xmm6
	movaps	[edi + GR_VERTEX_SPEC_OFFSET], xmm0
%endif ; !FX_PACKEDCOLOR
	add	esi, ebx
	add	edi, sizeof_GrVertex
	dec	ebp
	jnz	.5
%endif

%if (IND & SETUP_FOGC)
    ; Fog
	mov	esi, [tnl_vb + TNL_VB_FOGCOORD_DATA]
	mov	ebx, [tnl_vb + TNL_VB_FOGCOORD_STRIDE]
	shl	ebx, 4
	align	16
    .6:
	mov	eax, [esi]
	mov	[edi + GR_VERTEX_FOG_OFFSET], eax
	add	esi, ebx
	add	edi, sizeof_GrVertex
	dec	ebp
	jnz	.6
%endif

	pop	edi
	pop	ebp

%if FX_PACKEDCOLOR
	emms
%endif
	pop	ebp
	pop	edi
	pop	esi
	pop	ebx
	ret
endp


%undef TAG
%undef IND
