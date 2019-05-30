#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcce.h"
#define  ATOM_RAD 2.0
#define  TRUE     1
#define  FALSE    0

typedef struct {
  int x, y, z;
} INT_VECTOR;


int  insAtomToConf(CONF *conf, int ins);
void calc_gsize();
void alloc_3d_array(int x_size, int y_size, int z_size);
void free_3d_array(int x_size, int y_size, int z_size);
int  fill_grid(PROT prot);
int  mkacc(ATOM *atom);
float get_sas_res(ATOM atom, RES res);
void get_pdb_size(PROT prot);
void extend_grid();
void set_vdw_rad(PROT prot, float probe_rad); 
void reset_atom_rad(PROT prot, float probe_rad);
void trim_conf(PROT prot);
void copy_sas(PROT src, PROT target);

INT_VECTOR gsize;
INT_VECTOR gindex;
VECTOR xyz_min, xyz_max;
float  PROBE_RAD;
float  grid_interval;
float  area_coeff;
CONF   ***grid;
int    num_pts;


/* preset level 1: i = j = 2, 122 uniformly distributed points on a sphere */
float point_preset[][3] = {
      { 0.580799102783,      0.760826885700,      0.289507985115},
      { 0.525731086731,      0.850650787354,      0.000000000000},
      { 0.000000000000,      0.525731086731,     -0.850650787354},
      {-0.525731086731,      0.850650787354,      0.000000000000},
      { 0.000000000000,      0.525731086731,      0.850650787354},
      { 0.850650787354,      0.000000000000,      0.525731086731},
      {-0.850650787354,      0.000000000000,      0.525731086731},
      { 0.000000000000,     -0.525731086731,      0.850650787354},
      {-0.850650787354,      0.000000000000,     -0.525731086731},
      {-0.525731086731,     -0.850650787354,      0.000000000000},
      { 0.850650787354,      0.000000000000,     -0.525731086731},
      { 0.525731086731,     -0.850650787354,      0.000000000000},
      { 0.000000000000,     -0.525731086731,     -0.850650787354},
      { 0.577350258827,      0.577350258827,      0.577350258827},
      { 0.356822103262,      0.000000000000,     -0.934172332287},
      {-0.577350258827,      0.577350258827,     -0.577350258827},
      {-0.356822103262,      0.000000000000,      0.934172332287},
      { 0.934172332287,      0.356822103262,      0.000000000000},
      { 0.000000000000,      0.934172332287,     -0.356822103262},
      {-0.577350258827,      0.577350258827,      0.577350258827},
      { 0.577350258827,     -0.577350258827,      0.577350258827},
      {-0.934172332287,     -0.356822103262,      0.000000000000},
      { 0.577350258827,      0.577350258827,     -0.577350258827},
      { 0.000000000000,      0.934172332287,      0.356822103262},
      {-0.577350258827,     -0.577350258827,      0.577350258827},
      { 0.934172332287,     -0.356822103262,      0.000000000000},
      { 0.356822103262,      0.000000000000,      0.934172332287},
      {-0.356822103262,      0.000000000000,     -0.934172332287},
      { 0.000000000000,     -0.934172332287,     -0.356822103262},
      { 0.577350258827,     -0.577350258827,     -0.577350258827},
      { 0.000000000000,     -0.934172332287,      0.356822103262},
      {-0.934172332287,      0.356822103262,      0.000000000000},
      {-0.577350258827,     -0.577350258827,     -0.577350258827},
      { 1.000000000000,      0.000000000000,      0.000000000000},
      {-0.500000000000,     -0.309017002583,     -0.809017002583},
      {-1.000000000000,      0.000000000000,      0.000000000000},
      { 0.500000000000,     -0.309017002583,      0.809017002583},
      { 0.500000000000,      0.309017002583,     -0.809017002583},
      {-0.500000000000,      0.309017002583,     -0.809017002583},
      {-0.500000000000,     -0.309017002583,      0.809017002583},
      { 0.809017002583,     -0.500000000000,     -0.309017002583},
      {-0.309017002583,     -0.809017002583,      0.500000000000},
      {-0.309017002583,      0.809017002583,     -0.500000000000},
      {-0.809017002583,      0.500000000000,      0.309017002583},
      { 0.000000000000,     -1.000000000000,      0.000000000000},
      { 0.809017002583,      0.500000000000,     -0.309017002583},
      { 0.000000000000,      1.000000000000,      0.000000000000},
      {-0.809017002583,     -0.500000000000,      0.309017002583},
      { 0.809017002583,      0.500000000000,      0.309017002583},
      { 0.309017002583,      0.809017002583,     -0.500000000000},
      {-0.309017002583,     -0.809017002583,     -0.500000000000},
      { 0.000000000000,      0.000000000000,      1.000000000000},
      { 0.309017002583,      0.809017002583,      0.500000000000},
      { 0.000000000000,      0.000000000000,     -1.000000000000},
      { 0.309017002583,     -0.809017002583,      0.500000000000},
      {-0.309017002583,      0.809017002583,      0.500000000000},
      { 0.309017002583,     -0.809017002583,     -0.500000000000},
      {-0.809017002583,      0.500000000000,     -0.309017002583},
      {-0.809017002583,     -0.500000000000,     -0.309017002583},
      { 0.809017002583,     -0.500000000000,      0.309017002583},
      { 0.500000000000,      0.309017002583,      0.809017002583},
      { 0.500000000000,     -0.309017002583,     -0.809017002583},
      {-0.500000000000,      0.309017002583,      0.809017002583},
      { 0.178925782442,      0.291291087866,     -0.939752638340},
      {-0.580799102783,      0.760826885700,     -0.289507985115},
      {-0.178925782442,      0.291291087866,      0.939752638340},
      { 0.759724855423,      0.650244653225,     -0.000000000141},
      {-0.291291087866,      0.939752638340,     -0.178925782442},
      {-0.289507985115,      0.580799102783,      0.760826885700},
      { 0.760826885700,     -0.289507985115,      0.580799102783},
      {-0.939752638340,     -0.178925782442,      0.291291087866},
      { 0.580799102783,      0.760826885700,     -0.289507985115},
      {-0.000000000141,      0.759724855423,      0.650244653225},
      {-0.289507985115,     -0.580799102783,      0.760826885700},
      { 0.939752638340,     -0.178925782442,      0.291291087866},
      { 0.289507985115,      0.580799102783,      0.760826885700},
      {-0.178925782442,     -0.291291087866,      0.939752638340},
      { 0.178925782442,      0.291291087866,      0.939752638340},
      {-0.291291087866,      0.939752638340,      0.178925782442},
      {-0.650244653225,     -0.000000000141,     -0.759724855423},
      {-0.760826885700,     -0.289507985115,      0.580799102783},
      {-0.580799102783,      0.760826885700,      0.289507985115},
      {-0.760826885700,      0.289507985115,     -0.580799102783},
      {-0.291291087866,     -0.939752638340,     -0.178925782442},
      { 0.650244653225,     -0.000000000141,      0.759724855423},
      { 0.760826885700,     -0.289507985115,     -0.580799102783},
      { 0.291291087866,      0.939752638340,     -0.178925782442},
      { 0.760826885700,      0.289507985115,      0.580799102783},
      { 0.939752638340,     -0.178925782442,     -0.291291087866},
      {-0.178925782442,      0.291291087866,     -0.939752638340},
      { 0.291291087866,     -0.939752638340,      0.178925782442},
      {-0.939752638340,      0.178925782442,      0.291291087866},
      { 0.000000000141,     -0.759724855423,      0.650244653225},
      {-0.760826885700,      0.289507985115,      0.580799102783},
      {-0.580799102783,     -0.760826885700,     -0.289507985115},
      { 0.289507985115,     -0.580799102783,      0.760826885700},
      {-0.650244653225,      0.000000000141,      0.759724855423},
      { 0.291291087866,     -0.939752638340,     -0.178925782442},
      { 0.939752638340,      0.178925782442,      0.291291087866},
      { 0.178925782442,     -0.291291087866,      0.939752638340},
      { 0.650244653225,      0.000000000141,     -0.759724855423},
      {-0.759724855423,     -0.650244653225,     -0.000000000141},
      { 0.580799102783,     -0.760826885700,      0.289507985115},
      {-0.289507985115,     -0.580799102783,     -0.760826885700},
      { 0.289507985115,      0.580799102783,     -0.760826885700},
      {-0.760826885700,     -0.289507985115,     -0.580799102783},
      {-0.759724855423,      0.650244653225,      0.000000000141},
      { 0.000000000141,      0.759724855423,     -0.650244653225},
      { 0.289507985115,     -0.580799102783,     -0.760826885700},
      {-0.939752638340,     -0.178925782442,     -0.291291087866},
      {-0.289507985115,      0.580799102783,     -0.760826885700},
      { 0.939752638340,      0.178925782442,     -0.291291087866},
      {-0.000000000141,     -0.759724855423,     -0.650244653225},
      { 0.760826885700,      0.289507985115,     -0.580799102783},
      {-0.939752638340,      0.178925782442,     -0.291291087866},
      { 0.178925782442,     -0.291291087866,     -0.939752638340},
      { 0.580799102783,     -0.760826885700,     -0.289507985115},
      {-0.580799102783,     -0.760826885700,      0.289507985115},
      {-0.291291087866,     -0.939752638340,      0.178925782442},
      { 0.291291087866,      0.939752638340,      0.178925782442},
      {-0.178925782442,     -0.291291087866,     -0.939752638340},
      { 0.759724855423,     -0.650244653225,      0.000000000141}
};


