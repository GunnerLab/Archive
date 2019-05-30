#include <stdio.h>
#include <math.h>
#include "mcce.h"

#define VDW_CUTOFF_NEAR  1
#define VDW_ELIMIT_NEAR  999
#define VDW_CUTOFF_FAR   10
#define VDW_ELIMIT_FAR   0

/* This function uses AMBER C12 and C6 parameters to calculate
* Van der Waals potential. The parameters are read from paramter
* file amber.vdw and were originally cited from:
* http://solar.uits.indiana.edu/ad_html/Using_AutoDock_305.a.shtml#29613
*/

float cutoff_near2 = VDW_CUTOFF_NEAR * VDW_CUTOFF_NEAR;
float cutoff_far2  = VDW_CUTOFF_FAR  * VDW_CUTOFF_FAR;

float vdw(ATOM atom1, ATOM atom2)
{
    float       e = 0;
    float       d2, d6, d12;      /* distance with power 2, 6, and 12 */
    float       C12, C6;
    char        pair[4];
    VECTOR      v1, v2;
    FILE        *debug_fp;

    if (!atom1.on) return e;
    if (!atom2.on) return e;

    v1 = atom1.xyz;
    v2 = atom2.xyz;

    d2 = ddvv(v1, v2);

    if (d2 > cutoff_far2)
        e = VDW_ELIMIT_FAR;             /* Cutoff */
    else if (d2 < cutoff_near2)
        e = VDW_ELIMIT_NEAR;            /* Cutoff */
    else
    {                                   /* 12-6 L-J potential */
        pair[0] = atom1.name[1];        /* element name */
        pair[1] = '-';
        pair[2] = atom2.name[1];        /* element name */
        pair[3] = '\0';

        if (param_get("VDWAMBER", "C12", pair, &C12)) {
            param_get("VDWAMBER", "C12", "X-X", &C12);
            debug_fp = fopen(env.debug_log,"a");
            fprintf(debug_fp, "VDWAMBER C12   %s  %16.6f\n", pair, C12);
            fclose(debug_fp);
            param_sav("VDWAMBER", "C12", pair, &C12,sizeof(C12));
        }

        if(param_get("VDWAMBER", "C6", pair, &C6)) {
            param_get("VDWAMBER", "C6", "X-X", &C6);
            debug_fp = fopen(env.debug_log,"a");
            fprintf(debug_fp, "VDWAMBER C6    %s  %16.6f\n", pair, C6);
            fclose(debug_fp);
            param_sav("VDWAMBER", "C6", pair, &C6,sizeof(C6));
        }

        d6 = d2*d2*d2;
        d12 = d6*d6;
        e = C12/d12 - C6/d6;
    }
    
    return e;
}

float vdw_sim(ATOM atom1, ATOM atom2)
{   float f1 = env.vdwf1, f2 = env.vdwf2;   
                                /* vdw constants
                                 * vdw = epsilon((sigma/(r1+r2))^12 - (sigma/(r1+r2))^6)
                                 * f1: sigma=f1*(r1+r2)
                                 * f2: epsilon=f2
                                 */

    float       e = 0;
    float       d2, d6, d12;      /* distance with power 2, 6, and 12 */
    float       C12, C6;
    float s, s6, s12;
    VECTOR      v1, v2;

    if (!atom1.on) return e;
    if (!atom2.on) return e;

    v1 = atom1.xyz;
    v2 = atom2.xyz;

    d2 = ddvv(v1, v2);

    if (d2 > cutoff_far2)
        e = VDW_ELIMIT_FAR;             /* Cutoff */
    else if (d2 < cutoff_near2)
        e = VDW_ELIMIT_NEAR;            /* Cutoff */
    else
    {                                   /* 12-6 L-J potential */
        d6 = d2*d2*d2;
        d12 = d6*d6;

        s =  f1*(atom1.rad+atom1.rad);
        s6 = s*s*s*s*s*s;
        s12= s6*s6;

        C12=f2*s12;
        C6 =f2*s6;

        e = C12/d12 - C6/d6;
    }

    return e;
}

VECTOR vdw_frc(VECTOR v1, VECTOR v2, float C6, float C12) { 
    
    VECTOR frc;
    float d2, d4, d8, d14;
    VECTOR frc6,frc12;
    
    d2 = ddvv(v1, v2);
    if (d2 > cutoff_far2)  {frc.x=0;frc.y=0;frc.z=0;return frc;}
    if (d2 < cutoff_near2) d2 = cutoff_near2;
    
    d4 = d2*d2;
    d8 = d4*d4;
    d14 = d8*d4*d2;
    
    frc12 = vector_rescale(vector_vminusv(v2,v1), (-12.*C12/d14));
    frc6  = vector_rescale(vector_vminusv(v2,v1), (  6.*C6/d8));
    frc = vector_vplusv(frc12, frc6);
    return frc;
}

