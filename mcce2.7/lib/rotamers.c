#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "mcce.h"

typedef struct {
   int *nc_start;
   int *nc_rmdup;
   int *nc_swap;
   int *nc_rotate;
   int *nc_vdw;
   int *nc_hdir;
   int *nc_repack;
   int *nc_ioniz;
   int *nc_h;
   int *nc_oh;
   int *nc_elect;
} CONFSTAT;

time_t nowA, nowB, nowStart, nowEnd;
long    idum;

int delete_h(PROT prot);
int place_rot(PROT prot);
int place_rot_rule(RES *res, ROTAMER rule, int n);
int swing_rot(PROT prot);
int swing_rot_rule(RES *res, ROTAMER rule, float phi_degree);
void write_step2stat(FILE *fp, PROT prot, CONFSTAT stat);
int prune_by_vdw(PROT prot, float delta_E);
int rot_pack(PROT prot, int n);
int rot_refine(PROT prot, MICROSTATE state, float ****pariwise);
int place_missing(PROT prot, int handle_addconf);
int ionization(PROT prot);
int rm_dupconf(PROT prot, float prune_thr);
int rm_dupconf_hv(PROT prot);
int write_conflist(FILE *fp, PROT prot);
int load_listrot(char *fname, PROT prot);
int get_demetri_out(void);
int prune_hdirected(PROT prot);
float relax_this_pair(PROT prot, RES *res_a, int conf_a, RES *res_b, int conf_b);
int swing_conf_rot_rule(RES *res, int j, ROTAMER rule, float phi_degree);
extern int rot_swap(PROT prot);
extern int get_resnbrs(PROT prot, float dlimit);
extern int relaxation(PROT prot);
extern int relax_h(PROT prot);
extern int relax_water(PROT prot);
extern void collect_all_connect(int i_res, int i_conf, int i_atom, PROT prot, int *n_connect12, ATOM ***connect12, int *n_connect13, ATOM ***connect13, int *n_connect14, ATOM ***connect14);
extern int print_vdw_map(PROT prot);
int prune_pv(PROT prot, float c1, float c2, float c3);
int over_geo(CONF conf1, CONF conf2, float cutoff);
int over_ele(PROT prot, int ir, int ic, int jc, float cutoff);
int over_vdw(PROT prot, int ir, int ic, int jc, float cutoff);
float max_confSAS(PROT prot, int ir, int ic);
int label_exposed(PROT prot);

int rotamers()
{  FILE *fp;
   PROT prot;
   CONFSTAT confstat;
   int c, i, j, kr, kc;
   char sbuff[MAXCHAR_LINE];

   nowStart = time(NULL);
   idum = time(NULL);

   /* Load step 1 output pdb file */
   printf("   Load step 1 output file %s...\n", STEP1_OUT);
   if (!(fp=fopen(STEP1_OUT, "r"))) {
      printf("   FATAL: rotamers(): \"No step 1 output \"%s\".\n", STEP1_OUT);
      return USERERR;
   }
   prot = load_pdb(fp);
   if (prot.n_res == 0) {
      printf("   There are errors in pdb file, quiting ...\n");
      return USERERR;
   }
   printf("   Done\n\n"); fflush(stdout);
   fclose(fp);

   /* load CONFLIST1 */
   load_headlst(prot);

   /* prepare arrys to store number of conformers after rotate, swing, prune, repack,
    * place_missing, and optimize OH, last slot for total */
   confstat.nc_start  = (int *) calloc((prot.n_res+1) , sizeof(int));
   confstat.nc_rmdup  = (int *) calloc((prot.n_res+1) , sizeof(int));
   confstat.nc_swap   = (int *) calloc((prot.n_res+1) , sizeof(int));
   confstat.nc_rotate = (int *) calloc((prot.n_res+1) , sizeof(int));
   confstat.nc_vdw    = (int *) calloc((prot.n_res+1) , sizeof(int));
   confstat.nc_hdir   = (int *) calloc((prot.n_res+1) , sizeof(int));
   confstat.nc_repack = (int *) calloc((prot.n_res+1) , sizeof(int));
   confstat.nc_ioniz  = (int *) calloc((prot.n_res+1) , sizeof(int));
   confstat.nc_h      = (int *) calloc((prot.n_res+1) , sizeof(int));
   confstat.nc_oh     = (int *) calloc((prot.n_res+1) , sizeof(int));
   confstat.nc_elect  = (int *) calloc((prot.n_res+1) , sizeof(int));
   get_resnbrs(prot, 5.0);

   /* DEBUG
   for (i=0; i<prot.n_res; i++) {
      for (j=0; j<prot.res[i].n_ngh; j++) {
         printf("RES %s %04d: %s %04d\n", prot.res[i].resName, prot.res[i].resSeq, prot.res[i].ngh[j]->resName, prot.res[i].ngh[j]->resSeq);
      }
      printf("\n");
   }
   return 0;
   */

   /* count conformers */
   c = 0;
   for (i=0; i<prot.n_res; i++) {
      confstat.nc_start[i] = prot.res[i].n_conf-1;
      c+=(prot.res[i].n_conf-1);
   }
   confstat.nc_start[prot.n_res] = c;

   printf("   Rotamer statistics is dynamically updated in file \"%s\"\n\n", ROTSTAT);

   /* Delete extra confs */
   printf("   Remove redundant heavy atom conformers in %s\n", STEP1_OUT);
   printf("   %d conformers were deleted.\n   Done\n\n", rm_dupconf_hv(prot));
   c = 0;
   for (i=0; i<prot.n_res; i++) {
      confstat.nc_rmdup[i] = prot.res[i].n_conf-1;
      c+=(prot.res[i].n_conf-1);
   }
   confstat.nc_rmdup[prot.n_res] = c;
   fp = fopen(ROTSTAT,"w");
   if (fp) {
       fprintf(fp,"   Rotamer making statitics:\n");
       write_step2stat(fp, prot, confstat);
       printf("\n");
       fclose(fp);
   }

   /* Relabel rotamer history */
   for (i=0; i<prot.n_res; i++) {
      prot.res[i].n_conf_ori = prot.res[i].n_conf;
      for (j=1; j<prot.res[i].n_conf; j++) {
         sprintf(sbuff, "O%03X", j-1);
         strncpy(prot.res[i].conf[j].history+2, sbuff, 4);
      }
   }

   /* Relax water */
   if (env.relax_wat) {
       printf("   Relax crystal water..\n"); fflush(stdout);
       relax_water(prot);
       printf("   Done\n\n"); fflush(stdout);
   }

   printf("   Prepare for rotamer making ...\n");
   get_connect12(prot);
   //if (env.delete_h) {
       printf("   Deleting H atoms...%d H atoms were deleted.\n", delete_h(prot)); fflush(stdout);
   //}
   printf("   Assigning radii.\n"); fflush(stdout); assign_rad(prot);
   printf("   Estimating Solvent Accessible Surface (SAS).\n"); fflush(stdout); surfw(prot, 1.4);
   write_headlst(prot);
   printf("   Done.\n\n");

   /* Swap */
   if (env.rot_swap) {
       printf("   Swap atoms...\n"); fflush(stdout);
       rot_swap(prot);
       printf("   Done\n\n"); fflush(stdout);
   }
   /* count conformers */
   c = 0;
   for (i=0; i<prot.n_res; i++) {
      confstat.nc_swap[i] = prot.res[i].n_conf-1;
      c+=(prot.res[i].n_conf-1);
   }
   confstat.nc_swap[prot.n_res] = c;
   fp = fopen(ROTSTAT,"w");
   if (fp) {
       fprintf(fp,"   Rotamer making statitics:\n");
       write_step2stat(fp, prot, confstat);
       printf("\n");
       fclose(fp);
   }

   /* Place conformers */
   if (env.pack) {
      printf("   Place rotamers...\n"); fflush(stdout);
      place_rot(prot);
      printf("   Done\n\n"); fflush(stdout);
   }
   /* Do swing */
   if (env.swing) {
      printf("   Swing rotamers...\n"); fflush(stdout);
      swing_rot(prot);
      printf("   Done\n\n"); fflush(stdout);
   }
   /* count conformers */
   c = 0;
   for (i=0; i<prot.n_res; i++) {
      confstat.nc_rotate[i] = prot.res[i].n_conf-1;
      c+=(prot.res[i].n_conf-1);
   }
   confstat.nc_rotate[prot.n_res] = c;
   fp = fopen(ROTSTAT,"w");
   if (fp) {
       fprintf(fp,"   Rotamer making statitics:\n");
       write_step2stat(fp, prot, confstat);
       printf("\n");
       fclose(fp);
   }

   /* delete duplicate conformers */
   printf("   Delete duplicate conformers ..."); fflush(stdout);
   printf(" %d conformers deleted.\n", rm_dupconf_hv(prot));
   printf("   Done\n\n"); fflush(stdout);


   /* prune by self vdw */
   printf("   Prune rotamers by self VDW potential...\n");
   printf("   Creating connectivity table...\n"); fflush(stdout);
   get_connect12(prot);
   printf("   Computing self VDW potential. It may take a while...\n"); fflush(stdout);
   get_vdw0(prot);
   get_vdw1(prot);
   for (kr=0; kr<prot.n_res; kr++) {
      for (kc=1; kc<prot.res[kr].n_conf; kc++) {
          prot.res[kr].conf[kc].E_torsion = torsion_conf(&prot.res[kr].conf[kc]);
          prot.res[kr].conf[kc].E_self = prot.res[kr].conf[kc].E_vdw0 + prot.res[kr].conf[kc].E_vdw1 + prot.res[kr].conf[kc].E_torsion;
      }
   }
   printf("   Pruning rotamers...");fflush(stdout);
   printf("%d rotamers deleted.\n", prune_by_vdw(prot, env.vdw_cutoff));
   c = 0;
   for (i=0; i<prot.n_res; i++) {
      confstat.nc_vdw[i] = prot.res[i].n_conf-1;
      c+=(prot.res[i].n_conf-1);
   }
   confstat.nc_vdw[prot.n_res] = c;
   printf("   Done\n\n"); fflush(stdout);
   fp = fopen(ROTSTAT,"w");
   if (fp) {
       fprintf(fp,"   Rotamer making statitics:\n");
       write_step2stat(fp, prot, confstat);
       printf("\n");
       fclose(fp);
   }

   /* find the most exposed conformer */
   printf("   Tuning exposed rotamers...\n");fflush(stdout);
   label_exposed(prot); /* history[2] = 'E' for the most exposed */
   get_connect12(prot);
   get_vdw0(prot);
   get_vdw1(prot);
   printf("   Done\n\n"); fflush(stdout);

   /* Hydrogen bond directed rotamer making */
   //get_resnbrs(prot, 5.0);
   if (env.hdirected) {
      nowA = time(NULL);
      printf("   Hydrogen bond directed rotamer making...\n"); fflush(stdout);

      if (prune_hdirected(prot)) {
         printf("   Fatal error.\n");
         return USERERR;
      }

      /* refine and update */
      printf("   Updating self vdw\n"); fflush(stdout);
      get_vdw0(prot);
      get_vdw1(prot);
      for (kr=0; kr<prot.n_res; kr++) {
         for (kc=1; kc<prot.res[kr].n_conf; kc++) {
             prot.res[kr].conf[kc].E_torsion = torsion_conf(&prot.res[kr].conf[kc]);
             prot.res[kr].conf[kc].E_self = prot.res[kr].conf[kc].E_vdw0 + prot.res[kr].conf[kc].E_vdw1 + prot.res[kr].conf[kc].E_torsion;
         }
      }
      printf("   Pruning rotamers by new self energy...");fflush(stdout);
      printf("%d rotamers deleted.\n", prune_by_vdw(prot, env.vdw_cutoff));
      rm_dupconf_hv(prot);

      nowB= time(NULL);
      i=nowB - nowA;
      printf("   Done. Time spent = %d\n\n", i); fflush(stdout);
   }
   c = 0;
   for (i=0; i<prot.n_res; i++) {
      confstat.nc_hdir[i] = prot.res[i].n_conf-1;
      c+=(prot.res[i].n_conf-1);
   }
   confstat.nc_hdir[prot.n_res] = c;
   fp = fopen(ROTSTAT,"w");
   if (fp) {
       fprintf(fp,"   Rotamer making statitics:\n");
       write_step2stat(fp, prot, confstat);
       printf("\n");
       fclose(fp);
   }

   /* clean up memory DEBUG
   free(confstat.nc_start);
   free(confstat.nc_rmdup);
   free(confstat.nc_rotate);
   free(confstat.nc_vdw);
   free(confstat.nc_repack);
   free(confstat.nc_ioniz);
   free(confstat.nc_h);
   free(confstat.nc_oh);
   free(confstat.nc_elect);
   del_prot(&prot);

   return 0;
   */

   /* repack */
   printf("   Repack side chains %d times, remove inaccessible conformers...\n", env.repacks);
   fflush(stdout);
   rot_pack(prot, env.repacks);
   c = 0;
   for (i=0; i<prot.n_res; i++) {
      confstat.nc_repack[i] = prot.res[i].n_conf-1;
      c+=(prot.res[i].n_conf-1);
   }
   confstat.nc_repack[prot.n_res] = c;
   printf("   Done\n\n");
   fflush(stdout);
   fp = fopen(ROTSTAT,"w");
   if (fp) {
       fprintf(fp,"   Rotamer making statitics:\n");
       write_step2stat(fp, prot, confstat);
       printf("\n");
       fclose(fp);
   }


   /* Relabel rotamer history */
   for (i=0; i<prot.n_res; i++) {
      for (j=prot.res[i].n_conf_ori; j<prot.res[i].n_conf; j++) {
         if (prot.res[i].conf[j].history[2] == 'H') {
            sprintf(sbuff, "H%03X", j-1);
            strncpy(prot.res[i].conf[j].history+2, sbuff, 4);
         }
         else if (prot.res[i].conf[j].history[2] == 'E') {
            sprintf(sbuff, "E%03X", j-1);
            strncpy(prot.res[i].conf[j].history+2, sbuff, 4);
         }
         else {
            sprintf(sbuff, "R%03X", j-1);
            strncpy(prot.res[i].conf[j].history+2, sbuff, 4);
         }
      }
   }

   /* Write heavy atom rotamer pdb file, for recursive use of step 2 */
   if (!env.minimize_size) {
      if (!(fp=fopen(FN_HVROT, "w"))) {
         printf("   WARNING: Error in writing file %s\n\n", FN_HVROT);
      }
      else {
         write_pdb(fp, prot);
         fclose(fp);
      }
   }
   else {
      printf("   Skip writing heavy atome rotamer pdb file %s\n", FN_HVROT);
   }

   /* Make ionization states */
   printf("   Making ionization conformers...\n"); fflush(stdout);
   if (ionization(prot)) {
      printf("   FATAL: Fatal error reported by ionization()\n");
      return USERERR;
   }
   printf("   Done\n\n");
   fflush(stdout);
   c = 0;
   for (i=0; i<prot.n_res; i++) {
      confstat.nc_ioniz[i] = prot.res[i].n_conf-1;
      c+=(prot.res[i].n_conf-1);
   }
   confstat.nc_ioniz[prot.n_res] = c;
   fp = fopen(ROTSTAT,"w");
   fprintf(fp,"   Rotamer making statitics:\n");
   write_step2stat(fp, prot, confstat);
   printf("\n");
   fclose(fp);

   /* add h */
   printf("   Add H atoms...\n"); fflush(stdout);
   while(place_missing(prot,1) > 0);
   assign_rad(prot);
   printf("   Done\n\n"); fflush(stdout);


   c = 0;
   for (i=0; i<prot.n_res; i++) {
      confstat.nc_h[i] = prot.res[i].n_conf-1;
      c+=(prot.res[i].n_conf-1);
   }
   confstat.nc_h[prot.n_res] = c;
   fp = fopen(ROTSTAT,"w");
   fprintf(fp,"   Rotamer making statitics:\n");
   write_step2stat(fp, prot, confstat);
   printf("\n");
   fclose(fp);

   /* heavy atom relaxation */
   if (env.hv_relax_ncycle > 0) {
       printf("   Relaxation...\n"); fflush(stdout);
       relaxation(prot);
       /* add h again, this is due to protons are easily moved in relaxation program */
       delete_h(prot);
       rm_dupconf(prot, env.prune_thr);
       while(place_missing(prot,1) > 0);

       printf("   Done\n\n"); fflush(stdout);
   }


   /* optimize hydroxyl and water */
   if (env.relax_h) {
       printf("   Optimizing hydroxyl and water...\n");
       fflush(stdout);
       relax_h(prot);
   }
   else {
      printf("   NOT optimizing hydroxyl and water...\n");
   }
   /*
   else {
       opt_h(prot);
   }
   */

   printf("   Done\n\n");
   fflush(stdout);

   c = 0;
   for (i=0; i<prot.n_res; i++) {
      confstat.nc_oh[i] = prot.res[i].n_conf-1;
      c+=(prot.res[i].n_conf-1);
   }
   confstat.nc_oh[prot.n_res] = c;
   fp = fopen(ROTSTAT,"w");
   fprintf(fp,"   Rotamer making statitics:\n");
   write_step2stat(fp, prot, confstat);
   printf("\n");
   fclose(fp);

   printf("   Sorting conformers...\n"); fflush(stdout);
   sort_conf(prot);
   printf("   Done\n\n"); fflush(stdout);

   /* write full step2_out */
   assign_crg(prot);
   if (!env.minimize_size) {
      fp = fopen("step2_out.full", "w");
      write_pdb(fp, prot);
      fclose(fp);
   }
   else {
      printf("   Skip writing full rotamer file step2_out.full.\n");
   }

   printf("   Delete duplicate conformers ...\n");
   fflush(stdout);
   c = -1;
   while (c) {
     c = prune_pv(prot, env.prune_rmsd, env.prune_ele, env.prune_vdw);
     if (c) printf("      %d conformers deleted in this cycle\n", c);
   }
   printf("   Done\n\n");   fflush(stdout);
   c = 0;
   for (i=0; i<prot.n_res; i++) {
      confstat.nc_elect[i] = prot.res[i].n_conf-1;
      c+=(prot.res[i].n_conf-1);
   }
   confstat.nc_elect[prot.n_res] = c;
   fp = fopen(ROTSTAT,"w");
   fprintf(fp,"   Rotamer making statitics:\n");
   write_step2stat(fp, prot, confstat);
   printf("\n");
   fclose(fp);


   fp = fopen(ROTSTAT,"w");
   fprintf(fp,"   Rotamer making statitics:\n");
   write_step2stat(fp, prot, confstat);
   printf("\n");
   fclose(fp);

   /* write out */
   printf("   Write output...\n"); fflush(stdout);
   /* mcce pdb */
   fp = fopen(STEP2_OUT, "w");
   write_pdb(fp, prot);
   fclose(fp);
   /* conformer summary */
   fp = fopen(FN_CONFLIST2, "w");
   write_conflist(fp, prot);
   fclose(fp);

   /*
   get_demetri_out();
   */

   printf("   Done\n\n"); fflush(stdout);

   nowEnd = time(NULL);
   printf("   Total time for step2 (rotamer making) is %ld seconds.\n\n", nowEnd - nowStart);


   printf("   Output files:\n");
   printf("      %-16s: mcce pdb file, the input of step 3\n", STEP2_OUT);
   printf("      %-16s: conformer summary\n", FN_CONFLIST2);
   printf("      %-16s: rotamers without pairwise pruning.\n", "step2_out.full");
   printf("      %-16s: heavy atom rotamers, can be used recursively by step 2\n", FN_HVROT);
   printf("\n"); fflush(stdout);

   /* clean up memory */
   free(confstat.nc_start);
   free(confstat.nc_rmdup);
   free(confstat.nc_rotate);
   free(confstat.nc_vdw);
   free(confstat.nc_repack);
   free(confstat.nc_ioniz);
   free(confstat.nc_h);
   free(confstat.nc_oh);
   free(confstat.nc_elect);
   del_prot(&prot);

   return 0;
}

