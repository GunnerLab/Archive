#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mcce.h"

/* Standalone program of prining based on pairwise interaction vector
 * test comb/1lse/ on hestia with full CPU yfsong's version: user    46m5.200s
 */
int prune_pv(PROT prot, float c1, float c2, float c3);
int over_geo(CONF conf1, CONF conf2, float cutoff);
int over_ele(PROT prot, int ir, int ic, int jc, float cutoff);
int over_vdw(PROT prot, int ir, int ic, int jc, float cutoff);

int main(int argc, char *argv[])
{  FILE *pdb_fp, *fp;
   PROT prot;
   float c1, c2, c3;
   int N;

   db_open();
   if (get_env()) {
      printf("   This program needs run.prm in working directory.\n");
      return USERERR;
   }

   if (argc < 4) {
      c1 = 0.5;
      c2 = 1.0;
      c3 = 8.0;
   }
   else {
      c1 = atof(argv[1]);
      c2 = atof(argv[2]);
      c3 = atof(argv[3]);
   }

   /* load parameters */
   if ((fp=fopen(env.new_tpl, "r"))) {
      fclose(fp);
      load_param(env.new_tpl);
      printf("%s loaded.\n",env.new_tpl);
   }
   if (strlen(env.param)) {
      if (load_all_param(env.param)) {
         printf("   Can't load parameter files in %s\n",env.param);
         return USERERR;
      }
   }

   /* load pdb */
   if ((pdb_fp=fopen(STEP2_OUT, "r"))) {
      prot = load_pdb(pdb_fp);
      fclose(pdb_fp);
      if (prot.n_res == 0) {
         printf("   Fail to load pdb file: %s\n",STEP2_OUT);
         return USERERR;
      }
   }
   else {
      printf("   Specified PDB file \"%s\" was not found\n",STEP2_OUT);
      return USERERR;
   }

   id_conf(prot);
   N=prune_pv(prot, c1, c2, c3);

   printf("geo_cutoff = %.3f, ele_cutoff = %.3f, vdw_cutoff = %.3f, %d conf deleted\n", c1, c2, c3, N);
   write_pdb(stdout, prot);

   db_close();
   return 0;
}

int prune_pv(PROT prot, float c1, float c2, float c3)
{  int n = 0; /* number of conformers deleted */
   float cutoff_geo = c1;
   float cutoff_ele = c2;
   float cutoff_vdw = c3;
   int ir, ic, ia, jc;
      
   for (ir=0; ir<prot.n_res; ir++) {
      for (ic=1; ic<prot.res[ir].n_conf; ic++) {
         prot.res[ir].conf[ic].netcrg = 0.0;
         for (ia=0; ia<prot.res[ir].conf[ic].n_atom; ia++) {
            if (!prot.res[ir].conf[ic].atom[ia].on) continue;
            prot.res[ir].conf[ic].netcrg += prot.res[ir].conf[ic].atom[ia].crg;
         }
      }
   }


    for (ir = 0; ir< prot.n_res; ir++) {
        for (ic = 1; ic< prot.res[ir].n_conf; ic++) {
            prot.res[ir].conf[ic].on =1;
        }
    }

   /* get pairwise vector, ele pairwise + vdw pairwise 
    * We may want to keep the first generation conformers, non-rotamers,
    * Only rotamers (history[2] is 'R') would be deleted 
    */
   for (ir=0; ir<prot.n_res; ir++) {
      for (ic=1; ic<prot.res[ir].n_conf-1; ic++) {
         if (!prot.res[ir].conf[ic].on) continue;
         for (jc=2; jc<prot.res[ir].n_conf; jc++) {
            if (!prot.res[ir].conf[jc].on) continue;
            if (ic == jc) continue;
            if (prot.res[ir].conf[jc].history[2] != 'R') continue;
            if (strcmp(prot.res[ir].conf[ic].confName, prot.res[ir].conf[jc].confName)) continue;
            if (over_geo(prot.res[ir].conf[ic], prot.res[ir].conf[jc], cutoff_geo)) continue;
            if (over_ele(prot, ir, ic, jc, cutoff_ele)) continue;
            if (over_vdw(prot, ir, ic, jc, cutoff_vdw)) continue;
            prot.res[ir].conf[jc].on = 0;
         }
      }
   }

   /* delete conformers */
   for (ir=0; ir<prot.n_res; ir++) {
      for (ic=prot.res[ir].n_conf-1; ic>1; ic--) {
         if (!prot.res[ir].conf[ic].on) {
            del_conf(&prot.res[ir], ic);
            n++;
         }
      }
   }

   return n;
}

int over_geo(CONF conf1, CONF conf2, float cutoff)
{  int ia, ja;
   float dd_max = cutoff*cutoff;
   float dd;

   for (ia=0; ia<conf1.n_atom; ia++) {
      if (!conf1.atom[ia].on) continue;
      if ((ja=iatom(conf2.confName, conf1.atom[ia].name))<0) continue;
      if (!conf2.atom[ja].on) continue;
      dd = ddvv(conf1.atom[ia].xyz, conf2.atom[ja].xyz);
      if (dd>dd_max) return 1;
   }

   return 0;
}

int over_ele(PROT prot, int ir, int ic, int jc, float cutoff)
{  int kr, kc;
   float Ei, Ej;

   for (kr=0; kr<prot.n_res; kr++) {
      if (kr==ir) continue;
      
      /* only use the backbone and the first ionized conformer for comparison */
      Ei = Ecoulomb_conf2conf(prot, ir, ic, kr, 0, env.epsilon_prot);
      Ej = Ecoulomb_conf2conf(prot, ir, jc, kr, 0, env.epsilon_prot);
      if (fabs(Ei-Ej)>cutoff) return 1;
      for (kc=1; kc<prot.res[kr].n_conf; kc++) {
//         printf("%s, %8.3f\n", prot.res[kr].conf[kc].uniqID, prot.res[kr].conf[kc].netcrg);
         if (!prot.res[kr].conf[kc].on) continue;
         if (fabs(prot.res[kr].conf[kc].netcrg) > 0.01) {
            Ei = Ecoulomb_conf2conf(prot, ir, ic, kr, kc, env.epsilon_prot);
            Ej = Ecoulomb_conf2conf(prot, ir, jc, kr, kc, env.epsilon_prot);
            if (fabs(Ei-Ej)>cutoff) return 1;
            else break; /* next residue */
         }
      }
   }

   return 0;
}

int over_vdw(PROT prot, int ir, int ic, int jc, float cutoff)
{  int kr, kc;
   float Ei, Ej;

   for (kr=0; kr<prot.n_res; kr++) {
      if (kr==ir) continue;
      /* only use the backbone and the first ionized conformer for comparison */
      Ei = Evdw_conf2conf(prot, ir, ic, kr, 0);
      Ej = Evdw_conf2conf(prot, ir, jc, kr, 0);
      if (!(Ei>20.0 && Ej>20.0) && fabs(Ei-Ej)>cutoff) return 1;
      for (kc=1; kc<prot.res[kr].n_conf; kc++) {
         if (!prot.res[kr].conf[kc].on) continue;
         if (fabs(prot.res[kr].conf[kc].netcrg) > 0.01) {
            Ei = Evdw_conf2conf(prot, ir, ic, kr, kc);
            Ej = Evdw_conf2conf(prot, ir, jc, kr, kc);
            if (!(Ei>20.0 && Ej>20.0) && fabs(Ei-Ej)>cutoff) return 1;
            else break;
         }
      }
   }

   return 0;
}

