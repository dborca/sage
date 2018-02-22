#define WIN32_LEAN_AND_MEAN
#define _GDI32_ /* so that __declspec(dllimport) is disabled: */
                /* this file must not use any gdi.h functions. */
#undef __W32API_USE_DLLIMPORT /* non-standart mingw.org thing */
#include <windows.h>

#include "GL/gl.h"
#include "GL/sage.h"

#include "glinternal.h"
#include "main/context.h"
#include "util/cfg.h"


/*#include <stdio.h>
#define WINLOG(x)				\
    do {					\
	FILE *eff = fopen("sage.log", "at");	\
	fprintf x;				\
	fclose(eff);				\
    } while (0)*/
#define WINLOG(x)


#define YES(v) !strcmp(cfg_get(v, "n"), "y")


static int pixelFormat = 0;
static HGLRC ctx = NULL;
static int interval = 0;


PIXELFORMATDESCRIPTOR pfd[] = {
{
    sizeof(PIXELFORMATDESCRIPTOR),  /*  size of this pfd */
    1,                     /* version number */
    PFD_DRAW_TO_WINDOW |   /* support window */
    PFD_SUPPORT_OPENGL |   /* support OpenGL */
    PFD_DOUBLEBUFFER,      /* double buffered */
    PFD_TYPE_RGBA,         /* RGBA type */
    16,                    /* color depth */
    5, 0, 6, 5, 5, 11,     /* color bits */
    0,                     /* alpha buffer */
    0,                     /* shift bit */
    0,                     /* no accumulation buffer */
    0, 0, 0, 0,            /* accum bits ignored */
    16,                    /* z-buffer */
    0,                     /* stencil buffer */
    0,                     /* no auxiliary buffer */
    PFD_MAIN_PLANE,        /* main layer */
    0,                     /* reserved */
    0, 0, 0                /* layer masks ignored */
},
{ 
    sizeof(PIXELFORMATDESCRIPTOR),  /*  size of this pfd */
    1,                     /* version number */
    PFD_DRAW_TO_WINDOW |   /* support window */
    PFD_SUPPORT_OPENGL |   /* support OpenGL */
    PFD_DOUBLEBUFFER,      /* double buffered */
    PFD_TYPE_RGBA,         /* RGBA type */
    32,                    /* color depth */
    8, 0, 8, 8, 8, 16,     /* color bits */
    8,                     /* alpha buffer */
    24,                    /* shift bit */
    0,                     /* no accumulation buffer */
    0, 0, 0, 0,            /* accum bits ignored */
    24,                    /* z-buffer */
    8,                     /* stencil buffer */
    0,                     /* no auxiliary buffer */
    PFD_MAIN_PLANE,        /* main layer */
    0,                     /* reserved */
    0, 0, 0                /* layer masks ignored */
}
};


static int
pfd_len (void)
{
    int colordepth = sage_init();
    if (colordepth <= 0) {
	return 0;
    } else if (colordepth == 16) {
	return 1;
    } else {
	return (sizeof(pfd) / sizeof(pfd[0]));
    }
}


static const PIXELFORMATDESCRIPTOR *
pfd_format (int i)
{
    return &pfd[i];
}


/* Extensions *****************************************************************/


GLAPI const char * GLAPIENTRY
wglGetExtensionsStringEXT (void)
{
   return /*"WGL_3DFX_gamma_control "*/
	  "WGL_EXT_swap_control "
	  "WGL_EXT_extensions_string WGL_ARB_extensions_string";
}


GLAPI const char * GLAPIENTRY
wglGetExtensionsStringARB (HDC hdc)
{
   return wglGetExtensionsStringEXT();
}


GLAPI BOOL GLAPIENTRY
wglSwapIntervalEXT (int i)
{
    if (i > 0) {
	interval = i;
	return TRUE;
    }
    return FALSE;
}