int delete_h(PROT prot)
{  int n = 0;
   int i, j, k;

   for (i=0; i<prot.n_res; i++)
      for (j=0; j<prot.res[i].n_conf; j++)
        for (k=0; k<prot.res[i].conf[j].n_atom; k++)
           if (prot.res[i].conf[j].atom[k].name[1] == 'H' && prot.res[i].conf[j].atom[k].on) {
              prot.res[i].conf[j].atom[k].on = 0;
              n++;
           }

   return n;
}

int place_rot(PROT prot)
{  int i;
   int C;
   char C_str[5];
   ROTAMER rule;
   char sbuff[MAXCHAR_LINE];

   /* construct rotamers */
   for (i=0; i<prot.n_res; i++) {
      if (!prot.res[i].do_rot) {
         printf("   Skip rotamer making for residue \"%s%04d%c\"\n", prot.res[i].resName,
                                                                                 prot.res[i].resSeq,
                                                                                 prot.res[i].chainID);
         continue;
      }
      if (prot.res[i].sas > env.sas_cutoff) {
         C = (int) (prot.res[i].rotations/2.0+0.5);
         if (C <= 0) C = 1;
         printf("   Reduce rotation steps from %2d to %2d for residue \"%s%04d%c\" as SAS=%3.f%%\n",  prot.res[i].rotations,
                                                                                 C,
                                                                                 prot.res[i].resName,
                                                                                 prot.res[i].resSeq,
                                                                                 prot.res[i].chainID,
                                                                                 prot.res[i].sas*100.0);
         prot.res[i].rotations = C;
      }

      C = 0;
      while (1) {
         sprintf(C_str, "%d", C);
         if (param_get("ROTAMER", prot.res[i].resName, C_str, &rule)) break;
         sprintf(sbuff, " %s  ", rule.affected);
         sprintf(rule.affected, "%s", sbuff);
         if (place_rot_rule(&prot.res[i], rule, prot.res[i].rotations)) {
            printf("   WARNING: place_rot(): \"failed placing rotamers for residue \"%s %d %c\"\"\n",
                   prot.res[i].resName, prot.res[i].resSeq, prot.res[i].chainID);
            printf("            No rotamers were made for this residue.\n");
         }
         C++;
      }
   }

   return 0;
}


int place_rot_rule(RES *res, ROTAMER rule, int n)
{  VECTOR v1, v2, v3;
   GEOM op;
   LINE axis;
   int n_conf;
   int ins;
   int i, j, k, l, k_conf;
   float phi;
   ATOM atom1, atom2;
   char found;

   /* rotate */
   phi = 3.1415926*2.0/(float) n;
   n_conf = res->n_conf;

   for (j=1; j<n_conf; j++) { /* apply the rotation on passed in confs */
      geom_reset(&op);
      for (i=0; i<n-1; i++) {
         /* find the bond: atom2 is the second atom of the rotatable. This atom must
          * be in this residue. atom2->v2
          */
         if ((k=iatom(res->conf[j].confName, rule.atom2)) == -1) {
            printf("confname=\"%s\"\n",res->conf[j].confName);
            if ((k=iatom(res->conf[0].confName, rule.atom2)) == -1) {
               printf("   FATAL: place_rot_rule(): can't find atom \"%s\" in residue \"%s\"\n",
                       rule.atom2, res->resName);
               return USERERR;
            }
            else atom2 = res->conf[0].atom[k];
         }
         else atom2 = res->conf[j].atom[k];
         v2 = atom2.xyz;

         /* find the bond: atom1 is the first atom of the bond, it is one of the connected
          * atoms of atom2, but not necessary in this conformer or this residue */
         found = 0;
         if ((k=iatom(res->conf[j].confName, rule.atom1)) == -1) {
            l=0;
            while (atom2.connect12[l]!=NULL) {
               if (!strcmp(atom2.connect12[l]->name, rule.atom1)) {
                  atom1 = *atom2.connect12[l];
                  found = 1;
                  break;
               }
               l++;
            }
         }
         else {
            atom1 = res->conf[j].atom[k];
            found = 1;
         }

         if (found)  v1 = atom1.xyz;
         else {
            printf("   FATAL: place_rot_rule(): can't find atom \"%s\" connected to \"%s\" in residue \"%s\"\n",
                    rule.atom2, rule.atom1, res->resName);
            return USERERR;
         }

         /* find the bond: atom 2 -> v2 */
         axis = line_2v(v1, v2);
         geom_roll(&op, phi, axis);

         ins = ins_conf(res, res->n_conf, res->conf[j].n_atom);
         if (ins == USERERR) return USERERR;
         cpy_conf(&res->conf[ins], &res->conf[j]);
         strncpy(res->conf[ins].history+2, "Ro", 2);

         for (k=0; k<res->conf[j].n_atom; k++) {
            if (strstr(rule.affected, res->conf[j].atom[k].name)) {
               v3 = res->conf[j].atom[k].xyz;
               geom_apply(op, &v3);
               res->conf[ins].atom[k].xyz = v3;
            }
         }

         for (k_conf=1; k_conf<res->n_conf-1;k_conf++) {
            if(!cmp_conf_hv(res->conf[res->n_conf-1], res->conf[k_conf], 0.2)) {
               del_conf(res, res->n_conf-1);
               break;
            }
         }
      }
   }

   return 0;
}

int swing_rot(PROT prot)
{  int i;
   int C;
   char C_str[5];
   ROTAMER rule;
   char sbuff[MAXCHAR_LINE];

   /* construct rotamers */
   for (i=0; i<prot.n_res; i++) {
      C = 0;
      if (prot.res[i].sas > env.sas_cutoff) {
         printf("   Skip rotamer making for residue \"%s%03d%c\" as SAS=%3.f%%\n", prot.res[i].resName,
                                                                                 prot.res[i].resSeq,
                                                                                 prot.res[i].chainID,
                                                                                 prot.res[i].sas*100.0);
         continue;
      }
      if (!prot.res[i].do_sw) {
         printf("   Skip rotamer making for residue \"%s%04d%c\"\n", prot.res[i].resName,
                                                                                 prot.res[i].resSeq,
                                                                                 prot.res[i].chainID);
         continue;
      }

      while (1) {
         sprintf(C_str, "%d", C);
         if (param_get("ROTAMER", prot.res[i].resName, C_str, &rule)) break;
         sprintf(sbuff, " %s  ", rule.affected);
         sprintf(rule.affected, "%s", sbuff);
         if (swing_rot_rule(&prot.res[i], rule, prot.res[i].phi_swing)) {
            printf("   WARNING: swing_rot(): \"failed swinging rotamers for residue \"%s %d %c\"\"\n",
                   prot.res[i].resName, prot.res[i].resSeq, prot.res[i].chainID);
            printf("            No rotamers were made for this residue.\n");
         }
         C++;
      }
   }


   return 0;
}

int swing_rot_rule(RES *res, ROTAMER rule, float phi_degree)
{  VECTOR v1, v2, v3;
   GEOM op;
   LINE axis;
   int n_conf;
   int ins;
   int j, k, l;
   ATOM atom1, atom2;
   char found;
   float phi = phi_degree*3.1415926/180.0;

   /* rotate */
   n_conf = res->n_conf;

   for (j=1; j<n_conf; j++) { /* apply the rotation on passed in confs */

      if ((k=iatom(res->conf[j].confName, rule.atom2)) == -1) {
         printf("confname=\"%s\"\n",res->conf[j].confName);
         if ((k=iatom(res->conf[0].confName, rule.atom2)) == -1) {
            printf("   FATAL: place_rot_rule(): can't find atom \"%s\" in residue \"%s\"\n",
                    rule.atom2, res->resName);
            return USERERR;
         }
         else atom2 = res->conf[0].atom[k];
      }
      else atom2 = res->conf[j].atom[k];
      v2 = atom2.xyz;

      /* find the bond: atom1 is the first atom of the bond, it is one of the connected
       * atoms of atom2, but not necessary in this conformer or this residue */
      found = 0;
      if ((k=iatom(res->conf[j].confName, rule.atom1)) == -1) {
         l=0;
         while (atom2.connect12[l]!=NULL) {
            if (!strcmp(atom2.connect12[l]->name, rule.atom1)) {
               atom1 = *atom2.connect12[l];
               found = 1;
               break;
            }
            l++;
         }
      }
      else {
         atom1 = res->conf[j].atom[k];
         found = 1;
      }

      if (found)  v1 = atom1.xyz;
      else {
         printf("   FATAL: place_rot_rule(): can't find atom \"%s\" connected to \"%s\" in residue \"%s\"\n",
                 rule.atom2, rule.atom1, res->resName);
         return USERERR;
      }

      axis = line_2v(v1, v2);

      /* swing left */
      geom_reset(&op);
      geom_roll(&op, -phi, axis);
      ins = ins_conf(res, res->n_conf, res->conf[j].n_atom);
      if (ins == USERERR) return USERERR;

      cpy_conf(&res->conf[ins], &res->conf[j]);
      strncpy(res->conf[ins].history+4, "Sw", 2);
      for (k=0; k<res->conf[j].n_atom; k++) {
         if (strstr(rule.affected, res->conf[j].atom[k].name)) {
            v3 = res->conf[j].atom[k].xyz;
            geom_apply(op, &v3);
            res->conf[ins].atom[k].xyz = v3;
         }
      }

      /* swing right */
      geom_reset(&op);
      geom_roll(&op, phi, axis);
      ins = ins_conf(res, res->n_conf, res->conf[j].n_atom);
      if (ins == USERERR) return USERERR;

      cpy_conf(&res->conf[ins], &res->conf[j]);
      strncpy(res->conf[ins].history+4, "Sw", 2);
      for (k=0; k<res->conf[j].n_atom; k++) {
         if (strstr(rule.affected, res->conf[j].atom[k].name)) {
            v3 = res->conf[j].atom[k].xyz;
            geom_apply(op, &v3);
            res->conf[ins].atom[k].xyz = v3;
         }
      }

   }

   return 0;
}

void write_step2stat(FILE *fp, PROT prot, CONFSTAT stat)
{  int i;

   fprintf(fp, "   Residue   Start  Clean   Swap Rotate   Self  Hbond Repack  Ioni.   TorH     OH  Elect\n");
   for (i=0; i<prot.n_res; i++) {
      fprintf(fp, "   %3s%c%04d%7d%7d%7d%7d%7d%7d%7d%7d%7d%7d%7d\n",
                  prot.res[i].resName,
                  prot.res[i].chainID,
                  prot.res[i].resSeq,
                  stat.nc_start[i],
                  stat.nc_rmdup[i],
                  stat.nc_swap[i],
                  stat.nc_rotate[i],
                  stat.nc_vdw[i],
                  stat.nc_hdir[i],
                  stat.nc_repack[i],
                  stat.nc_ioniz[i],
                  stat.nc_h[i],
                  stat.nc_oh[i],
                  stat.nc_elect[i]);
   }
   fprintf(fp, "   Total   %7d%7d%7d%7d%7d%7d%7d%7d%7d%7d%7d\n",
               stat.nc_start[i],
               stat.nc_rmdup[i],
               stat.nc_swap[i],
               stat.nc_rotate[i],
               stat.nc_vdw[i],
               stat.nc_hdir[i],
               stat.nc_repack[i],
               stat.nc_ioniz[i],
               stat.nc_h[i],
               stat.nc_oh[i],
               stat.nc_elect[i]);

   return;
}

int prune_by_vdw(PROT prot, float delta_E)
{  int n = 0;
   int kr, kc;
   float E_low;

   for (kr=0; kr<prot.n_res; kr++) {
      if (prot.res[kr].n_conf>1)
         E_low = prot.res[kr].conf[1].E_self;
      for (kc=1; kc<prot.res[kr].n_conf; kc++) {
         if (E_low>prot.res[kr].conf[kc].E_self) E_low = prot.res[kr].conf[kc].E_self;
      }

      for (kc=2; kc<prot.res[kr].n_conf; kc++) {
         if (prot.res[kr].conf[kc].E_self - E_low > delta_E) {
            del_conf(&prot.res[kr], kc);
            kc--;
            n++;
         }
      }
   }
   return n;
}

