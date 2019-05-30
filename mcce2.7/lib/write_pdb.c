#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mcce.h"

int write_pdb(FILE *stream, PROT prot)
{  int i, j, k, iConf, c;
   c = 0;
   for (i=0; i<prot.n_res; i++) {
      iConf =0;
      for (j=0; j<prot.res[i].n_conf; j++) {
         for (k=0; k<prot.res[i].conf[j].n_atom; k++) {
            if (!(prot.res[i].conf[j].atom[k].on)) continue;
            if (c<99999) c++;
            fprintf(stream, "ATOM  %5d %4s%c%3s %c%04d%c%03d%8.3f%8.3f%8.3f %7.3f      %6.3f      %-10s\n",
                            c, prot.res[i].conf[j].atom[k].name,
                            prot.res[i].conf[j].altLoc,
                            prot.res[i].resName,
                            prot.res[i].chainID,
                            prot.res[i].resSeq,
                            prot.res[i].iCode,
                            iConf,
                            prot.res[i].conf[j].atom[k].xyz.x,
                            prot.res[i].conf[j].atom[k].xyz.y,
                            prot.res[i].conf[j].atom[k].xyz.z,
                            prot.res[i].conf[j].atom[k].rad,
                            prot.res[i].conf[j].atom[k].crg,
                            prot.res[i].conf[j].history);
         }
         iConf++;
      }
   }
   return 0;
}
