'''
This is a code to run mcce for the 
Protonation state effects in selective kinase inhibitors

'''

#!/usr/bin/python
import sys
import os
import time
import os.path
from tempfile import mkstemp
from shutil import move
from os import remove, close
from subprocess import call # This is needed to submit jobs
import subprocess
from argparse import ArgumentParser
from argparse import RawTextHelpFormatter

pdb_files = '/home/salah/johnChodera_project_on_github/mcce-charges/pdbs/'
destination_runs = '/home/salah/johnChodera_project_on_github/mcce-charges/mcce_runs/default2/'

dirs_in_pdb_files = [d for d in os.listdir(pdb_files) if os.path.isdir(os.path.join(pdb_files, d))]

Inhibitors_Names = [
	'Afatinib',
	'Afatinib',
	'Alectinib',
	'Axitinib',
	'Axitinib',
	'Bosutinib',
	'Ceritinib',
	'Cobimetinib',
	'Cobimetinib',
	'Crizotinib',
	'Crizotinib',
	'Crizotinib',
	'Crizotinib',
	'Crizotinib',
	'Dabrafenib',
	'Dabrafenib',
	'Dabrafenib',
	'Dasatinib',
	'Dasatinib',
	'Erlotinib',
	'Erlotinib',
	'Gefitinib',
	'Gefitinib',
	'Gefitinib',
	'Gefitinib',
	'Gefitinib',
	'Gefitinib',
	'Idelalisib',
	'Imatinib',
	'Imatinib',
	'Lapatinib',
	'Lenvatinib',
	'Nilotinib',
	'Osimertinib',
	'Palbociclib',
	'Pazopanib',
	'Ponatinib',
	'Ponatinib',
	'Ponatinib',
	'Ponatinib',
	'Ponatinib',
	'Ponatinib',
	'Ponatinib',
	'Ponatinib',
	'Ponatinib',
	'Ponatinib',
	'Regorafenib',
	'Sorafenib',
	'Sorafenib',
	'Sunitinib',
	'Tofacitinib',
]

PDB_Names = [
	'4G5J',
	'4G5P',
	'3AOX',
	'4AG8',
	'4AGC',
	'3UE4',
	'4MKC',
	'4AN2',
	'4LMN',
	'2XP2',
	'2YFX',
	'4ANQ',
	'4ANS',
	'2WGJ',
	'4XV2',
	'5CSW',
	'5HIE',
	'2GQG',
	'4XEY',
	'1M17',
	'4HJO',
	'2ITO',
	'2ITY',
	'2ITZ',
	'3UG2',
	'4I22',
	'4WKQ',
	'4XE0',
	'2HYY',
	'3PYY',
	'1XKK',
	'3WZD',
	'3CS9',
	'4ZAU',
	'2EUF',
	'3CJG',
	'3IK3',
	'3OXZ',
	'3ZOS',
	'4C8B',
	'4QRC',
	'4TYJ',
	'4U0I',
	'4UXQ',
	'4V01',
	'4V04',
	'2QU5',
	'3WZE',
	'4ASD',
	'4AGD',
	'3LXK',
]

PDB_Inhibitor_Dictionary = {
	'4G5J':'Afatinib',
	'4G5P':'Afatinib',
	'3AOX':'Alectinib',
	'4AG8':'Axitinib',
	'4AGC':'Axitinib',
	'3UE4':'Bosutinib',
	'4MKC':'Ceritinib',
	'4AN2':'Cobimetinib',
	'4LMN':'Cobimetinib',
	'2XP2':'Crizotinib',
	'2YFX':'Crizotinib',
	'4ANQ':'Crizotinib',
	'4ANS':'Crizotinib',
	'2WGJ':'Crizotinib',
	'4XV2':'Dabrafenib',
	'5CSW':'Dabrafenib',
	'5HIE':'Dabrafenib',
	'2GQG':'Dasatinib',
	'4XEY':'Dasatinib',
	'1M17':'Erlotinib',
	'4HJO':'Erlotinib',
	'2ITO':'Gefitinib',
	'2ITY':'Gefitinib',
	'2ITZ':'Gefitinib',
	'3UG2':'Gefitinib',
	'4I22':'Gefitinib',
	'4WKQ':'Gefitinib',
	'4XE0':'Idelalisib',
	'2HYY':'Imatinib',
	'3PYY':'Imatinib',
	'1XKK':'Lapatinib',
	'3WZD':'Lenvatinib',
	'3CS9':'Nilotinib',
	'4ZAU':'Osimertinib',
	'2EUF':'Palbociclib',
	'3CJG':'Pazopanib',
	'3IK3':'Ponatinib',
	'3OXZ':'Ponatinib',
	'3ZOS':'Ponatinib',
	'4C8B':'Ponatinib',
	'4QRC':'Ponatinib',
	'4TYJ':'Ponatinib',
	'4U0I':'Ponatinib',
	'4UXQ':'Ponatinib',
	'4V01':'Ponatinib',
	'4V04':'Ponatinib',
	'2QU5':'Regorafenib',
	'3WZE':'Sorafenib',
	'4ASD':'Sorafenib',
	'4AGD':'Sunitinib',
	'3LXK':'Tofacitinib'
}