int rot_pack(PROT prot, int n)
{   FILE *fp;
    int C, i, j;
    MICROSTATE state;
    int flips;
    float ****pairwise;
    int i_res,i_conf,i_atom,j_res,j_conf,j_atom,i_elem,j_elem;
    ATOM *atom_p,*iatom_p,*jatom_p;
    VECTOR v1,v2,*r_min,*r_max,res_dist,bigger_min,smaller_max;
    float pair_vdw, d2, d6, d12, e, C6, C12;
    int **n_connect12,**n_connect13,**n_connect14;
    ATOM ****connect12,****connect13,****connect14;
    int i_connect, done;
    int counter;
    char pipe;
    int   n_elem;
    char  elem[N_ELEM_MAX];
    float C6_matrix[N_ELEM_MAX][N_ELEM_MAX];
    float C12_matrix[N_ELEM_MAX][N_ELEM_MAX];
    float r12sq_max, r13sq_max, r14sq_max;
    float cutoff_near2 = VDW_CUTOFF_NEAR * VDW_CUTOFF_NEAR;
    float cutoff_far2  = VDW_CUTOFF_FAR  * VDW_CUTOFF_FAR;
    //int **stat,i_counter,vdw_counter[21];  /* Debug */
    //FILE *vdw_stat;

    if (n ==0) return 0;
    for (i_res=0; i_res< prot.n_res; i_res++) {
        STRINGS confs;
        int n_atom, ins;
        if (param_get("CONFLIST", prot.res[i_res].resName, "", &confs)) {
            continue;
        }
        for (i_conf=0; i_conf<confs.n; i_conf++) {
            if (!strncmp(confs.strings[i_conf]+3,"BK",2)) continue;
            if (param_get("NATOM", confs.strings[i_conf], "", &n_atom)) {
                continue;
            }
            if (!n_atom) {
               ins = ins_conf(&prot.res[i_res], prot.res[i_res].n_conf, n_atom);
               strcpy(prot.res[i_res].conf[ins].confName, confs.strings[i_conf]);
               strcpy(prot.res[i_res].conf[ins].history,  confs.strings[i_conf]+3);
               strcpy(prot.res[i_res].conf[ins].history+2,  "________");
            }
        }
    }

    /* preparing energy lookup table */
    counter = 0;
    for (i=0; i<prot.n_res; i++)  counter+=(prot.res[i].n_conf-1);
    printf("   Computing pairwise LJ potential. This may take a while.\n");
    i= (int) counter*counter*3.5E-5;
    printf("      Estimated time on AMD 1.6GHz is %d seconds.\n",i);
    fflush(stdout);

    /* timing begins, estimate T from counter */
    nowA = time(NULL);
    pairwise = calloc(prot.n_res, sizeof(void *));
    for (i_res = 0; i_res < prot.n_res; i_res++) {
        if (prot.res[i_res].n_ngh) free(prot.res[i_res].ngh);
        prot.res[i_res].n_ngh = 0;
        prot.res[i_res].ngh = NULL;
    }

    /* Setup C6,C12 matrix */
    n_elem = 0;
    for (i_res=0; i_res<prot.n_res; i_res++) {
        for (i_conf=0; i_conf<prot.res[i_res].n_conf; i_conf++) {
            for (i_atom=0; i_atom<prot.res[i_res].conf[i_conf].n_atom; i_atom++) {
                if (!prot.res[i_res].conf[i_conf].atom[i_atom].on) continue;
                for (i_elem=0;i_elem<n_elem;i_elem++)
                    if (elem[i_elem] == prot.res[i_res].conf[i_conf].atom[i_atom].name[1]) break;
                if (i_elem == n_elem) {
                    n_elem++;
                    elem[i_elem] = prot.res[i_res].conf[i_conf].atom[i_atom].name[1];
                }
                prot.res[i_res].conf[i_conf].atom[i_atom].i_elem = i_elem;
            }
        }
    }
    for (i_elem=0;i_elem<n_elem;i_elem++) {
        for (j_elem=0;j_elem<n_elem;j_elem++) {
            char pair[4];
            pair[0] = elem[i_elem];
            pair[1] = '-';
            pair[2] = elem[j_elem];
            pair[3] = '\0';

            if(param_get("VDWAMBER", "C6",  pair, &C6_matrix[i_elem][j_elem])) {
                param_get("VDWAMBER", "C6",  "X-X", &C6_matrix[i_elem][j_elem]);
            }
            if (param_get("VDWAMBER", "C12", pair, &C12_matrix[i_elem][j_elem])) {
                param_get("VDWAMBER", "C12", "X-X", &C12_matrix[i_elem][j_elem]);
            }
        }
    }

    /* Setup vdw matrix */
    r_min = calloc(prot.n_res, sizeof(VECTOR));
    r_max = calloc(prot.n_res, sizeof(VECTOR));
    for (i_res=0; i_res<prot.n_res; i_res++) {
        int initialized = 0;
        prot.res[i_res].i_res_prot = i_res;
        for (i_conf=1; i_conf<prot.res[i_res].n_conf; i_conf++) {
            for (i_atom=0; i_atom<prot.res[i_res].conf[i_conf].n_atom; i_atom++) {
                atom_p = &prot.res[i_res].conf[i_conf].atom[i_atom];
                if (!atom_p->on) continue;
                r_min[i_res] = atom_p->xyz;
                r_max[i_res] = atom_p->xyz;
                initialized = 1;
                break;
            }
            if (initialized) break;
        }
        for (i_conf=0; i_conf<prot.res[i_res].n_conf; i_conf++) {
            for (i_atom=0; i_atom<prot.res[i_res].conf[i_conf].n_atom; i_atom++) {
                atom_p = &prot.res[i_res].conf[i_conf].atom[i_atom];
                if (!atom_p->on) continue;
                atom_p->i_res_prot  = i_res;
                atom_p->i_conf_res  = i_conf;
                atom_p->i_atom_conf = i_atom;
                if (i_conf) {
                    if (atom_p->xyz.x < r_min[i_res].x) r_min[i_res].x = atom_p->xyz.x;
                    if (atom_p->xyz.y < r_min[i_res].y) r_min[i_res].y = atom_p->xyz.y;
                    if (atom_p->xyz.z < r_min[i_res].z) r_min[i_res].z = atom_p->xyz.z;
                    if (atom_p->xyz.x > r_max[i_res].x) r_max[i_res].x = atom_p->xyz.x;
                    if (atom_p->xyz.y > r_max[i_res].y) r_max[i_res].y = atom_p->xyz.y;
                    if (atom_p->xyz.z > r_max[i_res].z) r_max[i_res].z = atom_p->xyz.z;
                }
            }
        }
    }
    //stat = calloc(prot.n_res, sizeof(int *));  /* Debug */
    for (i_res=0; i_res<prot.n_res; i_res++) {
        //stat[i_res] = calloc(prot.n_res, sizeof(int));  /* Debug */
        /* setup connectivity for for conformers */
        n_connect12 = calloc(prot.res[i_res].n_conf, sizeof(int *));
        connect12 = calloc(prot.res[i_res].n_conf, sizeof(int *));
        n_connect13 = calloc(prot.res[i_res].n_conf, sizeof(int *));
        connect13 = calloc(prot.res[i_res].n_conf, sizeof(int *));
        n_connect14 = calloc(prot.res[i_res].n_conf, sizeof(int *));
        connect14 = calloc(prot.res[i_res].n_conf, sizeof(int *));
        for (i_conf=1; i_conf<prot.res[i_res].n_conf; i_conf++) {
            if (!prot.res[i_res].conf[i_conf].n_atom) continue;
            n_connect12[i_conf] = calloc(prot.res[i_res].conf[i_conf].n_atom, sizeof(int));
            connect12[i_conf]   = calloc(prot.res[i_res].conf[i_conf].n_atom, sizeof(ATOM **));
            n_connect13[i_conf] = calloc(prot.res[i_res].conf[i_conf].n_atom, sizeof(int));
            connect13[i_conf]   = calloc(prot.res[i_res].conf[i_conf].n_atom, sizeof(ATOM **));
            n_connect14[i_conf] = calloc(prot.res[i_res].conf[i_conf].n_atom, sizeof(int));
            connect14[i_conf]   = calloc(prot.res[i_res].conf[i_conf].n_atom, sizeof(ATOM **));

            for (i_atom=0; i_atom<prot.res[i_res].conf[i_conf].n_atom; i_atom++) {
                atom_p = &prot.res[i_res].conf[i_conf].atom[i_atom];
                if (!atom_p->on) continue;
                collect_all_connect(i_res, i_conf, i_atom, prot,
                &n_connect12[i_conf][i_atom], &connect12[i_conf][i_atom],
                &n_connect13[i_conf][i_atom], &connect13[i_conf][i_atom],
                &n_connect14[i_conf][i_atom], &connect14[i_conf][i_atom]);
            }
            r12sq_max = 0.;
            for (i_atom=0; i_atom<prot.res[i_res].conf[i_conf].n_atom; i_atom++) {
                atom_p = &prot.res[i_res].conf[i_conf].atom[i_atom];
                for (i_connect=0; i_connect<n_connect12[i_conf][i_atom]; i_connect++) {
                    d2 = ddvv(atom_p->xyz, connect12[i_conf][i_atom][i_connect]->xyz);
                    if (d2 > r12sq_max) r12sq_max = d2;
                }
            }
            r13sq_max = 0.;
            for (i_atom=0; i_atom<prot.res[i_res].conf[i_conf].n_atom; i_atom++) {
                atom_p = &prot.res[i_res].conf[i_conf].atom[i_atom];
                for (i_connect=0; i_connect<n_connect13[i_conf][i_atom]; i_connect++) {
                    d2 = ddvv(atom_p->xyz, connect13[i_conf][i_atom][i_connect]->xyz);
                    if (d2 > r13sq_max) r13sq_max = d2;
                }
            }
            r14sq_max = 0.;
            for (i_atom=0; i_atom<prot.res[i_res].conf[i_conf].n_atom; i_atom++) {
                atom_p = &prot.res[i_res].conf[i_conf].atom[i_atom];
                for (i_connect=0; i_connect<n_connect14[i_conf][i_atom]; i_connect++) {
                    d2 = ddvv(atom_p->xyz, connect14[i_conf][i_atom][i_connect]->xyz);
                    if (d2 > r14sq_max) r14sq_max = d2;
                }
            }
        }

        /* lower half of the matrix */
        for (j_res=0; j_res<i_res; j_res++) {
            int j_ngh;
            /* search if i_res is already in ngh list of j_res */
            for (j_ngh=0; j_ngh<prot.res[j_res].n_ngh; j_ngh++) {
                if (i_res == prot.res[j_res].ngh[j_ngh]->i_res_prot) {
                    int n_ngh;
                    /* add j_res into ngh list */
                    prot.res[i_res].n_ngh++;
                     n_ngh = prot.res[i_res].n_ngh;
                    prot.res[i_res].ngh = realloc(prot.res[i_res].ngh, n_ngh*sizeof(void *));
                    prot.res[i_res].ngh[n_ngh-1] = &prot.res[j_res];
                    /* pointing pairwise(i,j) to pairwise(j,i), which would be transposed for i_res */
                    pairwise[i_res] = realloc(pairwise[i_res], n_ngh*sizeof(void *));
                    pairwise[i_res][n_ngh-1] = pairwise[j_res][j_ngh];
                    break;
                }
            }
        }

        /* upper half of the matrix */
        for (j_res=i_res+1; j_res<prot.n_res; j_res++) {
            int n_ngh,all_small;
            /*
            check if i_res and j_res are close enough.
            .i_res:   |------------|
            .        r_min        r_max
            .j_res:                           |---------|
            .                                r_min     r_max
            .                              bigger_min
            .                smaller_max
            .                       --------->
            .                        distance
            */
            bigger_min = r_min[i_res];
            if (r_min[j_res].x > bigger_min.x) bigger_min.x = r_min[j_res].x;
            if (r_min[j_res].y > bigger_min.y) bigger_min.y = r_min[j_res].y;
            if (r_min[j_res].z > bigger_min.z) bigger_min.z = r_min[j_res].z;
            smaller_max = r_max[i_res];
            if (r_max[j_res].x < smaller_max.x) smaller_max.x = r_max[j_res].x;
            if (r_max[j_res].y < smaller_max.y) smaller_max.y = r_max[j_res].y;
            if (r_max[j_res].z < smaller_max.z) smaller_max.z = r_max[j_res].z;
            res_dist = vector_vminusv(bigger_min, smaller_max);
            if (res_dist.x < 0) res_dist.x = 0;
            if (res_dist.y < 0) res_dist.y = 0;
            if (res_dist.z < 0) res_dist.z = 0;
            if (vdotv(res_dist, res_dist) > cutoff_far2) continue;

            /* add j_res into ngh list */
            prot.res[i_res].n_ngh++;
             n_ngh = prot.res[i_res].n_ngh;
            prot.res[i_res].ngh = realloc(prot.res[i_res].ngh, n_ngh*sizeof(void *));
            prot.res[i_res].ngh[n_ngh-1] = &prot.res[j_res];
            pairwise[i_res] = realloc(pairwise[i_res], n_ngh*sizeof(void *));
            pairwise[i_res][n_ngh-1] = malloc(prot.res[i_res].n_conf * sizeof(void *));
            if (!pairwise[i_res][n_ngh-1]) {printf("Memory Error\n"); return USERERR;}
            for (i_conf=1; i_conf<prot.res[i_res].n_conf; i_conf++) {
                pairwise[i_res][n_ngh-1][i_conf] = malloc(prot.res[j_res].n_conf * sizeof(float));
                if (!pairwise[i_res][n_ngh-1][i_conf]) {printf("Memory Error\n"); return USERERR;}
            }

            /* calculate vdw matrix for (i_res,j_res) */
            all_small = 1;
            for (i_conf=1; i_conf<prot.res[i_res].n_conf; i_conf++) {
                if (!prot.res[i_res].conf[i_conf].n_atom) continue;
                for (j_conf=1; j_conf<prot.res[j_res].n_conf; j_conf++) {
                    if (!prot.res[j_res].conf[j_conf].n_atom) continue;

                    pair_vdw = 0.;
                    for (i_atom=0; i_atom<prot.res[i_res].conf[i_conf].n_atom; i_atom++) {
                        iatom_p = &prot.res[i_res].conf[i_conf].atom[i_atom];
                        if (!iatom_p->on) continue;
                        v1 = iatom_p->xyz;
                        for (j_atom=0; j_atom<prot.res[j_res].conf[j_conf].n_atom; j_atom++) {
                            jatom_p = &prot.res[j_res].conf[j_conf].atom[j_atom];
                            if (!jatom_p->on) continue;
                            v2 = jatom_p->xyz;

                            d2 = ddvv(v1, v2);
                            if (d2 < cutoff_far2) {
                                done = 0;

                                if (d2 <= r12sq_max) {
                                    for (i_connect=0;i_connect<n_connect12[i_conf][i_atom];i_connect++) {
                                        if (jatom_p == connect12[i_conf][i_atom][i_connect]) {
                                            done = 1;
                                        }
                                    }
                                }
                                if (done) continue;

                                if (d2 <= r13sq_max) {
                                    for (i_connect=0;i_connect<n_connect13[i_conf][i_atom];i_connect++) {
                                        if (jatom_p == connect13[i_conf][i_atom][i_connect]) {
                                            done = 1;
                                        }
                                    }
                                }
                                if (done) continue;

                                /* calculate vdw */
                                if (d2 < cutoff_near2)
                                    e = VDW_ELIMIT_NEAR;            /* Cutoff */
                                else {
                                    C6 = C6_matrix[iatom_p->i_elem][jatom_p->i_elem];
                                    C12 = C12_matrix[iatom_p->i_elem][jatom_p->i_elem];
                                    d6 = d2*d2*d2;
                                    d12 = d6*d6;
                                    e = C12/d12 - C6/d6;
                                }

                                if (d2 <= r14sq_max) {
                                    for (i_connect=0;i_connect<n_connect14[i_conf][i_atom];i_connect++) {
                                        if (jatom_p == connect14[i_conf][i_atom][i_connect]) {

                                            pair_vdw += e * env.factor_14lj;
                                            done = 1;
                                        }
                                    }
                                }

                                if (!done) {
                                    pair_vdw += e;
                                }
                            }
                        }
                    }
                    /*
                    float diff = pair_vdw - vdw_conf(i_res,i_conf,j_res,j_conf, prot);
                    if (fabs(diff) > 1e-2) {
                        printf("%8.3f,%d,%d,%d,%d\n",diff,i_res,i_conf,j_res,j_conf);
                    }
                    */
                    //stat[i_res][j_res]++;  /* Debug */

                    /* H bond energy correction */
                    pair_vdw += hbond_extra(prot.res[i_res].conf[i_conf], prot.res[j_res].conf[j_conf]);

                    pairwise[i_res][n_ngh-1][i_conf][j_conf] = pair_vdw;
                    if (pair_vdw > env.ngh_vdw_thr ||pair_vdw < -env.ngh_vdw_thr) {
                        all_small = 0;
                    }
                }
            }
            /* if all pairwise are smaller than threshold, remove j_res from neighbor list */
            if (all_small) {
                for (i_conf=1; i_conf<prot.res[i_res].n_conf; i_conf++) free(pairwise[i_res][n_ngh-1][i_conf]);
                free(pairwise[i_res][n_ngh-1]);
                prot.res[i_res].n_ngh--;
                n_ngh = prot.res[i_res].n_ngh;
                prot.res[i_res].ngh = realloc(prot.res[i_res].ngh, n_ngh*sizeof(void *));
                pairwise[i_res] = realloc(pairwise[i_res], n_ngh*sizeof(void *));
            }
        }

        /*
        printf("%s %c%4d, n_ngh=%3d, sas=%8.2f%%\n",
        prot.res[i_res].resName,prot.res[i_res].chainID,prot.res[i_res].resSeq,prot.res[i_res].n_ngh,100.*prot.res[i_res].sas);
        */

        for (i_conf=1; i_conf<prot.res[i_res].n_conf; i_conf++) {
            if (!prot.res[i_res].conf[i_conf].n_atom) continue;
            for (i_atom=0; i_atom<prot.res[i_res].conf[i_conf].n_atom; i_atom++) {
                atom_p = &prot.res[i_res].conf[i_conf].atom[i_atom];
                if (!atom_p->on) continue;
                free(connect12[i_conf][i_atom]);
                free(connect13[i_conf][i_atom]);
                free(connect14[i_conf][i_atom]);
            }
            free(n_connect12[i_conf]);
            free(n_connect13[i_conf]);
            free(n_connect14[i_conf]);
            free(connect12[i_conf]);
            free(connect13[i_conf]);
            free(connect14[i_conf]);
        }
        free(n_connect12);
        free(n_connect13);
        free(n_connect14);
        free(connect12);
        free(connect13);
        free(connect14);
    }

    /*
    memset(vdw_counter,0,21*sizeof(int));
    vdw_stat = fopen("vdw_stat.dat","w");
    for (i_res=0; i_res<prot.n_res; i_res++) {
        if (!(prot.res[i_res].n_conf-1)) continue;
        for (j_res=0; j_res<prot.n_res; j_res++) {
            if (!(prot.res[j_res].n_conf-1)) continue;
            i_counter = 20.*(float)stat[i_res][j_res]/(float)((prot.res[i_res].n_conf-1)*(prot.res[j_res].n_conf-1));
            vdw_counter[i_counter]++;
            fprintf(vdw_stat, "%4d %4d %8d %8d %8.2f\n",
            i_res, j_res,
            stat[i_res][j_res],
            (prot.res[i_res].n_conf-1)*(prot.res[j_res].n_conf-1),
            100.*(float)stat[i_res][j_res]/(float)((prot.res[i_res].n_conf-1)*(prot.res[j_res].n_conf-1)));
        }
    }
    for (i_counter=0;i_counter<21;i_counter++) printf("%d\n", vdw_counter[i_counter]);
    */

    nowB= time(NULL);
    i=nowB - nowA;
    printf("      Actual time is %d\n", i);
    /* timing end  */

    /* prepare a radom microstate */
    state.n = prot.n_res;
    state.res = (int *) malloc(prot.n_res * sizeof(int));

    for (i=0; i<prot.n_res; i++) {
        for (j=1; j<prot.res[i].n_conf; j++) {
            prot.res[i].conf[j].counter = 0;
            prot.res[i].conf[j].on      = 0;
        }
    }


    printf("   Repacking in progress, see %s for details...\n", env.progress_log);fflush(stdout);

    pipe = 1;
    if (!(fp=(fopen(env.progress_log, "a")))) {
       printf("   WARNING: can not open file %s to dump messages, use stdout instead\n", env.progress_log);
       fp = stdout;
       pipe = 0;
    }
    fprintf(fp, "   Repacking:\n");fflush(fp);
    for (i=0; i<n; i++) {
        /* initialize the microstate */
        for (j=0; j<prot.n_res; j++) {
            if (prot.res[j].n_conf == 0) {
                printf("   FATAL: rot_pack(): empty residue %s%c%04d.\n", prot.res[j].resName,prot.res[j].chainID,prot.res[j].resSeq);
                return USERERR;
            }
            else if (prot.res[j].n_conf <= 2) state.res[j] = prot.res[j].n_conf - 1;
            else state.res[j] = rand() % (prot.res[j].n_conf-1) + 1;
            if (state.res[j])
                prot.res[j].conf[state.res[j]].on = 1;
        }

        fprintf(fp, "   Repacking %d:", i); fflush(fp);
        for (j=0; j<100; j++) { /* maximum converge trials */
            /* refine the microstate */
            flips = rot_refine(prot, state, pairwise);
            fprintf(fp, " %d", flips); fflush(fp);
            if (flips == 0) break;
        }
        fprintf(fp, "\n"); fflush(fp);

        /* write out the microstate as a pdbfile, for marilyn's presentation
        {  int mi, mk, mc;
            mc = 0;
            FILE *stream;
            char sbuff[256];

            sprintf(sbuff,"state%03d.pdb", i);
            stream = fopen(sbuff, "w");

            for (mi=0; mi<prot.n_res; mi++) {
                for (mk=0; mk<prot.res[mi].conf[0].n_atom; mk++) {
                    if (!prot.res[mi].conf[0].atom[mk].on) continue;
                    mc++;
                    fprintf(stream, "ATOM  %5d %4s%c%3s %c%4d%c   %8.3f%8.3f%8.3f            %5s\n",
                    mc, prot.res[mi].conf[0].atom[mk].name,
                    prot.res[mi].conf[0].atom[mk].altLoc,
                    prot.res[mi].conf[0].atom[mk].resName,
                    prot.res[mi].conf[0].atom[mk].chainID,
                    prot.res[mi].conf[0].atom[mk].resSeq,
                    prot.res[mi].conf[0].atom[mk].iCode,
                    prot.res[mi].conf[0].atom[mk].xyz.x,
                    prot.res[mi].conf[0].atom[mk].xyz.y,
                    prot.res[mi].conf[0].atom[mk].xyz.z,
                    prot.res[mi].conf[0].confName);
                }

                if (prot.res[mi].n_conf < 2) continue;
                for (mk=0; mk<prot.res[mi].conf[state.res[mi]].n_atom; mk++) {
                    if (!prot.res[mi].conf[state.res[mi]].atom[mk].on) continue;
                    mc++;
                    fprintf(stream, "ATOM  %5d %4s%c%3s %c%4d%c   %8.3f%8.3f%8.3f            %5s\n",
                    mc, prot.res[mi].conf[state.res[mi]].atom[mk].name,
                    prot.res[mi].conf[state.res[mi]].atom[mk].altLoc,
                    prot.res[mi].conf[state.res[mi]].atom[mk].resName,
                    prot.res[mi].conf[state.res[mi]].atom[mk].chainID,
                    prot.res[mi].conf[state.res[mi]].atom[mk].resSeq,
                    prot.res[mi].conf[state.res[mi]].atom[mk].iCode,
                    prot.res[mi].conf[state.res[mi]].atom[mk].xyz.x,
                    prot.res[mi].conf[state.res[mi]].atom[mk].xyz.y,
                    prot.res[mi].conf[state.res[mi]].atom[mk].xyz.z,
                    prot.res[mi].conf[state.res[mi]].confName);
                }
            }
            fclose(stream);
        }
        */

        /* increase counters */
        for (j=0; j<prot.n_res; j++) {
            prot.res[j].conf[state.res[j]].counter ++;
        }
    }
    if (pipe) fclose(fp);

    for (i=0; i<prot.n_res; i++) {
        float total_occ = 0;
        for (j=1; j<prot.res[i].n_conf; j++) {
            total_occ += prot.res[i].conf[j].counter;
        }
        for (j=1; j<prot.res[i].n_conf; j++) {
            //prot.res[i].conf[j].occ = (float) prot.res[i].conf[j].counter/ (float)n;
            prot.res[i].conf[j].occ = (float) prot.res[i].conf[j].counter/ total_occ;
        }
    }


    for (i_res=0; i_res<prot.n_res; i_res++) {
        int i_ngh;
        for (i_ngh = 0; i_ngh < prot.res[i_res].n_ngh; i_ngh++) {
            j_res = prot.res[i_res].ngh[i_ngh]->i_res_prot;
            if (j_res < i_res) continue;
            for (i_conf = 1; i_conf<prot.res[i_res].n_conf; i_conf++) {
                free(pairwise[i_res][i_ngh][i_conf]);
            }
            free(pairwise[i_res][i_ngh]);
        }
        free(pairwise[i_res]);
        free(prot.res[i_res].ngh);
        prot.res[i_res].ngh = NULL;
        prot.res[i_res].n_ngh = 0;
    }

    C = 0;
    for (i=0; i<prot.n_res; i++) {
        for (j=prot.res[i].n_conf_ori; j<prot.res[i].n_conf; j++) {
            /* if (prot.res[i].conf[j].history[2] != 'H' && prot.res[i].conf[j].occ <= env.repack_cutoff) { */
            if (prot.res[i].conf[j].occ <= env.repack_cutoff &&
                prot.res[i].conf[j].history[2] != 'E') {
                del_conf(&prot.res[i], j);
                j--;
                C++;
            }
        }
    }


    free(state.res);
    for (i_res = 0; i_res < prot.n_res; i_res++) {
        if (!prot.res[i_res].n_ngh) continue;
        prot.res[i_res].n_ngh = 0;
        free(prot.res[i_res].ngh);
        prot.res[i_res].ngh = NULL;
    }

    return C;
}

