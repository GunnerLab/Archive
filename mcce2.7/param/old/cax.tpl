CONFLIST CAX        CAXBK CAX01 CAX+1  

NATOM    CAXBK      0
NATOM    CAX01      1
NATOM    CAX+1      1

IATOM    CAX01 CA   0
IATOM    CAX+1 CA   0

ATOMNAME CAX01    0 CA  
ATOMNAME CAX+1    0 CA


#1.Basic Conformer Information: name, pka, em, rxn.
#23456789A123456789B123456789C
PROTON   CAX01      0
PKA      CAX01      0.0
ELECTRON CAX01      0
EM       CAX01      0.0
RXN      CAX01      0

PROTON   CAX+1      0
PKA      CAX+1      0.0
ELECTRON CAX+1      2
EM       CAX+1      0.0
RXN      CAX+1      0

#2.Structure Connectivity
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
CONNECT  CAX01 CA   ion
CONNECT  CAX+1 CA   ion

DONOR    CAX+1 1     CA

#3.Atom Parameters: Partial Charges and Radii
# Radii(vdw) were collected from http://www.webelements.com/
RADIUS   CAX   CA   2.23

CHARGE   CAX01 CA   0.00
CHARGE   CAX+1 CA   2.00

