#include <stdio.h>
#include <stdlib.h>
#include "lib/mcce.h"

void welcome();

int main()
{
   /* Welcome */
   welcome();


   /* Do step 0, initialization */
   printf ("Step 0. Loading parameters\n");
   fflush (stdout);
   if (init ())
      return EXIT_FAILURE;
   else
      printf ("Step 0 Done.\n\n");


   /* Do step 1, premcce */
   if (env.do_premcce) {
      printf("Step 1. Test and format structral file\n"); fflush(stdout);
      if (premcce()) {return USERERR;}
      else printf("Step 1 Done.\n\n");
   }
   else printf("Not doing \"Step 1. Test and format structral file\"\n\n");


   /* Do step 2. rotamers */
   if (env.do_rotamers) {
      printf("Step 2. Make multi side chain conformers\n"); fflush(stdout);
      if (rotamers()) {return USERERR;}
      else printf("Step 2 Done.\n\n");
   }
   else printf("Not doing \"Step 2. Make multi side chain conformers\"\n\n");

   /* Do step 3. energies */
   if (env.do_energies) {
      printf("Step 3. Compute energy lookup table\n"); fflush(stdout);
      if (energies()) {return USERERR;}
      else printf("Step 3 Done.\n\n");
   }
   else printf("Not doing \"Step 3. Compute energy lookup table\"\n\n");

   /* Do step 4. Monte Carlo */
   if (env.do_monte) {
      printf("Step 4. Monte Carlo Sampling\n"); fflush(stdout);
      if (!env.monte_adv_opt) {
      if (monte()) {return USERERR;}
           else printf("Step 4 Done.\n\n");
       }
       else {
           if (monte2()) {return USERERR;}
           else printf("Step 4 Done.\n\n");
       }
   }
   else printf("Not doing \"Step 4. Monte Carlo Sampling\"\n\n");

   free_param();

   return 0;
}

void welcome()
{ printf ("===========================================================\n");
  printf ("<<< MCCE Multi-Conformation Continuum Electrostatics >>>   \n");
  printf ("-----------------------------------------------------------\n");
  printf ("Version:        2.7                                        \n");
  printf ("Home Page:      http://pka.engr.ccny.cuny.edu/~mcce        \n");
  printf ("Support:        jmao@ccny.cuny.edu                         \n");
  printf ("Developed by:   Junjun Mao, Yifan Song, Marilyn Gunner     \n");
  printf ("===========================================================\n");
  printf ("Last Updates:                                              \n");
  printf ("   03/23/2011  Two coding enhancement added to MCCE2.3     \n");
  printf ("               In memory parameter loading                 \n");
  printf ("               energy lookup table in text, non-0 only     \n");
  printf ("===========================================================\n");
  fflush (stdout);

   return;
}