static struct {
    const char *name;
    PROC proc;
} functab[] = {
#define ALIAS(x, y) \
    { "gl" #x #y, (PROC)gl##x },
#include "../../main/alias.h"
    { "glLockArraysEXT", (PROC)glLockArraysEXT },
    { "glUnlockArraysEXT", (PROC)glUnlockArraysEXT },
    { "glPolygonOffsetEXT", (PROC)glPolygonOffsetEXT },
    { "wglGetExtensionsStringEXT", (PROC)wglGetExtensionsStringEXT },
    { "wglGetExtensionsStringARB", (PROC)wglGetExtensionsStringARB },
    { "wglSwapIntervalEXT", (PROC)wglSwapIntervalEXT }
};


/* GDI functions **************************************************************/


GLAPI int GLAPIENTRY
wglChoosePixelFormat (HDC hdc, CONST PIXELFORMATDESCRIPTOR *ppfd)
{
    int i, numPixelFormats, bestIndex = -1;
    const PIXELFORMATDESCRIPTOR *ppfdBest = NULL;

    WINLOG((eff, "%s:\n", __FUNCTION__));

    if (ppfd->dwFlags != (ppfd->dwFlags & 
				    (
				    PFD_DRAW_TO_WINDOW |
				    PFD_DRAW_TO_BITMAP |
				    PFD_SUPPORT_GDI |
				    PFD_SUPPORT_OPENGL |
				    PFD_GENERIC_FORMAT |
				    PFD_NEED_PALETTE |
				    PFD_NEED_SYSTEM_PALETTE |
				    PFD_DOUBLEBUFFER |
				    PFD_STEREO |
				    /*PFD_SWAP_LAYER_BUFFERS |*/
				    PFD_DOUBLEBUFFER_DONTCARE |
				    PFD_STEREO_DONTCARE |
				    PFD_SWAP_COPY |
				    PFD_SWAP_EXCHANGE |
				    0)))
    {
	/* error: bad dwFlags */
	return 0;
    }

    switch (ppfd->iPixelType) {
	case PFD_TYPE_RGBA:
	case PFD_TYPE_COLORINDEX:
	    break;
	default:
	    /* error: bad iPixelType */
	    return 0;
    }

    switch (ppfd->iLayerType) {
	case PFD_MAIN_PLANE:
	case PFD_OVERLAY_PLANE:
	case PFD_UNDERLAY_PLANE:
	    break;
	default:
	    /* error: bad iLayerType */
	    return 0;
    }

    numPixelFormats = pfd_len();

    /* loop through candidate pixel format descriptors */
    for (i = 0; i < numPixelFormats; ++i) {
	const PIXELFORMATDESCRIPTOR *ppfdCandidate;

	ppfdCandidate = pfd_format(i);

	/*
	** Check attributes which must match
	*/
	if (ppfd->iPixelType != ppfdCandidate->iPixelType) {
	    continue;
	}

	if (ppfd->iLayerType != ppfdCandidate->iLayerType) {
	    continue;
	}

	if (((ppfd->dwFlags ^ ppfdCandidate->dwFlags) & ppfd->dwFlags) &
	    (PFD_DRAW_TO_WINDOW | PFD_DRAW_TO_BITMAP |
		PFD_SUPPORT_GDI | PFD_SUPPORT_OPENGL)) {
	    continue;
	}

	if (!(ppfd->dwFlags & PFD_DOUBLEBUFFER_DONTCARE)) {
	    if ((ppfd->dwFlags & PFD_DOUBLEBUFFER) !=
		(ppfdCandidate->dwFlags & PFD_DOUBLEBUFFER)) {
		continue;
	    }
	}

	if (!(ppfd->dwFlags & PFD_STEREO_DONTCARE)) {
	    if ((ppfd->dwFlags & PFD_STEREO) !=
		(ppfdCandidate->dwFlags & PFD_STEREO)) {
		if (!YES("wgl.ignore.stereo")) continue;
	    }
	}

	if (ppfd->iPixelType==PFD_TYPE_RGBA
	    && ppfd->cAlphaBits && !ppfdCandidate->cAlphaBits) {
	    if (!YES("wgl.ignore.alphabuffer")) continue;
	}

	if (ppfd->iPixelType==PFD_TYPE_RGBA
	    && ppfd->cAccumBits && !ppfdCandidate->cAccumBits) {
	    continue;
	}

	if (ppfd->cDepthBits && !ppfdCandidate->cDepthBits) {
	    continue;
	}

	if (ppfd->cStencilBits && !ppfdCandidate->cStencilBits) {
	    if (!YES("wgl.ignore.stencilbuffer")) continue;
	}

	if (ppfd->cAuxBuffers && !ppfdCandidate->cAuxBuffers) {
	    continue;
	}

	/*
	** See if candidate is better than the previous best choice
	*/
	if (bestIndex == -1) {
	    ppfdBest = ppfdCandidate;
	    bestIndex = i;
	    continue;
	}

	if ((ppfd->cColorBits > ppfdBest->cColorBits &&
		ppfdCandidate->cColorBits > ppfdBest->cColorBits) ||
	    (ppfd->cColorBits <= ppfdCandidate->cColorBits &&
		ppfdCandidate->cColorBits < ppfdBest->cColorBits)) {
	    ppfdBest = ppfdCandidate;
	    bestIndex = i;
	    continue;
	}

	if (ppfd->iPixelType==PFD_TYPE_RGBA
	    && ppfd->cAlphaBits
	    && ppfdCandidate->cAlphaBits > ppfdBest->cAlphaBits) {
	    ppfdBest = ppfdCandidate;
	    bestIndex = i;
	    continue;
	}

	if (ppfd->iPixelType==PFD_TYPE_RGBA
	    && ppfd->cAccumBits
	    && ppfdCandidate->cAccumBits > ppfdBest->cAccumBits) {
	    ppfdBest = ppfdCandidate;
	    bestIndex = i;
	    continue;
	}

	if ((ppfd->cDepthBits > ppfdBest->cDepthBits &&
		ppfdCandidate->cDepthBits > ppfdBest->cDepthBits) ||
	    (ppfd->cDepthBits <= ppfdCandidate->cDepthBits &&
		ppfdCandidate->cDepthBits < ppfdBest->cDepthBits)) {
	    ppfdBest = ppfdCandidate;
	    bestIndex = i;
	    continue;
	}

	if (ppfd->cStencilBits &&
		ppfdCandidate->cStencilBits > ppfdBest->cStencilBits) {
	    ppfdBest = ppfdCandidate;
	    bestIndex = i;
	    continue;
	}

	if (ppfd->cAuxBuffers &&
		ppfdCandidate->cAuxBuffers > ppfdBest->cAuxBuffers) {
	    ppfdBest = ppfdCandidate;
	    bestIndex = i;
	    continue;
	}
    }

    return bestIndex + 1;
}


GLAPI int GLAPIENTRY
wglDescribePixelFormat (HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd)
{
    PIXELFORMATDESCRIPTOR pfd;
    int numFormats;

    WINLOG((eff, "%s(%d, %d, %p)\n", __FUNCTION__, iPixelFormat, nBytes, (void *)ppfd));

    if (nBytes > sizeof(PIXELFORMATDESCRIPTOR)) {
	nBytes = sizeof(PIXELFORMATDESCRIPTOR);
    }

    numFormats = pfd_len();

    if (ppfd) {
	if (iPixelFormat < 1 || iPixelFormat > numFormats) {
	    return 0;
	}
	if (numFormats) {
	    if (nBytes) {
		memcpy(ppfd, pfd_format(iPixelFormat - 1), nBytes);
	    }
	}
    }

    return numFormats;
}


GLAPI int GLAPIENTRY
wglGetPixelFormat (HDC hdc)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return pixelFormat;
}


