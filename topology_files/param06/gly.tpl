#This is Stephanie, I want to edit this file

CONFLIST GLY        GLYBK 

NATOM    GLYBK      7

IATOM    GLYBK  N   0
IATOM    GLYBK  H   1
IATOM    GLYBK  CA  2
IATOM    GLYBK HA2  3
IATOM    GLYBK HA3  4
IATOM    GLYBK  O   5
IATOM    GLYBK  C   6

ATOMNAME GLYBK    0  N  
ATOMNAME GLYBK    1  H  
ATOMNAME GLYBK    2  CA 
ATOMNAME GLYBK    3 HA2 
ATOMNAME GLYBK    4 HA3 
ATOMNAME GLYBK    5  O  
ATOMNAME GLYBK    6  C  




#1.Basic Conformer Information: name, pka, em, rxn.
#23456789A123456789B123456789C

#2.Structure Connectivity
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
#ONNECT   conf atom  orbital  ires conn ires conn ires conn ires conn
#ONNECT |-----|----|---------|----|----|----|----|----|----|----|----|----|----|----|----|
CONNECT  GLYBK  N   sp2       -1    C   0     CA  0     H
CONNECT  GLYBK  H   s         0     N
CONNECT  GLYBK  CA  sp3       0     N   0     C   0    HA2  0    HA3
CONNECT  GLYBK HA2  s         0     CA
CONNECT  GLYBK HA3  s         0     CA 
CONNECT  GLYBK  O   sp2       0     C 
CONNECT  GLYBK  C   sp2       0     CA  0     O   1     N

#3.Atom Parameters: Partial Charges and Radii
# Radii from "Bondi, J.Phys.Chem., 68, 441, 1964."
RADIUS   GLY    N   1.50
RADIUS   GLY    H   1.00
RADIUS   GLY    CA  2.00
RADIUS   GLY   HA2  0.00
RADIUS   GLY   HA3  0.00
RADIUS   GLY    O   1.40
RADIUS   GLY    C   1.70

CHARGE   GLYBK  N    -0.350
CHARGE   GLYBK  H     0.250
CHARGE   GLYBK  CA    0.100
CHARGE   GLYBK  C     0.550
CHARGE   GLYBK  O    -0.550
