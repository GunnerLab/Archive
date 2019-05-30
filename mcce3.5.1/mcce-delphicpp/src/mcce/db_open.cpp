/*******************************************************************************
 * NAME
 *        db_open - open mcce parameter database
 *
 * SYNOPSIS
 *        #include <gdbm.h>
 *        #include <mcce.h>
 *
 *        int db_open();
 *
 * DESCRIPTION
 *        The db_open() function opens a paramter database for fast access to the
 *        parameter entries with the support  of  standard gdbm library.  If this
 *        function is called  while the database is  already open,  it prints out
 *        a warning message and use the existing database.  Use "-lgdbm"   option
 *        to  compile  these programs,  just  like  use "-lm" to compile programs
 *        calling math functions.
 *
 * RETURN VALUE
 *        The returned integer is the status, 0 is success, other numbers are the
 *        error code defined by gdbm.h.
 *
 * SEE ALSO
 *        db_close, iatom, param_get, param_sav
 *
 * EXAMPLE
 *       #include <stdio.h>
 *       #include <gdbm.h>
 *       #include "mcce.h"
 *
 *       int main()
 *       {  char  conformers[] = "ASP0, ASP1, ASP-";
 *          int   i = 10;
 *          float pKa = 5.7;
 *
 *          char  ret_conf[160];
 *          int   ret_i = 0;
 *          float ret_pKa = 0.0;
 *
 *          db_open();
 *
 *          param_sav("CONFLIST", "ASP",  "",   &conformers, sizeof(conformers));
 *          param_sav("IATOM",    "ASP0", "CG", &i, sizeof(int));
 *          param_sav("PKA",      "ASP-", "",   &pKa, sizeof(float));
 *
 *          param_get("CONFLIST", "ASP", "", &ret_conf);
 *          printf("%s\n",ret_conf);
 *
 *          ret_i = iatom("ASP0", "CG");
 *          printf("%d\n",ret_i);
 *
 *          param_get("PKA", "ASP-", "", &ret_pKa);
 *          printf("%.2f\n",ret_pKa);
 *
 *          db_close();
 *          return 0;
 *       }
 *
 * AUTHOR
 *        Junjun Mao, 06/02/2003
 *******************************************************************************/

#include "gdbm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcce.h"

GDBM_FILE param_db;
char gdbm_file[256];

int db_open()
{
    GDBM_FILE temp_db;
    
    //remove(DUMMY_GDBM);

    strcpy(gdbm_file, DUMMY_GDBM);
    mkstemp(gdbm_file);
    
    /* open the param database */
    temp_db = gdbm_open(gdbm_file, 16*1024, GDBM_WRCREAT, 0666, 0);
    
    if (gdbm_errno == 0)          /* success, accept temp_db */
        param_db = temp_db;
    else if (gdbm_errno == 10)    /* open existing database, use old param_db */
        printf("   db_open(): Warning, database is already open.\n");
    
    return gdbm_errno;
}


int db_close()
{
	gdbm_close(param_db);   /* close the param database */
	remove(gdbm_file);     /* delete temporary file */
	return 0;
}


int iatom(char *conf_name, char *atom_name)
{  datum pkey, pvalue;
   char key[MAXCHAR_LINE];
   char sbuff[MAXCHAR_LINE];
   int i_atom;

   /* construct the atom key */
   strcpy(key, "IATOM");
   strip(sbuff, conf_name); strcat(key, sbuff);
   strip(sbuff, atom_name); strcat(key, sbuff);
   pkey.dptr = key;
   pkey.dsize = strlen(key);

   /* find the atom in the database */
   if (gdbm_exists(param_db, pkey)) {        /* existing key */
      pvalue = gdbm_fetch(param_db, pkey);
      /* cast generic pointer pvalue.dptr to integer pointer by (int *), then
       * dereference the pointer to an integer by *,   so the return value is
       * an integer */
       i_atom = * (int *) pvalue.dptr;
       free(pvalue.dptr);
      return  i_atom;
   }
   else return -1;                           /* not in the database */
}


int param_sav(char *key1, char *key2, char *key3, void *value, int s)
{
	datum pkey, pvalue;
	char key[MAXCHAR_LINE];
	char sbuff[MAXCHAR_LINE];

	/* convert 3 key strings to one key, leading and ending spaces stripped */
	strip(key, key1);
	strip(sbuff, key2); strcat(key, sbuff);
	strip(sbuff, key3); strcat(key, sbuff);

	/* condtruct key, value pair */
	pkey.dptr = key;
	pkey.dsize = strlen(key);
	pvalue.dptr = (char *) value;
	pvalue.dsize = s;    /* The data size is passed in rather measured by strlen().
	 * When a generic pointer was passed in, the variable type
	 * was lost.     It became not reliable to detect the data
	 * length by the terminating NULL character,  thought most
	 * time strlen(value) would return the right size. */

	return gdbm_store(param_db, pkey, pvalue, GDBM_REPLACE);
}


int param_get(char *key1, char *key2, char *key3, void *value)
/* WARNING: if the expected value is a string, it has to be long enough
 *          (>=MAXCHAR_LINE) to accept the stored value to avoid over
 *          boundary writing.
 */
{  datum pkey, pvalue;
   char key[MAXCHAR_LINE];
   char sbuff[MAXCHAR_LINE];

   /* convert 3 key strings to one key, leading and ending spaces stripped */
   strip(key, key1);
   strip(sbuff, key2); strcat(key, sbuff);
   strip(sbuff, key3); strcat(key, sbuff);

   /* make the key string to be the database search key */
   pkey.dptr = key;
   pkey.dsize = strlen(key);

   /* get the value */
   if (!gdbm_exists(param_db, pkey)) return -1; /* failure */
   else {                                       /* success */
      pvalue = gdbm_fetch(param_db, pkey);
      memcpy(value, pvalue.dptr, pvalue.dsize);
      free(pvalue.dptr);
      return 0;
   }
}

int param_exist(char *key1, char *key2, char *key3)
{
    datum pkey;
    char key[MAXCHAR_LINE];
    char sbuff[MAXCHAR_LINE];

    /* convert 3 key strings to one key, leading and ending spaces stripped */
    strip(key, key1);
    strip(sbuff, key2); strcat(key, sbuff);
    strip(sbuff, key3); strcat(key, sbuff);

    /* make the key string to be the database search key */
    pkey.dptr = key;
    pkey.dsize = strlen(key);

    /* get the value */
    if (!gdbm_exists(param_db, pkey)) {
        return 0; /* failure */
    }
    else {
        return 1;  /* success */
    }
}
