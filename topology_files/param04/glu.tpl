### This is stephanie I want to edit this file

CONFLIST GLU        GLUBK GLU01 GLU02 GLU-1 

NATOM    GLUBK      6
NATOM    GLU01      10
NATOM    GLU02      10
NATOM    GLU-1      9

IATOM    GLUBK  N   0
IATOM    GLUBK  H   1
IATOM    GLUBK  CA  2
IATOM    GLUBK  HA  3
IATOM    GLUBK  C   4
IATOM    GLUBK  O   5
IATOM    GLU01  CB  0
IATOM    GLU01  HB2 1
IATOM    GLU01  HB3 2
IATOM    GLU01  CG  3
IATOM    GLU01  HG2 4
IATOM    GLU01  HG3 5
IATOM    GLU01  CD  6
IATOM    GLU01  OE1 7
IATOM    GLU01  HE1 8
IATOM    GLU01  OE2 9
IATOM    GLU02  CB  0
IATOM    GLU02  HB2 1
IATOM    GLU02  HB3 2
IATOM    GLU02  CG  3
IATOM    GLU02  HG2 4
IATOM    GLU02  HG3 5
IATOM    GLU02  CD  6
IATOM    GLU02  OE1 7
IATOM    GLU02  OE2 8
IATOM    GLU02  HE2 9
IATOM    GLU-1  CB  0
IATOM    GLU-1  HB2 1
IATOM    GLU-1  HB3 2
IATOM    GLU-1  CG  3
IATOM    GLU-1  HG2 4
IATOM    GLU-1  HG3 5
IATOM    GLU-1  CD  6
IATOM    GLU-1  OE1 7
IATOM    GLU-1  OE2 8

ATOMNAME GLUBK    0  N  
ATOMNAME GLUBK    1  H  
ATOMNAME GLUBK    2  CA 
ATOMNAME GLUBK    3  HA 
ATOMNAME GLUBK    4  C  
ATOMNAME GLUBK    5  O  
ATOMNAME GLU01    0  CB 
ATOMNAME GLU01    1  HB2 
ATOMNAME GLU01    2  HB3 
ATOMNAME GLU01    3  CG 
ATOMNAME GLU01    4  HG2 
ATOMNAME GLU01    5  HG3 
ATOMNAME GLU01    6  CD 
ATOMNAME GLU01    7  OE1
ATOMNAME GLU01    8  HE1
ATOMNAME GLU01    9  OE2
ATOMNAME GLU02    0  CB 
ATOMNAME GLU02    1  HB2 
ATOMNAME GLU02    2  HB3 
ATOMNAME GLU02    3  CG 
ATOMNAME GLU02    4  HG2 
ATOMNAME GLU02    5  HG3 
ATOMNAME GLU02    6  CD 
ATOMNAME GLU02    7  OE1
ATOMNAME GLU02    8  OE2
ATOMNAME GLU02    9  HE2
ATOMNAME GLU-1    0  CB 
ATOMNAME GLU-1    1  HB2 
ATOMNAME GLU-1    2  HB3 
ATOMNAME GLU-1    3  CG 
ATOMNAME GLU-1    4  HG2 
ATOMNAME GLU-1    5  HG3 
ATOMNAME GLU-1    6  CD 
ATOMNAME GLU-1    7  OE1
ATOMNAME GLU-1    8  OE2






#1.Basic Conformer Information: name, pka, em, rxn.
#23456789A123456789B123456789C
PROTON   GLU01      0
PROTON   GLU02      0
PROTON   GLU-1      -1
PKA      GLU01      0.0
PKA      GLU02      0.0
PKA      GLU-1      4.75
ELECTRON GLU01      0
ELECTRON GLU02      0
ELECTRON GLU-1      0
EM       GLU01      0.0
EM       GLU02      0.0
EM       GLU-1      0.0
#RXN      GLU01      -2.25
#RXN      GLU02      -2.25
#RXN      GLU-1      -18.01
RXN      GLU01      -3.10
RXN      GLU02      -3.10
RXN      GLU-1      -20.2

