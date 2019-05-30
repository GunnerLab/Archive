#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "mcce.h"

ENV env;

int init()
{   FILE *fp;
    time_t now;
    float kscale;

    printf("   Load run control file \"%s\"...\n", FN_RUNPRM); fflush(stdout);
    if (get_env()) {printf("   FATAL: init(): \"failed initializing.\"\n"); return USERERR;}
    else {printf("   Done\n\n"); fflush(stdout);}

    printf("   Tentatively load local param file \"%s\"...", env.new_tpl); fflush(stdout);
    if (env.do_premcce) remove(env.new_tpl);
    if ((fp=fopen(env.new_tpl, "r"))) {
        fclose(fp);
        if (load_param(env.new_tpl)) {
            printf("\n   FATAL: init(): Failed loading file \"%s\".\n", env.new_tpl);
            return USERERR;
        }
        printf("   File loaded.\n");
    }
    else printf("   No such file, ignore.\n");
    printf("   Done\n\n");
    fflush(stdout);

    printf("   Load parameters from directory \"%s\" ... \n", env.param); fflush(stdout);
    if (load_all_param(env.param)) {printf("   FATAL: init(): \"failed.\"\n"); return USERERR;}
    else {printf("   Done\n\n"); fflush(stdout);}

    printf("   Load linear free energy correction parameters from \"%s\"...", env.extra);fflush(stdout);
    if ((fp=fopen(env.extra, "r"))) {
        printf("%s\n", env.extra);
        fclose(fp);
        if (load_param(env.extra)) {
            printf("\n   FATAL: init(): Failed loading file \"%s\".\n", env.extra);
            return USERERR;
        }
        printf("   File loaded.\n");
    }
    else printf("   No such file, ignore.\n");
    printf("   Done\n\n");
    fflush(stdout);

    /* Order of scale values:
     * 1. default value from env.epsilon_prot
     * 2. tpl file (usually extra.tpl)
     */
    kscale = 1.0/env.epsilon_prot; /* scaling factor based on dielectric constant */

    if (param_get("SCALING", "VDW0", "", &env.scale_vdw0))  env.scale_vdw0   = 1.0*kscale;
    if (param_get("SCALING", "VDW1",  "", &env.scale_vdw1)) env.scale_vdw1   = 1.0*kscale;
    if (param_get("SCALING", "VDW",   "", &env.scale_vdw)) env.scale_vdw     = 1.0*kscale;
    if (param_get("SCALING", "TORS",  "", &env.scale_tor)) env.scale_tor     = 1.0*kscale;
    if (param_get("SCALING", "ELE",   "", &env.scale_ele)) env.scale_ele     = 1.0;
    if (param_get("SCALING", "DSOLV", "", &env.scale_dsolv)) env.scale_dsolv = 1.0;

    remove(env.debug_log);
    remove(env.progress_log);

    now = time(NULL);
    srand(now);

    return 0;
}


