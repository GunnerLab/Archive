CONFLIST HIS        HISBK HIS01 HIS02 HIS+1 #HIS-1 

NATOM    HISBK      6
NATOM    HIS01      11
NATOM    HIS02      11
NATOM    HIS+1      12
NATOM    HIS-1      10

IATOM    HISBK  N   0
IATOM    HISBK  H   1
IATOM    HISBK  CA  2
IATOM    HISBK  HA  3
IATOM    HISBK  C   4
IATOM    HISBK  O   5
IATOM    HIS01  CB  0
IATOM    HIS01 1HB  1
IATOM    HIS01 2HB  2
IATOM    HIS01  CG  3
IATOM    HIS01  ND1 4
IATOM    HIS01  CE1 5
IATOM    HIS01  HE1 6
IATOM    HIS01  NE2 7
IATOM    HIS01  HE2 8
IATOM    HIS01  CD2 9
IATOM    HIS01  HD2 10
IATOM    HIS02  CB  0
IATOM    HIS02 1HB  1
IATOM    HIS02 2HB  2
IATOM    HIS02  CG  3
IATOM    HIS02  ND1 4
IATOM    HIS02  HD1 5
IATOM    HIS02  CE1 6
IATOM    HIS02  HE1 7
IATOM    HIS02  NE2 8
IATOM    HIS02  CD2 9
IATOM    HIS02  HD2 10
IATOM    HIS+1  CB  0
IATOM    HIS+1 1HB  1
IATOM    HIS+1 2HB  2
IATOM    HIS+1  CG  3
IATOM    HIS+1  ND1 4
IATOM    HIS+1  HD1 5
IATOM    HIS+1  CE1 6
IATOM    HIS+1  HE1 7
IATOM    HIS+1  NE2 8
IATOM    HIS+1  HE2 9
IATOM    HIS+1  CD2 10
IATOM    HIS+1  HD2 11
IATOM    HIS-1  CB  0
IATOM    HIS-1 1HB  1
IATOM    HIS-1 2HB  2
IATOM    HIS-1  CG  3
IATOM    HIS-1  ND1 4
IATOM    HIS-1  CE1 5
IATOM    HIS-1  HE1 6
IATOM    HIS-1  NE2 7
IATOM    HIS-1  CD2 8
IATOM    HIS-1  HD2 9

ATOMNAME HISBK    0  N  
ATOMNAME HISBK    1  H  
ATOMNAME HISBK    2  CA 
ATOMNAME HISBK    3  HA 
ATOMNAME HISBK    4  C  
ATOMNAME HISBK    5  O  
ATOMNAME HIS01    0  CB 
ATOMNAME HIS01    1 1HB 
ATOMNAME HIS01    2 2HB 
ATOMNAME HIS01    3  CG 
ATOMNAME HIS01    4  ND1
ATOMNAME HIS01    5  CE1
ATOMNAME HIS01    6  HE1
ATOMNAME HIS01    7  NE2
ATOMNAME HIS01    8  HE2
ATOMNAME HIS01    9  CD2
ATOMNAME HIS01   10  HD2
ATOMNAME HIS02    0  CB 
ATOMNAME HIS02    1 1HB 
ATOMNAME HIS02    2 2HB 
ATOMNAME HIS02    3  CG 
ATOMNAME HIS02    4  ND1
ATOMNAME HIS02    5  HD1
ATOMNAME HIS02    6  CE1
ATOMNAME HIS02    7  HE1
ATOMNAME HIS02    8  NE2
ATOMNAME HIS02    9  CD2
ATOMNAME HIS02   10  HD2
ATOMNAME HIS+1    0  CB 
ATOMNAME HIS+1    1 1HB 
ATOMNAME HIS+1    2 2HB 
ATOMNAME HIS+1    3  CG 
ATOMNAME HIS+1    4  ND1
ATOMNAME HIS+1    5  HD1
ATOMNAME HIS+1    6  CE1
ATOMNAME HIS+1    7  HE1
ATOMNAME HIS+1    8  NE2
ATOMNAME HIS+1    9  HE2
ATOMNAME HIS+1   10  CD2
ATOMNAME HIS+1   11  HD2
ATOMNAME HIS-1    0  CB 
ATOMNAME HIS-1    1 1HB 
ATOMNAME HIS-1    2 2HB 
ATOMNAME HIS-1    3  CG 
ATOMNAME HIS-1    4  ND1
ATOMNAME HIS-1    5  CE1
ATOMNAME HIS-1    6  HE1
ATOMNAME HIS-1    7  NE2
ATOMNAME HIS-1    8  CD2
ATOMNAME HIS-1    9  HD2