int rot_refine(PROT prot, MICROSTATE state, float ****pairwise) {
    int i, i_ngh;
    float E_min;
    char switching;
    int i_res, j_res, i_conf, j_conf, n_candidate, candidate[9999];
    int n;
    int iRes[9999];  /* random residue and conformer indices */
    float E_state[9999];
    n = 0;

    /* remove Purify UMR error */
    memset(E_state, 0, 9999*sizeof(float));

    shuffle_n(iRes, prot.n_res);
    for (i=0; i<prot.n_res; i++) {                  /* loop over residues */
        float repack_e_thr;
        i_res = iRes[i];
        if (prot.res[i_res].n_conf < 1) continue;
        i_conf = state.res[i_res];

        E_min = 99999.;
        for (i_conf=1; i_conf<prot.res[i_res].n_conf; i_conf++) {       /* calculate energy for every conformer */
            E_state[i_conf] = prot.res[i_res].conf[i_conf].E_self;
            for (i_ngh = 0; i_ngh < prot.res[i_res].n_ngh; i_ngh++) {
                j_res = prot.res[i_res].ngh[i_ngh]->i_res_prot;
                j_conf = state.res[j_res];
                if (i_res < j_res)
                    E_state[i_conf] += pairwise[i_res][i_ngh][i_conf][j_conf];
                else
                    E_state[i_conf] += pairwise[i_res][i_ngh][j_conf][i_conf];  /* if i_res > j_res, matrix is transposed, look at the place when pairwise are calculated */
            }
            if (E_state[i_conf] > 99999.) {
                E_state[i_conf] = 99999.;
            }
            if (E_state[i_conf] < E_min) {
                E_min = E_state[i_conf];
            }
        }

        //printf("%d,%8.3f\n",i_res,E_min);
        /* if difference btw current conformer and minimum is bigger than threshold, switch */
        switching = 0;
        i_conf = state.res[i_res];
        if (prot.res[i_res].sas > 0.5) repack_e_thr = env.repack_e_thr_exposed;
        else repack_e_thr = 2.*prot.res[i_res].sas*env.repack_e_thr_exposed + (1.-2.*prot.res[i_res].sas)*env.repack_e_thr_buried;
        if (E_state[i_conf] - E_min > repack_e_thr) {
            switching = 1;
            n_candidate = 0;
        }

        for (i_conf=1; i_conf<prot.res[i_res].n_conf; i_conf++) {
            //printf("%d,%8.3f,%8.3f,%8.3f,%8.3f\n",i_res,E_min,E_state[i_conf],E_state[i_conf] - E_min,env.repack_e_thr);
            if (E_state[i_conf] - E_min < repack_e_thr) {
                prot.res[i_res].conf[i_conf].counter++;
                if (switching) {
                    if (E_state[i_conf] - E_min < repack_e_thr) {
                        n_candidate++;
                        candidate[n_candidate-1] = i_conf;
                    }
                }
            }
        }

        if (switching) {
            i_conf = state.res[i_res];
            prot.res[i_res].conf[i_conf].on = 0;
            i_conf = candidate[(int)(ran2(&idum) * (float)n_candidate)];
            prot.res[i_res].conf[i_conf].on = 1;
            //printf("switch %3d, from %3d to %3d. n_candi=%3d. n_conf=%3d,E_min=%10.3f\n",i_res,state.res[i_res],i_conf,n_candidate,prot.res[i_res].n_conf,E_min);
            state.res[i_res] = i_conf;
            n++;
        }
    }

    return n;
}

void sp3_3known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR r3, VECTOR *r4, float bond_len_04);
void sp3_2known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR *r3, VECTOR *r4, float bond_len_03, float bond_angle_304);
void sp3_1known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR *r3, VECTOR *r4, VECTOR *r5, float bond_len_03, float bond_angle_103, float torsion_angle_3012);
void sp2_2known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR *r3, float bond_len_03);
void sp2_1known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR *r3, VECTOR *r4, float bond_len_03, float bond_angle_103, float torsion_angle_3012);
void sp2d_3known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR r3, VECTOR *r4, float bond_len_04);
void sp3d2_5known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR r3, VECTOR r4, VECTOR r5, VECTOR *r6, float bond_len_06);
float get_bond_length(CONF *conf_p, ATOM *atom1_p, ATOM *atom2_p);
float get_bond_angle(CONF *conf_p, ATOM *atom0_p, ATOM *atom1_p, ATOM *atom2_p, char *orbital);