int surfw(PROT protein, float probe_rad)
{
  int   i, j, k;
  float res_sas;                /* sas per residue (disregard all other residues) */
  float atom_sas;               /* sas per atom */
  PROT  prot;

  prot = new_prot();
  cpy_prot(&prot, &protein);
  trim_conf(prot);

  PROBE_RAD  = probe_rad;       /* get parameters */
  grid_interval = PROBE_RAD + ATOM_RAD;
  num_pts = 122;
  area_coeff = 4 * 3.1415926 / (float)num_pts;

  get_pdb_size(prot);

  extend_grid();

  calc_gsize();

  gindex.x = gsize.x - 1;
  gindex.y = gsize.y - 1;
  gindex.z = gsize.z - 1;

  set_vdw_rad(prot, PROBE_RAD);

  alloc_3d_array(gsize.x, gsize.y, gsize.z);
  /* end drawing the grid */

  fill_grid(prot);

  /* make the surface for each atom */
  for (i = 0 ; i < prot.n_res; i++) {
    prot.res[i].sas = 0;
    for (j = 0; j < prot.res[i].n_conf; j++) {
      for (k = 0; k < prot.res[i].conf[j].n_atom; k++) {
        if (!(prot.res[i].conf[j].atom[k].on)) {
           /* printf("%s, %d, %s\n", prot.res[i].resName, prot.res[i].resSeq, prot.res[i].conf[j].atom[k].name); */
           continue;
        }
	mkacc(&(prot.res[i].conf[j].atom[k]));
	prot.res[i].sas += prot.res[i].conf[j].atom[k].sas;
      }
    }
  }

  /* compute the % of solvent-accessible surface of each residue */
  for (i = 0 ; i < prot.n_res; i++) {
    res_sas = 0;
    for (j = 0; j < prot.res[i].n_conf; j++) {
      for (k = 0; k < prot.res[i].conf[j].n_atom; k++) {
        if (!(prot.res[i].conf[j].atom[k].on)) continue;
        atom_sas = get_sas_res(prot.res[i].conf[j].atom[k], prot.res[i]);
        res_sas += atom_sas;
      }
    }
    prot.res[i].sas /= res_sas;
  }

  reset_atom_rad(prot, PROBE_RAD);

  /* print the surface area per atom */
  /*
  for (i = 0; i < prot.n_res; i++) {
    for (j = 0; j < prot.res[i].n_conf; j++) {
      for (k = 0; k < prot.res[i].conf[j].n_atom; k++) {
        if (prot.res[i].conf[j].atom[k].on) {
   	    printf("%4s  %4s %c %3d%8.3f\n",
            prot.res[i].conf[j].atom[k].name, prot.res[i].conf[j].atom[k].resName,
            prot.res[i].conf[j].atom[k].chainID, prot.res[i].conf[j].atom[k].resSeq,
            prot.res[i].conf[j].atom[k].sas);
	}
      }
    }
  }
  */

  /* print the surface area per residue */
  /*
  for (i = 0; i < prot.n_res; i++) {
    printf("%4s %c %4d %8.3f\n", prot.res[i].resName, prot.res[i].chainID, prot.res[i].resSeq,
            prot.res[i].sas);
  }
  */

  free_3d_array(gsize.x, gsize.y, gsize.z);
  copy_sas(prot, protein);
  del_prot(&prot);
  return 0;

}