# This is a list of all residues and cofactors that we have tpl files for 
good_res_list = [
	'26O',
	'7MQ',
	'ALA',
	'ARG',
	'ASN',
	'ASP',
	'BCB',
	'BCL',
	'BCR',
	'BCT',
	'BDL',
	'BKB',
	'BPB',
	'BPH',
	'_BR',
	'_CA',
	'CL1',
	'CLA',
	'CLD',
	'_CL',
	'_CL',
	'CTR',
	'CUA',
	'CUB',
	'_CU',
	'CYD',
	'CYL',
	'CYS',
	'DUA',
	'FAL',
	'FAR',
	'FE2',
	'FME',
	'FS2',
	'FS4',
	'GLN',
	'GLU',
	'GLY',
	'H3M',
	'HA0',
	'HA3',
	'HAM',
	'HAN',
	'HCQ',
	'HEAD2',
	'HEA',
	'HEB',
	'HEC',
	'HEF',
	'HEM',
	'HIB',
	'HIB',
	'HIL',
	'HIS',
	'HIS',
	'HLI',
	'HMA',
	'HMB',
	'HMC',
	'HOH',
	'ILE',
	'_LA',
	'LDA',
	'LEU',
	'LHG',
	'LYS',
	'MEL',
	'MEM',
	'MET',
	'_MG',
	'MYG',
	'_NA',
	'_NA',
	'NH4',
	'NS5',
	'NTG',
	'NTR',
	'OCS',
	'OEC',
	'OPC',
	'PAA',
	'PDD',
	'PEH',
	'PHE',
	'PHO',
	'PL1',
	'PL9',
	'PL9',
	'PQN',
	'PRO',
	'PTR',
	'RSB',
	'SER',
	'SO4',
	'THR',
	'TML',
	'TPO',
	'TRP',
	'TYF',
	'TYL',
	'TYR',
	'UB1',
	'UB6',
	'UBQ',
	'UNK',
	'UQ2',
	'VAL',
	'_ZN',
	'0WN',
	'1N1',
	'40L',
	'AQ4',
	'B49',
	'BOS',
	'EMH',
	'FMM',
	'KIM',
	'LQQ',
	'NIL',
	'RXT',
	'TRA',
	'XIN',
	'ZD6',
	'276',
	'4MK',
	'AXI',
	'BAX',
	'CAB',
	'EUI',
	'IRE',
	'LEV',
	'MI1',
	'P06',
	'STI',
	'VGH',
	'0LI',
	'DB8',
]

# This is a list of all kinase inhibitors that we have tpl files for
inhibitor_list = [
	'032',
	'0WN',
	'1N1',
	'40L',
	'AQ4',
	'B49',
	'BOS',
	'EMH',
	'FMM',
	'KIM',
	'LQQ',
	'NIL',
	'STI',
	'VGH',
	'YY3',
	'0LI',
	'1E8',
	'276',
	'4MK',
	'AXI',
	'BAX',
	'CAB',
	'EUI',
	'IRE',
	'LEV',
	'MI1',
	'P06',
	'RXT',
	'TRA',
	'XIN',
	'ZD6',
	'DB8',
]