int place_missing(PROT prot, int handle_addconf) {
    int         i_res, i_conf, i_atom, ins;
    FILE        *debug_fp;
    CONNECT     connect;
    char        orbital[10],sbuffer[5],name[MAXCHAR_LINE];
    RES         *res_p;
    CONF        *conf_p;
    ATOM        *atom_p, *back_atom_p;
    ATOM        *known_atoms[MAX_CONNECTED], *to_complete_atoms[MAX_CONNECTED], dummy_atom[MAX_CONNECTED];
    int         n_known, n_complete;
    int         i_connect, i_dummy, i_fold, i_complete;
    int         i_corner, j_corner, k_corner, l_corner, start;
    VECTOR      corners[4], v;
    float       bond_length, bond_angle, torsion_angle, a;
    TORS        tors;
    int         fatal = 0;
    int error;
    int kr,kc,ka;
    char sbuff[MAXCHAR_LINE],sbuff2[MAXCHAR_LINE], siatom[MAXCHAR_LINE];
    char resName[4];
    int  Missing, n_added=0;

    for (i_res=0; i_res<prot.n_res; i_res++) {
        res_p = &prot.res[i_res];
        for (i_conf=0; i_conf<prot.res[i_res].n_conf; i_conf++) {
            conf_p = &prot.res[i_res].conf[i_conf];
            for (i_atom=0; i_atom<prot.res[i_res].conf[i_conf].n_atom; i_atom++) {
                atom_p = &prot.res[i_res].conf[i_conf].atom[i_atom];
                if (atom_p->on) continue;

                sprintf(sbuffer,"%d",i_atom);
                if( param_get("ATOMNAME", conf_p->confName, sbuffer, name) ) {
                    strcpy(name, "    ");
                    fatal++;
                }
                while (strlen(name)<4) strcat(name, " ");
                strncpy(atom_p->name, name, 4);
                atom_p->name[4] = '\0';
		if (atom_p->name[1] != 'H') { /* heavy atom */
                    if(!param_get("CONNECT", conf_p->confName, atom_p->name, &connect) ) { /* find connectivity */
                        strip(orbital, connect.orbital);
                        if (strcmp(orbital,"ion")) { /* Not ion */
                            if (!strcmp(atom_p->name, " N  ") || !strcmp(atom_p->name, " CA ")) {
                                if (i_res>0) {
                                    if (!strcmp(prot.res[i_res-1].resName, "NTR") || !strcmp(prot.res[i_res-1].resName, "NTG")) {
                                        continue;
                                    }
                                }
                            }
                            if (!strcmp(atom_p->name, " C  ") || !strcmp(atom_p->name, " O  ")) {
                                if (i_res<prot.n_res-1) {
                                    if (!strcmp(prot.res[i_res+1].resName, "CTR")){
                                        continue;
                                    }
                                }
                            }

                            printf("    Warning! May add heavy atom %s onto residue \"%s %c%04d\"\n",atom_p->name, res_p->resName, res_p->chainID, res_p->resSeq);
                        }
                    }
                }
            }
        }
    }

    get_connect12(prot);

    error = 0;
    for (i_res=0; i_res<prot.n_res; i_res++) {
        res_p = &prot.res[i_res];
        for (i_conf=0; i_conf<prot.res[i_res].n_conf; i_conf++) {
            int k_conf; /* used for checking duplicates later in the loop */
            conf_p = &prot.res[i_res].conf[i_conf];
            for (i_atom=0; i_atom<prot.res[i_res].conf[i_conf].n_atom; i_atom++) {

                atom_p = &prot.res[i_res].conf[i_conf].atom[i_atom];
                if (!atom_p->on) continue;
                if( param_get("CONNECT", conf_p->confName, atom_p->name, &connect) ) {
                    debug_fp = fopen(env.debug_log, "a");
                    fprintf(debug_fp, "   Error! place_missing(): Can't find CONNECT parameter of conformer \"%s\" atom \"%s\"\n", conf_p->confName, atom_p->name);
                    fclose(debug_fp);
                    continue;
                }
                //printf("orbital %s\n",connect.orbital);
                strip(orbital, connect.orbital);
                if (!strcmp(orbital,"ion")) continue;

                /* Collect known atoms and unknown atoms */
                memset(known_atoms,0,MAX_CONNECTED*sizeof(void *));
                memset(to_complete_atoms,0,MAX_CONNECTED*sizeof(void *));
                n_known = 0;
                n_complete = 0;
                for (i_connect=0; i_connect < MAX_CONNECTED; i_connect++) {
                    if (!atom_p->connect12[i_connect]) break;
                    if (atom_p->connect12[i_connect]->on) {
                        n_known++;
                        known_atoms[n_known-1] = atom_p->connect12[i_connect];
                    }
                    else {
                        n_complete++;
                        to_complete_atoms[n_complete-1] = atom_p->connect12[i_connect];
                    }
                }

                if (!n_complete) continue;

                if (!strcmp(orbital, "sp3")) {
                    /* If total number connected atoms is less than 4 for sp3, use dummy atoms to complete 4 slots */
                    if ( (n_known + n_complete) < 4 ) {
                        for (i_dummy = n_complete; i_dummy < (4-n_known); i_dummy++) {
                            to_complete_atoms[i_dummy] = &dummy_atom[i_dummy];
                        }
                    }

                    if (n_known == 3) {
                        bond_length = get_bond_length(conf_p,atom_p,to_complete_atoms[0]);

                        sp3_3known( atom_p->xyz,
                        known_atoms[0]->xyz,
                        known_atoms[1]->xyz,
                        known_atoms[2]->xyz,
                        &to_complete_atoms[0]->xyz,
                        bond_length );

                        for (i_complete=0; i_complete<n_complete; i_complete++) to_complete_atoms[i_complete]->on = 1;
                        n_added++;
                        get_connect12_conf(i_res,i_conf,prot);
                    }
                    else if (n_known == 2) {
                        bond_length = get_bond_length(conf_p,atom_p,to_complete_atoms[0]);
                        //bond_angle = 109.;
                        bond_angle = get_bond_angle(conf_p, atom_p, known_atoms[0], to_complete_atoms[0], orbital);

                        sp3_2known( atom_p->xyz,
                        known_atoms[0]->xyz,
                        known_atoms[1]->xyz,
                        &to_complete_atoms[0]->xyz,
                        &to_complete_atoms[1]->xyz,
                        bond_length,
                        bond_angle);

                        for (i_complete=0; i_complete<n_complete; i_complete++) to_complete_atoms[i_complete]->on = 1;
                        n_added++;
                        get_connect12_conf(i_res,i_conf,prot);

                        if (!i_conf) continue;
                        if (!handle_addconf) continue;
                        if ( (n_known + n_complete) < 4 ) {
                            //printf("   Debugging! Case sp3, n_known = 2, n_complete = 1\n");
                            ins = ins_conf(res_p, res_p->n_conf, conf_p->n_atom);
                            if (ins == USERERR) return USERERR;
                            conf_p = &prot.res[i_res].conf[i_conf];
                            if (cpy_conf(&res_p->conf[res_p->n_conf-1], conf_p)) {
                                printf("   Error! place_missing(): couldn't copy the conformer \"%s\" in residue %s %d, to new position k_conf = %d\n",conf_p->confName,res_p->resName, res_p->resSeq, res_p->n_conf-1); fatal++;
                            }
                            get_connect12_conf(i_res, res_p->n_conf-1, prot);

                            sp3_2known( atom_p->xyz,
                            known_atoms[0]->xyz,
                            known_atoms[1]->xyz,
                            &to_complete_atoms[1]->xyz,
                            &to_complete_atoms[0]->xyz,
                            bond_length,
                            bond_angle);

                            for (i_complete=0; i_complete<n_complete; i_complete++) to_complete_atoms[i_complete]->on = 1;
                            n_added++;
                            get_connect12_conf(i_res, i_conf, prot);
                        }
                    }
                    else if (n_known == 1) {
                        //printf("   Debugging! residue %s%4d,conformer %s\n", res_p->resName,res_p->resSeq,conf_p->confName);
                        /* Get TORSION parameter, if not exit then make one */
                        if ( param_get("TORSION",conf_p->confName, to_complete_atoms[0]->name, &tors) ) {
                            strncpy(resName, conf_p->confName, 3); resName[3] = '\0';
                            if ( param_get("TORSION",resName, to_complete_atoms[0]->name, &tors) ) {
                                memset(&tors,0,sizeof(TORS));
                                strcpy(tors.atom1, atom_p->name);
                                strcpy(tors.atom2, known_atoms[0]->name);
                                for (i_connect=0; i_connect < MAX_CONNECTED; i_connect++) {
                                    if (!known_atoms[0]->connect12[i_connect]) break;
                                    if (!known_atoms[0]->connect12[i_connect]->on) continue;
                                    if (!strcmp(known_atoms[0]->connect12[i_connect]->name, atom_p->name)) continue;
                                    strcpy(tors.atom3, known_atoms[0]->connect12[i_connect]->name);
                                    break;
                                }
                                tors.V2[0]     = 0.;
                                tors.n_fold[0] = 3.;
                                tors.gamma[0]  = 0.;
                                tors.opt_hyd = 0;
                                param_sav("TORSION", conf_p->confName, to_complete_atoms[0]->name, &tors, sizeof(TORS));
                                debug_fp = fopen(env.debug_log, "a");
                                fprintf(debug_fp, "TORSION  %s %s %s %s %s  f  %9.1f %9.0f %9.2f\n",
                                conf_p->confName,
                                to_complete_atoms[0]->name,
                                tors.atom1,
                                tors.atom2,
                                tors.atom3,
                                tors.V2[0],
                                tors.n_fold[0],
                                tors.gamma[0]);
                                fclose(debug_fp);
                                error++;
                            }
                        }

                        if (strcmp(atom_p->name, tors.atom1)) {
                            printf("   Error! Atom %s is in torsion parameter of conformer %s atom %s, but connected atom %s was found\n",
                            tors.atom1,conf_p->confName,to_complete_atoms[0]->name,atom_p->name);
                            continue;
                        }
                        if (strcmp(known_atoms[0]->name, tors.atom2)) {
                            printf("   Error! Atom %s is in torsion parameter of conformer %s atom %s, but connected atom %s was found\n",
                            tors.atom2,conf_p->confName,to_complete_atoms[0]->name,known_atoms[0]->name);
                            continue;
                        }
                        back_atom_p = NULL;
                        for (i_connect=0; i_connect < MAX_CONNECTED; i_connect++) {
                            if (!known_atoms[0]->connect12[i_connect]) break;
                            if (!known_atoms[0]->connect12[i_connect]->on) continue;
                            if (!strcmp(known_atoms[0]->connect12[i_connect]->name, tors.atom3)) {back_atom_p = known_atoms[0]->connect12[i_connect]; break;}
                        }
                        if (!back_atom_p) {
                            printf("   Error! place_missing(): cannot put hydrogen on atom %s in residue %s %d, because %s-%s- N/A is unexpected.\n",
                            atom_p->name, res_p->resName, res_p->resSeq, atom_p->name, known_atoms[0]->name);
                            continue;
                        }

                        bond_length = get_bond_length(conf_p,atom_p,to_complete_atoms[0]);
                        bond_angle  = get_bond_angle(conf_p, atom_p, known_atoms[0], to_complete_atoms[0], orbital);
                        //bond_angle = 109.;

                        for (i_fold=0; i_fold<tors.n_fold[0]; i_fold++) {
                            if (i_fold) {
                                if (!handle_addconf) break;

                                ins = ins_conf(res_p, res_p->n_conf, conf_p->n_atom);
                                if (ins == USERERR) return USERERR;
                                conf_p = &prot.res[i_res].conf[i_conf];
                                if (cpy_conf(&res_p->conf[res_p->n_conf-1], conf_p)) {printf("   Error! place_missing(): couldn't copy the conformer \"%s\" in residue %s %d, to new position k_conf = %d\n",conf_p->confName,res_p->resName, res_p->resSeq, res_p->n_conf-1); fatal++;}
                                get_connect12_conf(i_res, res_p->n_conf-1, prot);
                                //printf("   Debugging! residue %s%4d,conformer %s to conformer %s\n", res_p->resName,res_p->resSeq,conf_p->confName,res_p->conf[res_p->n_conf-1].confName);
                            }

                            torsion_angle = (env.PI + tors.gamma[0] + (i_fold+1.)*2.*env.PI)/tors.n_fold[0];
                            sp3_1known(atom_p->xyz,
                            known_atoms[0]->xyz,
                            back_atom_p->xyz,
                            &to_complete_atoms[0]->xyz,
                            &to_complete_atoms[1]->xyz,
                            &to_complete_atoms[2]->xyz,
                            bond_length,
                            bond_angle,
                            torsion_angle );

                            //printf("   Debugging! i_fold %d, n_fold %f, residue %s%4d,history %s,hyd_multi %d, pos %d\n",i_fold,tors.n_fold, res_p->resName,res_p->resSeq,conf_p->history,hyd_multi,i_conf);
                            //printf("   Debugging2! residue%d %s%4d,conformer%d %s and last conformer %s\n", i_res, res_p->resName,res_p->resSeq,i_conf,conf_p->confName,res_p->conf[res_p->n_conf-1].confName);
                            for (i_complete=0; i_complete<n_complete; i_complete++) to_complete_atoms[i_complete]->on = 1;
                            n_added++;
                            get_connect12_conf(i_res, i_conf, prot);
                        }
                    }
                    else if (n_known == 0) {
                        int counter;
                        bond_length = get_bond_length(conf_p,atom_p,to_complete_atoms[0]);
                        /* Define four corners of the box, atom would be placed on the conner. */
                        a = bond_length/sqrt(3.);
                        counter = 0;
                        v.x =  a; v.y =  a; v.z =  a; corners[0] = vector_vplusv(atom_p->xyz, v);
                        v.x = -a; v.y = -a; v.z =  a; corners[1] = vector_vplusv(atom_p->xyz, v);
                        v.x = -a; v.y =  a; v.z = -a; corners[2] = vector_vplusv(atom_p->xyz, v);
                        v.x =  a; v.y = -a; v.z = -a; corners[3] = vector_vplusv(atom_p->xyz, v);
                        for (i_corner = 0; i_corner < 4; i_corner++) {

                            if (n_complete <=1) start = 3; else start = i_corner+1;
                            for (j_corner = start; j_corner < 4; j_corner++) {

                                if (n_complete <=2) start = 3; else start = j_corner+1;
                                for (k_corner = start; k_corner < 4; k_corner++) {

                                    if (n_complete <=3) start = 3; else start = k_corner+1;
                                    for (l_corner = start; l_corner < 4; l_corner++) {

                                        to_complete_atoms[0]->xyz = corners[i_corner];
                                        to_complete_atoms[1]->xyz = corners[j_corner];
                                        to_complete_atoms[2]->xyz = corners[k_corner];
                                        to_complete_atoms[3]->xyz = corners[l_corner];

                                        to_complete_atoms[0]->on = 1;
                                        to_complete_atoms[1]->on = 1;
                                        to_complete_atoms[2]->on = 1;
                                        to_complete_atoms[3]->on = 1;
                                        n_added++;

                                        if (!handle_addconf) continue;

                                        ins = ins_conf(res_p, res_p->n_conf, conf_p->n_atom);
                                        if (ins == USERERR) return USERERR;
                                        conf_p = &prot.res[i_res].conf[i_conf];
                                        if (cpy_conf(&res_p->conf[res_p->n_conf-1], conf_p)) {printf("   Error! place_missing(): couldn't copy the conformer \"%s\" in residue %s %d, to new position k_conf = %d\n",conf_p->confName,res_p->resName, res_p->resSeq, res_p->n_conf-1); fatal++;}
                                        get_connect12_conf(i_res, i_conf, prot);
                                    }
                                }
                            }
                        }

                        v.x = -a; v.y = -a; v.z = -a; corners[0] = vector_vplusv(atom_p->xyz, v);
                        v.x =  a; v.y =  a; v.z = -a; corners[1] = vector_vplusv(atom_p->xyz, v);
                        v.x =  a; v.y = -a; v.z =  a; corners[2] = vector_vplusv(atom_p->xyz, v);
                        v.x = -a; v.y =  a; v.z =  a; corners[3] = vector_vplusv(atom_p->xyz, v);
                        for (i_corner = 0; i_corner < 4; i_corner++) {

                            if (n_complete <=1) start = 3; else start = i_corner+1;
                            for (j_corner = start; j_corner < 4; j_corner++) {

                                if (n_complete <=2) start = 3; else start = j_corner+1;
                                for (k_corner = start; k_corner < 4; k_corner++) {

                                    if (n_complete <=3) start = 3; else start = k_corner+1;
                                    for (l_corner = start; l_corner < 4; l_corner++) {

                                        to_complete_atoms[0]->xyz = corners[i_corner];
                                        to_complete_atoms[1]->xyz = corners[j_corner];
                                        to_complete_atoms[2]->xyz = corners[k_corner];
                                        to_complete_atoms[3]->xyz = corners[l_corner];

                                        to_complete_atoms[0]->on = 1;
                                        to_complete_atoms[1]->on = 1;
                                        to_complete_atoms[2]->on = 1;
                                        to_complete_atoms[3]->on = 1;
                                        n_added++;

                                        if (!handle_addconf) continue;
                                        ins = ins_conf(res_p, res_p->n_conf, conf_p->n_atom);
                                        if (ins == USERERR) return USERERR;
                                        conf_p = &prot.res[i_res].conf[i_conf];
                                        if (cpy_conf(&res_p->conf[res_p->n_conf-1], conf_p)) {printf("   Error! place_missing(): couldn't copy the conformer \"%s\" in residue %s %d, to new position k_conf = %d\n",conf_p->confName,res_p->resName, res_p->resSeq, res_p->n_conf-1); fatal++;}
                                        get_connect12_conf(i_res, res_p->n_conf-1, prot);
                                    }
                                }
                            }
                        }

                        if (handle_addconf) {
                            del_conf(res_p, res_p->n_conf-1);
                        }
                        conf_p = &prot.res[i_res].conf[i_conf];
                        continue;
                    }
                    else {
                        printf("   Error! place_missing(): number of known atoms can't be handled, sp3 with %i known atoms\n",n_known);
                        fatal++;
                    }
                }
                else if (!strcmp(orbital, "sp2")) {
                    if ( (n_known + n_complete) < 3 ) {
                        for (i_dummy = n_complete; i_dummy < (3-n_known); i_dummy++) {
                            to_complete_atoms[i_dummy] = &dummy_atom[i_dummy];
                        }
                    }
                    if (n_known == 2) {
                        bond_length = get_bond_length(conf_p,atom_p,to_complete_atoms[0]);

                        sp2_2known( atom_p->xyz,
                        known_atoms[0]->xyz,
                        known_atoms[1]->xyz,
                        &to_complete_atoms[0]->xyz,
                        bond_length );

                        if (atom_p->connect12[2])
                        //printf("   Debugging2! residue%d %s%4d,conformer%d %s atom %s connect to %s, n_complete=%d\n", i_res, res_p->resName,res_p->resSeq,i_conf,conf_p->confName, atom_p->name, atom_p->connect12[2]->name,n_complete);
                        for (i_complete=0; i_complete<n_complete; i_complete++) to_complete_atoms[i_complete]->on = 1;
                        n_added++;
                        get_connect12_conf(i_res, i_conf, prot);
                    }
                    else if (n_known == 1) {
                        if ( param_get("TORSION",conf_p->confName, to_complete_atoms[0]->name, &tors) ) {
                            strncpy(resName, conf_p->confName, 3); resName[3] = '\0';
                            if ( param_get("TORSION",resName, to_complete_atoms[0]->name, &tors) ) {
                                memset(&tors,0,sizeof(TORS));
                                strcpy(tors.atom1, atom_p->name);
                                strcpy(tors.atom2, known_atoms[0]->name);
                                for (i_connect=0; i_connect < MAX_CONNECTED; i_connect++) {
                                    if (!known_atoms[0]->connect12[i_connect]) break;
                                    if (!known_atoms[0]->connect12[i_connect]->on) continue;
                                    if (!strcmp(known_atoms[0]->connect12[i_connect]->name, atom_p->name)) continue;
                                    strcpy(tors.atom3, known_atoms[0]->connect12[i_connect]->name);
                                    break;
                                }
                                tors.V2[0]     = 0.;
                                tors.n_fold[0] = 2.;
                                tors.gamma[0]  = 180.;
                                tors.opt_hyd = 0;
                                debug_fp = fopen(env.debug_log, "a");
                                fprintf(debug_fp, "TORSION  %s %s %s %s %s  f  %9.1f %9.0f %9.2f\n",
                                conf_p->confName,
                                to_complete_atoms[0]->name,
                                tors.atom1,
                                tors.atom2,
                                tors.atom3,
                                tors.V2[0],
                                tors.n_fold[0],
                                tors.gamma[0]);
                                fclose(debug_fp);
                                error++;
                            }
                        }
                        if (strcmp(atom_p->name, tors.atom1)) {
				printf("   Error! Cannot add atom \'%s\' onto \'%s\' because TORSION parameter of atom \'%s\' doesn't match the connected atoms in this protein, double check the TORSION parameter. (Res: %s %c%04d)\n",
				to_complete_atoms[0]->name,atom_p->name,to_complete_atoms[0]->name,res_p->resName,res_p->chainID,res_p->resSeq);
				continue;
			}
                        if (strcmp(known_atoms[0]->name, tors.atom2)) {
                                printf("   Error! Cannot add atom \'%s\' onto \'%s\' because TORSION parameter of atom \'%s\' doesn't match the connected atoms in this protein, double check the TORSION parameter. (Res: %s %c%04d)\n",
                                to_complete_atoms[0]->name,atom_p->name,to_complete_atoms[0]->name,res_p->resName,res_p->chainID,res_p->resSeq);
                                continue;
                        }
                        back_atom_p = NULL;
                        for (i_connect=0; i_connect < MAX_CONNECTED; i_connect++) {
                            if (!known_atoms[0]->connect12[i_connect]) break;
                            if (!known_atoms[0]->connect12[i_connect]->on) continue;
                            if (!strcmp(known_atoms[0]->connect12[i_connect]->name, tors.atom3)) {back_atom_p = known_atoms[0]->connect12[i_connect]; break;}
                        }
                        if (!back_atom_p) {
                                printf("   Error! Cannot add atom \'%s\' onto \'%s\' because TORSION parameter of atom \'%s\' doesn't match the connected atoms in this protein, double check the TORSION parameter. (Res: %s %c%04d)\n",
                                to_complete_atoms[0]->name,atom_p->name,to_complete_atoms[0]->name,res_p->resName,res_p->chainID,res_p->resSeq);
                                continue;
                        }

                        bond_length = get_bond_length(conf_p,atom_p,to_complete_atoms[0]);
                        bond_angle = get_bond_angle(conf_p, atom_p, known_atoms[0], to_complete_atoms[0], orbital);
                        //bond_angle = 120.;

                        for (i_fold=0; i_fold<tors.n_fold[0]; i_fold++) {
                            if (i_fold) {
                                if (!i_conf) break;
                                if (!handle_addconf) break;
                                ins = ins_conf(res_p, res_p->n_conf, conf_p->n_atom);
                                if (ins == USERERR) return USERERR;
                                conf_p = &prot.res[i_res].conf[i_conf];
                                if (cpy_conf(&res_p->conf[res_p->n_conf-1], conf_p)) {printf("   Error! place_missing(): couldn't copy the conformer \"%s\" in residue %s %d, to new position k_conf = %d\n",conf_p->confName,res_p->resName, res_p->resSeq, res_p->n_conf-1); fatal++;}
                                get_connect12_conf(i_res, res_p->n_conf-1, prot);
                            }
                            torsion_angle = (env.PI + tors.gamma[0] + (i_fold+1.)*2.*env.PI)/tors.n_fold[0];
                            sp2_1known(atom_p->xyz,
                            known_atoms[0]->xyz,
                            back_atom_p->xyz,
                            &to_complete_atoms[0]->xyz,
                            &to_complete_atoms[1]->xyz,
                            bond_length,
                            bond_angle,
                            torsion_angle );

                            for (i_complete=0; i_complete<n_complete; i_complete++) to_complete_atoms[i_complete]->on = 1;
                            n_added++;
                            get_connect12_conf(i_res,i_conf,prot);
                        }
                    }
                    else if (n_known == 0) {
                        printf("   Error! no rule to add atom \'%s\' onto \'%s\': (atom %s in conformer %s have no atom connected)\n",to_complete_atoms[0]->name, atom_p->name, atom_p->name,conf_p->confName);
                    }
                    else {
                        printf("   Error! place_missing(): number of connected atoms wrong, %i n_connected atoms\n",n_known);
                        fatal++;
                    }
                }
                else if (!strcmp(orbital, "sp2d")) {
                    if ( (n_known + n_complete) < 4 ) {
                        for (i_dummy = n_complete; i_dummy < (4-n_known); i_dummy++) {
                            to_complete_atoms[i_dummy] = &dummy_atom[i_dummy];
                        }
                    }

                    if (n_known == 3) {
                        bond_length = get_bond_length(conf_p,atom_p,to_complete_atoms[0]);

                        sp2d_3known( atom_p->xyz,
                        known_atoms[0]->xyz,
                        known_atoms[1]->xyz,
                        known_atoms[2]->xyz,
                        &to_complete_atoms[0]->xyz,
                        bond_length );

                        for (i_complete=0; i_complete<n_complete; i_complete++) to_complete_atoms[i_complete]->on = 1;
                        n_added++;
                        get_connect12_conf(i_res,i_conf,prot);
                    }
                    else {
                        printf("   Error! place_missing(): number of known atoms can't be handled, sp2d with %i known atoms\n",n_known);
                        fatal++;
                    }
                }
                else if (!strcmp(orbital, "sp3d2")) {
                    if ( (n_known + n_complete) < 6 ) {
                        for (i_dummy = n_complete; i_dummy < (6-n_known); i_dummy++) {
                            to_complete_atoms[i_dummy] = &dummy_atom[i_dummy];
                        }
                    }

                    if (n_known == 5) {
                        bond_length = get_bond_length(conf_p,atom_p,to_complete_atoms[0]);

                        sp3d2_5known( atom_p->xyz,
                        known_atoms[0]->xyz,
                        known_atoms[1]->xyz,
                        known_atoms[2]->xyz,
                        known_atoms[3]->xyz,
                        known_atoms[4]->xyz,
                        &to_complete_atoms[0]->xyz,
                        bond_length );

                        for (i_complete=0; i_complete<n_complete; i_complete++) to_complete_atoms[i_complete]->on = 1;
                        n_added++;
                        get_connect12_conf(i_res,i_conf,prot);
                    }
                    else {
                        printf("   Error! place_missing(): number of known atoms can't be handled, sp3d2 with %i known atoms\n",n_known);
                        fatal++;
                    }
                }
                else {
                    printf("   Error! place_missing(): Don't know yet how to deal with orbital type %s for atom %s in conformer %s\n",orbital,atom_p->name, conf_p->confName);
                }
                /* delete dulipcates */
                for (k_conf=prot.res[i_res].n_conf-1; k_conf>=1; k_conf--) {
                   int j_conf;
                   for (j_conf=1; j_conf<k_conf; j_conf++) {
                       if( strcmp(prot.res[i_res].conf[k_conf].confName, prot.res[i_res].conf[j_conf].confName)) continue;
                       if(!cmp_conf(prot.res[i_res].conf[k_conf], prot.res[i_res].conf[j_conf], 0.01)) {
                          del_conf(&prot.res[i_res], k_conf);
                          break;
                       }
                   }
                }

                /* END of this atom */
            }
        }

        for (i_conf=prot.res[i_res].n_conf-1; i_conf>=1; i_conf--) {
            int j_conf;
            for (j_conf=1; j_conf<i_conf; j_conf++) {
                if( strcmp(prot.res[i_res].conf[i_conf].confName, prot.res[i_res].conf[j_conf].confName)) continue;
                if(!cmp_conf(prot.res[i_res].conf[i_conf], prot.res[i_res].conf[j_conf], 0.01)) {
                    del_conf(&prot.res[i_res], i_conf);
                    break;
                }
            }
        }

        while (1) {
            int  counter, j_conf;
            for (i_conf=0; i_conf<res_p->n_conf; i_conf++) {
                if (!strncmp(res_p->conf[i_conf].history+6,"____",4)) break;
            }
            if (i_conf >= prot.res[i_res].n_conf) break;
            counter = 0;
            for (j_conf=0; j_conf<res_p->n_conf; j_conf++) {
                if (strncmp(res_p->conf[i_conf].history,res_p->conf[j_conf].history,6)) continue;
                res_p->conf[j_conf].history[6] = 'M';
                sprintf(sbuff,"%03d",counter);
                strncpy(res_p->conf[j_conf].history+7,sbuff,3);
                counter++;
            }
        }
    }

    /* Check for if there are still missing atoms after protonation. the way of treating NTR and CTR here is not good and standard */
    if (!n_added) {
        Missing = 0;
        for (kr=0; kr<prot.n_res; kr++) {
            for (kc=0; kc<prot.res[kr].n_conf; kc++) {
                for (ka=0; ka<prot.res[kr].conf[kc].n_atom; ka++) {
                    if (!prot.res[kr].conf[kc].atom[ka].on) {
                        sprintf(siatom, "%3d", ka);
                        if (param_get("ATOMNAME", prot.res[kr].conf[kc].confName, siatom, sbuff)) {
                            printf("   Missing ATOMNAME records for slot \"%d\" of conformer %s.\n",
                            ka, prot.res[kr].conf[kc].confName);
                            Missing++;
                        }
                        else {
                            if (!strcmp(prot.res[kr-1].resName, "NTR") || !strcmp(prot.res[kr-1].resName, "NTG")) {
                                if (!strncmp(sbuff+1,"HA",2)) sbuff[0] = ' ';
                                if (!strncmp(sbuff+1,"H ",2)) sbuff[0] = '1';
                                if (!strncmp(sbuff+1,"H\0",2)) sbuff[0] = '1';
                                if (!param_get("IATOM","NTRBK",sbuff,sbuff2)) continue;
                                if (!param_get("IATOM","NTR01",sbuff,sbuff2)) continue;
                                if (!param_get("IATOM","NTGBK",sbuff,sbuff2)) continue;
                                if (!param_get("IATOM","NTG01",sbuff,sbuff2)) continue;
                            }
                            if (!strcmp(prot.res[kr+1].resName, "CTR")) {
                                sbuff[0] = ' ';
                                if (!param_get("IATOM","CTRBK",sbuff,sbuff2)) continue;
                                if (!param_get("IATOM","CTR01",sbuff,sbuff2)) continue;
                            }
                            printf("n_atom = %d\n", prot.res[kr].conf[kc].n_atom);
                            printf("   Missing atom %s at slot %4d of conf %s in \"%s %c %3d %02d\".\n",
                            sbuff, ka, prot.res[kr].conf[kc].confName, prot.res[kr].resName, prot.res[kr].chainID, prot.res[kr].resSeq,kc);
                            Missing++;
                        }
                    }
                }
            }
        }
    }
    if (error) {
       printf("   %d TORSION parameters were guessed and recorded in file %s.\n", error, env.debug_log);
       printf("   Modify and put them into a param file to have the full control.\n" );
    }
    return n_added;
}

