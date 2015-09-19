;---------------------------------------
; SSE
;---------------------------------------
%define X			0
%define Y			1
%define Z			2
%define W			3
%define SHUF(_x, _y, _z, _w)	(((_x)<<0) | ((_y)<<2) | ((_z)<<4) | ((_w)<<6))


;---------------------------------------
; CPU
;---------------------------------------
%define _CPU_HAS_CPUID		32768
%define _CPU_FEATURE_MMX	1
%define _CPU_FEATURE_SSE	2
%define _CPU_FEATURE_SSE2	4
%define _CPU_FEATURE_3DNOW	8
%define _CPU_FEATURE_3DNOWPLUS	16
%define _CPU_FEATURE_MMXPLUS	32


;---------------------------------------
; OpenGL
;---------------------------------------
%define GL_TEXTURE0		33984


;---------------------------------------
; Core
;---------------------------------------
%define MAT_MAT			0

%define TEX_UNIT_OBJECT		8
%define sizeof_TEX_UNIT		252
%define TEX_OBJ_DRIVERDATA	1776


;---------------------------------------
; TNL
;---------------------------------------
%define TNL_COLOR0_BIT		2
%define TNL_NORMAL_BIT		8
%define TNL_TEXCOORD0_BIT	16

%define sizeof_TNL_ARRAY	12

%define PRIM_COUNT		4
%define PRIM_ORMASK		12
%define sizeof_TNL_PRIMITIVE	16

%define TNL_VB_NUM		0
%define TNL_VB_MAX		4
%define TNL_VB_LEN		8
%define TNL_VB_FLAGS		12
%define TNL_VB_CLIPMASK		16
%define TNL_VB_VERTEX_STRIDE	20
%define TNL_VB_VERTEX_DATA	24
%define TNL_VB_VERTEX_PTR	28
%define TNL_VB_COLOR0_STRIDE	32
%define TNL_VB_COLOR0_DATA	36
%define TNL_VB_COLOR0_PTR	40
%define TNL_VB_COLOR1_STRIDE	44
%define TNL_VB_COLOR1_DATA	48
%define TNL_VB_COLOR1_PTR	52
%define TNL_VB_NORMAL_STRIDE	56
%define TNL_VB_NORMAL_DATA	60
%define TNL_VB_NORMAL_PTR	64
%define TNL_VB_TEXCOORD0_STRIDE	68
%define TNL_VB_TEXCOORD0_DATA	72
%define TNL_VB_TEXCOORD0_PTR	76
%define TNL_VB_FOGCOORD_STRIDE	284
%define TNL_VB_FOGCOORD_DATA	288
%define TNL_VB_FOGCOORD_PTR	292
%define TNL_VB_CLIP		308
%define TNL_VB_NDC		312
%define TNL_VB_VEYE		396
%define TNL_VB_NEYE		400
%define TNL_VB_VEYN		404
%define TNL_VB_REFL		408

%define qN			1
%define qF			2
%define qT			4
%define qB			8
%define qL			16
%define qR			32
