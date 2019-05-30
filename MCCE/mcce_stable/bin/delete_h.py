#!/usr/bin/python
# delete H atoms from pdb file
import sys

if __name__ == "__main__":
   n=len(sys.argv)
   if n<2:
      print "delete_h.py pdbfile"
      sys.exit(0)

   lines=open(sys.argv[1]).readlines()
   for line in lines:
      if (line[:6] == "ATOM  " or line[:6] == "HETATM") and (line[13] == "H" or line[12] == "H"):
         continue
      else:
         print line,