void sp3_3known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR r3, VECTOR *r4, float bond_len_04) {

    /*
    .     r4
    .     |
    .     r0
    .    /|\
    .  r1 r2 r3
    .
    . r0 is sp3 type.
    . r0, r1, r2, r3's coordinates are known.
    . r4's coordinate is to be determined.
    */

    VECTOR n01;
    VECTOR n02;
    VECTOR n03;
    VECTOR n04;

    n01 = vector_normalize(vector_vminusv(r1, r0));
    n02 = vector_normalize(vector_vminusv(r2, r0));
    n03 = vector_normalize(vector_vminusv(r3, r0));
    n04 = vector_normalize(vector_neg(vector_sum3v(n01,n02,n03)));

    if (r4) *r4  = vector_vplusv(r0, vector_rescale(n04, bond_len_04));
}

void sp3_2known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR *r3, VECTOR *r4, float bond_len_03, float bond_angle_304) {

    /*
    .    r3 r4
    .     \/
    .     r0
    .    /|
    .  r1 r2
    .
    .
    . r0 is sp3 type.
    . r0, r1, r2's coordinates are known.
    . r3, r4's coordinates are to be determined.
    */

    float half_angle;

    VECTOR n01;
    VECTOR n02;
    VECTOR bisect304;	/* bisector of angle r3-r0-r4 */
    VECTOR norm102;	/* normal of plane r1-r0-r2 */

    n01 = vector_normalize(vector_vminusv(r1, r0));
    n02 = vector_normalize(vector_vminusv(r2, r0));

    bisect304 = vector_normalize(vector_neg(vector_vplusv(n01,n02)));

    norm102 = vector_normalize(vector_vxv(n01, n02));

    half_angle = bond_angle_304 / 2.;

    if (r3) *r3 = vector_vplusv(
                        r0,
                        vector_rescale(
                                vector_vplusv(
                                        vector_rescale(bisect304,cos(half_angle)),
                                        vector_rescale(norm102,sin(half_angle))),
                                bond_len_03));
    if (r4) *r4 = vector_vplusv(
                        r0,
                        vector_rescale(
                                vector_vminusv(
                                        vector_rescale(bisect304,cos(half_angle)),
                                        vector_rescale(norm102,sin(half_angle))),
                                bond_len_03));

    //if (r3) *r3 = r0[i] + bond_len_03 * (bisect304[i] * cos(half_angle) + norm102[i] * sin(half_angle));
    //if (r4) *r4 = r0[i] + bond_len_03 * (bisect304[i] * cos(half_angle) - norm102[i] * sin(half_angle));
}

void sp3_1known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR *r3, VECTOR *r4, VECTOR *r5, float bond_len_03, float bond_angle_103, float torsion_angle_3012) {

    /*
    .	r3 r4 r5
    .	  \|/
    .	   r0			  z (n10)
    .	   |			  |_ y (norm_0_1_n012)
    .	   r1			 /
    .	   \			x (norm012)
    .	    r2
    . r0 is sp3 type.
    . r0, r1, r2's coordinates are known.
    . r3, r4, r5's coordinates are to be determined.
    */


    VECTOR n10;
    VECTOR n12;
    VECTOR norm012;	/* normal of plane r0-r1-r2 */
    VECTOR norm_0_1_n012;	/* normal of plane r0-r1-norm012 */

    float theta;
    float phi;

    n10 = vector_normalize(vector_vminusv(r0, r1));
    n12 = vector_normalize(vector_vminusv(r2, r1));

    norm012 = vector_normalize(vector_vxv(n10,n12));
    norm_0_1_n012 = vector_normalize(vector_vxv(n10,norm012));

    theta = env.PI - bond_angle_103;
    if (r3) {
        phi = torsion_angle_3012 - env.PI/2.;
        *r3 = vector_vplusv(r0, vector_rescale(
                vector_sum3v(   vector_rescale(norm012,cos(phi)*sin(theta)),
                                vector_rescale(norm_0_1_n012,sin(phi)*sin(theta)),
                                vector_rescale(n10,cos(theta))),
                bond_len_03));
    }
    if (r4) {
        phi = torsion_angle_3012 + env.PI/6.;
        *r4 = vector_vplusv(r0, vector_rescale(
                vector_sum3v(   vector_rescale(norm012,cos(phi)*sin(theta)),
                                vector_rescale(norm_0_1_n012,sin(phi)*sin(theta)),
                                vector_rescale(n10,cos(theta))),
                bond_len_03));
    }
    if (r5) {
        phi = torsion_angle_3012 + 5.*env.PI/6.;
        *r5 = vector_vplusv(r0, vector_rescale(
                vector_sum3v(   vector_rescale(norm012,cos(phi)*sin(theta)),
                                vector_rescale(norm_0_1_n012,sin(phi)*sin(theta)),
                                vector_rescale(n10,cos(theta))),
                bond_len_03));
    }

    /* ^       ^                      ^                      ^              */
    /* r = r ( i*cos(phi)sin(theta) + j*sin(phi)sin(theta) + k*cos(theta) ) */
    /*
    if (r3) {
        for (i=0; i<3; i++) {
            r3[i] = r0[i] + bond_len_03 * (norm012[i]*cos(phi)*sin(theta)
            +                        norm_0_1_n012[i]*sin(phi)*sin(theta)
            +                                  n10[i]         *cos(theta) );
        }
    }

    if (r4) {
	phi = ( torsion_angle_3012 + 30 );
        for (i=0; i<3; i++) {
            r4[i] = r0[i] + bond_len_03 * (norm012[i]*cos(phi)*sin(theta)
            +                        norm_0_1_n012[i]*sin(phi)*sin(theta)
            +                                  n10[i]         *cos(theta) );
        }
    }

    if (r5) {
	phi = ( torsion_angle_3012 + 150 );
        for (i=0; i<3; i++) {
            r5[i] = r0[i] + bond_len_03 * (norm012[i]*cos(phi)*sin(theta)
            +                        norm_0_1_n012[i]*sin(phi)*sin(theta)
            +                                  n10[i]         *cos(theta) );
        }
    }
    */
}

void sp2_2known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR *r3, float bond_len_03) {

    /*
    .     r3
    .     |
    .     r0
    .    / \
    .  r1   r2
    .
    . r0 is sp3 type.
    . r0, r1, r2's coordinates are known.
    . r3's coordinate is to be determined.
    */

    VECTOR n01;
    VECTOR n02;
    VECTOR n03;

    n01 = vector_normalize(vector_vminusv(r1, r0));
    n02 = vector_normalize(vector_vminusv(r2, r0));
    n03 = vector_normalize(vector_neg(vector_vplusv(n01,n02)));

    if (r3) *r3  = vector_vplusv(r0, vector_rescale(n03, bond_len_03));
}

void sp2_1known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR *r3, VECTOR *r4, float bond_len_03, float bond_angle_103, float torsion_angle_3012) {

    /*
    .	r3   r4
    .	  \ /
    .	   r0			  z (n10)
    .	   |			  |_ y (norm_0_1_n012)
    .	   r1			 /
    .	   \			x (norm012)
    .	    r2
    . r0 is sp3 type.
    . r0, r3, r4's coordinates are known.
    . r1, r2's coordinates are to be determined.

    */

    VECTOR n10;
    VECTOR n12;
    VECTOR norm012;	/* normal of plane r0-r1-r2 */
    VECTOR norm_0_1_n012;	/* normal of plane r0-r1-norm012 */

    float theta;
    float phi;

    n10 = vector_normalize(vector_vminusv(r0, r1));
    n12 = vector_normalize(vector_vminusv(r2, r1));

    norm012 = vector_normalize(vector_vxv(n10,n12));
    norm_0_1_n012 = vector_normalize(vector_vxv(n10,norm012));

    theta = env.PI - bond_angle_103;
    if (r3) {
        phi = torsion_angle_3012 - env.PI/2.;
        *r3 = vector_vplusv(r0, vector_rescale(
                vector_sum3v(   vector_rescale(norm012,cos(phi)*sin(theta)),
                                vector_rescale(norm_0_1_n012,sin(phi)*sin(theta)),
                                vector_rescale(n10,cos(theta))),
                bond_len_03));
    }
    if (r4) {
        phi = torsion_angle_3012 + env.PI/2.;
        *r4 = vector_vplusv(r0, vector_rescale(
                vector_sum3v(   vector_rescale(norm012,cos(phi)*sin(theta)),
                                vector_rescale(norm_0_1_n012,sin(phi)*sin(theta)),
                                vector_rescale(n10,cos(theta))),
                bond_len_03));
    }

    /* ^       ^                      ^                      ^              */
    /* r = r ( i*cos(phi)sin(theta) + j*sin(phi)sin(theta) + k*cos(theta) ) */
    /*
    for (i=0; i<3; i++) {
        r3[i] = r0[i] + bond_len_03 * (norm012[i]*cos(phi)*sin(theta)
        +                        norm_0_1_n012[i]*sin(phi)*sin(theta)
        +                                  n10[i]         *cos(theta) );
    }

    if (r4) {
        phi = (torsion_angle_3012 + 90);
        for (i=0; i<3; i++) {
            r4[i] = r0[i] + bond_len_03 * (norm012[i]*cos(phi)*sin(theta)
            +                        norm_0_1_n012[i]*sin(phi)*sin(theta)
            +                                  n10[i]         *cos(theta) );
        }
    }
    */
}

void sp2d_3known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR r3, VECTOR *r4, float bond_len_04) {

    /*
    .     r4
    .     |
    .  r3-r0-r1
    .     |
    .     r2
    .
    . r0 is sp2d/dsp2 type.
    . r0, r1, r2, r3's coordinates are known.
    . r4's coordinate is to be determined.
    */

    VECTOR n01, n02, n03, n04, n13, swp;
    float product;
    int on_a_line;

    n01 = vector_normalize(vector_vminusv(r1, r0));
    n02 = vector_normalize(vector_vminusv(r2, r0));
    n03 = vector_normalize(vector_vminusv(r3, r0));

    /* find the two vectors on a line */
    on_a_line = 12;
    product = vdotv(n01, n02);
    if (vdotv(n01, n03) < product) {
        on_a_line = 13;
        product = vdotv(n01, n03);
    }
    if (vdotv(n02, n03) < product) {
        on_a_line = 23;
        product = vdotv(n02, n03);
    }
    /* make n01 and n03 on a line */
    if (on_a_line == 12) {
        swp = n02;
        n02 = n03;
        n03 = swp;
    }
    if (on_a_line == 23) {
        swp = n02;
        n02 = n01;
        n01 = swp;
    }

    /* consider if r1,r2,r3 are a little bended, try to guess the direction for r4 */
    n13 = vector_normalize(vector_vminusv(n03,n01)); /* average of n01 and n03 (if they are not exactly on a line */
    n02 = vector_normalize(vector_vminusv(n02,vector_rescale(n02, vdotv(n02,n13)))); /* n02 should be normal to n13 */
    n04 = vector_neg(n02);

    if (r4) *r4  = vector_vplusv(r0, vector_rescale(n04, bond_len_04));
}

void sp3d2_5known(VECTOR r0, VECTOR r1, VECTOR r2, VECTOR r3, VECTOR r4, VECTOR r5, VECTOR *r6, float bond_len_06) {

    /*
    .      r4  r3
    .       \ /
    .  r5-- r0 --r6
    .       / \
    .      r1  r2
    .
    . r0 is sp3d2 type.
    . r0, r1, r2, r3, r4, r5's coordinates are known.
    . r6's coordinate is to be determined.
    . In this figure, r0,r1,r2,r3,r4 and in a same plane (alpha)
    . r05, r06 are both normal to the plane alpha.
    */

    VECTOR n[6],n13,n24;
    float product, min_product,p13,p24;
    int i,j,perpend;

    n[1] = vector_normalize(vector_vminusv(r1, r0));
    n[2] = vector_normalize(vector_vminusv(r2, r0));
    n[3] = vector_normalize(vector_vminusv(r3, r0));
    n[4] = vector_normalize(vector_vminusv(r4, r0));
    n[5] = vector_normalize(vector_vminusv(r5, r0));

    /* find the four vectors on a plane (or find one vector that's perpendicular to all the others) */
    min_product = 1;
    for (i=1;i<6;i++) {
        product = 0;
        for (j=1;j<6;j++) {
            if (i==j) continue;
            /* remember the angle btw i and j that's lest close to 90 */
            if (fabs(vdotv(n[i],n[j])) > product) {
                product = fabs(vdotv(n[i],n[j]));
            }
        }
        if (product < min_product) {
            min_product = product;
            perpend = i;
        }
    }

    /* find r1-r3 and r2-r4 */
    p13 = 0;
    p24 = 0;
    for (i=1;i<6;i++) {
        if (i == perpend) continue;
        for (j=i+1;j<6;j++) {
            if (j == perpend) continue;

            if (fabs(vdotv(n[i],n[j])) > p13) {
                p24 = p13;
                n24 = n13;
                p13 = fabs(vdotv(n[i],n[j]));
                n13 = vector_normalize(vector_vminusv(n[i], n[j]));
            }
            else if (fabs(vdotv(n[i],n[j])) > p24) {
                p24 = fabs(vdotv(n[i],n[j]));
                n24 = vector_normalize(vector_vminusv(n[i], n[j]));
            }
        }
    }

    n[0] = vector_normalize(vector_vxv(n13,n24));
    if (vdotv(n[perpend],n[0]) > 0) {
        n[0] = vector_neg(n[0]);
    }

    if (r6) *r6  = vector_vplusv(r0, vector_rescale(n[0], bond_len_06));
}


#define DEFAULT_RAD 0.75
float get_bond_length(CONF *conf_p, ATOM *atom1_p, ATOM *atom2_p) {
    float rad1=0;
    float rad2=0;
    char  elem1[5]="    ";
    char  elem2[5]="    ";
    char  bond_len[MAXCHAR_LINE], *sbuffer, resName[4];
    float len;
    int   exist;

    /* first search for specifically defined bond length */
    exist = 1;
    if ( param_get("BOND_LEN",conf_p->confName, atom1_p->name, bond_len) ) {
        strncpy(resName, conf_p->confName, 3); resName[3] = '\0';
        if ( param_get("BOND_LEN",resName, atom1_p->name, bond_len) ) {
            exist = 0;
        }
    }
    if (exist) {
        sbuffer = strstr(bond_len, atom2_p->name);
        if (sbuffer) {
            sbuffer[11] = '\0';
            len = atof(sbuffer + 5);
            return len;
        }
    }

    exist = 1;
    if ( param_get("BOND_LEN",conf_p->confName, atom2_p->name, bond_len) ) {
        strncpy(resName, conf_p->confName, 3); resName[3] = '\0';
        if ( param_get("BOND_LEN",resName, atom2_p->name, bond_len) ) {
            exist = 0;
        }
    }
    if (exist) {
        sbuffer = strstr(bond_len, atom1_p->name);
        if (sbuffer) {
            sbuffer[11] = '\0';
            len = atof(sbuffer + 5);
            return len;
        }
    }

    /* then use covalence radii to get bond length */
    elem1[0]=atom1_p->name[1];
    if (param_get("RADCOVAL", "SINGL", elem1, &rad1)) {
        printf("   Warning! get_bond_length(): Can't find covalent radius for %s, use default radius %f instead\n", atom1_p->name, DEFAULT_RAD);
        rad1 = DEFAULT_RAD;
        param_sav("RADCOVAL", "SINGL", elem1, &rad1, sizeof(float));
    }
    elem2[0]=atom2_p->name[1];
    if (param_get("RADCOVAL", "SINGL", elem2, &rad2)) {
        printf("   Warning! get_bond_length(): Can't find covalent radius for %s, use default radius %f instead\n", atom2_p->name, DEFAULT_RAD);
        rad2 = DEFAULT_RAD;
        param_sav("RADCOVAL", "SINGL", elem2, &rad2, sizeof(float));
    }
    return rad1+rad2;
}

float get_bond_angle(CONF *conf_p, ATOM *atom0_p, ATOM *atom1_p, ATOM *atom2_p, char *orbital) {
    char  bond_ang[MAXCHAR_LINE], *sbuff, *sbuffer1, *sbuffer2, resName[4];
    int   exist;

    exist = 1;
    if ( param_get("BOND_ANG",conf_p->confName, atom0_p->name, bond_ang) ) {
        strncpy(resName, conf_p->confName, 3); resName[3] = '\0';
        if ( param_get("BOND_ANG",resName, atom0_p->name, bond_ang) ) {
            exist = 0;
        }
    }

    /*
    .....................01234567890123456789012345678901234567890
    .0123456789012345678901234567890123456789012345678901234567890
    .                   |one block of def|
    .#-------|-----|----|----|----|------|----|----|------|
    .BOND_ANG HEA   FE    N A  OH   109.0
    */
    if (exist) {
        sbuff = bond_ang;
        while (1) {
            sbuffer1 = strstr(sbuff, atom1_p->name); /* look for atom1 */
            sbuffer2 = strstr(sbuff, atom2_p->name); /* look for atom2 */
            if (sbuffer1 && sbuffer2) {  /* both are found */
                if (fabs(strlen(sbuffer1)-strlen(sbuffer2)) <= 6) { /* within one block of angle definition */
                    if (strlen(sbuffer1)<strlen(sbuffer2)) {
                        sbuffer1[11] = '\0';
                        return env.d2r * atof(sbuffer1+5);
                    }
                    else {
                        sbuffer2[11] = '\0';
                        return env.d2r * atof(sbuffer2+5);
                    }
                }
                else {
                    if (strlen(sbuffer1)<strlen(sbuffer2)) sbuff = sbuffer2;
                    else sbuff = sbuffer1;
                }
            }
            else break;
        }
    }

    if (!strcmp(orbital, "sp2"))         return env.d2r * 120.;
    else if (!strcmp(orbital, "sp3"))    return env.d2r * 109.;
    else if (!strcmp(orbital, "sp3d2"))  return env.d2r * 90.;
    else if (!strcmp(orbital, "sp2d"))   return env.d2r * 90.;
    else {
        return env.d2r * 109;
    }
}