# This is a list of all kinase inhibitors with DM at the end (needed for step 4)
inhibitor_list2 = [
	'032DM',
	'0WNDM',
	'1N1DM',
	'40LDM',
	'AQ4DM',
	'B49DM',
	'BOSDM',
	'EMHDM',
	'FMMDM',
	'KIMDM',
	'LQQDM',
	'NILDM',
	'STIDM',
	'VGHDM',
	'YY3DM',
	'0LIDM',
	'1E8DM',
	'276DM',
	'4MKDM',
	'AXIDM',
	'BAXDM',
	'CABDM',
	'EUIDM',
	'IREDM',
	'LEVDM',
	'MI1DM',
	'P06DM',
	'RXTDM',
	'TRADM',
	'XINDM',
	'ZD6DM',
	'DB8DM',
]

def replace(file_path, pattern, subst):
    """ 
	Writes the content to a new file and replaces the old file with the new file 
	"""
	#Create temp file
    fh, abs_path = mkstemp()
    with open(abs_path,'w') as new_file:
        with open(file_path) as old_file:
            for line in old_file:
                new_file.write(line.replace(pattern, subst))
    close(fh)
    #Remove original file
    remove(file_path)
    #Move new file
    move(abs_path, file_path)	

def deleteHH(dirName,fname_base):
	"""
	Deletes the hydrogens from the PDB file. Deletes the row that coresponds to column 3 if column starts with H
	"""
	#print fname_base
	lines_ = open(fname_base).readlines()
	filename = fname_base.split('/')
	create_new = dirName+'/' + 'new'+filename[-1]
	with open(create_new,'w') as tpl:
		for line in lines_:
			a = line
			line = line.split()
			if len(line) > 6:
				if line[3] != 'HOH':
					if line[2][:1] != 'H' and line[2] != 'OXT':
						tpl.write(a)
	tpl.close()

def delete_cofactors(dirName,fname_base):
	# Deletes any extra cofactors that we don't have tpl files for (not in good_res_list)
	#print fname_base
	lines_ = open(fname_base).readlines()
	filename = fname_base.split('/')
	create_new = dirName+'/' + 'good'+filename[-1]
	with open(create_new,'w') as good_pdb:
		for line in lines_:
			a = line
			#print a
			line = line.split()
			if len(line) > 6:
				if line[0] == 'HETATM':
					if line[3] in good_res_list:
						good_pdb.write(a)
					else:
						continue
				else:
					good_pdb.write(a)
	good_pdb.close()

def change_runprm(runprm,prot,str1,str2,str3,str4):
	# Changes run.prm 4 steps
	lines_ = open(runprm).readlines()
	create_new = runprm+'2'
	with open(create_new,'w') as prm:
		for line in lines_:
			a = line
			line = line.split()
			if len(line) != 0:
				if line[-1] == '(INPDB)':
					newLine = 'goodnew'+prot+'   '+line[-1]+'\n'
					prm.write(newLine)
				elif line[-1] == '(DO_PREMCCE)':
					newLine = str1+'    '+line[-1]+'\n'
					prm.write(newLine)
				elif line[-1] == '(DO_ROTAMERS)':
					newLine = str2+'    '+line[-1]+'\n'
					prm.write(newLine)
				elif line[-1] == '(DO_ENERGY)':
					newLine = str3+'    '+line[-1]+'\n'
					prm.write(newLine)
				elif line[-1] == '(DO_MONTE)':
					newLine = str4+'    '+line[-1]+'\n'
					prm.write(newLine)
				else:
					prm.write(a)
				
def submit_job(mysubmit_dir):
	# Submit jobs
	os.chdir(mysubmit_dir)
	qsub_call = "qsub %s"
	print mysubmit_dir
	call(qsub_call % "submit.sh", shell=True)
	time.sleep(2.0)

