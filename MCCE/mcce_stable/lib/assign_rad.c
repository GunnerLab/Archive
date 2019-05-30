#include <stdio.h>
#include "mcce.h"

int assign_rad(PROT prot)
{  int i, j, k;
   float r;
   FILE *debug_fp;
   int err=0;

   for (i=0; i<prot.n_res; i++) {
      for (j=0; j<prot.res[i].n_conf; j++) {
         for (k=0; k<prot.res[i].conf[j].n_atom; k++) {
            if (prot.res[i].conf[j].atom[k].on) {
               if (param_get("RADIUS", prot.res[i].resName, prot.res[i].conf[j].atom[k].name, &r)) {
		  debug_fp = fopen(env.debug_log,"a");
                  fprintf(debug_fp, "RADIUS   %s   %s   %4.2f\n", prot.res[i].resName, prot.res[i].conf[j].atom[k].name, env.default_radius);
		  fclose(debug_fp);
		  err=1;
                  prot.res[i].conf[j].atom[k].rad = env.default_radius;
                  param_sav("RADIUS", prot.res[i].resName, prot.res[i].conf[j].atom[k].name, &env.default_radius, sizeof(float));
               }
               else prot.res[i].conf[j].atom[k].rad = r;
            }
         }
      }
   }
   if (err) printf("   Warning! assign_rad(): missing parameter(s), default value is used and saved in %s.\n", env.debug_log); 
   return 0;
}
