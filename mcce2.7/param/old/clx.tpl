CONFLIST CLX        CLXBK CLX01 CLX-1  

NATOM    CLXBK      0
NATOM    CLX01      1
NATOM    CLX-1      1

IATOM    CLX01 CL   0
IATOM    CLX-1 CL   0

ATOMNAME CLX01    0 CL  
ATOMNAME CLX-1    0 CL


#1.Basic Conformer Information: name, pka, em, rxn.
#23456789A123456789B123456789C
PROTON   CLX01      0
PKA      CLX01      0.0
ELECTRON CLX01      0
EM       CLX01      0.0
RXN      CLX01      0

PROTON   CLX-1      0
PKA      CLX-1      0.0
ELECTRON CLX-1      1
EM       CLX-1      0.0
RXN      CLX-1      0

#2.Structure Connectivity
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
CONNECT  CLX01 CL   ion
CONNECT  CLX-1 CL   ion

DONOR    CLX-1 1     CL

#3.Atom Parameters: Partial Charges and Radii       NEED TO CORRECT THIS
# Radii(vdw) were collected from http://www.webelements.com/
RADIUS   CLX   CL   2.23

CHARGE   CLX01 CL   0.00
CHARGE   CLX-1 CL   1.00