def do_together():
	# This function runs mcce with inhibitor bonded to protein
	for topDir in dirs_in_pdb_files:
		#print type(topDir)
		onlyfiles = [f for f in os.listdir(pdb_files+topDir) if os.path.isfile(os.path.join(pdb_files+topDir, f))]
		pdbfile = topDir.split('-')
		mydirectory = destination_runs + pdbfile[0]
		if not os.path.exists(mydirectory):
			sys_call = 'mkdir ' + destination_runs + pdbfile[0]
			os.system(sys_call)
		for pdb_file in onlyfiles:
			if "_fixed_ph7.4.pdb" in pdb_file:
				# mydirectory is the dir where we great the runs
				mydirectory = destination_runs + pdbfile[0] + '/'+ pdb_file[:4]+'_together' 
				if not os.path.exists(mydirectory):
					sys_call = 'mkdir ' + destination_runs + pdbfile[0] + '/'+ pdb_file[:4]+'_together' 
					os.system(sys_call)
				
				# Copy PDB file to mydirectory
				sys_call = 'cp ' + pdb_files + topDir + '/' + pdb_file + ' ' + mydirectory
				os.system(sys_call)
				
				# Delete H atoms from PDB  
				fname_base = mydirectory + '/' + pdb_file
				dirName =  mydirectory
				deleteHH(dirName,fname_base)
				
				# Delete non-inhibitor cofactors 
				fname_base = mydirectory + '/new' + pdb_file
				dirName =  mydirectory
				delete_cofactors(dirName,fname_base)
				
				
				# Copy run.prm to mydirectory
				runprm_location = '/home/salah/johnChodera_project_on_github/mcce-charges/mcce_bin/run.prm'
				sys_call = 'cp '+ runprm_location + ' ' + mydirectory
				os.system(sys_call)
				
				# Copy submit.sh to mydirectory
				submit_location = '/home/salah/johnChodera_project_on_github/mcce-charges/mcce_bin/submit.sh'
				sys_call = 'cp ' + submit_location + ' '+  mydirectory
				os.system(sys_call)
				# Change the job name in submit.sh 
				infile = mydirectory+'/submit.sh'
				new_word = '-N mcce_'+pdb_file[:4]
				replace(infile,'-N mcce',new_word)
				
				# Change run.prm
				runprm = mydirectory+'/run.prm'
				change_runprm(runprm,pdb_file,'t','t','t','f')
				sys_call = 'mv '+mydirectory+'/run.prm2 '+ mydirectory+'/run.prm'
				os.system(sys_call)
				
				# Submit jobs
				mysubmit_dir = mydirectory
				submit_job(mysubmit_dir)
	print 'Done'

def editHead3(head3_lst):
	template = '{0:6}{1:15}{2:2}{3:4.2f}{4:7.3f}{5:6.0f}{6:6.2f}{7:3d}{8:3d}{9:8.3f}{10:8.3f}{11:8.3f}{12:8.3f}{13:8.3f}{14:8.3f}{15:>12}\n'
	lines_ = open(head3_lst).readlines()
	with open(head3_lst+'new3','w') as head3:
		for line in lines_:
			a = line
			line = line.split()
			if line[1][:3] in inhibitor_list:
				if line[1][:5] in inhibitor_list2:
					line[2] = 't'
				line[9] = '0.000'
				head3.write(template.format(line[0], line[1],line[2] ,float(line[3]), float(line[4]),float(line[5]),float(line[6]),int(line[7]),int(line[8]),float(line[9]),float(line[10]),float(line[11]),float(line[12]),float(line[13]),float(line[14]),line[15]))
			else:
				head3.write(a)
	head3.close()
	move(head3_lst+'new3', head3_lst)
	print ' '
	
def do_step4_on_together():
	for topDir in dirs_in_pdb_files:
		onlyfiles = [f for f in os.listdir(pdb_files+topDir) if os.path.isfile(os.path.join(pdb_files+topDir, f))]
		pdbfile = topDir.split('-')
		for pdb_file in onlyfiles:
			if "_fixed_ph7.4.pdb" in pdb_file:
				mydirectory = destination_runs + pdbfile[0] + '/'+ pdb_file[:4]+'_together/' 
				
				# Copy submit.sh to mydirectory
				submit_location = '/home/salah/johnChodera_project_on_github/mcce-charges/mcce_bin/submit.sh'
				sys_call = 'cp ' + submit_location + ' '+  mydirectory
				os.system(sys_call)
				# Change the job name in submit.sh 
				infile = mydirectory+'/submit.sh'
				new_word = '-N '+pdbfile[0]+'_'+pdb_file[:4]
				replace(infile,'-N mcce',new_word)
				
				# Edit head3.lst file in mydirectory
				if os.path.exists(mydirectory+'head3.lst'):
					head3_lst = mydirectory+'head3.lst'
					editHead3(head3_lst)
				else: 
					print 'head3.lst does not exist in '+ mydirectory
					
				# Edit run.prm
				runprm = mydirectory+'/run.prm'
				change_runprm(runprm,pdb_file,'f','f','f','t')
				sys_call = 'mv '+mydirectory+'/run.prm2 '+ mydirectory+'/run.prm'
				os.system(sys_call)
				
				
				
				# Submit jobs
				mysubmit_dir = mydirectory
				submit_job(mysubmit_dir)
	print 'Done with do_step4_on_together'
			