int ionization(PROT prot)
{  int kr, kc, ka, ic, ia, natoms, ins;
   STRINGS confs;
   int nconf;
   ATOM *swap;

   for (kr=0; kr< prot.n_res; kr++) {
      if (param_get("CONFLIST", prot.res[kr].resName, "", &confs)) {
         printf("   FATAL: ionization(): No CONFLIST for residue %s\n", prot.res[kr].resName);
         return USERERR;
      }
      nconf=prot.res[kr].n_conf;
      for (kc=1; kc<nconf; kc++) {
          if (!prot.res[kr].conf[kc].n_atom) continue;
         /* each conformer will be multipied to the conformers in the conflist */
         for (ic=1; ic<confs.n; ic++) {
            if (strcmp(prot.res[kr].conf[kc].confName, confs.strings[ic])) {
               if (param_get("NATOM", confs.strings[ic], "", &natoms)) {
                  printf("   FATAL: ionization(): No NATOM for conformer %s\n", confs.strings[ic]);
                  return USERERR;
               }
               ins = ins_conf(&prot.res[kr], prot.res[kr].n_conf, natoms);
               if (ins == USERERR) return USERERR;

               /* I am going to inherite the parent conformer but the atoms */
               swap=prot.res[kr].conf[ins].atom;
               memcpy(&prot.res[kr].conf[ins], &prot.res[kr].conf[kc], sizeof(CONF));
               prot.res[kr].conf[ins].atom = swap; prot.res[kr].conf[ins].n_atom=natoms;

               /* update the confName and history */
               strcpy(prot.res[kr].conf[ins].confName, confs.strings[ic]); /* update confName */
               strncpy(prot.res[kr].conf[ins].history, confs.strings[ic]+3, 2); /* update history */

               for (ka=0; ka<prot.res[kr].conf[kc].n_atom; ka++) {
                  ia = iatom(prot.res[kr].conf[ins].confName, prot.res[kr].conf[kc].atom[ka].name);
                  if (prot.res[kr].conf[kc].atom[ka].on && ia>=0) {
                     strcpy(prot.res[kr].conf[ins].atom[ia].confName, confs.strings[ic]); /* update confname */
                     strncpy(prot.res[kr].conf[ins].atom[ia].history, confs.strings[ic]+3, 2); /* update history */
                     prot.res[kr].conf[ins].atom[ia] = prot.res[kr].conf[kc].atom[ka];
                  }
               }
            }
         }
      }
   }

   return 0;
}

int rm_dupconf_hv(PROT prot)
{  int kr, kc, ic;
   int C = 0;

   for (kr=0; kr<prot.n_res; kr++) {
      for (kc=1; kc<prot.res[kr].n_conf; kc++) {
         for (ic=kc+1; ic<prot.res[kr].n_conf; ic++) {
            if(!cmp_conf_hv(prot.res[kr].conf[kc], prot.res[kr].conf[ic], env.prune_thr)) {
               //printf("debugging, %d,%d,%d\n",prot.res[kr].resSeq,kc,ic);
               del_conf(&prot.res[kr], ic);
               ic--;
               C++;
            }
         }
      }
   }

   return C;
}

int rm_dupconf(PROT prot, float prune_thr) {
    int i_res;
    int rm_counter=0;
    for (i_res=0; i_res<prot.n_res; i_res++) {
        int i_conf;
        for (i_conf=prot.res[i_res].n_conf-1; i_conf>=1; i_conf--) {
            int j_conf;
            for (j_conf=1; j_conf<i_conf; j_conf++) {
                if( strcmp(prot.res[i_res].conf[i_conf].confName, prot.res[i_res].conf[j_conf].confName)) continue;
                if(!cmp_conf(prot.res[i_res].conf[i_conf], prot.res[i_res].conf[j_conf], prune_thr)) {
                    del_conf(&prot.res[i_res], i_conf);
                    rm_counter++;
                    break;
                }
            }
        }
    }
    return rm_counter;
}

int write_conflist(FILE *fp, PROT prot)
{  int kr, kc, ka;

   id_conf(prot);
   get_connect12(prot);
   get_vdw0(prot);
   get_vdw1(prot);
   for (kr=0; kr<prot.n_res; kr++) {
      for (kc=1; kc<prot.res[kr].n_conf; kc++) {
          prot.res[kr].conf[kc].E_torsion = torsion_conf(&prot.res[kr].conf[kc]);
          prot.res[kr].conf[kc].E_self = prot.res[kr].conf[kc].E_vdw0 + prot.res[kr].conf[kc].E_vdw1 + prot.res[kr].conf[kc].E_torsion;
          if (prot.res[kr].conf[kc].E_vdw0 > 999.) prot.res[kr].conf[kc].E_vdw0 = 999.;
          if (prot.res[kr].conf[kc].E_vdw1 > 999.) prot.res[kr].conf[kc].E_vdw1 = 999.;
          if (prot.res[kr].conf[kc].E_torsion > 999.) prot.res[kr].conf[kc].E_torsion = 999.;
          if (prot.res[kr].conf[kc].E_self > 999.) prot.res[kr].conf[kc].E_self = 999.;
      }
   }
   fprintf(fp, "CONFORMER         crg  vdw0  vdw1  tors  epol  self   occ  Hb     history\n");
   for (kr=0; kr<prot.n_res; kr++) {
      for (kc=1; kc<prot.res[kr].n_conf; kc++) {
         prot.res[kr].conf[kc].netcrg = 0.0;
         for (ka=0; ka<prot.res[kr].conf[kc].n_atom; ka++) {
            if (!prot.res[kr].conf[kc].atom[ka].on) continue;
            prot.res[kr].conf[kc].netcrg += prot.res[kr].conf[kc].atom[ka].crg;
         }

         if (fabs(prot.res[kr].conf[kc].netcrg - rintf(prot.res[kr].conf[kc].netcrg)) > 0.001) {
            printf("   WARNING: Conformer %s has non integer charge %6.3f\n", prot.res[kr].conf[kc].uniqID,
                                                                        prot.res[kr].conf[kc].netcrg);
            fflush(stdout);
         }

         fprintf(fp, "%s %6.3f%6.1f%6.1f%6.1f%6.1f%6.1f%6.3f  %2s  %10s\n",prot.res[kr].conf[kc].uniqID,
                                                                    prot.res[kr].conf[kc].netcrg,
                                                                    prot.res[kr].conf[kc].E_vdw0,
                                                                    prot.res[kr].conf[kc].E_vdw1,
                                                                    prot.res[kr].conf[kc].E_torsion,
                                                                    prot.res[kr].conf[kc].E_epol,
                                                                    prot.res[kr].conf[kc].E_self,
                                                                    prot.res[kr].conf[kc].occ,
                                                                    prot.res[kr].conf[kc].history+8,
                                                                    prot.res[kr].conf[kc].history);
      }
   }

   for (kr=0; kr<prot.n_res; kr++) {
         prot.res[kr].conf[0].netcrg = 0.0;
         for (ka=0; ka<prot.res[kr].conf[0].n_atom; ka++) {
            if (!prot.res[kr].conf[0].atom[ka].on) continue;
            prot.res[kr].conf[0].netcrg += prot.res[kr].conf[0].atom[ka].crg;
         }

         if (fabs(prot.res[kr].conf[0].netcrg - (int) prot.res[kr].conf[0].netcrg) > 0.001) {
            printf("   WARNING: Conformer %s has non integer charge %6.3f\n", prot.res[kr].conf[0].uniqID,
                                                                        prot.res[kr].conf[0].netcrg);
            fflush(stdout);
         }
   }

   return 0;
}

int get_demetri_out(void)
{ char line[160], num[2]=" ", atomname[6], conformerlable;
  int i;
  FILE *fp, *fp2;
  fp = fopen("step2_out.pdb","r+");
  fp2 = fopen("demetri_out.pdb","w");
  rewind(fp);
  rewind(fp2);

  while(fgets(line, sizeof(line), fp))
      {  if (strncmp("__", line+82, 2) || strncmp("__", line+84, 2)) {
            strncpy(line+7, "AB  ", 4);
         }
         else  {
            strncpy(line+7, "AA  ", 4);
         }
         /*deal with the atom name part in the next */
         strncpy(atomname, line+12, 5);
         atomname[5]=' ';

         if( !strncmp(line+17,"CTR",3) || !strncmp(line+17,"NTR",3))
            atomname[4] = 'T';
         if(atomname[0]<=57 && atomname[0]>=49)
         { atomname[5] = atomname[0];
            atomname[0] = ' ';
         }
         else if(atomname[0] != ' ')
         { for(i=4; i>=0 ; i--)
               atomname[i+1] = atomname[i];
            atomname[0] = ' ';
         }
         for(i=2; i<6; i++)
         { if(' ' == atomname[i-1] && atomname[i]!=' ')
            { atomname[i-1] = atomname[i];
               atomname[i] = ' ';
               i=i-2;
            }
         }
         strncpy(line+12, atomname,5);

/* Special Cases */
         strncpy(atomname, line+13, 6);
         if(!strncmp(atomname,"H ",4))
         { line[14] = 'N';
         }
         else if(!strncmp(atomname,"HXTTCT",6))
         { line[14] = 'T'; line[15] =' ' ; line[16] = ' ';
         }
         else if(!strncmp(atomname,"OXTTCT",6))
         { line[14] = 'T'; line[15] ='2' ; line[16] = ' ';
         }
         else if(!strncmp(atomname,"OT CT",6))
            line[15] = '1';
         else if(!strncmp(atomname,"HT1 NT",6))
            line[15] = '3';
         else if(!strncmp(atomname,"H1A PA",6) || !strncmp(atomname,"H2A PA",6))
         { line[14] = 'T'; line[15] =' ';
         }
         else if(!strncmp(atomname,"H1D PD",6) || !strncmp(atomname,"H2D PD",6))
         { line[14] = 'T'; line[15] =' ';
         }
         else if(!strncmp(atomname,"HE1 GL",6) || !strncmp(atomname,"HE2 GL",6))
         { line[14] = 'T'; line[15] =' ';
         }
         else if(!strncmp(atomname,"HD1 AS",6) || !strncmp(atomname,"HD2 AS",6))
         { line[14] = 'T'; line[15] =' ';
         }

         /*deal with the residue name part in the next*/
         if('+' == line[80])
            line[19] = '+';
         else if ('-' == line[80])
            line[19] = '-';

         /*deal with the conformor number part in the next*/
         for(i=23; ' '==line[i]; i++)
            line[i]='0';
         num[1] = line[29];
         num[0] = line[28];
         conformerlable = atoi(num);
         if(0 == conformerlable)
            conformerlable = 65;
         else if(conformerlable > 0 && conformerlable <= 26)
            conformerlable += 64;
         else if(conformerlable >26 && conformerlable <=52)
            conformerlable += 70;
         else
         { printf("\n   The number of conformers exceeds 52!\n");
            return USERERR;
         }
         line[26] = conformerlable;
         line[29] = ' ';
         line[28] = ' ';
         line[27] = ' ';

         for(i=80; i<90 ; i++)
            line[i] = ' ';

         fprintf(fp2,"%s",line);

      }

  rewind(fp);
  rewind(fp2);
  fclose(fp);
  fclose(fp2);
  return 0;
}

int load_listrot(char *fname, PROT prot)
{  FILE *fp;
   int i;
   char line[MAXCHAR_LINE];
   ATOM atom;

   if ((fp=fopen(fname, "r")) == NULL) {
      printf("   Read file %s error\n", fname);
      return USERERR;
   }

   memset(line, 0, sizeof(line));
   while (fgets(line, sizeof(line), fp) != NULL) {
      atom = pdbline2atom(line);

      for (i=0; i<prot.n_res; i++) {
         /* allow raw pdb comparison */
         if (atom.chainID == ' ') atom.chainID = '_';

         if (prot.res[i].iCode == atom.iCode && \
             prot.res[i].chainID == atom.chainID && \
             prot.res[i].resSeq == atom.resSeq && \
             !strncmp(prot.res[i].resName, atom.resName, 3)) {
            prot.res[i].do_rot = 1;
         }
      }

      memset(line, 0, sizeof(line));
   }

   return 0;
}

int prune_hdirected(PROT prot)
{  float d_low = 2.5;
   float d_high = 3.5;
   float vdw_limit = 20.0; /* kCal/mol */
   char  HaveON;
   int ires, iconf, iatom, jres, jconf, jatom;
   float dd, dd_low, dd_high, d_relax;
   char need_relax;
   float vdw_pair;
   int n_deleted=0;
   int i_nconf, j_nconf;

   dd_low = d_low*d_low;
   dd_high = d_high*d_high;

   for (ires=0; ires<prot.n_res; ires++) {
      prot.res[ires].i_res_prot = ires;
   }

   for (ires=0; ires<prot.n_res; ires++) {
      /* Do I have O or N?*/
      HaveON = 0;
      if (prot.res[ires].n_conf >= 2) {
         for (iatom = 0; iatom <prot.res[ires].conf[1].n_atom; iatom++) {
            if (prot.res[ires].conf[1].atom[iatom].on) {
               if (prot.res[ires].conf[1].atom[iatom].name[1] == 'O' || prot.res[ires].conf[1].atom[iatom].name[1] == 'N') {
                  HaveON = 1;
                  break;
               }
            }
         }
      }

      if (!HaveON) continue;  /* nex residue when no O or N atomes*/

      i_nconf = prot.res[ires].n_conf;
      for (iconf=1; iconf<i_nconf; iconf++) {
         if (prot.res[ires].conf[iconf].history[2] == 'H') break;

         for (jres=0; jres<prot.res[ires].n_ngh; jres++) {
            if (prot.res[ires].ngh[jres]->i_res_prot <= ires) continue;
            j_nconf = prot.res[ires].ngh[jres]->n_conf;

	    for (jconf=1; jconf<j_nconf; jconf++) {
               if (prot.res[ires].ngh[jres]->conf[jconf].history[2] == 'H') break;

               /* now, got the pair, check ON - ON distance */
               need_relax = 0; /* by default, no relaxation */
               for (iatom = 0; iatom <prot.res[ires].conf[iconf].n_atom; iatom++) {
                  if (need_relax) break;
                  if (!prot.res[ires].conf[iconf].atom[iatom].on) continue;
                  if (prot.res[ires].conf[iconf].atom[iatom].name[1] != 'N' && prot.res[ires].conf[iconf].atom[iatom].name[1] != 'O') continue;
                  for (jatom = 0; jatom <prot.res[ires].ngh[jres]->conf[jconf].n_atom; jatom++) {
                     if (!prot.res[ires].ngh[jres]->conf[jconf].atom[jatom].on) continue;
                     if (prot.res[ires].ngh[jres]->conf[jconf].atom[jatom].name[1] != 'N' && prot.res[ires].ngh[jres]->conf[jconf].atom[jatom].name[1] != 'O') continue;

                     dd = ddvv(prot.res[ires].conf[iconf].atom[iatom].xyz,prot.res[ires].ngh[jres]->conf[jconf].atom[jatom].xyz);

                     if (dd>dd_low && dd<dd_high) {
                        vdw_pair = vdw_conf_hv(ires, iconf, prot.res[ires].ngh[jres]->i_res_prot, jconf, prot);
                        if (vdw_pair<vdw_limit) {
                           need_relax = 1;
                           break;
                        }
                     }
                  }
               }
               if (need_relax) {
		  d_relax = relax_this_pair(prot, &prot.res[ires], iconf, prot.res[ires].ngh[jres], jconf);
                  /* DEBUG
                  printf("%9.6f %9.6f\n",sqrt(dd), d_relax);
                  fflush(stdout);
                  */
               }
            }
         }
      }

      /* for this polar/ionizable residue, delete non derived conformers
      for (iconf=2; iconf<prot.res[ires].n_conf; iconf++) {
         if (prot.res[ires].conf[iconf].history[2] == 'H') break;
         del_conf(&prot.res[ires], iconf);
         iconf--;
      }
      */

      for (iconf=2; iconf<prot.res[ires].n_conf; iconf++) {
         if (prot.res[ires].conf[iconf].history[2] == 'H') break;
      }

      if (prot.res[ires].n_conf - iconf >= 2) {

         /* delete conformers with high offset to meet conformer number limit */
         qsort((prot.res[ires].conf+iconf), prot.res[ires].n_conf-iconf, sizeof(CONF), cmp_Eself);
         /* DEBUG
         for (jconf=iconf; jconf<prot.res[ires].n_conf; jconf++) {
            printf("%6d, %8.4f %10s\n", jconf, prot.res[ires].conf[jconf].E_self, prot.res[ires].conf[jconf].history);
         }
      */
      }
      for (jconf=prot.res[ires].n_conf-1; jconf>=env.hdirlimt+iconf; jconf--) {
         del_conf(&prot.res[ires], jconf);
         n_deleted++;
      }
   }
   printf("   %d conformers are deleted to fit the limit (%d) of derived conformer number\n", n_deleted, env.hdirlimt);

   return 0;
}

