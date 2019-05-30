#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "mcce.h"

int randomize_side(); 
int rand_rot_rule(RES *res, ROTAMER rule, float phi_degree);

int main()
{

   /* Do step 0, initialization */
   db_open();
   printf("Step 0. Initialize enviroment\n"); fflush(stdout);
   if (init()) {
      db_close();
      printf("Help message: double check file \"run.prm\" in current directory.\n");
      return USERERR;
   }
   else printf("Step 0 Done.\n\n");

   randomize_side();

   db_close();
   return 0;
}

int randomize_side() 
{  FILE *fp;
   int i;
   int C;
   char C_str[5];
   ROTAMER rule;
   char sbuff[MAXCHAR_LINE];
   float phi_rad;
   PROT prot;
   
   printf("   Load step 1 output file %s...\n", STEP1_OUT);
   if (!(fp=fopen(STEP1_OUT, "r"))) {
      printf("   FATAL: rotamers(): \"No step 1 output \"%s\".\n", STEP1_OUT);
      return USERERR;
   }   
   prot = load_pdb(fp);
   fclose(fp);
   printf("   Done\n\n");
   get_connect12(prot);
      
   /* construct rotamers */
   srand(time(0));
   for (i=0; i<prot.n_res; i++) {
      C = 0;
      while (1) {
         sprintf(C_str, "%d", C);
         if (param_get("ROTAMER", prot.res[i].resName, C_str, &rule)) break;
         sprintf(sbuff, " %s  ", rule.affected);
         sprintf(rule.affected, sbuff);
         phi_rad = (float) rand()/RAND_MAX*360.0; /* 0 - 360 degrees*/
         if (rand_rot_rule(&prot.res[i], rule, phi_rad)) {
            printf("   WARNING: randomize_side(): \"failed swinging rotamers for residue \"%s %d %c\"\"\n",
                   prot.res[i].resName, prot.res[i].resSeq, prot.res[i].chainID);
            printf("            No rotamers were made for this residue.\n");
         }
         C++;
      }
   }

   fp = fopen("rand.pdb", "w");
   write_pdb(fp, prot);
   fclose(fp);
   return 0;
}


int rand_rot_rule(RES *res, ROTAMER rule, float phi_degree)
{  VECTOR v1, v2, v3;
   GEOM op;
   LINE axis;
   int k, l;
   ATOM atom1, atom2;
   char found;
   float phi = phi_degree*3.1415926/180.0;


   if ((k=iatom(res->conf[1].confName, rule.atom2)) == -1) {
      printf("confname=\"%s\"\n",res->conf[1].confName);
      if ((k=iatom(res->conf[0].confName, rule.atom2)) == -1) {
         printf("   FATAL: place_rot_rule(): can't find atom \"%s\" in residue \"%s\"\n",
                 rule.atom2, res->resName);
         return USERERR;
      }
      else atom2 = res->conf[1].atom[k];
   }
   else atom2 = res->conf[1].atom[k];
   v2 = atom2.xyz;

   /* find the bond: atom1 is the first atom of the bond, it is one of the connected
    * atoms of atom2, but not necessary in this conformer or this residue */
   found = 0;
   if ((k=iatom(res->conf[1].confName, rule.atom1)) == -1) {
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
      atom1 = res->conf[1].atom[k];
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

   for (k=0; k<res->conf[1].n_atom; k++) {
      if (strstr(rule.affected, res->conf[1].atom[k].name)) {
         v3 = res->conf[1].atom[k].xyz;
         geom_apply(op, &v3);
         res->conf[1].atom[k].xyz = v3;
      }
   }


   return 0;
}