def fixstep2(step2_out_file):
	lines_ = open(step2_out_file).readlines()
	for line in lines_:
		a = line
		line = line.split()
		create_new = step2_out_file+'2'
		with open(create_new,'a') as step2:
			if line[3] in inhibitor_list:
				continue
			else:
				step2.write(a)
	
def do_proteinalone():
	for topDir in dirs_in_pdb_files:
		#print type(topDir)
		onlyfiles = [f for f in os.listdir(pdb_files+topDir) if os.path.isfile(os.path.join(pdb_files+topDir, f))]
		pdbfile = topDir.split('-')
		mydirectory = destination_runs + pdbfile[0]
		if not os.path.exists(mydirectory):
			sys_call = 'mkdir ' + destination_runs + pdbfile[0]
			os.system(sys_call)
		for pdb_file in onlyfiles:
			if "_fixed_ph7.4.pdb" in pdb_file:
				# mydirectory is the dir where we create the runs
				mydirectory = destination_runs + pdbfile[0] + '/'+ pdb_file[:4]+'_proteinalone' 
				if not os.path.exists(mydirectory):
					sys_call = 'mkdir ' + mydirectory 
					os.system(sys_call)
				
				# Copy step2_out.pdb from the do_together() runs
				step2_out_file = destination_runs + pdbfile[0] + '/'+ pdb_file[:4]+'_together/step2_out.pdb'
				fixstep2(step2_out_file)
				sys_call = 'mv '+destination_runs + pdbfile[0] + '/'+ pdb_file[:4]+'_together/step2_out.pdb2 '+ mydirectory+'/step2_out.pdb'
				os.system(sys_call)
				
				# Copy run.prm to mydirectory
				runprm_location = '/home/salah/johnChodera_project_on_github/mcce-charges/mcce_bin/run.prm'
				sys_call = 'cp '+ runprm_location + ' ' + mydirectory
				os.system(sys_call)
				
				# Copy submit.sh to mydirectory
				submit_location = '/home/salah/johnChodera_project_on_github/mcce-charges/mcce_bin/submit.sh'
				sys_call = 'cp ' + submit_location + ' '+  mydirectory
				os.system(sys_call)
				# Change the job name in submit.sh 
				infile = mydirectory+'/submit.sh'
				new_word = '-N '+pdbfile[0]+'_'+pdb_file[:4]
				replace(infile,'-N mcce',new_word)
				
				# Change run.prm
				runprm = mydirectory+'/run.prm'
				change_runprm(runprm,pdb_file,'f','f','t','t')
				sys_call = 'mv '+mydirectory+'/run.prm2 '+ mydirectory+'/run.prm'
				os.system(sys_call)
				
				# Submit jobs
				mysubmit_dir = mydirectory
				submit_job(mysubmit_dir)
	print 'Done'
	
def fixstep2_for_inhibitor(step2_out_file):
	lines_ = open(step2_out_file).readlines()
	for line in lines_:
		a = line
		line = line.split()
		create_new = step2_out_file+'2'
		with open(create_new,'a') as step2:
			if line[3] in inhibitor_list:
				step2.write(a)
			else:
				continue
	
