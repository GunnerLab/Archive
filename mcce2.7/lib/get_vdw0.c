#include <stdio.h>
#include "mcce.h"

int     n_elem;
float   C6_matrix[N_ELEM_MAX][N_ELEM_MAX];
float   C12_matrix[N_ELEM_MAX][N_ELEM_MAX];

void get_vdw0(PROT prot)
{
    int i, j;
    
    assign_rad(prot);
    assign_crg(prot);
    get_connect12(prot);
    setup_C6_C12(prot);
    setup_vdw_fast(prot);
    
    for (i=0; i<prot.n_res; i++) {
        setup_connect_res(prot, i);
        prot.res[i].conf[0].E_vdw0 = 0.0; /* conformer 0 is defined to have 0 torsion */
        
        for (j=1; j<prot.res[i].n_conf; j++) {
            prot.res[i].conf[j].E_vdw0 = vdw_conf_fast(i, j, i, j, prot, 0);
        }
        free_connect_res(prot, i);
    }
    return;
}
