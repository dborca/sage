typedef void *HPBUFFERARB;


/* WGL_ARB_pixel_format ******************************************************/


static BOOL GLAPIENTRY
wg_GetPixelFormatAttribivARB (HDC hdc,
			      int iPixelFormat,
			      int iLayerPlane,
			      UINT nAttributes,
			      const int *piAttributes,
			      int *piValues)
{
    return FALSE;
}


static BOOL GLAPIENTRY
wg_GetPixelFormatAttribfvARB (HDC hdc,
			      int iPixelFormat,
			      int iLayerPlane,
			      UINT nAttributes,
			      const int *piAttributes,
			      FLOAT *pfValues)
{
    return FALSE;
}


static BOOL GLAPIENTRY
wg_ChoosePixelFormatARB (HDC hdc,
			 const int *piAttribIList,
			 const FLOAT *pfAttribFList,
			 UINT nMaxFormats,
			 int *piFormats,
			 UINT *nNumFormats)
{
    return FALSE;
}


/* WGL_ARB_render_texture ****************************************************/


static BOOL GLAPIENTRY
wg_BindTexImageARB (HPBUFFERARB hPbuffer, int iBuffer)
{
    return FALSE;
}


static BOOL GLAPIENTRY
wg_ReleaseTexImageARB (HPBUFFERARB hPbuffer, int iBuffer)
{
    return FALSE;
}


static BOOL GLAPIENTRY
wg_SetPbufferAttribARB (HPBUFFERARB hPbuffer,
			const int *piAttribList)
{
    return FALSE;
}


/* WGL_ARB_pbuffer ***********************************************************/


static HPBUFFERARB GLAPIENTRY
wg_CreatePbufferARB (HDC hDC,
		     int iPixelFormat,
		     int iWidth,
		     int iHeight,
		     const int *piAttribList)
{
    return NULL;
}


static HDC GLAPIENTRY
wg_GetPbufferDCARB (HPBUFFERARB hPbuffer)
{
    return NULL;
}


static int GLAPIENTRY
wg_ReleasePbufferDCARB (HPBUFFERARB hPbuffer, HDC hDC)
{
    return -1;
}


static BOOL GLAPIENTRY
wg_DestroyPbufferARB (HPBUFFERARB hPbuffer)
{
    return FALSE;
}


static BOOL GLAPIENTRY
wg_QueryPbufferARB (HPBUFFERARB hPbuffer,
		    int iAttribute,
		    int *piValue)
{
    return FALSE;
}


/* GL_NV_register_combiners **************************************************/


static void GLAPIENTRY
g_CombinerInputNV (GLenum stage,
                   GLenum portion,
                   GLenum variable,
                   GLenum input,
                   GLenum mapping,
                   GLenum componentUsage)
{
}


static void GLAPIENTRY
g_CombinerOutputNV (GLenum stage,
                    GLenum portion, 
                    GLenum abOutput,
                    GLenum cdOutput,
                    GLenum sumOutput,
                    GLenum scale,
                    GLenum bias,
                    GLboolean abDotProduct,
                    GLboolean cdDotProduct,
                    GLboolean muxSum)
{
}


static void GLAPIENTRY
g_CombinerParameterfNV (GLenum pname,
                        GLfloat param)
{
}


static void GLAPIENTRY
g_CombinerParameterfvNV (GLenum pname,
                         const GLfloat *params)
{
}


static void GLAPIENTRY
g_CombinerParameteriNV (GLenum pname,
                        GLint param)
{
}


static void GLAPIENTRY
g_CombinerParameterivNV (GLenum pname,
                         const GLint *params)
{
}


static void GLAPIENTRY
g_FinalCombinerInputNV (GLenum variable,
                        GLenum input,
                        GLenum mapping,
                        GLenum componentUsage)
{
}


static void GLAPIENTRY
g_GetCombinerInputParameterfvNV (GLenum stage,
                                 GLenum portion,
                                 GLenum variable,
                                 GLenum pname,
                                 GLfloat *params)
{
}


static void GLAPIENTRY
g_GetCombinerInputParameterivNV (GLenum stage,
                                 GLenum portion,
                                 GLenum variable,
                                 GLenum pname,
                                 GLint *params)
{
}


static void GLAPIENTRY
g_GetCombinerOutputParameterfvNV (GLenum stage,
                                  GLenum portion, 
                                  GLenum pname,
                                  GLfloat *params)
{
}


static void GLAPIENTRY
g_GetCombinerOutputParameterivNV (GLenum stage,
                                  GLenum portion, 
                                  GLenum pname,
                                  GLint *params)
{
}


static void GLAPIENTRY
g_GetFinalCombinerInputParameterfvNV (GLenum variable,
                                      GLenum pname,
                                      GLfloat *params)
{
}


static void GLAPIENTRY
g_GetFinalCombinerInputParameterivNV (GLenum variable,
                                      GLenum pname,
                                      GLint *params)
{
}


/* GL_NV_register_combiners2 *************************************************/


static void GLAPIENTRY
g_CombinerStageParameterfvNV (GLenum stage,
                              GLenum pname,
                              const GLfloat *params)
{
}


static void GLAPIENTRY
g_GetCombinerStageParameterfvNV (GLenum stage,
                                 GLenum pname,
                                 GLfloat *params)
{
}
