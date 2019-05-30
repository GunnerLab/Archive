Landscape Code Health: [![Code Health](https://landscape.io/github/GunnerLab/automate-mcce/master/landscape.svg?style=flat)](https://landscape.io/github/GunnerLab/automate-mcce/master)


#### automate-mcce
A Python module to efficiently automate MCCE calculations.
The automate-mcce module can be used to: 

- Simplifiy setting up MCCE calculations by automatically creating parameter files, editing MCCE parameters without manually handling run.prm files and creating job submit scripts.
- Automate MCCE calculations on multiple PDB files. 
- Use automated MCCE calculations as part of calculation workflows that use additional components (e.g., hydrogen bond analysis) 

Future updates will also include automated post-processing capabilities. 

#### Prerequisites
To run MCCE calculations using `automated_mcce.py`, you will need the anaconda python distribution and a working MCCE installtion.
#### Installation
Simply clone the repository and run `automated_mcce.py` like any other Python program. 
```
    git clone git@github.com:GunnerLab/automate-mcce.git
```
#### Usage
Script: `automated_mcce.py`

Help for `automated_mcce.py` (obtained via `python automated_mcce.py -h`) is:

```
Run MCCE on multiple PDB files

optional arguments:
  -h, --help            show this help message and exit

required arguments:
  -i INPUT_DIRECTORY, --input_directory INPUT_DIRECTORY
                        path to the directory containing input PDB files,
                        already preprocessed for MCCE calculations
  -d DESTINATION_DIRECTORY, --destination_directory DESTINATION_DIRECTORY
                        path to the directory where output of mcce
                        calculations will be stored.
  -e MCCE_DIRECTORY, --mcce_directory MCCE_DIRECTORY
                        path to the directory where MCCE is installed.
```

#### Example
As an example, you can run a sample mcce calculation on a directory containing pdb files inside the tests directory.
`python automated_mcce.py -i tests/test_pdbs/ -d ~/test/ -e ~/mcce`
Please note that this will run the quick MCCE protocol. To specify the type of MCCE protocol, you will have to edit the `automated_run` function in `automated_mcce`. For more information see help for class `MCCE_Params`.  