#2.Structure Connectivity
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
#ONNECT   conf atom  orbital  ires conn ires conn ires conn ires conn
#ONNECT |-----|----|---------|----|----|----|----|----|----|----|----|----|----|----|----|
CONNECT  GLUBK  N   sp2       -1    C   0     CA  0     H
CONNECT  GLUBK  H   s         0     N
CONNECT  GLUBK  CA  sp3       0     N   0     C   0     CB  0     HA
CONNECT  GLUBK  HA  s         0     CA
CONNECT  GLUBK  C   sp2       0     CA  0     O   1     N
CONNECT  GLUBK  O   sp2       0     C
CONNECT  GLU01  CB  sp3       0     CA  0     CG  0    HB2  0    HB3
CONNECT  GLU01  HB2 s         0     CB
CONNECT  GLU01  HB3 s         0     CB
CONNECT  GLU01  CG  sp3       0     CB  0     CD  0    HG2  0    HG3
CONNECT  GLU01  HG2 s         0     CG
CONNECT  GLU01  HG3 s         0     CG
CONNECT  GLU01  CD  sp2       0     CG  0     OE1 0     OE2
CONNECT  GLU01  OE1 sp3       0     CD  0     HE1 
CONNECT  GLU01  HE1 s         0     OE1
CONNECT  GLU01  OE2 s         0     CD
CONNECT  GLU02  CB  sp3       0     CA  0     CG  0     HB2 0    HB3
CONNECT  GLU02  HB2 s         0     CB
CONNECT  GLU02  HB3 s         0     CB
CONNECT  GLU02  CG  sp3       0     CB  0     CD  0     HG2 0    HG3
CONNECT  GLU02  HG2  s         0     CG
CONNECT  GLU02  HG3  s         0     CG
CONNECT  GLU02  CD  sp2       0     CG  0     OE1 0     OE2
CONNECT  GLU02  OE1 s         0     CD
CONNECT  GLU02  OE2 sp3       0     CD  0     HE2
CONNECT  GLU02  HE2 sp3       0     OE2
CONNECT  GLU-1  CB  sp3       0     CA  0     CG  0    HB2  0    HB3
CONNECT  GLU-1  HB2 s         0     CB
CONNECT  GLU-1  HB3 s         0     CB
CONNECT  GLU-1  CG  sp3       0     CB  0     CD  0    HG2  0    HG3
CONNECT  GLU-1  HG2 s         0     CG
CONNECT  GLU-1  HG3 s         0     CG
CONNECT  GLU-1  CD  sp2       0     CG  0     OE1 0     OE2
CONNECT  GLU-1  OE1 sp2       0     CD
CONNECT  GLU-1  OE2 sp2       0     CD
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
#-------|-----|----|----|----|----|----|---------|---------|---------|----
#        CONF  ATOM ATOM ATOM ATOM      phi0(min)  n_fold   Amplitude(barrier,kcal/mol)    

ACCEPTOR GLU01  OE2  CD 
DONOR    GLU01  HE1  OE1
ACCEPTOR GLU02  OE1  CD 
DONOR    GLU02  HE2  OE2
ACCEPTOR GLU-1  OE1  CD 
ACCEPTOR GLU-1  OE2  CD 

#3.Atom Parameters: Partial Charges and Radii
# Radii from "Bondi, J.Phys.Chem., 68, 441, 1964."
RADIUS   GLU    N   1.50
RADIUS   GLU    H   1.00
RADIUS   GLU    CA  2.00
RADIUS   GLU    HA  0.00
RADIUS   GLU    C   1.70
RADIUS   GLU    O   1.40
RADIUS   GLU    CB  2.00
RADIUS   GLU    HB2 0.00
RADIUS   GLU    HB3 0.00
RADIUS   GLU    CG  2.00
RADIUS   GLU    HG2 0.00
RADIUS   GLU    HG3 0.00
RADIUS   GLU    CD  1.70
RADIUS   GLU    OE1 1.40
RADIUS   GLU    HE1 1.00
RADIUS   GLU    OE2 1.40
RADIUS   GLU    HE2 1.00


CHARGE   GLUBK  N    -0.350
CHARGE   GLUBK  H     0.250
CHARGE   GLUBK  CA    0.100
CHARGE   GLUBK  C     0.550
CHARGE   GLUBK  O    -0.550
CHARGE   GLU-1  CG    0.000
CHARGE   GLU-1  CD    0.100
CHARGE   GLU-1  OE1  -0.550
CHARGE   GLU-1  OE2  -0.550
CHARGE   GLU01  CD    0.550
CHARGE   GLU01  OE1  -0.495
CHARGE   GLU01  OE2  -0.490
CHARGE   GLU01  HE1   0.435
CHARGE   GLU02  CD    0.550
CHARGE   GLU02  OE1  -0.490
CHARGE   GLU02  OE2  -0.495
CHARGE   GLU02  HE2   0.435

#=========================================================================
#        GRP   #      BOND     AFFECTED_ATOMS
#123456789012345678901234567890
#-------|---|----|-|---------|----|----|----|----|----|----|----|----|----
ROTAMER  GLU   0     CA - CB   CG   CD   OE1  OE2
ROTAMER  GLU   1     CB - CG   CD   OE1  OE2
ROTAMER  GLU   2     CG - CD   OE1  OE2
#=========================================================================
