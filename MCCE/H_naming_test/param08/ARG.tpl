####################################
# Topology File for:
# ARG
# Extracted from: ARG_ideal.pdb
#
# Created on: 2015-01-12
#
# Created with: tpl_maker.py
####################################

CONFLIST ARG        ARGBK ARG01 ARGDM 

NATOM    ARGBK      0
NATOM    ARG01      27
NATOM    ARGDM      0

IATOM    ARG01  N    0   
IATOM    ARG01  CA   1   
IATOM    ARG01  C    2   
IATOM    ARG01  O    3   
IATOM    ARG01  CB   4   
IATOM    ARG01  CG   5   
IATOM    ARG01  CD   6   
IATOM    ARG01  NE   7   
IATOM    ARG01  CZ   8   
IATOM    ARG01  NH1  9   
IATOM    ARG01  NH2  10  
IATOM    ARG01  OXT  11  
IATOM    ARG01  H    12  
IATOM    ARG01  H2   13  
IATOM    ARG01  HA   14  
IATOM    ARG01  HB2  15  
IATOM    ARG01  HB3  16  
IATOM    ARG01  HG2  17  
IATOM    ARG01  HG3  18  
IATOM    ARG01  HD2  19  
IATOM    ARG01  HD3  20  
IATOM    ARG01  HE   21  
IATOM    ARG01 HH11  22  
IATOM    ARG01 HH12  23  
IATOM    ARG01 HH21  24  
IATOM    ARG01 HH22  25  
IATOM    ARG01  HXT  26  

ATOMNAME ARG01    0  N    
ATOMNAME ARG01    1  CA   
ATOMNAME ARG01    2  C    
ATOMNAME ARG01    3  O    
ATOMNAME ARG01    4  CB   
ATOMNAME ARG01    5  CG   
ATOMNAME ARG01    6  CD   
ATOMNAME ARG01    7  NE   
ATOMNAME ARG01    8  CZ   
ATOMNAME ARG01    9  NH1  
ATOMNAME ARG01   10  NH2  
ATOMNAME ARG01   11  OXT  
ATOMNAME ARG01   12  H    
ATOMNAME ARG01   13  H2   
ATOMNAME ARG01   14  HA   
ATOMNAME ARG01   15  HB2  
ATOMNAME ARG01   16  HB3  
ATOMNAME ARG01   17  HG2  
ATOMNAME ARG01   18  HG3  
ATOMNAME ARG01   19  HD2  
ATOMNAME ARG01   20  HD3  
ATOMNAME ARG01   21  HE   
ATOMNAME ARG01   22 HH11 
ATOMNAME ARG01   23 HH12 
ATOMNAME ARG01   24 HH21 
ATOMNAME ARG01   25 HH22 
ATOMNAME ARG01   26  HXT  

# 1. Basic Conformer Information:
# Number of protons and electrons, pKa, Em, and Reaction Field Energy (RXN)
# PROTON SECTION: PROTON means charge:
PROTON   ARG01      0    
PROTON   ARGDM      0    

# Solution pKa Section: pKa data from CRC Handbook of Chemistry and Physics
PKA      ARG01      0.0  
PKA      ARGDM      0.0  

#ELECTRON SECTION:
ELECTRON ARG01      0.0  
ELECTRON ARGDM      0.0  

# EM SECTION:
EM       ARG01      0.0  
EM       ARGDM      0.0  

# REACTION FIELD ENERGY SECTION:

#  ARG01 UnknownA: angle not found, UnknownB: bond length not found.
#ONNECT   conf atom  orbital  ires conn ires conn ires conn ires conn 
#ONNECT |-----|----|---------|----|----|----|----|----|----|----|----|----|----|
CONNECT  ARG01  N      sp3     0    CA   0    H    0    H2  
CONNECT  ARG01  CA     sp3     0    N    0    C    0    CB   0    HA  
CONNECT  ARG01  C      sp2     0    CA   0    O    0    OXT  
CONNECT  ARG01  O      sp2     0    C   
CONNECT  ARG01  CB     sp3     0    CA   0    CG   0    HB2  0    HB3  
CONNECT  ARG01  CG     sp3     0    CB   0    CD   0    HG2  0    HG3  
CONNECT  ARG01  CD     sp3     0    CG   0    NE   0    HD2  0    HD3  
CONNECT  ARG01  NE     sp3     0    CD   0    CZ   0    HE  
CONNECT  ARG01  CZ     sp2     0    NE   0    NH1  0    NH2  
CONNECT  ARG01  NH1    sp3     0    CZ   0   HH11  0   HH12 
CONNECT  ARG01  NH2    sp3     0    CZ   0   HH21  0   HH22 
CONNECT  ARG01  OXT    sp3     0    C    0    HXT  
CONNECT  ARG01  H       s      0    N   
CONNECT  ARG01  H2      s      0    N   
CONNECT  ARG01  HA      s      0    CA  
CONNECT  ARG01  HB2     s      0    CB  
CONNECT  ARG01  HB3     s      0    CB  
CONNECT  ARG01  HG2     s      0    CG  
CONNECT  ARG01  HG3     s      0    CG  
CONNECT  ARG01  HD2     s      0    CD  
CONNECT  ARG01  HD3     s      0    CD  
CONNECT  ARG01  HE      s      0    NE  
CONNECT  ARG01 HH11     s      0    NH1  
CONNECT  ARG01 HH12     s      0    NH1  
CONNECT  ARG01 HH21     s      0    NH2  
CONNECT  ARG01 HH22     s      0    NH2  
CONNECT  ARG01  HXT     s      0    OXT  


# Atom Parameters:
# Van Der Waals Radii. See source for reference
RADIUS   ARG01  N     1.55   
RADIUS   ARG01  CA    1.7    
RADIUS   ARG01  C     1.7    
RADIUS   ARG01  O     1.52   
RADIUS   ARG01  CB    1.7    
RADIUS   ARG01  CG    1.7    
RADIUS   ARG01  CD    1.7    
RADIUS   ARG01  NE    1.55   
RADIUS   ARG01  CZ    1.7    
RADIUS   ARG01  NH1   1.55   
RADIUS   ARG01  NH2   1.55   
RADIUS   ARG01  OXT   1.52   
RADIUS   ARG01  H     1.2    
RADIUS   ARG01  H2    1.2    
RADIUS   ARG01  HA    1.2    
RADIUS   ARG01  HB2   1.2    
RADIUS   ARG01  HB3   1.2    
RADIUS   ARG01  HG2   1.2    
RADIUS   ARG01  HG3   1.2    
RADIUS   ARG01  HD2   1.2    
RADIUS   ARG01  HD3   1.2    
RADIUS   ARG01  HE    1.2    
RADIUS   ARG01 HH11   1.2    
RADIUS   ARG01 HH12   1.2    
RADIUS   ARG01 HH21   1.2    
RADIUS   ARG01 HH22   1.2    
RADIUS   ARG01  HXT   1.2    