int get_env()
{   FILE *fp;
    char sbuff[256];
    char *str1;

    memset(&env, 0, sizeof(ENV));

    /* Default values */
    env.minimize_size = 0;
    env.PI                = 4.*atan(1.);
    env.d2r               = env.PI/180.;

    strcpy(env.debug_log,    "debug.log");
    strcpy(env.new_tpl,      "new.tpl");
    strcpy(env.progress_log, "progress.log");
    strcpy(env.extra, "extra.tpl");
    env.epsilon_columb = 4;
    env.reassign     = 0;
    env.delphi_start = 0;
    env.delphi_end   = 999999;
    env.rot_specif   = 0;
    env.prune_thr = 0.2;
    env.ngh_vdw_thr  = 0.1;
    env.repack_e_thr_exposed  = 0.1;
    env.repack_e_thr_buried  = 2.;
    env.nconf_limit       =    0;
    env.relax_wat         =    1;

    env.hdirected         =    0;
    env.hdirdiff          =  1.0;
    env.hdirlimt          =   36;

    env.water_relax_thr   =  2.4;

    env.hv_relax_ncycle     =  0;
    env.hv_relax_niter      = 50;
    env.hv_relax_vdw_thr    =  5;
    env.hv_relax_hv_vdw_thr =  5;
    env.hv_relax_dt         =  1;
    env.hv_tors_scale       =  1;
    env.hv_relax_constraint =  1.;
    env.hv_relax_constraint_frc = 10.;
    env.hv_relax_n_shake    =  3000;
    env.hv_relax_shake_tol =  0.0001;  /* Ratio to constraint distance */
    env.hv_relax_include_ngh    =  0;
    env.hv_relax_ngh_thr    =  4.;
    env.prune_rmsd        = 5.0;
    env.prune_ele         = 1.5;
    env.prune_vdw         = 12.0;


    env.relax_n_hyd       =    6;
    env.relax_clash_thr   =  10.;

    env.recalc_tors     = 0;

    env.default_radius = 1.7;
    env.factor_14lj = 0.5;
    env.epsilon_columb = 1;

    env.warn_pairwise     = 20.0;
    env.big_pairwise      = 5.0;

    env.monte_adv_opt     =    0;
    env.anneal_temp_start = ROOMT;
    env.anneal_nstep      =    1;
    env.monte_tsx         =    0;
    env.anneal_niter_step =   30;
    env.monte_niter_max   =   -1;
    env.adding_conf       =    0;
    env.monte_old_input   =    0;
    env.monte_niter_chk   =  100;
    env.monte_converge    = 1e-4;
    env.monte_do_energy   =    0;
    env.monte_print_nonzero =  1;
    strcpy(env.delphi_folder, "/tmp");
    env.delphi_clean      =  1;


    /* open "run.prm" to read in mcce environment variables */
    if ((fp=fopen(FN_RUNPRM, "r")) == NULL) {
        printf("   FATAL: get_env(): \"No run control file %s.\"\n", FN_RUNPRM);
        return USERERR;
    }

    /* user values */
    while (fgets(sbuff, sizeof(sbuff), fp)) {
        if (sbuff[0] == '#') {
            continue;
        }
        if (strstr(sbuff, "(INPDB)")) {
            strcpy(env.inpdb, strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MCCE_HOME)")) {
            strcpy(env.mcce_home, strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(DEBUG_LOG)")) {
            strcpy(env.debug_log, strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(PROGRESS_LOG)")) {
            strcpy(env.progress_log, strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(NEWTPL)")) {
            strcpy(env.new_tpl, strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(EXTRA)")) {
            strcpy(env.extra, strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MINIMIZE_SIZE)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.minimize_size = 1;
            else env.minimize_size = 0;
        }


        else if (strstr(sbuff, "(DO_PREMCCE)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') {
                env.do_premcce = 1;
            }
            else env.do_premcce = 0;
        }
        else if (strstr(sbuff, "(TERMINALS)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') {
                env.terminals = 1;
            }
            else env.terminals = 0;
        }
        else if (strstr(sbuff, "(DO_ROTAMERS)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.do_rotamers = 1;
            else env.do_rotamers = 0;
        }
        else if (strstr(sbuff, "(DO_ENERGY)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.do_energies = 1;
            else env.do_energies = 0;
        }
        else if (strstr(sbuff, "(DO_MONTE)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.do_monte = 1;
            else env.do_monte = 0;
        }

        else if (strstr(sbuff, "(ROT_SWAP)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.rot_swap = 1;
            else env.rot_swap = 0;
        }
        else if (strstr(sbuff, "(DEFAULT_RADIUS)")) {
            env.default_radius = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(FACTOR_14LJ)")) {
            env.factor_14lj = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(EPSILON_COLUMB)")) {
            env.epsilon_columb = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(CLASH_DISTANCE)")) {
            env.clash_distance = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(H2O_SASCUTOFF)")) {
            env.h2o_sascutoff = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(RENAME_RULES)")) {
            strcpy(env.rename_rules, strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(ROT_SPECIF)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.rot_specif = 1;
            else env.rot_specif = 0;
        }
        else if (strstr(sbuff, "(SWING)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.swing = 1;
            else env.swing = 0;
        }
        else if (strstr(sbuff, "(HDIRECTED)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.hdirected = 1;
            else env.hdirected = 0;
        }
        else if (strstr(sbuff, "(HDIRDIFF)")) {
            env.hdirdiff = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(HDIRLIMT)")) {
            env.hdirlimt = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(PACK)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.pack = 1;
            else env.pack = 0;
        }
        else if (strstr(sbuff, "(PRUNE_THR)")) {
            env.prune_thr = atof(strtok(sbuff, " "));
        }

        else if (strstr(sbuff, "(SAS_CUTOFF)")) {
            env.sas_cutoff = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(VDW_CUTOFF)")) {
            env.vdw_cutoff = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(REPACK_CUTOFF)")) {
            env.repack_cutoff = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(REPACK_E_THR_EXPOSED)")) {
            env.repack_e_thr_exposed = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(REPACK_E_THR_BURIED)")) {
            env.repack_e_thr_buried = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(NGH_VDW_THR)")) {
            env.ngh_vdw_thr = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(PHI_SWING)")) {
            env.phi_swing = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(ROTATIONS)")) {
            env.rotations = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(ROTAMER_LIMIT)")) {
            env.rotamer_limit = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(REPACKS)")) {
            env.repacks = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(NCONF_LIMIT)")) {
            env.nconf_limit = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(RELAX_WAT)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.relax_wat = 1;
            else env.relax_wat = 0;
        }
        else if (strstr(sbuff, "(WATER_RELAX_THR)")) {
            env.water_relax_thr = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(HV_RELAX_NCYCLE)")) {
            env.hv_relax_ncycle = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(HV_RELAX_NITER)")) {
            env.hv_relax_niter = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(HV_RELAX_VDW_THR)")) {
            env.hv_relax_vdw_thr = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(HV_RELAX_HV_VDW_THR)")) {
            env.hv_relax_hv_vdw_thr = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(HV_RELAX_DT)")) {
            env.hv_relax_dt = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(HV_TORS_SCALE)")) {
            env.hv_tors_scale = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(HV_RELAX_CONSTRAINT)")) {
            env.hv_relax_constraint = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(HV_RELAX_CONSTRAINT_FRC)")) {
            env.hv_relax_constraint_frc = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(HV_RELAX_N_SHAKE)")) {
            env.hv_relax_n_shake = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(HV_RELAX_SHAKE_TOL)")) {
            env.hv_relax_shake_tol = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(HV_RELAX_INCLUDE_NGH)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.hv_relax_include_ngh = 1;
            else env.hv_relax_include_ngh = 0;
        }
        else if (strstr(sbuff, "(HV_RELAX_NGH_THR)")) {
            env.hv_relax_ngh_thr = atof(strtok(sbuff, " "));
        }

        else if (strstr(sbuff, "(RELAX_H)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.relax_h = 1;
            else env.relax_h = 0;
        }
        else if (strstr(sbuff, "(RELAX_E_THR)")) {
            env.relax_e_thr = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(RELAX_NSTATES)")) {
            env.relax_nstates = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(RELAX_N_HYD)")) {
            env.relax_n_hyd = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(RELAX_CLASH_THR)")) {
            env.relax_clash_thr = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(RELAX_PHI)")) {
            env.relax_phi = 3.1415926/180.0 * atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(RELAX_NITER)")) {
            env.relax_niter = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(RELAX_TORQ_THR)")) {
            env.relax_torq_thr = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(PRUNE_RMSD)")) {
            env.prune_rmsd = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(PRUNE_ELE)")) {
            env.prune_ele = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(PRUNE_VDW)")) {
            env.prune_vdw = atof(strtok(sbuff, " "));
        }


        else if (strstr(sbuff, "(REASSIGN)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') {
                env.reassign = 1;
            }
            else env.average_pairwise = 0;
        }
        else if (strstr(sbuff, "(EPSILON_PROT)")) {
            env.epsilon_prot = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(EPSILON_SOLV)")) {
            env.epsilon_solv = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(GRIDS_DELPHI)")) {
            env.grids_delphi = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(GRIDS_PER_ANG)")) {
            env.grids_per_ang = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(RADIUS_PROBE)")) {
            env.radius_probe = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(IONRAD)")) {
            env.ionrad = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(SALT)")) {
            env.salt = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(DELPHI_EXE)")) {
            strcpy(env.delphi_exe, strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(DELPHI_FAILS)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 'd' || str1[0] == 'D') {
                env.delphi_fails = 'd';
            }
            else  env.delphi_fails = 's';
        }
        else if (strstr(sbuff, "(RECALC_TORS)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') {
                env.recalc_tors = 1;
            }
            else env.recalc_tors = 0;
        }

        else if (strstr(sbuff, "(AVERAGE_PAIRWISE)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') {
                env.average_pairwise = 1;
            }
            else env.average_pairwise = 0;
        }
        else if (strstr(sbuff, "(WARN_PAIRWISE)")) {
            env.warn_pairwise = atof(strtok(sbuff, " "));
        }

        else if (strstr(sbuff, "(MONTE_ADV_OPT)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.monte_adv_opt = 1;
            else env.monte_adv_opt = 0;
        }
        else if (strstr(sbuff, "(MONTE_SEED)")) {
            env.monte_seed = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_T)")) {
            env.monte_temp = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_RUNS)")) {
            env.monte_runs = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_NSTART)")) {
            env.monte_nstart = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_NEQ)")) {
            env.monte_neq = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_NITER)")) {
            env.monte_niter = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_FLIPS)")) {
            env.monte_flips = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_TRACE)")) {
            env.monte_trace = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_TSX)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') {
                env.monte_tsx = 1;
            }
            else env.monte_tsx = 0;
        }
        else if (strstr(sbuff, "(MONTE_N_REDUCE)")) {
            env.monte_n_red = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_REDUCE)")) {
            env.monte_reduce = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(NSTATE_MAX)")) {
            env.nstate_max = atoi(strtok(sbuff, " "));
        }

        else if (strstr(sbuff, "(ADDING_CONF)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') {
                env.adding_conf = 1;
            }
            else env.adding_conf = 0;
        }
        else if (strstr(sbuff, "(MONTE_OLD_INPUT)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') {
                env.monte_old_input = 1;
            }
            else env.monte_old_input = 0;
        }
        else if (strstr(sbuff, "(MONTE_NITER_CYCLE)")) {
            env.monte_niter_cycle = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_NITER_MIN)")) {
            env.monte_niter_min = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_NITER_MAX)")) {
            env.monte_niter_max = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_NITER_CHK)")) {
            env.monte_niter_chk = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_CONVERGE)")) {
            env.monte_converge = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(MONTE_DO_ENERGY)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') {
                env.monte_do_energy = 1;
            }
            else env.monte_do_energy = 0;
        }
        else if (strstr(sbuff, "(MONTE_PRINT_NONZERO)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') {
                env.monte_print_nonzero = 1;
            }
            else env.monte_print_nonzero = 0;
        }

        else if (strstr(sbuff, "(ANNEAL_TEMP_START)")) {
            env.anneal_temp_start = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(ANNEAL_NSTEP)")) {
            env.anneal_nstep = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(ANNEAL_NITER_STEP)")) {
            env.anneal_niter_step = atoi(strtok(sbuff, " "));
        }

        else if (strstr(sbuff, "(TITR_TYPE)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 'p' || str1[0] == 'P') env.titr_type = 'p';
            else env.titr_type = 'e';
        }
        else if (strstr(sbuff, "(TITR_PH0)")) {
            env.titr_ph0 = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(TITR_EH0)")) {
            env.titr_eh0 = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(TITR_PHD)")) {
            env.titr_phd = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(TITR_EHD)")) {
            env.titr_ehd = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(TITR_STEPS)")) {
            env.titr_steps = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(BIG_PAIRWISE)")) {
            env.big_pairwise = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(VDWF1)")) {
            env.vdwf1 = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(VDWF2)")) {
            env.vdwf2 = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(ROTATE_RES)")) {
            strcpy(env.rotate_res, strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(DELPHI_START)")) {
            env.delphi_start = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(DELPHI_END)")) {
            env.delphi_end = atoi(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(SCALE_VDW)")) {
            env.scale_vdw = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(SCALE_ELE)")) {
            env.scale_ele = atof(strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(SKIP_ELE)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.skip_ele = 1;
            else env.skip_ele = 0;
        }
        else if (strstr(sbuff, "(DELPHI_FOLDER)")) {
            strcpy(env.delphi_folder, strtok(sbuff, " "));
        }
        else if (strstr(sbuff, "(DELPHI_CLEAN)")) {
            str1 = strtok(sbuff, " ");
            if (str1[0] == 't' || str1[0] == 'T') env.delphi_clean = 1;
            else env.delphi_clean = 0;
        }

    }

    fclose(fp);
    if (env.monte_niter_cycle <= 0) env.monte_niter_cycle = 1000;
    if (env.repack_e_thr_exposed <= 0) env.repack_e_thr_exposed = 1e-4;
    if (env.repack_e_thr_buried <= 0) env.repack_e_thr_buried = 1e-4;

    /* round the dielectric constant to the nearest integer number */
    sprintf(env.param, "%s/param%02d", env.mcce_home, (int) (env.epsilon_prot));

    return 0;
}

