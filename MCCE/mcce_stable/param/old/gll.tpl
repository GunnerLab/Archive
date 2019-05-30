CONFLIST GLL        GLLBK GLL01 

NATOM    GLLBK      6
NATOM    GLL01      9

IATOM    GLLBK  N   0
IATOM    GLLBK  H   1
IATOM    GLLBK  CA  2
IATOM    GLLBK  HA  3
IATOM    GLLBK  C   4
IATOM    GLLBK  O   5
IATOM    GLL01  CB  0
IATOM    GLL01 1HB  1
IATOM    GLL01 2HB  2
IATOM    GLL01  CG  3
IATOM    GLL01 1HG  4
IATOM    GLL01 2HG  5
IATOM    GLL01  CD  6
IATOM    GLL01  OE1 7
IATOM    GLL01  OE2 8

ATOMNAME GLLBK    0  N  
ATOMNAME GLLBK    1  H  
ATOMNAME GLLBK    2  CA 
ATOMNAME GLLBK    3  HA 
ATOMNAME GLLBK    4  C  
ATOMNAME GLLBK    5  O  
ATOMNAME GLL01    0  CB 
ATOMNAME GLL01    1 1HB 
ATOMNAME GLL01    2 2HB 
ATOMNAME GLL01    3  CG 
ATOMNAME GLL01    4 1HG 
ATOMNAME GLL01    5 2HG 
ATOMNAME GLL01    6  CD 
ATOMNAME GLL01    7  OE1
ATOMNAME GLL01    8  OE2






#1.Basic Conformer Information: name, pka, em, rxn.
#23456789A123456789B123456789C
PROTON   GLL01      0
PKA      GLL01      0.0
ELECTRON GLL01      0
EM       GLL01      0.0
RXN      GLL01      -3.81

#2.Structure Connectivity
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
CONNECT  GLLBK  N   sp2       -1    C   0     CA  0     H
CONNECT  GLLBK  H   s         0     N
CONNECT  GLLBK  CA  sp3       0     N   0     C   0     CB  0     HA
CONNECT  GLLBK  HA  s         0     CA
CONNECT  GLLBK  C   sp2       0     CA  0     O   1     N
CONNECT  GLLBK  O   sp2       0     C
CONNECT  GLL01  CB  sp3       0     CA  0     CG  0    1HB  0    2HB
CONNECT  GLL01 1HB  s         0     CB
CONNECT  GLL01 2HB  s         0     CB
CONNECT  GLL01  CG  sp3       0     CB  0     CD  0    1HG  0    2HG
CONNECT  GLL01 1HG  s         0     CG
CONNECT  GLL01 2HG  s         0     CG
CONNECT  GLL01  CD  sp2       0     CG  0     OE1 0     OE2
CONNECT  GLL01  OE1 sp3       0     CD  LIG   ? 
CONNECT  GLL01  OE2 s         0     CD
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
H_DIHED  GLLBK  1    H    N    CA   C   60
H_DIHED  GLL01  1    OE1  CD   CG  180

ACCEPTOR GLL01  1    CD - OE2
DONOR    GLL01  1    OE1- 

#3.Atom Parameters: Partial Charges and Radii
# Radii from "Bondi, J.Phys.Chem., 68, 441, 1964."
RADIUS   GLL    N   1.55
RADIUS   GLL    H   1.20
RADIUS   GLL    CA  1.70
RADIUS   GLL    HA  1.20
RADIUS   GLL    C   1.70
RADIUS   GLL    O   1.52
RADIUS   GLL    CB  1.70
RADIUS   GLL   1HB  1.20
RADIUS   GLL   2HB  1.20
RADIUS   GLL    CG  1.70
RADIUS   GLL   1HG  1.20
RADIUS   GLL   2HG  1.20
RADIUS   GLL    CD  1.70
RADIUS   GLL    OE1 1.52
RADIUS   GLL    OE2 1.52

#=========================================================================
#        GRP   #      BOND     AFFECTED_ATOMS
#123456789012345678901234567890
#-------|---|----|-|---------|----|----|----|----|----|----|----|----|----
#=========================================================================