/* calculate number of grids in the x, y, z axis */
void calc_gsize()
{
  gsize.x = (int)((xyz_max.x - xyz_min.x) / grid_interval + 1);
  gsize.y = (int)((xyz_max.y - xyz_min.y) / grid_interval + 1);
  gsize.z = (int)((xyz_max.z - xyz_min.z) / grid_interval + 1);

  return;
}


/* remove all conformers other than conform 0 and 1 */
void trim_conf(PROT prot) {
  int i, j;

  for (i = 0; i < prot.n_res; i++) {
    if (prot.res[i].n_conf <= 2) continue;
    for (j = prot.res[i].n_conf; j > 1; j--) {
      del_conf( &(prot.res[i]), j-1 );
    }
  }

  return;
}


/* malloc a 3-D array of size x_size * y_size * z_size, and set array[i][j][k] to 0 */
void alloc_3d_array (int x_size, int y_size, int z_size)
{
  int i, j, k;

  grid = (CONF ***)malloc(x_size * sizeof(CONF **)); /* cast "void *" to "CONF ***" */

  for (i = 0; i < x_size; i++) 
    grid[i] = (CONF **) malloc( y_size * sizeof(CONF *) ); 

  for (i = 0; i < x_size; i++) {
    for (j = 0; j < y_size; j++)
      grid[i][j] = (CONF *) malloc(z_size * sizeof(CONF)); 
  }
  
  for (i = 0; i < x_size; i++) 
    for (j = 0; j < y_size; j++)
      for (k = 0; k < z_size; k++) {
	grid[i][j][k].n_atom = 0;
        grid[i][j][k].atom = NULL;
      }

  return;
}



