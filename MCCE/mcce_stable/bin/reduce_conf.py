#!/usr/bin/python

fort38 = open('fort.38').readlines()
unoccupied = []
for line in fort38:
   occ = line.split()
   sumocc = 0.0
   for x in occ[1:]:
      sumocc += abs(float(x))
   if sumocc < 0.001: unoccupied.append(occ[0])

for line in open('step3_out.pdb').readlines():
   keyword = line[17:20]+line[80:82]+line[21:30]
   if keyword[10] == ' ': keyword = keyword[:10] + '_' + keyword[11:]
   if keyword not in unoccupied or line[82:86] == "O000": print line,
