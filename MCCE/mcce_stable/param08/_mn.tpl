CONFLIST _MN        _MNBK _MN+2 _MNDM

NATOM    _MNBK      0
NATOM    _MN+2      1
NATOM    _MNDM      0

IATOM    _MN+2 MN   0

ATOMNAME _MN+2    0 MN  


#1.Basic Conformer Information: name, pka, em, rxn.
#23456789A123456789B123456789C
PROTON   _MN+2      0
PKA      _MN+2      0.0
ELECTRON _MN+2      0
EM       _MN+2      0.0
RXN      _MN+2      -33.53

#2.Structure Connectivity
#23456789A123456789B123456789C123456789D123456789E123456789F123456789G123456789H123456789I
CONNECT  _MN+2 MN   ion

#3.Atom Parameters: Partial Charges and Radii
# Radii(vdw) were collected from http://www.webelements.com/
RADIUS   _MN   MN   2.23

CHARGE   _MN+2 MN   2.00