int fill_grid(PROT prot)
{
  int ix, iy, iz;                  /* x, y, z grid index for an atom */
  int i, j, k;
  int iConf;
  int ins_pos;

  /* put each atom into approriate grid box */
  for (i = 0; i < prot.n_res; i++) {
    iConf = 0;
    for (j = 0; j < prot.res[i].n_conf; j++) {
      for (k = 0; k < prot.res[i].conf[j].n_atom; k++) {
        if (!prot.res[i].conf[j].atom[k].on) continue;
        ix = (int) ((prot.res[i].conf[j].atom[k].xyz.x - xyz_min.x) / grid_interval);
        iy = (int) ((prot.res[i].conf[j].atom[k].xyz.y - xyz_min.y) / grid_interval);
        iz = (int) ((prot.res[i].conf[j].atom[k].xyz.z - xyz_min.z) / grid_interval);

        ins_pos = insAtomToConf(&(grid[ix][iy][iz]), grid[ix][iy][iz].n_atom);
        grid[ix][iy][iz].atom[ins_pos] = prot.res[i].conf[j].atom[k];
      }
      iConf++;
    }
  }

  return 0;
}



int mkacc(ATOM *atom)
{
  VECTOR point;
  int i, j, k, l, m;
  float d_square;
  int count;
  int ix, iy, iz;
  int ax, ay, az;         /* the grid index of this atom */
  int status;

  count = num_pts;

  ax = (int) ((atom -> xyz.x - xyz_min.x) / grid_interval);
  ay = (int) ((atom -> xyz.y - xyz_min.y) / grid_interval);
  az = (int) ((atom -> xyz.z - xyz_min.z) / grid_interval);

  /* loop over each point */

  for (m = 0; m < num_pts; m++) {
    status = TRUE;

    point.x = point_preset[m][0] * atom -> rad + atom -> xyz.x;
    point.y = point_preset[m][1] * atom -> rad + atom -> xyz.y;
    point.z = point_preset[m][2] * atom -> rad + atom -> xyz.z;

    /* find out which grid this point belongs to */
    ix = (int) ((point.x - xyz_min.x) / grid_interval);
    iy = (int) ((point.y - xyz_min.y) / grid_interval);
    iz = (int) ((point.z - xyz_min.z) / grid_interval);

    if (grid[ix][iy][iz].n_atom >= 1) {
      for (l = 0; l < grid[ix][iy][iz].n_atom; l++) {
        if (grid[ix][iy][iz].atom[l].on &&
            status == TRUE && 
            (ix != ax || iy != ay || iz != az ||
             memcmp(atom, &(grid[ix][iy][iz].atom[l]), sizeof(ATOM)))) {
            d_square = ddvv(point, grid[ix][iy][iz].atom[l].xyz);
            if (d_square < grid[ix][iy][iz].atom[l].rad * grid[ix][iy][iz].atom[l].rad) {
              status = FALSE;
              count --;
            }
        }
      }
    }

    for (i = ix - 1; i <= ix + 1; i++) {
      if (status == FALSE) break;
      for (j = iy - 1; j <= iy + 1; j++) {
        if (status == FALSE) break;
        for (k = iz - 1; k <= iz + 1; k++) {
          if (status == FALSE) break;
          for (l = 0; l < grid[i][j][k].n_atom; l++) {
            if (grid[i][j][k].atom[l].on && status == TRUE) {
               if (i != ax || j != ay || k != az ||
                   memcmp(atom, &(grid[i][j][k].atom[l]), sizeof(ATOM))) {
                  d_square = ddvv(point, grid[i][j][k].atom[l].xyz);
                  if (d_square < grid[i][j][k].atom[l].rad * grid[i][j][k].atom[l].rad) {
                      status = FALSE;
                      count --;
	           }
               }      
            }
          }
        }      
      }
    }
  }

  atom->sas = area_coeff * atom->rad * atom->rad * (float) count;

  return 0;
}