GLAPI BOOL GLAPIENTRY
wglSetPixelFormat (HDC hdc, int iPixelFormat, CONST PIXELFORMATDESCRIPTOR *ppfd)
{
    int numPixelFormats;
    PIXELFORMATDESCRIPTOR pfd;

    WINLOG((eff, "%s(%d, %p)\n", __FUNCTION__, iPixelFormat, (void *)ppfd));

    /* It's an error to set the pixel format more than once */
    if (pixelFormat != 0) {
	return FALSE;
    }

    numPixelFormats = wglDescribePixelFormat(hdc, iPixelFormat,
					     sizeof(PIXELFORMATDESCRIPTOR),
					     &pfd);
    if (numPixelFormats == 0) {
	return FALSE;
    }

    pixelFormat = iPixelFormat;
    return TRUE;
}


GLAPI BOOL GLAPIENTRY
wglSwapBuffers (HDC hdc)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    sage_swap(interval);
    return TRUE;
}


GLAPI int GLAPIENTRY
ChoosePixelFormat (HDC hdc, CONST PIXELFORMATDESCRIPTOR *ppfd)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglChoosePixelFormat(hdc, ppfd);
}


GLAPI int GLAPIENTRY
DescribePixelFormat (HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglDescribePixelFormat(hdc, iPixelFormat, nBytes, ppfd);
}


