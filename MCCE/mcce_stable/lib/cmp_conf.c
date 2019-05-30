#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mcce.h"

int cmp_conf(CONF conf1, CONF conf2, float IDEN_THR) {
    int iatom, jatom,n_on1,n_on2;
    float IDEN_THR2 = IDEN_THR*IDEN_THR;
    
    //if (strcmp(conf1.confName, conf2.confName)) return -1;
    if (conf1.n_atom != conf2.n_atom) return -1;
    
    n_on1 = 0;
    for (iatom = 0; iatom<conf1.n_atom; iatom++) {
	        if (conf1.atom[iatom].on) n_on1++;
	        }
    n_on2 = 0;
    for (jatom = 0; jatom<conf2.n_atom; jatom++) {
	        if (conf2.atom[jatom].on) n_on2++;
	        }
    if (n_on1 != n_on2) return -2;

    for (iatom = 0; iatom<conf1.n_atom; iatom++) {
        if (!conf1.atom[iatom].on) continue;
        for (jatom = 0; jatom<conf2.n_atom; jatom++) {
            if (!conf2.atom[jatom].on) continue;
            
            if (fabs(conf1.atom[iatom].crg - conf2.atom[jatom].crg) > 1e-3) continue;
            if (strncmp(conf1.atom[iatom].name+1, conf2.atom[jatom].name+1, 2) ) continue;
            if (ddvv(conf1.atom[iatom].xyz, conf2.atom[jatom].xyz) < IDEN_THR2) break;
        }
        if (jatom == conf2.n_atom) return -1;
    }
    return 0;
}

int cmp_conf_hv(CONF conf1, CONF conf2, float IDEN_THR) {
    int iatom, jatom;
    float IDEN_THR2 = IDEN_THR*IDEN_THR;
    int n_hv1=0, n_hv2=0;
    //if (strcmp(conf1.confName, conf2.confName)) return -1;
    //if (conf1.n_atom != conf2.n_atom) return -1;
    /* check if they same number of heavy atoms */
    for (iatom = 0; iatom<conf1.n_atom; iatom++) {
        if (!conf1.atom[iatom].on) continue;
        if (conf1.atom[iatom].name[1] == 'H') continue;
        n_hv1++;
    }
    for (jatom = 0; jatom<conf2.n_atom; jatom++) {
        if (!conf2.atom[jatom].on) continue;
        if (conf2.atom[jatom].name[1] == 'H') continue;
        n_hv2++;
    }
    if (n_hv1!=n_hv2) return -1;
    
    for (iatom = 0; iatom<conf1.n_atom; iatom++) {
        if (!conf1.atom[iatom].on) continue;
        if (conf1.atom[iatom].name[1] == 'H') continue;
        
        for (jatom = 0; jatom<conf2.n_atom; jatom++) {
            if (!conf2.atom[jatom].on) continue;
            if (conf2.atom[jatom].name[1] == 'H') continue;
            
            if (strncmp(conf1.atom[iatom].name+1, conf2.atom[jatom].name+1, 2) ) continue;
            if (ddvv(conf1.atom[iatom].xyz, conf2.atom[jatom].xyz) < IDEN_THR2) break;
        }
        if (jatom == conf2.n_atom) return -1;
    }
    return 0;
}