void get_pdb_size(PROT prot)
{
  int i, j, k;

  xyz_min.x =  1.0E20;
  xyz_max.x = -1.0E20;
  xyz_min.y =  1.0E20;
  xyz_max.y = -1.0E20;
  xyz_min.z =  1.0E20;
  xyz_max.z = -1.0E20;

  for (i = 0; i < prot.n_res; i++) {
    for (j = 0; j < prot.res[i].n_conf; j++) {
      for (k = 0; k < prot.res[i].conf[j].n_atom; k++) {
        if (!(prot.res[i].conf[j].atom[k].on)) continue;

        if      (xyz_min.x > prot.res[i].conf[j].atom[k].xyz.x)
                 xyz_min.x = prot.res[i].conf[j].atom[k].xyz.x;
        if       (xyz_max.x < prot.res[i].conf[j].atom[k].xyz.x)
                 xyz_max.x = prot.res[i].conf[j].atom[k].xyz.x;

        if      (xyz_min.y > prot.res[i].conf[j].atom[k].xyz.y)
                 xyz_min.y = prot.res[i].conf[j].atom[k].xyz.y;
        if      (xyz_max.y < prot.res[i].conf[j].atom[k].xyz.y)
                 xyz_max.y = prot.res[i].conf[j].atom[k].xyz.y;

        if      (xyz_min.z > prot.res[i].conf[j].atom[k].xyz.z)
                 xyz_min.z = prot.res[i].conf[j].atom[k].xyz.z;
        if      (xyz_max.z < prot.res[i].conf[j].atom[k].xyz.z)
                 xyz_max.z = prot.res[i].conf[j].atom[k].xyz.z;
      }
    }
  }

  /*
  printf("min: %8.3f %8.3f %8.3f\n", xyz_min.x, xyz_min.y, xyz_min.z);
  printf("max: %8.3f %8.3f %8.3f\n", xyz_max.x, xyz_max.y, xyz_max.z);
   */

  return;
}


