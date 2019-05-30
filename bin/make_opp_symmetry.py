'''
This code should take two directories as an input and create an excel
'''
#!/usr/bin/python
import sys
import os
import time
import os.path
import math
import datetime
import commands
import re
import tempfile
from tempfile import mkstemp
from shutil import move
import numpy as np


def main(dir_one,dir_two):
	# Look for the wanted conformers
	# I want to look for this list 
	conf_list = [
	'MNC+3',
	'MNC+4',
	'OEH-1',
	'OEH-2',
	]
	
	col_number = 4
	
	onlyfiles_in_dir_one = [f for f in os.listdir(dir_one) if os.path.isfile(os.path.join(dir_one, f))]
	onlyfiles_in_dir_two = [f for f in os.listdir(dir_two) if os.path.isfile(os.path.join(dir_two, f))]
	
	list_one_one = [] 
	list_two = []
	list_two_two = []
	opp_list_1 = []   # Horizontal names
	opp_list_2 = []
	file_list_1 = []  # vertical  names
	file_list_2 = []
	start_ = True
	line_list = []
	
	for everyitem in onlyfiles_in_dir_one:
		if everyitem[:5] in conf_list or everyitem[:4] in conf_list:
			opp_list_1.append(everyitem[:10])
			lines_ = open(dir_one+everyitem).readlines()
			for line in lines_:
				#print line
				line_list.append(line[0:5])
				line = line.split()
				if line[1][:5] in conf_list and start_:
					print line
					file_list_1.append(line[1])
			start_ = False
			
	for everyitem in onlyfiles_in_dir_one:
		list_one = []
		if everyitem[:5] in conf_list or everyitem[:4] in conf_list:
			lines_ = open(dir_one+everyitem).readlines()
			for line in lines_:
				line = line.split()
				#line = re.split('[ -]',line)
				if line[1][:5] in conf_list or line[1][:4] in conf_list:
					#print line
					list_one.append(line[col_number])
			start_ = False
			list_one_one.append(list_one)
	
	
	#print line_list
	
	start_ = True
	for everyitem in onlyfiles_in_dir_two:
		if everyitem[:5] in conf_list or everyitem[:4] in conf_list:
			opp_list_2.append(everyitem[:10])
			lines_ = open(dir_two+everyitem).readlines()
			for line in lines_:
				line = line.split()
				#line = re.split('[ -]',line)
				if line[1][:5] in conf_list and start_:
					file_list_2.append(line[1])
			start_ = False
	
	
	for everyitem in onlyfiles_in_dir_two:
		list_two = []
		if everyitem[:5] in conf_list or everyitem[:4] in conf_list:
			lines_ = open(dir_two+everyitem).readlines()
			for line in lines_:
				line = line.split()
				#line = re.split('[ -]',line)
				if line[1][:5] in conf_list or line[1][:4] in conf_list:
					list_two.append(line[col_number])
			start_ = False
			list_two_two.append(list_two)
	
	
	
	
	
	
	# do A to B
	
	
	good_list = []
	good_list2 = []
	
	i = 0
	with open('kkk.txt','w') as sym:
		for item in file_list_1:
			if item[:10] in opp_list_1:
				good_list.append(opp_list_1.index(item[:10]))
				sym.write(opp_list_1[opp_list_1.index(item[:10])]+' ')
		sym.write('\n')
		
		for itemmm in file_list_1:
			sym.write(itemmm+' ')
			if itemmm[:10] in opp_list_1:
				theindex = opp_list_1.index(item[:10])
				for itemm in list_one_one[good_list[i]]:
					sym.write(itemm+' ')
				sym.write('\n')
			i += 1
			
			
		sym.write('\n\n')
		i = 0
		
		print file_list_1
		print file_list_2
		
		for item in file_list_1:
			if item[:10] in opp_list_2:
				good_list2.append(opp_list_2.index(item[:10]))
				sym.write(opp_list_2[opp_list_2.index(item[:10])]+' ')
		sym.write('\n')
		
		for itemmm in file_list_1:
			sym.write(itemmm+' ')
			if itemmm[:10] in opp_list_2:
				theindex = opp_list_2.index(item[:10])
				for itemm in list_two_two[good_list2[i]]:
					sym.write(itemm+' ')
				sym.write('\n')
			i += 1

	
	sym.close()
	
	

# Using entry point approach for future conda packaging
def entry_point(dir_one,dir_two):
    main(dir_one,dir_two)
	

if __name__ == "__main__":
	if len(sys.argv) < 2:
		print "Specify two input directories\n"
		sys.exit()
	dir_one = sys.argv[1]
	dir_two = sys.argv[2]
	
	entry_point(dir_one,dir_two)
	
	
	