def do_inhibitoralone():
	for topDir in dirs_in_pdb_files:
		onlyfiles = [f for f in os.listdir(pdb_files+topDir) if os.path.isfile(os.path.join(pdb_files+topDir, f))]
		pdbfile = topDir.split('-')
		mydirectory = destination_runs + pdbfile[0]

		if not os.path.exists(mydirectory):
			sys_call = 'mkdir ' + destination_runs + pdbfile[0]
			os.system(sys_call)
		for pdb_file in onlyfiles:
			if "_fixed_ph7.4.pdb" in pdb_file:
				# mydirectory is the dir where we create the runs
				mydirectory = destination_runs + pdbfile[0] + '/'+ pdb_file[:4]+'_inhibitoralone' 
				
				if not os.path.exists(mydirectory):
					sys_call = 'mkdir ' + mydirectory 
					os.system(sys_call)
				
				# Copy step2_out from _together
				step2_out_file = destination_runs + pdbfile[0] + '/'+ pdb_file[:4]+'_together/step2_out.pdb'
				fixstep2_for_inhibitor(step2_out_file)
				sys_call = 'mv '+destination_runs + pdbfile[0] + '/'+ pdb_file[:4]+'_together/step2_out.pdb2 '+ mydirectory+'/step2_out.pdb'
				os.system(sys_call)
				
				# Copy run.prm to mydirectory
				runprm_location = '/home/salah/johnChodera_project_on_github/mcce-charges/mcce_bin/run.prm'
				sys_call = 'cp '+ runprm_location + ' ' + mydirectory
				os.system(sys_call)
				
				# Copy submit.sh to mydirectory
				submit_location = '/home/salah/johnChodera_project_on_github/mcce-charges/mcce_bin/submit.sh'
				sys_call = 'cp ' + submit_location + ' '+  mydirectory
				os.system(sys_call)
				# Change the job name in submit.sh 
				infile = mydirectory+'/submit.sh'
				new_word = '-N '+pdbfile[0][:4]+'_'+pdb_file[:4]
				replace(infile,'-N mcce',new_word)
				
				# Change run.prm
				runprm = mydirectory+'/run.prm'
				change_runprm(runprm,pdb_file,'f','f','t','f')
				sys_call = 'mv '+mydirectory+'/run.prm2 '+ mydirectory+'/run.prm'
				os.system(sys_call)
				
				# Submit jobs
				mysubmit_dir = mydirectory
				submit_job(mysubmit_dir)
			
	print 'Done with _inhibitoralone'
	
def do_step4_on_inhibitoralone():
	for topDir in dirs_in_pdb_files:
		onlyfiles = [f for f in os.listdir(pdb_files+topDir) if os.path.isfile(os.path.join(pdb_files+topDir, f))]
		pdbfile = topDir.split('-')
		for pdb_file in onlyfiles:
			if "_fixed_ph7.4.pdb" in pdb_file:
				mydirectory = destination_runs + pdbfile[0] + '/'+ pdb_file[:4]+'_inhibitoralone/' 
				
				# Edit head3.lst file in mydirectory
				if os.path.exists(mydirectory+'head3.lst'):
					head3_lst = mydirectory+'head3.lst'
					editHead3(head3_lst)
				else: 
					print 'head3.lst does not exist in '+ mydirectory
					
				# Edit run.prm
				runprm = mydirectory+'/run.prm'
				change_runprm(runprm,pdb_file,'f','f','f','t')
				sys_call = 'mv '+mydirectory+'/run.prm2 '+ mydirectory+'/run.prm'
				os.system(sys_call)
				
				# Submit the job again
				# Submit jobs
				mysubmit_dir = mydirectory
				submit_job(mysubmit_dir)
	print 'Done with do_step4_on_inhibitoralone'

def check_together():
	dirs_in_mcce_runs_default = [d for d in os.listdir(destination_runs) if os.path.isdir(os.path.join(destination_runs, d))]
	for topDir in dirs_in_mcce_runs_default:
		dirs_in_dir_mcce_runs_default = [d for d in os.listdir(destination_runs+topDir) if os.path.isdir(os.path.join(destination_runs+topDir, d))]
		for final_dir in dirs_in_dir_mcce_runs_default:
			if '_together' in final_dir:
				mydirectory = destination_runs + topDir +'/'+ final_dir
				# check if  step2 file in mydirectory
				#if not os.path.exists(mydirectory+'/step2_out.pdb'):
					#print 'step2 does not exist in '+ mydirectory
				
				# check if  head3.lst file in mydirectory
				if not os.path.exists(mydirectory+'head3.lst'):
					print 'head3 does not exist in '+ mydirectory
					
				#check if  sum_crg and fort.38 file in mydirectory
				#if not os.path.exists(mydirectory+'fort.38'):
					#print 'fort.38 does not exist in '+ mydirectory