GLAPI int GLAPIENTRY
GetPixelFormat (HDC hdc)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglGetPixelFormat(hdc);
}


GLAPI BOOL GLAPIENTRY
SetPixelFormat (HDC hdc, int iPixelFormat, CONST PIXELFORMATDESCRIPTOR *ppfd)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglSetPixelFormat(hdc, iPixelFormat, ppfd);
}


GLAPI BOOL GLAPIENTRY
SwapBuffers (HDC hdc)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglSwapBuffers(hdc);
}


/* WGL functions **************************************************************/


GLAPI BOOL GLAPIENTRY
wglCopyContext (HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return FALSE;
}


GLAPI HGLRC GLAPIENTRY
wglCreateContext (HDC hdc)
{
    const PIXELFORMATDESCRIPTOR *ppfd;

    WINLOG((eff, "%s:\n", __FUNCTION__));

    if (pixelFormat == 0) {
	return NULL;
    }
    ppfd = pfd_format(pixelFormat - 1);

    return (HGLRC)sage_open(ppfd->dwFlags & PFD_DOUBLEBUFFER,
			    ppfd->cRedBits, ppfd->cGreenBits, ppfd->cBlueBits,
			    ppfd->cAlphaBits,
			    ppfd->cDepthBits, ppfd->cStencilBits);
}


GLAPI HGLRC GLAPIENTRY
wglCreateLayerContext (HDC hdc, int iLayerPlane)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return NULL;
}


GLAPI BOOL GLAPIENTRY
wglDeleteContext (HGLRC hglrc)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));

    sage_bind(NULL, NULL, 0, 0);
    sage_shut((sageContext *)hglrc);
    sage_fini();

    pixelFormat = 0;
    return TRUE;
}


GLAPI BOOL GLAPIENTRY
wglDescribeLayerPlane (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LPLAYERPLANEDESCRIPTOR plpd)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return FALSE;
}


GLAPI HGLRC GLAPIENTRY
wglGetCurrentContext (VOID)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return ctx;
}


GLAPI HDC GLAPIENTRY
wglGetCurrentDC (VOID)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return NULL;
}


GLAPI int GLAPIENTRY
wglGetLayerPaletteEntries (HDC hdc, int iLayerPlane, int iStart, int cEntries, /*CONST*/ COLORREF *pcr)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return 0;
}


GLAPI PROC GLAPIENTRY
wglGetProcAddress (LPCSTR lpszProc)
{
    int i, n = sizeof(functab) / sizeof(functab[0]);
    WINLOG((eff, "%s(%s)\n", __FUNCTION__, lpszProc));
    for (i = 0; i < n; i++) {
	if (!strcmp(lpszProc, functab[i].name)) {
	    return functab[i].proc;
	}
    }
    return NULL;
}


GLAPI BOOL GLAPIENTRY
wglMakeCurrent (HDC hdc, HGLRC hglrc)
{
    HWND hWnd = NULL;
    int width = 0, height = 0;

    WINLOG((eff, "%s(%p, %p)\n", __FUNCTION__, (void *)hdc, (void *)hglrc));

    if (hglrc != NULL) {
	RECT cliRect;

	if (!(hWnd = WindowFromDC(hdc))) {
	    return FALSE;
	}

	ShowWindow(hWnd, SW_SHOWNORMAL);
	SetForegroundWindow(hWnd);
	GetClientRect(hWnd, &cliRect);

	width = cliRect.right;
	height = cliRect.bottom;
    }

    if (sage_bind((sageContext *)hglrc, hWnd, width, height) == 0) {
	ctx = hglrc;
	return TRUE;
    }
    return FALSE;
}


GLAPI BOOL GLAPIENTRY
wglRealizeLayerPalette (HDC hdc, int iLayerPlane, BOOL bRealize)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return FALSE;
}


GLAPI int GLAPIENTRY
wglSetLayerPaletteEntries (HDC hdc, int iLayerPlane, int iStart, int cEntries, CONST COLORREF *pcr)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return 0;
}


GLAPI BOOL GLAPIENTRY
wglShareLists (HGLRC hglrc1, HGLRC hglrc2)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return FALSE;
}


GLAPI BOOL GLAPIENTRY
wglSwapLayerBuffers (HDC hdc, UINT fuPlanes)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglSwapBuffers(hdc);
}


