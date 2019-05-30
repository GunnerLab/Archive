CONFLIST CYL        CYLBK CYL01 CYL02 

NATOM    CYLBK      6
NATOM    CYL01      4
NATOM    CYL02      4

IATOM    CYLBK  N   0
IATOM    CYLBK  H   1
IATOM    CYLBK  CA  2
IATOM    CYLBK  HA  3
IATOM    CYLBK  C   4
IATOM    CYLBK  O   5
IATOM    CYL01  CB  0
IATOM    CYL01 1HB  1
IATOM    CYL01 2HB  2
IATOM    CYL01  SG  3
IATOM    CYL02  CB  0
IATOM    CYL02 1HB  1
IATOM    CYL02 2HB  2
IATOM    CYL02  SG  3

ATOMNAME CYLBK    0  N  
ATOMNAME CYLBK    1  H  
ATOMNAME CYLBK    2  CA 
ATOMNAME CYLBK    3  HA 
ATOMNAME CYLBK    4  C  
ATOMNAME CYLBK    5  O  
ATOMNAME CYL01    0  CB 
ATOMNAME CYL01    1 1HB 
ATOMNAME CYL01    2 2HB 
ATOMNAME CYL01    3  SG 
ATOMNAME CYL02    0  CB 
ATOMNAME CYL02    1 1HB 
ATOMNAME CYL02    2 2HB 
ATOMNAME CYL02    3  SG 




#1.Basic Conformer Information: name, pka, em, rxn.
#23456789A123456789B123456789C
PROTON   CYL01      0
PKA      CYL01      0.0
ELECTRON CYL01      0
EM       CYL01      0.0
EM       CYL01      0.0
RXN      CYL01      0.0

PROTON   CYL02      0
PKA      CYL02      0.0
ELECTRON CYL02      0
EM       CYL02      0.0
EM       CYL02      0.0
RXN      CYL02      0.0

#2.Structure Connectivity
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
CONNECT  CYLBK  N   sp2       -1    C   0     CA  0     H
CONNECT  CYLBK  H   s         0     N
CONNECT  CYLBK  CA  sp3       0     N   0     C   0     CB  0     HA
CONNECT  CYLBK  HA  s         0     CA
CONNECT  CYLBK  C   sp2       0     CA  0     O   1     N
CONNECT  CYLBK  O   sp2       0     C
CONNECT  CYL01  CB  sp3       0     CA  0     SG  0    1HB  0    2HB
CONNECT  CYL01 1HB  s         0     CB
CONNECT  CYL01 2HB  s         0     CB
CONNECT  CYL01  SG  sp2       0     CB  LIG  CU2  LIG  CU1

CONNECT  CYL02  CB  sp3       0     CA  0     SG  0    1HB  0    2HB
CONNECT  CYL02 1HB  s         0     CB
CONNECT  CYL02 2HB  s         0     CB
CONNECT  CYL02  SG  sp2       0     CB  LIG  CU2  LIG  CU1

#3.Atom Parameters: Partial Charges and Radii
CHARGE   CYL01  CB   0.001
CHARGE   CYL01  CB  -0.001

CHARGE   CYL02  CB   0.001
CHARGE   CYL02  CB  -0.001

# Radii from "Bondi, J.Phys.Chem., 68, 441, 1964."
RADIUS   CYL    N   1.55
RADIUS   CYL    H   1.20
RADIUS   CYL    CA  1.70
RADIUS   CYL    HA  1.20
RADIUS   CYL    C   1.70
RADIUS   CYL    O   1.52
RADIUS   CYL    CB  1.70
RADIUS   CYL   1HB  1.20
RADIUS   CYL   2HB  1.20
RADIUS   CYL    SG  1.80

#=========================================================================
#        GRP   #      BOND     AFFECTED_ATOMS
#123456789012345678901234567890
#-------|---|----|-|---------|----|----|----|----|----|----|----|----|----
#=========================================================================