float relax_this_pair(PROT prot, RES *res_a, int conf_a, RES *res_b, int conf_b)
{  int i, n_a, n_b, kc;
   int C;
   char C_str[5];
   ROTAMER rule;
   char sbuff[MAXCHAR_LINE];
   int iconf, jconf, iatom, jatom;
   float dd3, dd, dd3_min, dd_min;
   int i_min, j_min;
   int ic;

   /* remember the number of the current conformers */
   n_a = res_a->n_conf;
   n_b = res_b->n_conf;

   /* make a copy of passed in conformer */
   ins_conf(res_a, res_a->n_conf, res_a->conf[conf_a].n_atom);
   cpy_conf(&res_a->conf[res_a->n_conf-1], &res_a->conf[conf_a]);
   ins_conf(res_b, res_b->n_conf, res_b->conf[conf_b].n_atom);
   cpy_conf(&res_b->conf[res_b->n_conf-1], &res_b->conf[conf_b]);


   /* a side rotamer relaxation */
   C = 0;
   while (1) {
      sprintf(C_str, "%d", C);
      if (param_get("ROTAMER", res_a->resName, C_str, &rule)) break;
      sprintf(sbuff, " %s  ", rule.affected);
      sprintf(rule.affected, "%s", sbuff);
      kc = res_a->n_conf;
      for (i=n_a; i<kc; i++) {
         if (swing_conf_rot_rule(res_a, i, rule, 4.0)) {
            printf("      Error in H bond relaxation.\n");
         }
      }
      C++;
   }

   /* b side rotamer relaxation */
   C = 0;
   while (1) {
      sprintf(C_str, "%d", C);
      if (param_get("ROTAMER", res_b->resName, C_str, &rule)) break;
      sprintf(sbuff, " %s  ", rule.affected);
      sprintf(rule.affected, "%s", sbuff);
      kc = res_b->n_conf;
      for (i=n_b; i<kc; i++) {
         if (swing_conf_rot_rule(res_b, i, rule, 4.0)) {
            printf("      Error in H bond relaxation.\n");
         }
      }
      C++;
   }

   /* refine rotamer distance to 3.0 */
   i_min = n_a;
   j_min = n_b;
   dd3_min = 1000000.0;
   dd_min =  1000000.0;
   for (iconf=n_a; iconf<res_a->n_conf; iconf++) {
      for (jconf=n_b; jconf<res_b->n_conf; jconf++) {
         for (iatom=0; iatom<res_a->conf[iconf].n_atom; iatom++) {
            if (!res_a->conf[iconf].atom[iatom].on) continue;
            if (res_a->conf[iconf].atom[iatom].name[1] != 'N' && res_a->conf[iconf].atom[iatom].name[1] != 'O') continue;
            for (jatom=0; jatom<res_b->conf[jconf].n_atom; jatom++) {
               if (!res_b->conf[jconf].atom[jatom].on) continue;
               if (res_b->conf[jconf].atom[jatom].name[1] != 'N' && res_b->conf[jconf].atom[jatom].name[1] != 'O') continue;
               dd = ddvv(res_a->conf[iconf].atom[iatom].xyz, res_b->conf[jconf].atom[jatom].xyz);
               dd3 = fabs(dd-9.0);
               if (dd3<dd3_min) {
                  dd3_min = dd3;
                  dd_min  = dd;
                  strncpy(res_a->conf[i_min].history+2, "___", 3);
                  strncpy(res_b->conf[j_min].history+2, "___", 3);
                  strncpy(res_a->conf[iconf].history+2, "MIN", 3);
                  strncpy(res_b->conf[jconf].history+2, "MIN", 3);
                  i_min = iconf;
                  j_min = jconf;
                  /* DEBUG
                  printf ("%.6f, %d(%.3f, %.3f, %.3f) %d(%.3f, %.3f, %.3f)\n", sqrt(dd_min),
                     iconf,
                     res_a->conf[iconf].atom[iatom].xyz.x,
                     res_a->conf[iconf].atom[iatom].xyz.y,
                     res_a->conf[iconf].atom[iatom].xyz.z,
                     jconf,
                     res_b->conf[jconf].atom[jatom].xyz.x,
                     res_b->conf[jconf].atom[jatom].xyz.y,
                     res_b->conf[jconf].atom[jatom].xyz.z);
                  */
               }
            }
         }
      }
   }


   for (iconf=n_a; iconf<res_a->n_conf; iconf++) {
      if (strncmp(res_a->conf[iconf].history+2,"MIN",3)) {
         del_conf(res_a, iconf);
         iconf--;
      }
   }
   for (iconf=n_b; iconf<res_b->n_conf; iconf++) {
      if (strncmp(res_b->conf[iconf].history+2,"MIN",3)) {
         del_conf(res_b, iconf);
         iconf--;
      }
   }


   /* SECOND ROUND, repeat with swing angle 1 degree */
   C = 0;
   while (1) {
      sprintf(C_str, "%d", C);
      if (param_get("ROTAMER", res_a->resName, C_str, &rule)) break;
      sprintf(sbuff, " %s  ", rule.affected);
      sprintf(rule.affected, "%s", sbuff);
      kc = res_a->n_conf;
      for (i=n_a; i<kc; i++) {
         if (swing_conf_rot_rule(res_a, i, rule, 2.0)) {
            printf("      Error in H bond relaxation.\n");
         }
      }
      C++;
   }
   C = 0;
   while (1) {
      sprintf(C_str, "%d", C);
      if (param_get("ROTAMER", res_b->resName, C_str, &rule)) break;
      sprintf(sbuff, " %s  ", rule.affected);
      sprintf(rule.affected, "%s", sbuff);
      kc = res_b->n_conf;
      for (i=n_b; i<kc; i++) {
         if (swing_conf_rot_rule(res_b, i, rule, 2.0)) {
            printf("      Error in H bond relaxation.\n");
         }
      }
      C++;
   }
   i_min = n_a;
   j_min = n_b;
   dd3_min = 1000000.0;
   dd_min =  1000000.0;
   for (iconf=n_a; iconf<res_a->n_conf; iconf++) {
      for (jconf=n_b; jconf<res_b->n_conf; jconf++) {
         for (iatom=0; iatom<res_a->conf[iconf].n_atom; iatom++) {
            if (!res_a->conf[iconf].atom[iatom].on) continue;
            if (res_a->conf[iconf].atom[iatom].name[1] != 'N' && res_a->conf[iconf].atom[iatom].name[1] != 'O') continue;
            for (jatom=0; jatom<res_b->conf[jconf].n_atom; jatom++) {
               if (!res_b->conf[jconf].atom[jatom].on) continue;
               if (res_b->conf[jconf].atom[jatom].name[1] != 'N' && res_b->conf[jconf].atom[jatom].name[1] != 'O') continue;
               dd = ddvv(res_a->conf[iconf].atom[iatom].xyz, res_b->conf[jconf].atom[jatom].xyz);
               dd3 = fabs(dd-9.0);
               if (dd3<dd3_min) {
                  dd3_min = dd3;
                  dd_min  = dd;
                  strncpy(res_a->conf[i_min].history+2, "___", 3);
                  strncpy(res_b->conf[j_min].history+2, "___", 3);
                  strncpy(res_a->conf[iconf].history+2, "MIN", 3);
                  strncpy(res_b->conf[jconf].history+2, "MIN", 3);
                  i_min = iconf;
                  j_min = jconf;
               }
            }
         }
      }
   }
   for (iconf=n_a; iconf<res_a->n_conf; iconf++) {
      if (strncmp(res_a->conf[iconf].history+2,"MIN",3)) {
         del_conf(res_a, iconf);
         iconf--;
      }
   }
   for (iconf=n_b; iconf<res_b->n_conf; iconf++) {
      if (strncmp(res_b->conf[iconf].history+2,"MIN",3)) {
         del_conf(res_b, iconf);
         iconf--;
      }
   }


   /* THIRD ROUND, repeat with swing angle 1 degree */
   C = 0;
   while (1) {
      sprintf(C_str, "%d", C);
      if (param_get("ROTAMER", res_a->resName, C_str, &rule)) break;
      sprintf(sbuff, " %s  ", rule.affected);
      sprintf(rule.affected, "%s", sbuff);
      kc = res_a->n_conf;
      for (i=n_a; i<kc; i++) {
         if (swing_conf_rot_rule(res_a, i, rule, 1.0)) {
            printf("      Error in H bond relaxation.\n");
         }
      }
      C++;
   }
   C = 0;
   while (1) {
      sprintf(C_str, "%d", C);
      if (param_get("ROTAMER", res_b->resName, C_str, &rule)) break;
      sprintf(sbuff, " %s  ", rule.affected);
      sprintf(rule.affected, "%s", sbuff);
      kc = res_b->n_conf;
      for (i=n_b; i<kc; i++) {
         if (swing_conf_rot_rule(res_b, i, rule, 1.0)) {
            printf("      Error in H bond relaxation.\n");
         }
      }
      C++;
   }
   i_min = n_a;
   j_min = n_b;
   dd3_min = 1000000.0;
   dd_min =  1000000.0;
   for (iconf=n_a; iconf<res_a->n_conf; iconf++) {
      for (jconf=n_b; jconf<res_b->n_conf; jconf++) {
         for (iatom=0; iatom<res_a->conf[iconf].n_atom; iatom++) {
            if (!res_a->conf[iconf].atom[iatom].on) continue;
            if (res_a->conf[iconf].atom[iatom].name[1] != 'N' && res_a->conf[iconf].atom[iatom].name[1] != 'O') continue;
            for (jatom=0; jatom<res_b->conf[jconf].n_atom; jatom++) {
               if (!res_b->conf[jconf].atom[jatom].on) continue;
               if (res_b->conf[jconf].atom[jatom].name[1] != 'N' && res_b->conf[jconf].atom[jatom].name[1] != 'O') continue;
               dd = ddvv(res_a->conf[iconf].atom[iatom].xyz, res_b->conf[jconf].atom[jatom].xyz);
               dd3 = fabs(dd-9.0);
               if (dd3<dd3_min) {
                  dd3_min = dd3;
                  dd_min  = dd;
                  strncpy(res_a->conf[i_min].history+2, "___", 3);
                  strncpy(res_b->conf[j_min].history+2, "___", 3);
                  strncpy(res_a->conf[iconf].history+2, "MIN", 3);
                  strncpy(res_b->conf[jconf].history+2, "MIN", 3);
                  i_min = iconf;
                  j_min = jconf;
               }
            }
         }
      }
   }
   for (iconf=n_a; iconf<res_a->n_conf; iconf++) {
      if (strncmp(res_a->conf[iconf].history+2,"MIN",3)) {
         del_conf(res_a, iconf);
         iconf--;
      }
   }
   for (iconf=n_b; iconf<res_b->n_conf; iconf++) {
      if (strncmp(res_b->conf[iconf].history+2,"MIN",3)) {
         del_conf(res_b, iconf);
         iconf--;
      }
   }

   res_a->conf[n_a].history[2] = 'H';
   res_b->conf[n_b].history[2] = 'H';
   res_a->conf[n_a].E_self = dd3_min;
   res_b->conf[n_b].E_self = dd3_min;

   /* delete the identical confomers with higher E_self */
   for (ic=res_a->n_conf-2; ic>0; ic--) {
      if (res_a->conf[ic].history[2] != 'H') continue;
      if(!cmp_conf_hv(res_a->conf[res_a->n_conf-1], res_a->conf[ic], env.hdirdiff)) {
         if (res_a->conf[ic].E_self >= dd3_min) {
            del_conf(res_a, ic);
         }
         else {
            del_conf(res_a, res_a->n_conf-1);
            break;
         }
      }
   }

   for (ic=res_b->n_conf-2; ic>0; ic--) {
      if (res_b->conf[ic].history[2] != 'H') continue;
      if(!cmp_conf_hv(res_b->conf[res_b->n_conf-1], res_b->conf[ic], env.hdirdiff)) {
         if (res_b->conf[ic].E_self >= dd3_min) {
            del_conf(res_b, ic);
         }
         else {
            del_conf(res_b, res_b->n_conf-1);
            break;
         }
      }
   }

   return sqrt(dd_min);
}

int swing_conf_rot_rule(RES *res, int j, ROTAMER rule, float phi_degree)
{  VECTOR v1, v2, v3;
   GEOM op;
   LINE axis;
   int ins;
   int k, l;
   ATOM atom1, atom2;
   char found;
   float phi = phi_degree*3.1415926/180.0;

   k=iatom(res->conf[j].confName, rule.atom2);

   if ((k=iatom(res->conf[j].confName, rule.atom2)) == -1) {
      printf("confname=\"%s\"\n",res->conf[j].confName);
      if ((k=iatom(res->conf[0].confName, rule.atom2)) == -1) {
         printf("   FATAL: place_rot_rule(): can't find atom \"%s\" in residue \"%s\"\n",
                 rule.atom2, res->resName);
         return USERERR;
      }
      else atom2 = res->conf[0].atom[k];
   }
   else atom2 = res->conf[j].atom[k];
   v2 = atom2.xyz;

   /* find the bond: atom1 is the first atom of the bond, it is one of the connected
    * atoms of atom2, but not necessary in this conformer or this residue */
   found = 0;
   if ((k=iatom(res->conf[j].confName, rule.atom1)) == -1) {
      l=0;
      while (atom2.connect12[l]!=NULL) {
         if (!strcmp(atom2.connect12[l]->name, rule.atom1)) {
            atom1 = *atom2.connect12[l];
            found = 1;
            break;
         }
         l++;
      }
   }
   else {
      atom1 = res->conf[j].atom[k];
      found = 1;
   }

   if (found)  v1 = atom1.xyz;
   else {
      printf("   FATAL: place_rot_rule(): can't find atom \"%s\" connected to \"%s\" in residue \"%s\"\n",
              rule.atom2, rule.atom1, res->resName);
      return USERERR;
   }

   axis = line_2v(v1, v2);

   /* swing left */
   geom_reset(&op);
   geom_roll(&op, -phi, axis);
   ins = ins_conf(res, res->n_conf, res->conf[j].n_atom);
   if (ins == USERERR) return USERERR;

   cpy_conf(&res->conf[ins], &res->conf[j]);
   strncpy(res->conf[ins].history+2, "HOSw", 4);
   for (k=0; k<res->conf[j].n_atom; k++) {
      if (strstr(rule.affected, res->conf[j].atom[k].name)) {
         v3 = res->conf[j].atom[k].xyz;
         geom_apply(op, &v3);
         res->conf[ins].atom[k].xyz = v3;
      }
   }

   /* swing right */
   geom_reset(&op);
   geom_roll(&op, phi, axis);
   ins = ins_conf(res, res->n_conf, res->conf[j].n_atom);
   if (ins == USERERR) return USERERR;

   cpy_conf(&res->conf[ins], &res->conf[j]);
   strncpy(res->conf[ins].history+2, "HOSw", 4);
   for (k=0; k<res->conf[j].n_atom; k++) {
      if (strstr(rule.affected, res->conf[j].atom[k].name)) {
         v3 = res->conf[j].atom[k].xyz;
         geom_apply(op, &v3);
         res->conf[ins].atom[k].xyz = v3;
      }
   }

   return 0;
}


int prune_pv(PROT prot, float c1, float c2, float c3)
{  int n = 0; /* number of conformers deleted */
   float cutoff_geo = c1;
   float cutoff_ele = c2;
   float cutoff_vdw = c3;
   int ir, ic, ia, jc;


   id_conf(prot);

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

int label_exposed(PROT prot)
{  int kr, kc, ka, i;
   float max_sas, av_sas, n_atom;

   sas_native(prot);
   for (kr=0; kr<prot.n_res; kr++) {
      for (kc=1; kc<prot.res[kr].n_conf; kc++) {
         av_sas = 0.0;
         n_atom = 0;
         for (ka=0; ka<prot.res[kr].conf[kc].n_atom; ka++) {
            if (!prot.res[kr].conf[kc].atom[ka].on) continue;
            n_atom++;
            av_sas += prot.res[kr].conf[kc].atom[ka].sas;
         }
         av_sas /= n_atom;

         prot.res[kr].conf[kc].E_rxn =av_sas;
      }

      if (prot.res[kr].n_conf>1) {
         i=1;
         max_sas = prot.res[kr].conf[1].E_rxn;
      }
      else i =0;

      for (kc=2; kc<prot.res[kr].n_conf; kc++) {
         if (prot.res[kr].conf[kc].E_rxn > max_sas) {
            max_sas = prot.res[kr].conf[kc].E_rxn;
            i=kc;
         }
      }

      if (max_sas > 0.05 && i && prot.res[kr].conf[i].history[2] != 'O') {
         /* Here we found the non-native exposed side chain confomer
          * We will relax this conformer to maximize the exposure.
          */
         printf("   Optimizing %s%4d Before = %.3f", prot.res[kr].resName, prot.res[kr].resSeq, prot.res[kr].conf[i].E_rxn);
         printf(" After = %.3f\n", max_confSAS(prot, kr, i));
         /* max_confSAS(prot, kr, i); */

      }
   }
   return 0;
}

float max_confSAS(PROT prot, int ir, int ic)
{  int nconf,i, kc, ka;
   int C;
   char C_str[5];
   ROTAMER rule;
   char sbuff[MAXCHAR_LINE];
   float av_sas, max_sas;
   int n_atom;

   for (i=0; i<prot.n_res; i++) {
      if (prot.res[i].n_conf > 1) prot.res[i].conf[1].on = 1;
      prot.res[i].conf[0].on = 1;
      for (kc=2; kc<prot.res[i].n_conf; kc++) prot.res[i].conf[kc].on = 0;
   }


 /* remember the number of the current conformers */
   nconf = prot.res[ir].n_conf;

   /* make a copy of passed in conformer */
   ins_conf(&prot.res[ir], nconf, prot.res[ir].conf[ic].n_atom);
   cpy_conf(&prot.res[ir].conf[nconf], &prot.res[ir].conf[ic]);

   /* conformer relaxation */
   C = 0;
   /* printf("      n_conf = %4d", prot.res[ir].n_conf);*/
   while (1) {
      sprintf(C_str, "%d", C);
      if (param_get("ROTAMER", prot.res[ir].resName, C_str, &rule)) break;
      sprintf(sbuff, " %s  ", rule.affected);
      sprintf(rule.affected, "%s", sbuff);
      kc = prot.res[ir].n_conf;
      for (i=nconf; i<kc; i++) {
         if (swing_conf_rot_rule(&prot.res[ir], i, rule, 5.0)) {
            printf("      Error in H bond relaxation.\n");
         }
      }
      C++;
   }

   /* compute SAS of conformers in this residue */
   surfw_res(prot, ir, env.radius_probe);
   for (kc = nconf; kc < prot.res[ir].n_conf; kc++) {
   //for (kc = 1; kc < prot.res[ir].n_conf; kc++) {
      av_sas = 0.0;
      n_atom = 0;
      for (ka=0; ka<prot.res[ir].conf[kc].n_atom; ka++) {
         if (!prot.res[ir].conf[kc].atom[ka].on) continue;
         n_atom++;
         av_sas += prot.res[ir].conf[kc].atom[ka].sas;
      }
      av_sas /= n_atom;
      prot.res[ir].conf[kc].E_rxn =av_sas;
   }

   i=ic;
   max_sas = prot.res[ir].conf[ic].E_rxn;
   for (kc=prot.res[ir].n_conf-1; kc >= nconf; kc--) {
      if (prot.res[ir].conf[kc].E_rxn > max_sas) {
         max_sas = prot.res[ir].conf[kc].E_rxn;
         i=kc;
      }
   }

   prot.res[ir].conf[i].history[2] = 'E';

   for (kc=prot.res[ir].n_conf-1; kc >= nconf; kc--) {
      if (prot.res[ir].conf[kc].history[2] != 'E') del_conf(&prot.res[ir], kc);
   }
   if (prot.res[ir].conf[ic].history[2] != 'E') del_conf(&prot.res[ir], ic);

   return max_sas;
}