GLAPI BOOL GLAPIENTRY
wglUseFontBitmapsA (HDC hdc, DWORD first, DWORD count, DWORD listBase)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return FALSE;
}


GLAPI BOOL GLAPIENTRY
wglUseFontBitmapsW (HDC hdc, DWORD first, DWORD count, DWORD listBase)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return FALSE;
}


GLAPI BOOL GLAPIENTRY
wglUseFontOutlinesA (HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return FALSE;
}


GLAPI BOOL GLAPIENTRY
wglUseFontOutlinesW (HDC hdc, DWORD first, DWORD count, DWORD listBase, FLOAT deviation, FLOAT extrusion, int format, LPGLYPHMETRICSFLOAT lpgmf)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return FALSE;
}


/* ICD functions **************************************************************/


#if 0
typedef struct _icdTable {
    DWORD size;
    PROC table[336];
} ICDTABLE, *PICDTABLE;

static ICDTABLE icdTable = { 336, {
} };


GLAPI BOOL GLAPIENTRY
DrvCopyContext (HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglCopyContext(hglrcSrc, hglrcDst, mask);
}


GLAPI HGLRC GLAPIENTRY
DrvCreateContext (HDC hdc)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglCreateContext(hdc);
}


GLAPI BOOL GLAPIENTRY
DrvDeleteContext (HGLRC hglrc)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglDeleteContext(hglrc);
}


GLAPI HGLRC GLAPIENTRY
DrvCreateLayerContext (HDC hdc, int iLayerPlane)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglCreateContext(hdc);
}


GLAPI PICDTABLE GLAPIENTRY
DrvSetContext (HDC hdc, HGLRC hglrc, void *callback)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglMakeCurrent(hdc, hglrc) ? &icdTable : NULL;
}


GLAPI BOOL GLAPIENTRY
DrvReleaseContext (HGLRC hglrc)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return TRUE;
}


GLAPI BOOL GLAPIENTRY
DrvShareLists (HGLRC hglrc1, HGLRC hglrc2)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglShareLists(hglrc1, hglrc2);
}


GLAPI BOOL GLAPIENTRY
DrvDescribeLayerPlane (HDC hdc, int iPixelFormat, int iLayerPlane, UINT nBytes, LPLAYERPLANEDESCRIPTOR plpd)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglDescribeLayerPlane(hdc, iPixelFormat, iLayerPlane, nBytes, plpd);
}


GLAPI int GLAPIENTRY
DrvSetLayerPaletteEntries (HDC hdc, int iLayerPlane, int iStart, int cEntries, CONST COLORREF *pcr)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglSetLayerPaletteEntries(hdc, iLayerPlane, iStart, cEntries, pcr);
}


GLAPI int GLAPIENTRY
DrvGetLayerPaletteEntries (HDC hdc, int iLayerPlane, int iStart, int cEntries, COLORREF *pcr)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglGetLayerPaletteEntries(hdc, iLayerPlane, iStart, cEntries, pcr);
}


GLAPI BOOL GLAPIENTRY
DrvRealizeLayerPalette (HDC hdc, int iLayerPlane, BOOL bRealize)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglRealizeLayerPalette(hdc, iLayerPlane, bRealize);
}


GLAPI BOOL GLAPIENTRY
DrvSwapLayerBuffers (HDC hdc, UINT fuPlanes)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglSwapLayerBuffers(hdc, fuPlanes);
}

GLAPI int GLAPIENTRY
DrvDescribePixelFormat (HDC hdc, int iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglDescribePixelFormat(hdc, iPixelFormat, nBytes, ppfd);
}


GLAPI PROC GLAPIENTRY
DrvGetProcAddress (LPCSTR lpszProc)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglGetProcAddress(lpszProc);
}


GLAPI BOOL GLAPIENTRY
DrvSetPixelFormat (HDC hdc, int iPixelFormat)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglSetPixelFormat(hdc, iPixelFormat, NULL);
}


GLAPI BOOL GLAPIENTRY
DrvSwapBuffers (HDC hdc)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return wglSwapBuffers(hdc);
}


GLAPI BOOL GLAPIENTRY
DrvValidateVersion (DWORD version)
{
    WINLOG((eff, "%s:\n", __FUNCTION__));
    return TRUE;
}
#endif
