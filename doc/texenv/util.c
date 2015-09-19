#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "util.h"


char *
str_dup (const char *source)
{
   char *p = malloc(strlen(source) + 1);
   if (p != NULL) {
      strcpy(p, source);
   }
   return p;
}


void
next_token (void)
{
    get_token();

#if 0
    if (token.type != T_EOI) {
	printf("token = %s\n", token.sym);
    }
#endif
}
