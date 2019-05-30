###Stephanie:I want to edit this file
 

CONFLIST GLN        GLNBK GLN01

NATOM    GLNBK      6
NATOM    GLN01      11

IATOM    GLNBK  N   0
IATOM    GLNBK  H   1
IATOM    GLNBK  CA  2
IATOM    GLNBK  HA  3
IATOM    GLNBK  C   4
IATOM    GLNBK  O   5
IATOM    GLN01  CB  0
IATOM    GLN01 HB2  1
IATOM    GLN01 HB3  2
IATOM    GLN01  CG  3
IATOM    GLN01 HG2  4
IATOM    GLN01 HG3  5
IATOM    GLN01  CD  6
IATOM    GLN01  OE1 7
IATOM    GLN01  NE2 8
IATOM    GLN01 HE21 9
IATOM    GLN01 HE22 10

ATOMNAME GLNBK    0  N  
ATOMNAME GLNBK    1  H  
ATOMNAME GLNBK    2  CA 
ATOMNAME GLNBK    3  HA 
ATOMNAME GLNBK    4  C  
ATOMNAME GLNBK    5  O  
ATOMNAME GLN01    0  CB 
ATOMNAME GLN01    1  HB2 
ATOMNAME GLN01    2  HB3 
ATOMNAME GLN01    3  CG 
ATOMNAME GLN01    4  HG2 
ATOMNAME GLN01    5  HG3 
ATOMNAME GLN01    6  CD 
ATOMNAME GLN01    7  OE1
ATOMNAME GLN01    8  NE2
ATOMNAME GLN01    9 HE21
ATOMNAME GLN01   10 HE22






#1.Basic Conformer Information: name, pka, em, rxn.
#23456789A123456789B123456789C
PROTON   GLN01      0
PKA      GLN01      0.0
ELECTRON GLN01      0
EM       GLN01      0.0
RXN      GLN01      -3.5

#2.Structure Connectivity
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
#ONNECT   conf atom  orbital  ires conn ires conn ires conn ires conn
#ONNECT |-----|----|---------|----|----|----|----|----|----|----|----|----|----|----|----|
CONNECT  GLNBK  N   sp2       -1    C   0     CA  0     H
CONNECT  GLNBK  H   s         0     N
CONNECT  GLNBK  CA  sp3       0     N   0     C   0     CB  0     HA
CONNECT  GLNBK  HA  s         0     CA
CONNECT  GLNBK  C   sp2       0     CA  0     O   1     N
CONNECT  GLNBK  O   sp2       0     C
CONNECT  GLN01  CB  sp3       0     CA  0     CG  0    HB2  0    HB3
CONNECT  GLN01 HB2  s         0     CB
CONNECT  GLN01 HB3  s         0     CB
CONNECT  GLN01  CG  sp3       0     CB  0    CD   0    HG2  0    HG3
CONNECT  GLN01 HG2  s         0     CG
CONNECT  GLN01 HG3  s         0     CG
CONNECT  GLN01  CD  sp2       0     CG  0     OE1 0     NE2
CONNECT  GLN01  OE1 sp2       0     CD
CONNECT  GLN01  NE2 sp2       0     CD  0    HE21 0    HE22
CONNECT  GLN01 HE21 s         0     NE2
CONNECT  GLN01 HE22 s         0     NE2
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
ACCEPTOR GLN01  OE1  CD 
DONOR    GLN01 HE21  NE2
DONOR    GLN01 HE22  NE2

#3.Atom Parameters: Partial Charges and Radii
# Radii from "Bondi, J.Phys.Chem., 68, 441, 1964."
RADIUS   GLN    N   1.50
RADIUS   GLN    H   1.00
RADIUS   GLN    CA  2.00
RADIUS   GLN    HA  0.00
RADIUS   GLN    C   1.70
RADIUS   GLN    O   1.40
RADIUS   GLN    CB  2.00
RADIUS   GLN   HB2  0.00
RADIUS   GLN   HB3  0.00
RADIUS   GLN    CG  2.00
RADIUS   GLN   HG2  0.00
RADIUS   GLN   HG3  0.00
RADIUS   GLN    CD  1.70
RADIUS   GLN    OE1 1.40
RADIUS   GLN    NE2 1.50
RADIUS   GLN   HE21 1.00
RADIUS   GLN   HE22 1.00

CHARGE   GLNBK  N    -0.350
CHARGE   GLNBK  H     0.250
CHARGE   GLNBK  CA    0.100
CHARGE   GLNBK  C     0.550
CHARGE   GLNBK  O    -0.550
CHARGE   GLN01  CD    0.550
CHARGE   GLN01  OE1  -0.550
CHARGE   GLN01  NE2  -0.780
CHARGE   GLN01 HE21   0.390
CHARGE   GLN01 HE22   0.390

#=========================================================================
#        GRP   #      BOND     AFFECTED_ATOMS
#123456789012345678901234567890
#-------|---|----|-|---------|----|----|----|----|----|----|----|----|----
ROTAMER  GLN   0     CA - CB   CG   CD   OE1  NE2
ROTAMER  GLN   1     CB - CG   CD   OE1  NE2
ROTAMER  GLN   2     CG - CD   OE1  NE2
#=========================================================================

#-------|---|----|-|---------|---------|---------|---------|---------|---------|
ROT_SWAP GLN   0     OE1- NE2

