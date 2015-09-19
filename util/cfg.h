#ifndef CFG_H_included
#define CFG_H_included

int cfg_load (const char *filename);
const char *cfg_get (const char *var, const char *def);
int cfg_browse (int (*fn) (const char *var, const char *val));
void cfg_kill (void);

#endif
