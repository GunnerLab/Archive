#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mcce.h"

int print_surfw(PROT prot);

int main(int argc, char *argv[])
{  FILE *pdb_fp, *fp;
   PROT prot;
 
   
   db_open();
   if (get_env()) {
      printf("   This program needs run.prm in working directory.\n");
      return USERERR;
   }

   if (argc < 2) {
      printf("surfmcce mcce_pdb_file\n");
      return 0;
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
   if ((pdb_fp=fopen(argv[1], "r"))) {
      prot = load_pdb(pdb_fp);
      fclose(pdb_fp);
      if (prot.n_res == 0) {
         printf("   Fail to load pdb file: %s\n",STEP2_OUT);
         return USERERR;
      }
   }
   else {
      printf("   Specified PDB file \"%s\" was not found\n",argv[1]);
      return USERERR;
   }
   
   surfw(prot, 1.4);
   print_surfw(prot);
   
   return 0;
}


int print_surfw(PROT prot)
{
  int  i, j, k;
  int  n_conf;    /* number of conformers to print sas, only conf 0 and 1 are printed */
  char atmFile[] = "acc.atm";
  char resFile[] = "acc.res";
  FILE *atmOut, *resOut;
  float res_sas;


  if ( !(atmOut = fopen(atmFile, "w")) ) {
    printf("   premcce: unable to open %s for writing.\n", atmFile);
    return USERERR;
  }
  if ( !(resOut = fopen(resFile, "w")) ) {
    printf("   premcce: unable to open %s for writing.\n", resFile);
    return USERERR;
  }

  /* only prints out sas values for conformer 0 and 1 */
  for (i=0; i<prot.n_res; i++) {
    if (prot.res[i].n_conf <= 2)
      n_conf = prot.res[i].n_conf;
    else
      n_conf = 2;
    for (j=0; j<n_conf; j++) {
      for (k=0; k<prot.res[i].conf[j].n_atom; k++) {
         if (!(prot.res[i].conf[j].atom[k].on)) continue;
         fprintf(atmOut, "ATOM  %5s %3s %c%04d %8.3f\n",
         prot.res[i].conf[j].atom[k].name,
         prot.res[i].resName,
         prot.res[i].chainID,
         prot.res[i].resSeq,
         prot.res[i].conf[j].atom[k].sas);
      }
    }
  }

  for (i=0; i<prot.n_res; i++) {
    res_sas = 0;
    if (prot.res[i].n_conf <= 2)
      n_conf = prot.res[i].n_conf;
    else
      n_conf = 2;
    for (j=0; j<n_conf; j++) {
      for (k=0; k<prot.res[i].conf[j].n_atom; k++) {
         if (!(prot.res[i].conf[j].atom[k].on)) continue;
         res_sas += prot.res[i].conf[j].atom[k].sas;
      }
    }
    fprintf(resOut, "RES   %3s %c%04d %8.3f %8.3f\n",
    prot.res[i].resName,
    prot.res[i].chainID,
    prot.res[i].resSeq,
    res_sas,
    prot.res[i].sas);
  }

  fclose(atmOut);
  fclose(resOut);

  return 0;
}
