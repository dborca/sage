%include "xos.inc"

%include "x86.inc"


segment	TEXT


	align	16
proc	cpusoft
	push	ebx
	push	ebp
    ; Initialize
	xor	ebp, ebp
    ; Check for CPUID instruction
	pushfd
	pop	edx
	mov	ecx, edx
	xor	edx, 0x00200000
	push	edx
	popfd
	pushfd
	pop	edx
	push	ecx
	popfd
	xor	edx, ecx
	jz	.done
    ; OK, we have CPUID
	mov	ebp, _CPU_HAS_CPUID
    ; Try CPUID(0) to get signature and max standard level
	xor	eax, eax
	cpuid
	test	eax, eax
	jz	.done
    ; Save signature
	mov	eax, [esp + 12]
	test	eax, eax
	jz	.noSave
	mov	[eax], ebx
	mov	[eax + 4], edx
	mov	[eax + 8], ecx
	mov	byte [eax + 12], 0
    .noSave:
    ; Check if we have extended info
	cmp	ebx, 'Auth'
	jne	.noAMD
	cmp	edx, 'enti'
	jne	.noAMD
	cmp	ecx, 'cAMD'
	jne	.noAMD
    ; Query extended info
	mov	eax, 0x80000000
	cpuid
	cmp	eax, 0x80000001
	jb	.noAMD
	mov	eax, 0x80000001
	cpuid
	test	edx, 0x80000000
	jz	.no3DNow
	or	ebp, _CPU_FEATURE_3DNOW
    .no3DNow:
	test	edx, 0x40000000
	jz	.no3DNowPlus
	or	ebp, _CPU_FEATURE_3DNOWPLUS
    .no3DNowPlus:
	test	edx, 0x00400000
	jz	.noMMXPlus
	or	ebp, _CPU_FEATURE_MMXPLUS
    .noMMXPlus:
    .noAMD:
    ; Query standard info
	mov	eax, 1
	cpuid

	test	edx, 0x00800000
	jz	.noMMX
	or	ebp, _CPU_FEATURE_MMX
    .noMMX:
	test	edx, 0x02000000
	jz	.noSSE
	or	ebp, _CPU_FEATURE_SSE
    .noSSE:
	test	edx, 0x04000000
	jz	.noSSE2
	or	ebp, _CPU_FEATURE_SSE2
    .noSSE2:
    .done:
	mov	eax, ebp
	pop	ebp
	pop	ebx
	ret
endp
