#This is Stephanie, I want to edit this file 
CONFLIST PRO        PROBK PRO01

NATOM    PROBK      5
NATOM    PRO01      9

IATOM    PROBK  N   0
IATOM    PROBK  CA  1
IATOM    PROBK  HA  2
IATOM    PROBK  C   3
IATOM    PROBK  O   4
IATOM    PRO01  CB  0
IATOM    PRO01  HB2 1
IATOM    PRO01  HB3 2
IATOM    PRO01  CG  3
IATOM    PRO01  HG2 4
IATOM    PRO01  HG3 5
IATOM    PRO01  CD  6
IATOM    PRO01  HD2 7
IATOM    PRO01  HD3 8

ATOMNAME PROBK    0  N  
ATOMNAME PROBK    1  CA 
ATOMNAME PROBK    2  HA 
ATOMNAME PROBK    3  C  
ATOMNAME PROBK    4  O  
ATOMNAME PRO01    0  CB 
ATOMNAME PRO01    1  HB2 
ATOMNAME PRO01    2  HB3 
ATOMNAME PRO01    3  CG 
ATOMNAME PRO01    4  HG2 
ATOMNAME PRO01    5  HG3 
ATOMNAME PRO01    6  CD
ATOMNAME PRO01    7  HD2
ATOMNAME PRO01    8  HD3






#1.Basic Conformer Information: name, pka, em, rxn.
#23456789A123456789B123456789C
PROTON   PRO01      0
PKA      PRO01      0.0
ELECTRON PRO01      0
EM       PRO01      0.0
RXN      PRO01      0.00

#2.Structure Connectivity
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
CONNECT  PROBK  N   sp2       -1    C   0     CA  0     CD
CONNECT  PROBK  CA  sp3       0     N   0     C   0     CB  0     HA
CONNECT  PROBK  HA  s         0     CA
CONNECT  PROBK  C   sp2       0     CA  0     O   1     N
CONNECT  PROBK  O   s         0     C
CONNECT  PRO01  CB  sp3       0     CA  0     CG  0     HB2 0     HB3
CONNECT  PRO01  HB2 s         0     CB
CONNECT  PRO01  HB3 s         0     CB
CONNECT  PRO01  CG  sp3       0     CB  0     CD  0     HG2 0     HG3
CONNECT  PRO01  HG2 s         0     CG
CONNECT  PRO01  HG3 s         0     CG
CONNECT  PRO01  CD  sp3       0     CG  0     N   0     HD2  0     HD3
CONNECT  PRO01  HD2 s         0     CD
CONNECT  PRO01  HD3 s         0     CD

#3.Atom Parameters: Partial Charges and Radii
CHARGE   PROBK N    -0.100
CHARGE   PROBK CA    0.100
CHARGE   PROBK C     0.550
CHARGE   PROBK O    -0.550

# Radii from "Bondi, J.Phys.Chem., 68, 441, 1964."
RADIUS   PRO    N   1.50
RADIUS   PRO    CA  2.00
RADIUS   PRO    HA  0.00
RADIUS   PRO    C   1.70
RADIUS   PRO    O   1.40
RADIUS   PRO    CB  2.00
RADIUS   PRO    HB2 0.00
RADIUS   PRO    HB3 0.00
RADIUS   PRO    CG  2.00
RADIUS   PRO    HG2  0.00
RADIUS   PRO    HG3  0.00
RADIUS   PRO    CD  2.00
RADIUS   PRO    HD2 2.00
RADIUS   PRO    HD3 2.00

#4.Rotomer

