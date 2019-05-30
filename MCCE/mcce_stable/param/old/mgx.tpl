CONFLIST MGX        MGXBK MGX+2 

NATOM    MGXBK      0
NATOM    MGX01      1
NATOM    MGX+1      1
NATOM    MGX+2      1

IATOM    MGX01 MG   0
IATOM    MGX+1 MG   0
IATOM    MGX+2 MG   0

ATOMNAME MGX01    0 MG  
ATOMNAME MGX+1    0 MG

ATOMNAME MGX+2    0 MG



#1.Basic Conformer Information: name, pka, em, rxn.
#23456789A123456789B123456789C
PROTON   MGX01      0
PKA      MGX01      0.0
ELECTRON MGX01      0
EM       MGX01      0.0
RXN      MGX01      0

PROTON   MGX+1      0
PKA      MGX+1      0.0
ELECTRON MGX+1      1
EM       MGX+1      0.0
RXN      MGX+1      0

PROTON   MGX+2      0
PKA      MGX+2      0.0
ELECTRON MGX+2      2
EM       MGX+2      0.0
RXN      MGX+2      0

#2.Structure Connectivity
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
CONNECT  MGX01 MG   ion 
CONNECT  MGX+1 MG   ion
CONNECT  MGX+2 MG   ion

#3.Atom Parameters: Partial Charges and Radii
# Radii(vdw) were collected from http://www.webelements.com/
RADIUS   MGX   MG   1.73

CHARGE   MGX01 MG   0.00
CHARGE   MGX+1 MG   1.00
CHARGE   MGX+2 MG   2.00