#1.Basic Conformer Information: name, pka, em, rxn.
#Marilyn 6/11/03
#23456789A123456789B123456789C
PROTON   HIS01      0
PROTON   HIS02      0
PROTON   HIS+1      +1
PROTON   HIS-1      -1
PKA      HIS01      0.0
PKA      HIS02      0.0
PKA      HIS+1      6.5
PKA      HIS-1      14.4
ELECTRON HIS01      0
ELECTRON HIS02      0
ELECTRON HIS+1      0
ELECTRON HIS-1      0
EM       HIS01      0.0
EM       HIS02      0.0
EM       HIS+1      0.0
EM       HIS-1      0.0
#RXN      HIS01      -1.77
#RXN      HIS02      -1.77
#RXN      HIS+1      -12.22
RXN      HIS01      -3.9
RXN      HIS02      -4.4
RXN      HIS+1      -16.3
RXN      HIS-1      -25.6

#2.Structure Connectivity
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
CONNECT  HISBK  N   sp2       -1    C   0     CA  0     H
CONNECT  HISBK  H   s         0     N
CONNECT  HISBK  CA  sp3       0     N   0     C   0     CB  0     HA
CONNECT  HISBK  HA  s         0     CA
CONNECT  HISBK  C   sp2       0     CA  0     O   1     N
CONNECT  HISBK  O   sp2       0     C

CONNECT  HIS01  CB  sp3       0     CA  0     CG  0    1HB  0    2HB
CONNECT  HIS01 1HB  s         0     CB
CONNECT  HIS01 2HB  s         0     CB
CONNECT  HIS01  CG  sp2       0     CB  0     ND1 0     CD2
CONNECT  HIS01  ND1 sp2       0     CG  0     CE1
CONNECT  HIS01  CE1 sp2       0     ND1 0     NE2 0     HE1
CONNECT  HIS01  HE1 s         0     CE1
CONNECT  HIS01  NE2 sp2       0     CE1 0     CD2 0     HE2
CONNECT  HIS01  HE2 s         0     NE2
CONNECT  HIS01  CD2 sp2       0     NE2 0     CG  0     HD2
CONNECT  HIS01  HD2 s         0     CD2

CONNECT  HIS02  CB  sp3       0     CA  0     CG  0    1HB  0    2HB
CONNECT  HIS02 1HB  s         0     CB
CONNECT  HIS02 2HB  s         0     CB
CONNECT  HIS02  CG  sp2       0     CB  0     ND1 0     CD2
CONNECT  HIS02  ND1 sp2       0     CG  0     CE1 0     HD1
CONNECT  HIS02  HD1 s         0     ND1
CONNECT  HIS02  CE1 sp2       0     ND1 0     NE2 0     HE1
CONNECT  HIS02  HE1 s         0     CE1
CONNECT  HIS02  NE2 sp2       0     CE1 0     CD2
CONNECT  HIS02  CD2 sp2       0     NE2 0     CG  0     HD2
CONNECT  HIS02  HD2 s         0     CD2

CONNECT  HIS+1  CB  sp3       0     CA  0     CG  0    1HB  0    2HB
CONNECT  HIS+1 1HB  s         0     CB
CONNECT  HIS+1 2HB  s         0     CB
CONNECT  HIS+1  CG  sp2       0     CB  0     ND1 0     CD2
CONNECT  HIS+1  ND1 sp2       0     CG  0     CE1 0     HD1
CONNECT  HIS+1  HD1 s         0     ND1
CONNECT  HIS+1  CE1 sp2       0     ND1 0     NE2 0     HE1
CONNECT  HIS+1  HE1 s         0     CE1
CONNECT  HIS+1  NE2 sp2       0     CE1 0     CD2 0     HE2
CONNECT  HIS+1  HE2 s         0     NE2
CONNECT  HIS+1  CD2 sp2       0     NE2 0     CG  0     HD2
CONNECT  HIS+1  HD2 s         0     CD2