void extend_grid()
{
  xyz_min.x -= 2 * grid_interval+1.0;
  xyz_min.y -= 2 * grid_interval+1.0;
  xyz_min.z -= 2 * grid_interval+1.0;
  xyz_max.x += 2 * grid_interval+1.0;
  xyz_max.y += 2 * grid_interval+1.0;
  xyz_max.z += 2 * grid_interval+1.0;

  return;
}


void set_vdw_rad(PROT prot, float probe_rad)
{

  int i, j, k;

  for (i = 0; i < prot.n_res; i++) 
    for (j = 0; j < prot.res[i].n_conf; j++) 
      for (k = 0; k < prot.res[i].conf[j].n_atom; k++) 
        prot.res[i].conf[j].atom[k].rad += probe_rad;

  return;
} 


void reset_atom_rad(PROT prot, float probe_rad)
{

  int i, j, k;

  for (i = 0; i < prot.n_res; i++) 
    for (j = 0; j < prot.res[i].n_conf; j++) 
      for (k = 0; k < prot.res[i].conf[j].n_atom; k++) 
        prot.res[i].conf[j].atom[k].rad -= probe_rad;

  return;
} 


void free_3d_array(int x_size, int y_size, int z_size)
{
  int i, j, k;

  for (i = 0; i < x_size; i++)
    for (j = 0; j < y_size; j++) {
      for (k = 0; k < z_size; k++) { 
        if (grid[i][j][k].atom) 
          free(grid[i][j][k].atom);
      }
      free(grid[i][j]);
    }

  for (i = 0; i < x_size; i++) 
    free(grid[i]);

  free(grid);

  return;
}


float get_sas_res(ATOM atom, RES res)
{
  int j, k, m;
  int count;
  int status;
  VECTOR point;
  float distance;
  float surface_area;

  count = num_pts;

  for (m = 0; m < num_pts; m++) {
    status = TRUE;
    point.x = point_preset[m][0] * atom.rad + atom.xyz.x;
    point.y = point_preset[m][1] * atom.rad + atom.xyz.y;
    point.z = point_preset[m][2] * atom.rad + atom.xyz.z;

    for (j = 0; j < res.n_conf; j++) {
      if (status == FALSE) break;
      for (k = 0; k < res.conf[j].n_atom; k++) {
        if (res.conf[j].atom[k].on &&
            status == TRUE && 
            memcmp(&atom, &(res.conf[j].atom[k]), sizeof(ATOM))) {
              distance = ddvv(point, res.conf[j].atom[k].xyz);
              if (distance < res.conf[j].atom[k].rad * res.conf[j].atom[k].rad) {
                count--; status = FALSE;
              }
        }
      }
    }
  }

  surface_area = area_coeff * atom.rad * atom.rad * (float)count;
  return surface_area;
}



int insAtomToConf(CONF *conf, int ins)
{ if (ins > conf->n_atom) {
    printf("insAtomToConf(): off range insertion.\n");
    return USERERR;
  }

  /* resize the memory */
  if (!(conf->atom = (ATOM *) realloc(conf->atom, (conf->n_atom + 1) * sizeof(ATOM)))) {
    printf("insAtomToConf(): Fails resizing memory.\n");
    return USERERR;
  }
  conf->n_atom ++;

  /* move contents after position "ins" forward by 1 */
  memmove(conf->atom+ins+1, conf->atom+ins, (conf->n_atom - ins - 1) * sizeof(ATOM));

  /* reset the new slot */
  memset(conf->atom+ins, 0, sizeof(ATOM));

  return ins;
}


/* copy the SAS value back to the original pdb */
void copy_sas(PROT src, PROT target)
{
  int i, j, k;

  for (i=0; i<src.n_res; i++) {
    target.res[i].sas = src.res[i].sas;
    for (j=0; j<src.res[i].n_conf; j++) {
      for (k=0; k<src.res[i].conf[j].n_atom; k++) {
        if (!(src.res[i].conf[j].atom[k].on)) continue;
        target.res[i].conf[j].atom[k].sas = src.res[i].conf[j].atom[k].sas;
      }
    }
  }

  return;
}



