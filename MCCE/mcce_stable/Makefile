CC      = gcc -g 
LDIR = lib
DEPS    = $(LDIR)/mcce.h
LIB     = $(LDIR)/mcce.a
AR      = ar
ARFLAGS = rvs

_SRC     =  ins_res.c       strip.c            del_conf.c        del_prot.c\
load_all_param.c  del_res.c load_param.c       cpy_conf.c        cpy_prot.c\
new_prot.c  cpy_res.c       get_files.c        db.c              geom.c\
free_strings.c  ins_conf.c  pdbline2atom.c     premcce.c init.c  load_pdb.c\
write_pdb.c rotamers.c      assign_rad.c       get_connect12.c   surfw.c\
vdw.c       vdw_conf.c      shuffle_n.c        cmp_conf.c        sort_conf.c\
sort_res.c	id_conf.c       energies.c         assign_crg.c      coulomb.c\
coulomb_conf.c get_vdw0.c   get_vdw1.c         relax_water.c     relax_h.c\
monte.c     monte2.c        ran2.c             relaxation.c      collect_connect.c\
torsion.c	vdw_fast.c      hbond_extra.c      swap.c            quick_e.c\
sas_native.c check_tpl.c    del_dir.c make_matrices.c

SRC = $(patsubst %,$(LDIR)/%,$(_SRC))
DELPHI = bin/delphi

bin/mcce: mcce.c $(LIB) $(DEPS) $(DELPHI)
	$(CC) -o bin/mcce mcce.c $(LIB) -lm 

$(DELPHI): $(LDIR)/delphi/delphi
	$(MAKE) -C $(LDIR)/delphi
	cp $(LDIR)/delphi/delphi $(DELPHI)

OBJ     = $(SRC:.c=.o)

$(LIB): $(OBJ)
	cd $(LDIR)
	$(AR) $(ARFLAGS) $(LIB) $(OBJ)

$(LDIR)/%.o: $(LDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


clean:
	-rm -f bin/mcce bin/delphi $(LIB) $(LDIR)/*.o
	$(MAKE) clean -C $(LDIR)/delphi

cleanbin/mcce:
	-rm -f bin/mcce bin/delphi $(LIB) $(LDIR)/*.o
	$(MAKE) clean -C $(LDIR)/delphi