CONNECT  HIS-1  CB  sp3       0     CA  0     CG  0    1HB  0    2HB
CONNECT  HIS-1 1HB  s         0     CB
CONNECT  HIS-1 2HB  s         0     CB
CONNECT  HIS-1  CG  sp2       0     CB  0     ND1 0     CD2
CONNECT  HIS-1  ND1 sp2       0     CG  0     CE1
CONNECT  HIS-1  CE1 sp2       0     ND1 0     NE2 0     HE1
CONNECT  HIS-1  HE1 s         0     CE1
CONNECT  HIS-1  NE2 sp2       0     CE1 0     CD2
CONNECT  HIS-1  CD2 sp2       0     NE2 0     CG  0     HD2
CONNECT  HIS-1  HD2 s         0     CD2

#3.Atom Parameters: Partial Charges and Radii
# Radii from "Bondi, J.Phys.Chem., 68, 441, 1964."
RADIUS   HIS    N   1.50
RADIUS   HIS    H   1.00
RADIUS   HIS    CA  2.00
RADIUS   HIS    HA  0.00
RADIUS   HIS    C   1.70
RADIUS   HIS    O   1.40
RADIUS   HIS    CB  2.00
RADIUS   HIS   1HB  0.00
RADIUS   HIS   2HB  0.00
RADIUS   HIS    CG  1.70
RADIUS   HIS    ND1 1.50
RADIUS   HIS    HD1 1.00
RADIUS   HIS    CE1 1.70
RADIUS   HIS    HE1 1.00
RADIUS   HIS    NE2 1.50
RADIUS   HIS    HE2 1.00
RADIUS   HIS    CD2 1.70
RADIUS   HIS    HD2 1.00

CHARGE   HISBK  N    -0.350
CHARGE   HISBK  H     0.250
CHARGE   HISBK  CA    0.100
CHARGE   HISBK  C     0.550
CHARGE   HISBK  O    -0.550

CHARGE   HIS01  CB    0.125
CHARGE   HIS01  CG    0.155
CHARGE   HIS01  NE2  -0.400
CHARGE   HIS01  HE2   0.400
CHARGE   HIS01  CE1   0.155
CHARGE   HIS01  HE1   0.125
CHARGE   HIS01  ND1  -0.560
CHARGE   HIS01  CD2  -0.125
CHARGE   HIS01  HD2   0.125

CHARGE   HIS02  CB    0.125
CHARGE   HIS02  CG    0.155
CHARGE   HIS02  NE2  -0.560
CHARGE   HIS02  CE1   0.155
CHARGE   HIS02  HE1   0.125
CHARGE   HIS02  ND1  -0.400
CHARGE   HIS02  HD1   0.400
CHARGE   HIS02  CD2  -0.125
CHARGE   HIS02  HD2   0.125

CHARGE   HIS+1  CB    0.125
CHARGE   HIS+1  CG    0.142
CHARGE   HIS+1  NE2  -0.350
CHARGE   HIS+1  CE1   0.141
CHARGE   HIS+1  ND1  -0.350
CHARGE   HIS+1  CD2   0.142
CHARGE   HIS+1  HD1   0.450
CHARGE   HIS+1  HD2   0.125
CHARGE   HIS+1  HE1   0.125
CHARGE   HIS+1  HE2   0.450

CHARGE   HIS-1  CB   -0.272
CHARGE   HIS-1  CG    0.548
CHARGE   HIS-1  ND1  -0.917
CHARGE   HIS-1  CE1   0.282
CHARGE   HIS-1  HE1  -0.003
CHARGE   HIS-1  NE2  -0.917
CHARGE   HIS-1  CD2   0.282
CHARGE   HIS-1  HD2  -0.003

#=========================================================================
#        GRP   #      BOND     AFFECTED_ATOMS
#123456789012345678901234567890
#-------|---|----|-|---------|----|----|----|----|----|----|----|----|----
ROTAMER  HIS   0     CA - CB   CG   ND1  CE1  CD2  NE2
ROTAMER  HIS   1     CB - CG   ND1  CE1  CD2  NE2
#=========================================================================