def check_proteinalone():
	for topDir in dirs_in_pdb_files:
		onlyfiles = [f for f in os.listdir(pdb_files+topDir) if os.path.isfile(os.path.join(pdb_files+topDir, f))]
		pdbfile = topDir.split('-')
		for pdb_file in onlyfiles:
			if "_fixed_ph7.4.pdb" in pdb_file:
				mydirectory = destination_runs + pdbfile[0] + '/'+ pdb_file[:4]+'_proteinalone/' 

				# check if  step2 file in mydirectory
				if not os.path.exists(mydirectory+'step2_out.pdb'):
					print 'step2 does not exist in '+ mydirectory
				
				# check if  head3.lst file in mydirectory
				if not os.path.exists(mydirectory+'head3.lst'):
					print 'head3 does not exist in '+ mydirectory
					
				# check if  sum_crg and fort.38 file in mydirectory
				#if not os.path.exists(mydirectory+'fort.38'):
					#print 'fort.38 does not exist in '+ mydirectory


def check_inhibitoralone():
	for topDir in dirs_in_pdb_files:
		onlyfiles = [f for f in os.listdir(pdb_files+topDir) if os.path.isfile(os.path.join(pdb_files+topDir, f))]
		pdbfile = topDir.split('-')
		for pdb_file in onlyfiles:
			if "_fixed_ph7.4.pdb" in pdb_file:
				mydirectory = destination_runs + pdbfile[0] + '/'+ pdb_file[:4]+'_inhibitoralone/' 

				# check if  step2 file in mydirectory
				if not os.path.exists(mydirectory+'step2_out.pdb'):
					print 'step2 does not exist in '+ mydirectory
				
				# check if  head3.lst file in mydirectory
				if not os.path.exists(mydirectory+'head3.lst'):
					print 'head3 does not exist in '+ mydirectory
					
				# check if  sum_crg and fort.38 file in mydirectory
				if not os.path.exists(mydirectory+'fort.38'):
					print 'fort.38 does not exist in '+ mydirectory					



					
def do_what(str_1):
	if str_1 == '_together':
		do_together()
	elif str_1 == '_proteinalone':
		do_proteinalone()
	elif str_1 == '_inhibitoralone':
		do_inhibitoralone()
	elif str_1 == 'do_step4_on_together':
		do_step4_on_together()
	elif str_1 == 'do_step4_on_inhibitoralone':
		do_step4_on_inhibitoralone()
	elif str_1 == 'check_together':
		check_together()
	elif str_1 == 'check_proteinalone':
		check_proteinalone()
	elif str_1 == 'check_inhibitoralone':
		check_inhibitoralone()
	else:
		print 'Nothing happened'
		
def parse_args():
	"""Parse the command line arguments and perform some validation on the
	arguments
	Returns
	-------
	args : argparse.Namespace
	The namespace containing the arguments
	"""
	parser = ArgumentParser(description='''Run MCCE on multiple PDB files''',formatter_class=RawTextHelpFormatter)
	required = parser.add_argument_group('required argument')
	required.add_argument('-do', '--str_1', required=True, type=str,
                          help='''what kind of calculations you want to do, you can do: \n_together: this runs step 1,2,3. You need to run do_step4_on_together when done \ndo_step4_on_together: this runs step 4 after editing head3.lst \n_proteinalone: this runs step 1,2,3,4 \n_inhibitoralone: this runs step 1,2,3.  You need to run do_step4_on_inhibitoralone when done \ndo_step4_on_inhibitoralone: this runs step 4 after editing head3.lst \nFor example:  python run_mcce_default.py -do do_step4_on_together''')
	args = parser.parse_args()
	return args

def main():
    args = parse_args()
    do_what(args.str_1)
	
def entry_point():
    main()
			
if __name__ == '__main__':
    entry_point()

	
	
