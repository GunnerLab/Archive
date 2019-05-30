####################################
# Topology File for:
# PHE
# Extracted from: PHE_ideal.pdb
#
# Created on: 2015-01-12
#
# Created with: tpl_maker.py
####################################

CONFLIST PHE        PHEBK PHE01 PHEDM 

NATOM    PHEBK      0
NATOM    PHE01      23
NATOM    PHEDM      0

IATOM    PHE01  N    0   
IATOM    PHE01  CA   1   
IATOM    PHE01  C    2   
IATOM    PHE01  O    3   
IATOM    PHE01  CB   4   
IATOM    PHE01  CG   5   
IATOM    PHE01  CD1  6   
IATOM    PHE01  CD2  7   
IATOM    PHE01  CE1  8   
IATOM    PHE01  CE2  9   
IATOM    PHE01  CZ   10  
IATOM    PHE01  OXT  11  
IATOM    PHE01  H    12  
IATOM    PHE01  H2   13  
IATOM    PHE01  HA   14  
IATOM    PHE01  HB2  15  
IATOM    PHE01  HB3  16  
IATOM    PHE01  HD1  17  
IATOM    PHE01  HD2  18  
IATOM    PHE01  HE1  19  
IATOM    PHE01  HE2  20  
IATOM    PHE01  HZ   21  
IATOM    PHE01  HXT  22  

ATOMNAME PHE01    0  N    
ATOMNAME PHE01    1  CA   
ATOMNAME PHE01    2  C    
ATOMNAME PHE01    3  O    
ATOMNAME PHE01    4  CB   
ATOMNAME PHE01    5  CG   
ATOMNAME PHE01    6  CD1  
ATOMNAME PHE01    7  CD2  
ATOMNAME PHE01    8  CE1  
ATOMNAME PHE01    9  CE2  
ATOMNAME PHE01   10  CZ   
ATOMNAME PHE01   11  OXT  
ATOMNAME PHE01   12  H    
ATOMNAME PHE01   13  H2   
ATOMNAME PHE01   14  HA   
ATOMNAME PHE01   15  HB2  
ATOMNAME PHE01   16  HB3  
ATOMNAME PHE01   17  HD1  
ATOMNAME PHE01   18  HD2  
ATOMNAME PHE01   19  HE1  
ATOMNAME PHE01   20  HE2  
ATOMNAME PHE01   21  HZ   
ATOMNAME PHE01   22  HXT  

# 1. Basic Conformer Information:
# Number of protons and electrons, pKa, Em, and Reaction Field Energy (RXN)
# PROTON SECTION: PROTON means charge:
PROTON   PHE01      0    
PROTON   PHEDM      0    

# Solution pKa Section: pKa data from CRC Handbook of Chemistry and Physics
PKA      PHE01      0.0  
PKA      PHEDM      0.0  

#ELECTRON SECTION:
ELECTRON PHE01      0.0  
ELECTRON PHEDM      0.0  

# EM SECTION:
EM       PHE01      0.0  
EM       PHEDM      0.0  

# REACTION FIELD ENERGY SECTION:

#  PHE01 UnknownA: angle not found, UnknownB: bond length not found.
#ONNECT   conf atom  orbital  ires conn ires conn ires conn ires conn 
#ONNECT |-----|----|---------|----|----|----|----|----|----|----|----|----|----|
CONNECT  PHE01  N      sp3     0    CA   0    H    0    H2  
CONNECT  PHE01  CA     sp3     0    N    0    C    0    CB   0    HA  
CONNECT  PHE01  C      sp2     0    CA   0    O    0   OXT  
CONNECT  PHE01  O      sp2     0    C   
CONNECT  PHE01  CB     sp3     0    CA   0    CG   0   HB2   0   HB3  
CONNECT  PHE01  CG     sp2     0    CB   0   CD1   0   CD2  
CONNECT  PHE01 CD1     sp2     0    CG   0   CE1   0   HD1  
CONNECT  PHE01 CD2     sp2     0    CG   0   CE2   0   HD2  
CONNECT  PHE01 CE1     sp2     0   CD1   0    CZ   0   HE1  
CONNECT  PHE01 CE2     sp2     0   CD2   0    CZ   0   HE2  
CONNECT  PHE01  CZ     sp2     0   CE1   0   CE2   0    HZ  
CONNECT  PHE01 OXT     sp3     0    C    0   HXT  
CONNECT  PHE01  H       s      0    N   
CONNECT  PHE01  H2      s      0    N   
CONNECT  PHE01  HA      s      0    CA  
CONNECT  PHE01 HB2      s      0    CB  
CONNECT  PHE01 HB3      s      0    CB  
CONNECT  PHE01 HD1      s      0   CD1  
CONNECT  PHE01 HD2      s      0   CD2  
CONNECT  PHE01 HE1      s      0   CE1  
CONNECT  PHE01 HE2      s      0   CE2  
CONNECT  PHE01  HZ      s      0    CZ  
CONNECT  PHE01 HXT      s      0   OXT  


# Atom Parameters:
# Van Der Waals Radii. See source for reference
RADIUS   PHE01  N     1.55   
RADIUS   PHE01  CA    1.7    
RADIUS   PHE01  C     1.7    
RADIUS   PHE01  O     1.52   
RADIUS   PHE01  CB    1.7    
RADIUS   PHE01  CG    1.7    
RADIUS   PHE01  CD1   1.7    
RADIUS   PHE01  CD2   1.7    
RADIUS   PHE01  CE1   1.7    
RADIUS   PHE01  CE2   1.7    
RADIUS   PHE01  CZ    1.7    
RADIUS   PHE01  OXT   1.52   
RADIUS   PHE01  H     1.2    
RADIUS   PHE01  H2    1.2    
RADIUS   PHE01  HA    1.2    
RADIUS   PHE01  HB2   1.2    
RADIUS   PHE01  HB3   1.2    
RADIUS   PHE01  HD1   1.2    
RADIUS   PHE01  HD2   1.2    
RADIUS   PHE01  HE1   1.2    
RADIUS   PHE01  HE2   1.2    
RADIUS   PHE01  HZ    1.2    
RADIUS   PHE01  HXT   1.2    

