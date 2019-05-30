CONFLIST VAL        VALBK VAL01

NATOM    VALBK      6
NATOM    VAL01      10

IATOM    VALBK  N   0
IATOM    VALBK  H   1
IATOM    VALBK  CA  2
IATOM    VALBK  HA  3
IATOM    VALBK  C   4
IATOM    VALBK  O   5
IATOM    VAL01  CB  0
IATOM    VAL01  HB  1
IATOM    VAL01  CG1 2
IATOM    VAL01 1HG1 3
IATOM    VAL01 2HG1 4
IATOM    VAL01 3HG1 5
IATOM    VAL01  CG2 6
IATOM    VAL01 1HG2 7
IATOM    VAL01 2HG2 8
IATOM    VAL01 3HG2 9

ATOMNAME VALBK    0  N  
ATOMNAME VALBK    1  H  
ATOMNAME VALBK    2  CA 
ATOMNAME VALBK    3  HA 
ATOMNAME VALBK    4  C  
ATOMNAME VALBK    5  O  
ATOMNAME VAL01    0  CB 
ATOMNAME VAL01    1  HB 
ATOMNAME VAL01    2  CG1
ATOMNAME VAL01    3 1HG1
ATOMNAME VAL01    4 2HG1
ATOMNAME VAL01    5 3HG1
ATOMNAME VAL01    6  CG2
ATOMNAME VAL01    7 1HG2
ATOMNAME VAL01    8 2HG2
ATOMNAME VAL01    9 3HG2






#1.Basic Conformer Information: name, pka, em, rxn.
#marilyn 6/11/03
#23456789A123456789B123456789C
PROTON   VAL01      0
PKA      VAL01      0.0
ELECTRON VAL01      0
EM       VAL01      0.0
RXN      VAL01      0.00

#2.Structure Connectivity
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
#ONNECT   conf atom  orbital  ires conn ires conn ires conn ires conn
#ONNECT |-----|----|---------|----|----|----|----|----|----|----|----|----|----|----|----|
CONNECT  VALBK  N   sp2       -1    C   0     CA  0     H 
CONNECT  VALBK  H   s         0     N
CONNECT  VALBK  CA  sp3       0     N   0     C   0     CB  0     HA
CONNECT  VALBK  HA  s         0     CA
CONNECT  VALBK  C   sp2       0     CA  0     O   1     N
CONNECT  VALBK  O   sp2       0     C
CONNECT  VAL01  CB  sp3       0     CA  0     CG1 0     CG2 0     HB
CONNECT  VAL01  HB  s         0     CB
CONNECT  VAL01  CG1 sp3       0     CB  0    1HG1 0    2HG1 0    3HG1
CONNECT  VAL01 1HG1 s         0     CG1
CONNECT  VAL01 2HG1 s         0     CG1
CONNECT  VAL01 3HG1 s         0     CG1
CONNECT  VAL01  CG2 sp3       0     CB  0    1HG2 0    2HG2 0    3HG2
CONNECT  VAL01 1HG2 s         0     CG2
CONNECT  VAL01 2HG2 s         0     CG2
CONNECT  VAL01 3HG2 s         0     CG2
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I

#3.Atom Parameters: Partial Charges and Radii
CHARGE   VALBK  N    -0.350
CHARGE   VALBK  H     0.250
CHARGE   VALBK  CA    0.100
CHARGE   VALBK  C     0.550
CHARGE   VALBK  O    -0.550

# Radii from "Bondi, J.Phys.Chem., 68, 441, 1964."
RADIUS   VAL    N   1.50
RADIUS   VAL    H   1.00
RADIUS   VAL    CA  2.00
RADIUS   VAL    HA  0.00
RADIUS   VAL    C   1.70
RADIUS   VAL    O   1.40
RADIUS   VAL    CB  2.00
RADIUS   VAL    HB  0.00
RADIUS   VAL    CG1 2.00
RADIUS   VAL   1HG1 0.00
RADIUS   VAL   2HG1 0.00
RADIUS   VAL   3HG1 0.00
RADIUS   VAL   CG2  2.00
RADIUS   VAL   1HG2 0.00
RADIUS   VAL   2HG2 0.00
RADIUS   VAL   3HG2 0.00

#=========================================================================
#        GRP   #      BOND     AFFECTED_ATOMS
#123456789012345678901234567890
#-------|---|----|-|---------|----|----|----|----|----|----|----|----|----
ROTAMER  VAL   0     CA - CB   CG1  CG2
#=========================================================================

