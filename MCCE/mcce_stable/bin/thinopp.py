#!/usr/bin/python
# load all opp file, store in a matrix, and print to files for significant interactions
import os, sys
import psyco
psyco.full()

class PW:
   def __init__(self):
      self.pw={}
      self.pw={}
      self.pw={}
      self.pw={}
      self.opps=[]
      return
   def get_opp_names(self):
      fnames=os.listdir("./")
      opp_names=[]
      for fname in fnames:
         if fname[len(fname)-4:len(fname)] == ".opp":
            opp_names.append(fname)
      self.opps = opp_names
      return
   def print_opp_names(self):
      for fname in self.opps:
         print fname
   def load(self):
      if not self.opps:
         self.get_opp_names()
      for fname in self.opps:
         #ARG03A0020_010.opp
         conf1=fname[:14]
         lines = open(fname).readlines()
         for line in lines:
            if len(line) < 20: continue
            fields = line.split()
            conf2=fields[1]
            ele = float(fields[2])
            vdw = float(fields[3])
            raw = float(fields[4])
            crt = float(fields[5])
            if self.pw.has_key(conf1):
               self.pw[conf1][conf2]=(ele, vdw, raw, crt)
            else:
               self.pw[conf1] = {}
               self.pw[conf1][conf2]= (ele, vdw, raw, crt)
   def filter(self, t):
      count = 0
      confs=self.pw.keys()
      for conf1 in confs:
         for conf2 in confs:
            if abs(self.pw[conf1][conf2][0]) > t and abs(self.pw[conf1][conf2][1]) > t:
               count += 1
      return count

if __name__== "__main__":
   pwtable = PW()
#   pwtable.get_opp_names()
#   pwtable.print_opp_names()
   pwtable.load()
   t=float(sys.argv[1])
   print "Number of entries with ele or/and vdw greater than %f is %d" % (t, pwtable.filter(t))
   
