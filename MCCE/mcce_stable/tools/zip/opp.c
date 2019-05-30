#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "mcce.h"

int main(int argc, char *argv[])
{  EMATRIX ematrix, ematrix2;
   int i, j, counter;
   struct stat buf;
   int is_dir;
   char option;
   char param[256];
   char match;
   
   if (argc<3) {
      printf("zopp [-x dir | -a dir | -d conf | -u dir]\n\
This program manipilates energy lookup table %s in working directory.\n\
       -x dir:    extract the energy lookup table to the directory named dir\n\
       -a dir:    append \"energies.opp\" in dir to the current one in working directory\n\
       -u dir:    update energies.opp and head3.lst with a new calculation\n\
       -d conf:   display pairwise interaction of conf (ID as in head3.lst)\n", ENERGY_TABLE);
       return 0;
   }
   else {
      if (argv[1][0] == '-') {
         option = argv[1][1];
         strcpy(param, argv[2]);
      }
      else {
         printf("Unrecognized switch\n");
         return 0;
      }
   }

   ematrix.n = 0;
   if (load_energies(&ematrix, ".") < 0) {
      printf("   Error loading energy table\n");
      return USERERR;
   }


   switch (option) {
      case 'x':
         /* create a directory */
         if (stat(param, &buf)) { /* no such file or dir */
            if (mkdir(param, 0755)) {
               printf("   FATAL: Failed creating directory %s, no write permission.\n", param);
               return USERERR;
            }
         }
         else { /* there is a file or dir named as this */
            is_dir = S_ISDIR(buf.st_mode);
            if (!is_dir) { /* it is not a directory */
               printf("   FATAL: %s exists but is not a directory.\n", param);
               return USERERR;
            }
         }
         if (extract_matrix(&ematrix, param)) {
            printf("   Error extracting energy table\n");
            return USERERR;
         }
         break;
      
      case 'a':
         /* load the second one */
         ematrix2.n = 0;
         if (load_energies(&ematrix2, argv[2]) < 0) {
            printf("   Error loading energy table\n");
            return USERERR;
         }
         /* mergy */
         if (ematrix.n != ematrix2.n) {
            printf("Energy lookup table sizes mismatch, %d and %d\n", ematrix.n, ematrix2.n);
            return USERERR;
         }
         for (i=0; i<ematrix.n; i++) {
            if (ematrix2.conf[i].on == 't') {
               memcpy(&ematrix.conf[i], &ematrix2.conf[i], sizeof(CONF_HEAD));
               memcpy(ematrix.pw[i], ematrix2.pw[i], ematrix.n*sizeof(PAIRWISE));
            }
         }
         /* write the mergied */
         write_energies(&ematrix, ".");
         
         free_ematrix(&ematrix);
         break;
      case 'u':
         /* load the second one */
         ematrix2.n = 0;
         if (load_energies(&ematrix2, argv[2]) < 0) {
            printf("   Error loading energy table\n");
            return USERERR;
         }
         /* update, will use the conformers in matrix 2 */
         for (i=0; i<ematrix2.n; i++) {
            if (ematrix2.conf[i].on == 't') { /* always the new calculation */
               memcpy(&ematrix.conf[i], &ematrix2.conf[i], sizeof(CONF_HEAD));
               memcpy(ematrix.pw[i], ematrix2.pw[i], ematrix.n*sizeof(PAIRWISE));
            }
            else { /* get a number from the old matrix */
               //pass;
            }
         }
         /* write the updated */
         write_energies(&ematrix2, ".");
         
         free_ematrix(&ematrix2);
         break;
      case 'd':
         match = '\0';
         for (i=0; i<ematrix.n; i++) {
            if (!strcmp(ematrix.conf[i].uniqID, param)) {
               counter = 0;
               for (j=0; j<ematrix.n; j++) {
                  counter++;
                  printf("%05d %s %8.3f%8.3f%8.3f%8.3f %s\n", counter, ematrix.conf[j].uniqID, ematrix.pw[i][j].ele, ematrix.pw[i][j].vdw, ematrix.pw[i][j].crt, ematrix.pw[i][j].ori, ematrix.pw[i][j].mark);
               }
               match = 't';
            }
         }
         if (match != 't') printf("conformer %s not found in energy table\n", param);
         break;
      default:
         printf("unrecognized option -%c", option);
         return 0;
   }
  

   free_ematrix(&ematrix);

   return 0;
}
