#!/usr/bin/python

# This program compares conformers in a residue:
# 1. H bond partners
# 2. Torsion angles
# 3. RMSD to conformers of the same residue

import sys, math

class CONF:
   def __init__(self):
      self.atom = []


def get_rmsd(conf1, conf2):
   C = 0
   S = 0.0
   biggest_of_smallest = 0.0
   big_pair = "", "", 0.0
   for atom1 in conf1.atom:
      smallest = 1000000.0
      for atom2 in conf2.atom:
         if atom1[0][1:3] != atom2[0][1:3]: continue
         dx = atom2[1]-atom1[1]
         dy = atom2[2]-atom1[2]
         dz = atom2[3]-atom1[3]
         dd = dx*dx+dy*dy+dz*dz
         if dd < smallest:
            smallest = dd
            atomx = atom2
            atom_pair = atom1[0], atom2[0], math.sqrt(dd)

      if smallest < 10000.0:
         C += 1
         S += smallest
         if biggest_of_smallest < smallest:
            biggest_of_smallest = smallest
            big_pair = atom_pair

   return ((math.sqrt(S/C), big_pair))


if len(sys.argv) < 2:
   print "SYNTAX: res_confs.py IDstring1 [IDstring2 ...]"
   print "Description: This program computes geometry differences of conformers within a residue."
   print "             It reads in step3_out.pdb"
   print "             IDstring1, IDstring2 etc are the string(s) used to identify a unique residue."
   sys.exit(0)

pdblines = [x for x in open("step3_out.pdb").readlines() if x[:6] == "ATOM  " or x[:6] == "HETATM"]

# identify a conformer
IDstrings = sys.argv[1:]
conformerlines = []
for line in pdblines:
   yes = 1
   for ID in IDstrings:
      if line.find(ID) < 0:
         yes = 0
         break
   if yes: conformerlines.append(line)

# How many conformers?
conformerlines = [x for x in conformerlines if x.find("BK") < 0]
if not conformerlines:
   IDs = ""
   for ID in IDstrings: IDs = IDs + "\"%s\", " % ID
   IDs = IDs[:len(IDs)-2]
   print "No conformers match the ID strings: ",IDs
   sys.exit(0)

more = 0
conformerID = conformerlines[0][17:30]
for line in conformerlines[1:]:
   if conformerID != line[17:30]:
      more = 1
      break
if more:
   for line in conformerlines:
      print line,
   print "The ID strings return more than one conformer shown above."
   sys.exit(0)
input_conf = conformerID

# extract residue
ID = conformerID[:10]
residuelines = []
for line in pdblines:
   if line.find(ID) > 0 and line.find("BK") <0:
      residuelines.append(line)


conformers = []
line = residuelines[0]
oldID = line[27:30]+line[80:82]
conformer = CONF()
conformer.ID = line[17:20]+line[80:82]+line[21:30]
conformer.atom.append((line[12:16], float(line[30:38]), float(line[38:46]), float(line[46:54])))
for line in residuelines[1:]:
   newID = line[27:30]+line[80:82]
   if newID == oldID:
      conformer.atom.append((line[12:16], float(line[30:38]), float(line[38:46]), float(line[46:54])))
   else:
      conformers.append(conformer)
      conformer = CONF()
      oldID = newID
      conformer.ID = line[17:20]+line[80:82]+line[21:30]
      conformer.atom.append((line[12:16], float(line[30:38]), float(line[38:46]), float(line[46:54])))
# update after last line
conformers.append(conformer)

N = len(conformers)
for i in range(N):
   if input_conf == conformers[i].ID[:3]+' '+conformers[i].ID[5:]:
      iconf = i
      break

# RMSD
rmsd = [(0.0, ("", "")) for i in range(N)]
conf1 = conformers[iconf]
for i in range(N):
   conf2 = conformers[i]
   rmsd[i]=get_rmsd(conf1, conf2)


for i in range(N):
   print "%14s: %6.3f"%(conformers[i].ID, rmsd[i][0]),
   if rmsd[i][0]>0.001:
      print "\"%4s\"-\"%4s\"=%6.3f" % (rmsd[i][1][0], rmsd[i][1][1], rmsd[i][1][2])
   else:
      print
