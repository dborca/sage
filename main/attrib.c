#include <stdlib.h>
#include <string.h>

#include "GL/gl.h"

#include "glinternal.h"
#include "glapi.h"
#include "context.h"
#include "matrix.h"
#include "tnl/tnl.h"


#define MAX_ATTRIB_STACK 16
#define MAX_CLIENT_ATTRIB_STACK 16


typedef struct ATTRIBUTE {
    struct ATTRIBUTE *next;
    GLbitfield type;
    void *data;
} ATTRIBUTE;


int ctx_attrib_depth;
int ctx_client_attrib_depth;
ATTRIBUTE *ctx_attrib_stack[MAX_ATTRIB_STACK];
ATTRIBUTE *ctx_client_attrib_stack[MAX_CLIENT_ATTRIB_STACK];
static ATTRIBUTE *apool;


void
attr_init (void)
{
    ctx_attrib_depth =
    ctx_client_attrib_depth = 0;
    apool = NULL;
}


static ATTRIBUTE *
attr_alloc (void)
{
    ATTRIBUTE *node;
    if (apool != NULL) {
	node = apool;
	apool = apool->next;
    } else {
	node = malloc(sizeof(ATTRIBUTE));
    }
    return node;
}


static void
attr_free (ATTRIBUTE *node)
{
    if (node != NULL) {
	node->next = apool;
	apool = node;
    }
}


void
attr_fini (void)
{
    ATTRIBUTE *node, *tmp;

    for (node = apool; node != NULL; ) {
	tmp = node;
	node = node->next;
	free(tmp);
    }
}


void GLAPIENTRY
imm_PushClientAttrib (GLbitfield mask)
{
    ATTRIBUTE *list, *node;

    if (ctx_client_attrib_depth == MAX_CLIENT_ATTRIB_STACK) {
	gl_error(NULL, GL_STACK_OVERFLOW, "%s(%x)\n", mask);
	return;
    }

    list = NULL;

    if (mask & GL_CLIENT_VERTEX_ARRAY_BIT) {
	TNL_CLIENT *data = malloc(sizeof(TNL_CLIENT));
	if (data == NULL) {
	    return; /* XXX GL_ERROR */
	}
	node = attr_alloc();
	if (node == NULL) {
	    free(data);
	    return; /* XXX GL_ERROR */
	}
	memcpy(data, &tnl_client, sizeof(TNL_CLIENT));
	node->data = data;
	node->type = GL_CLIENT_VERTEX_ARRAY_BIT;
	node->next = list;
	list = node;
    }

    ctx_client_attrib_stack[ctx_client_attrib_depth++] = list;

    if (mask & ~GL_CLIENT_VERTEX_ARRAY_BIT) {
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "incomplete push for ClientAttrib\n");
    }
}


void GLAPIENTRY
imm_PopClientAttrib (void)
{
    ATTRIBUTE *list, *node;

    if (ctx_client_attrib_depth == 0) {
	gl_error(NULL, GL_STACK_UNDERFLOW, "%s()\n");
	return;
    }

    list = ctx_client_attrib_stack[--ctx_client_attrib_depth];

    while (list != NULL) {
	node = list;
	list = list->next;
	if (node->type == GL_CLIENT_VERTEX_ARRAY_BIT) {
	    TNL_CLIENT *data = node->data;
	    memcpy(&tnl_client, data, sizeof(TNL_CLIENT));
	    free(data);
	}
	attr_free(node);
    }
}


void GLAPIENTRY
imm_PushAttrib (GLbitfield mask)
{
    ATTRIBUTE *list, *node;

    if (ctx_attrib_depth == MAX_ATTRIB_STACK) {
	gl_error(NULL, GL_STACK_OVERFLOW, "%s(%x)\n", mask);
	return;
    }

    list = NULL;

    if (mask & GL_VIEWPORT_BIT) {
	MATRIX *data = malloc(sizeof(MATRIX));
	if (data == NULL) {
	    return; /* XXX GL_ERROR */
	}
	node = attr_alloc();
	if (node == NULL) {
	    free(data);
	    return; /* XXX GL_ERROR */
	}
	memcpy(data, &ctx_mx_viewport, sizeof(MATRIX));
	node->data = data;
	node->type = GL_VIEWPORT_BIT;
	node->next = list;
	list = node;
    }
    if (mask & GL_COLOR_BUFFER_BIT) {
	gl_color_attrib *data = malloc(sizeof(gl_color_attrib));
	if (data == NULL) {
	    return; /* XXX GL_ERROR */
	}
	node = attr_alloc();
	if (node == NULL) {
	    free(data);
	    return; /* XXX GL_ERROR */
	}
	memcpy(data, &ctx_color, sizeof(gl_color_attrib));
	node->data = data;
	node->type = GL_COLOR_BUFFER_BIT;
	node->next = list;
	list = node;
    }
    if (mask & GL_FOG_BIT) {
	gl_fog_attrib *data = malloc(sizeof(gl_fog_attrib));
	if (data == NULL) {
	    return; /* XXX GL_ERROR */
	}
	node = attr_alloc();
	if (node == NULL) {
	    free(data);
	    return; /* XXX GL_ERROR */
	}
	memcpy(data, &ctx_fog, sizeof(gl_fog_attrib));
	node->data = data;
	node->type = GL_FOG_BIT;
	node->next = list;
	list = node;
    }

    ctx_attrib_stack[ctx_attrib_depth++] = list;

    if (mask & ~(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT | GL_FOG_BIT)) {
	gl_cry(__FILE__, __LINE__, __FUNCTION__, "incomplete push for Attrib %x\n", mask & ~(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT | GL_FOG_BIT));
    }
}


void GLAPIENTRY
imm_PopAttrib (void)
{
    ATTRIBUTE *list, *node;
    
    if (ctx_attrib_depth == 0) {
	gl_error(NULL, GL_STACK_UNDERFLOW, "%s()\n");
	return;
    }
    
    list = ctx_attrib_stack[--ctx_attrib_depth];

    while (list != NULL) {
	node = list;
	list = list->next;
	if (node->type == GL_VIEWPORT_BIT) {
	    MATRIX *data = node->data;
	    FLUSH_VERTICES();
	    memcpy(&ctx_mx_viewport, data, sizeof(MATRIX));
	    free(data);
	} else if (node->type == GL_COLOR_BUFFER_BIT) {
	    /* XXX incorrect, we have to signal this to the driver */
	    gl_color_attrib *data = node->data;
	    FLUSH_VERTICES();
	    memcpy(&ctx_color, data, sizeof(gl_color_attrib));
	    free(data);
	} else if (node->type == GL_FOG_BIT) {
	    /* XXX incorrect, we have to signal this to the driver */
	    gl_fog_attrib *data = node->data;
	    FLUSH_VERTICES();
	    memcpy(&ctx_fog, data, sizeof(gl_fog_attrib));
	    free(data);
	}
	attr_free(node);
    }
}
