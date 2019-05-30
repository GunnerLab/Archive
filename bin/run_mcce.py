"""
Automate mcce calculations for testing a new mcce version.
Description
-----------
This code is used to test a new mcce version 


Examples
--------
python run_mcce_test.py -i proteins_list.txt

"""

#!/usr/bin/python
import os
import datetime
import time
from datetime import timedelta
from shutil import copyfile # This is needed to copy files
from subprocess import call # This is needed to submit jobs
from argparse import ArgumentParser
from shutil import move
from tempfile import mkstemp
from os import remove, close


i = datetime.datetime.now()

path, dirs, files = os.walk(".").next()
dir_count = len(dirs) - 1


today_date = ("%s-%s-%s" % (i.month, i.day, i.year) )

theOutputDir = "outputs" + str(dir_count + 1) + "_" + today_date 
if not os.path.exists(theOutputDir):
    os.makedirs(theOutputDir)

os.chdir(theOutputDir)


# Changes run.prm 4 steps
def change_runprm(runprm,pdb_name,str1,str2,str3,str4):
	lines_ = open(runprm).readlines()
	create_new = runprm+'2'
	with open(create_new,'w') as prm:
		for line in lines_:
			a = line
			line = line.split()
			#print line[-1]
			if len(line) != 0:
				if line[-1] == '(INPDB)':
					newLine = 'new'+pdb_name+'   '+line[-1]+'\n'
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



	
def replace(file_path, pattern, subst):
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
	

def setUpRuns(input_file, local=False):
	pdb_file_name = '../'+input_file  # This is a list of PDBs file 
	
	with open(pdb_file_name, "r") as ins:
			for line in ins:
				if line.startswith("#"):
					continue
				else:
					line = line[:-1]
					os.makedirs(line, 0755)
					
					# Copy run.prm
					source = "/home/salah/benchmark_proteins/good_proteins/run.prm"
					runprm = "/home/salah/benchmark_proteins/" + theOutputDir + "/" + line + "/run.prm"
					copyfile(source, runprm) 
					pdb_name = line+'.pdb'
					change_runprm(runprm,pdb_name,'f','f','t','t')
					sys_call = 'mv '+"/home/salah/benchmark_proteins/" + theOutputDir + "/" + line + "/run.prm2 "+ runprm
					os.system(sys_call)
					
					source = "/home/salah/benchmark_proteins/good_proteins/"+line+'/step2_out.pdb'
					step2 = "/home/salah/benchmark_proteins/" + theOutputDir + "/" + line + "/step2_out.pdb"
					copyfile(source, step2)
					
					
					
					source = "/home/salah/benchmark_proteins/good_proteins/submit.sh"
					infile = "/home/salah/benchmark_proteins/" + theOutputDir + "/" + line + "/submit.sh"
					copyfile(source, infile)
					new_word = '-N mcce_'+line[:4]
					replace(infile,'-N mcce',new_word)
					submit_runs(theOutputDir,line)
	


def submit_runs(theOutputDir,line):
	theDir = "/home/salah/benchmark_proteins/" + theOutputDir + "/" + line + "/"
	os.chdir(theDir)
	qsub_call = "qsub %s"
	call(qsub_call % "submit.sh", shell=True)
	# Go up two dir
	goback = "/home/salah/benchmark_proteins/" + theOutputDir + "/"
	os.chdir(goback)
			

def parse_args():
    """Parse the command line arguments and perform some validation on the
    arguments
    Returns
    -------
    args : argparse.Namespace
        The namespace containing the arguments
    """
    parser = ArgumentParser(description='''Run MCCE on multiple PDB files for testing''')
    required = parser.add_argument_group('required argument')
    required.add_argument('-i', '--input_file', required=True, type=str,
                          help='''path to the directory containing input
                          PDB files, already preprocessed for MCCE calculations''')
    args = parser.parse_args()
    return args
	
	
def main():
    args = parse_args()
    setUpRuns(args.input_file)

				  
# Using entry point approach for future conda packaging
def entry_point():
    main()
			
if __name__ == '__main__':
    entry_point()
