;---------------------------------------
; platform defines
;---------------------------------------
%define XOS_DJGPP 1
%define XOS_LINUX 2
%define XOS_WIN32 4

%define STDCALL   0
%define ELFTYPE   0

;---------------------------------------
; pick up the right OS
;---------------------------------------
%ifdef __DJGPP__
%define XOS XOS_DJGPP
%elifdef __linux__
%define XOS XOS_LINUX
%define ELFTYPE 1
%elifdef __WIN32__
%define XOS XOS_WIN32
%define STDCALL 1
%else
%error Unknown OS
%endif

;---------------------------------------
; general purpose macros
;---------------------------------------
%macro	extrn	1-2
	%if STDCALL && (%0 > 1)
		%define %1 %1@%2
	%endif
		extern	%1
%endmacro

%macro		proc	1-2
	%push	proc
	%define	%$ret	RET
	%define	%$end	%1_end
	%if STDCALL && (%0 > 1)
		%if (%2 > 0)
			%define	%$ret	RET %2
		%endif
		%define %1 %1@%2
	%endif
	%if ELFTYPE
		global	%1:function (%$end - %1)
	%else
		global	%1
	%endif
%1:
%endmacro

%macro		endp	0
	%ifnctx	proc
		%error	Mismatched ENDP/PROC
	%else
		%$end:
		%pop
	%endif
%endmacro

%macro		ret	0
	%ifnctx	proc
		RET
	%else
		%$ret
	%endif
%endmacro

;---------------------------------------
; Windows
;---------------------------------------
%if XOS == XOS_WIN32

%define TEXT  .text
%define DATA  .data
%define CONST .rdata

%macro	dllexp	1
	export	%1
%endmacro

%endif

;---------------------------------------
; DJGPP
;---------------------------------------
%if XOS == XOS_DJGPP

%define TEXT  .text
%define DATA  .data
%define CONST .rodata

%macro	dllexp	1
%endmacro

%endif

;---------------------------------------
; Linux
;---------------------------------------
%if XOS == XOS_LINUX

%define TEXT  .text
%define DATA  .data
%define CONST .rodata

%macro	dllexp	1
%endmacro

%endif
