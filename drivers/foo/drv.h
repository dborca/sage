#ifndef DRV_H_included
#define DRV_H_included

typedef struct {
    GLfloat s, t, r, q;
} TexCoord;

typedef struct {
    GLfloat r, g, b, a;
    GLfloat x, y, z, oow;
    TexCoord texcoord[TNL_MAX_TEXCOORD];
} SWvertex;


extern SWvertex *vb;

int drv_multipass_none (int pass);

void setup_tri_pointers (void);


struct sageContext {
    void *drawable;		/**< Current drawable */
};


#endif
