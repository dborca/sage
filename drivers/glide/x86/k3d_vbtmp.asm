	align	16
proc	TAG(k3d_emitvertices)
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
	movd	mm2, [ctx_mx_viewport + MAT_MAT + 00 * 4]
	punpckldq mm2, [ctx_mx_viewport + MAT_MAT + 05 * 4]
	movd	mm3, [ctx_mx_viewport + MAT_MAT + 10 * 4]
	movq	mm4, [ctx_mx_viewport + MAT_MAT + 12 * 4]
	movd	mm5, [ctx_mx_viewport + MAT_MAT + 14 * 4]
	align	16
    .0:
	mov	al, [ebx]
	mov	edx, 0x3f800000
	test	al, al
	jnz	.1
	movq	mm0, [esi]		;    x     |   y
	movd	mm1, [esi + 8]		;    z     |   0
	mov	edx, [esi + 12]
	pfmul	mm0, mm2		;  x*v00   |  y*v05
	pfmul	mm1, mm3		;  z*v10   |   0
	pfadd	mm0, mm4		; x*v00+v12|y*v05+v13
	pfadd	mm1, mm5		; z*v10+v14|   0
	movq	[edi + GR_VERTEX_X_OFFSET], mm0
	movd	[edi + GR_VERTEX_OOZ_OFFSET], mm1
	align	16
    .1:
	mov	[edi + GR_VERTEX_OOW_OFFSET], edx
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
	pxor	mm6, mm6
	movq	mm7, [f_bbbb]
	shl	ebx, 4
	align	16
    .2:
	movq	mm0, [esi]
	movq	mm1, [esi + 8]
	pfmul	mm0, mm7
	pfmul	mm1, mm7
%if FX_PACKEDCOLOR
	pf2id	mm0, mm0		; r 0 0 0 g 0 0 0	(pf2iw)
	pf2id	mm1, mm1		; b 0 0 0 a 0 0 0	(pf2iw)
	packssdw mm0, mm1		; r 0 g 0 b 0 a 0
	packuswb mm0, mm0		; r g b a r g b a
	movd	eax, mm0		; r g b a
	bswap	eax			; a b g r
	ror	eax, 8			; b g r a
	mov	[edi + GR_VERTEX_PARGB_OFFSET], eax
%else  ; !FX_PACKEDCOLOR
	pfmin	mm0, mm7
	pfmin	mm1, mm7
	pfmax	mm0, mm6
	pfmax	mm1, mm6
	movq	[edi + GR_VERTEX_RGB_OFFSET], mm0
	movq	[edi + GR_VERTEX_RGB_OFFSET + 8], mm1
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
	pxor	mm7, mm7
	shl	ebx, 4
	mov	eax, edx
	imul	eax, sizeof_TEX_UNIT
	mov	eax, [ctx_texture + eax + TEX_UNIT_OBJECT]
	mov	eax, [eax + TEX_OBJ_DRIVERDATA]
	test	eax, eax
	jz	.3
	movq	mm7, [eax + TFX_OBJ_SSCALE]
	align	16
    .3:
	movd	mm6, [edi + GR_VERTEX_OOW_OFFSET]
	movq	mm0, [esi]
%if (IND & SETUP_PTX0)
	movd	mm1, [esi + 12]
	punpckldq mm6, mm6
	pfmul	mm0, mm7
	pfmul	mm0, mm6
	pfmul	mm1, mm6
	movq	[edi + GR_VERTEX_SOW_TMU0_OFFSET], mm0
	movd	[edi + GR_VERTEX_OOW_TMU0_OFFSET], mm1
%else
	punpckldq mm6, mm6
	pfmul	mm0, mm7
	pfmul	mm0, mm6
	movq	[edi + GR_VERTEX_SOW_TMU0_OFFSET], mm0
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
	pxor	mm7, mm7
	shl	ebx, 4
	mov	eax, edx
	imul	eax, sizeof_TEX_UNIT
	mov	eax, [ctx_texture + eax + TEX_UNIT_OBJECT]
	mov	eax, [eax + TEX_OBJ_DRIVERDATA]
	test	eax, eax
	jz	.4
	movq	mm7, [eax + TFX_OBJ_SSCALE]
	align	16
    .4:
	movd	mm6, [edi + GR_VERTEX_OOW_OFFSET]
	movq	mm0, [esi]
%if (IND & SETUP_PTX1)
	movd	mm1, [esi + 12]
	punpckldq mm6, mm6
	pfmul	mm0, mm7
	pfmul	mm0, mm6
	pfmul	mm1, mm6
	movq	[edi + GR_VERTEX_SOW_TMU1_OFFSET], mm0
	movd	[edi + GR_VERTEX_OOW_TMU1_OFFSET], mm1
%else
	punpckldq mm6, mm6
	pfmul	mm0, mm7
	pfmul	mm0, mm6
	movq	[edi + GR_VERTEX_SOW_TMU1_OFFSET], mm0
%endif
%if ((IND & (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1|SETUP_PTX1)) == (SETUP_TEX0|SETUP_PTX0|SETUP_TEX1))
	; tex0 projected + tex1 unprojected = emit fake q1
	movd	[edi + GR_VERTEX_OOW_TMU1_OFFSET], mm6
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
	pxor	mm6, mm6
	movq	mm7, [f_bbbb]
	shl	ebx, 4
	align	16
    .5:
	movq	mm0, [esi]
	movq	mm1, [esi + 8]
	pfmul	mm0, mm7
	pfmul	mm1, mm7
%if FX_PACKEDCOLOR
	pf2id	mm0, mm0		; r 0 0 0 g 0 0 0	(pf2iw)
	pf2id	mm1, mm1		; b 0 0 0 a 0 0 0	(pf2iw)
	packssdw mm0, mm1		; r 0 g 0 b 0 a 0
	packuswb mm0, mm0		; r g b a r g b a
	movd	eax, mm0		; r g b a
	bswap	eax			; a b g r
	ror	eax, 8			; b g r a
	mov	[edi + GR_VERTEX_PSPEC_OFFSET], eax
%else  ; !FX_PACKEDCOLOR
	pfmin	mm0, mm7
	pfmin	mm1, mm7
	pfmax	mm0, mm6
	pfmax	mm1, mm6
	movq	[edi + GR_VERTEX_SPEC_OFFSET], mm0
	movd	[edi + GR_VERTEX_SPEC_OFFSET + 8], mm1
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

	femms
	pop	ebp
	pop	edi
	pop	esi
	pop	ebx
	ret
endp


%undef TAG
%undef IND
