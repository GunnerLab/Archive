#!/usr/bin/python2

import sys

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'Usage:'
        print '  ', sys.argv[0], 'hicup_pdbdict_file'
        sys.exit(0)

    fin = open(sys.argv[1]).readlines()

    for i_line in range(0, len(fin)):
        if (fin[i_line][:7] == 'RESIDUE'):
            resname = fin[i_line][10:13]
          
    print '#ONNECT   conf atom  orbital  ires conn ires conn ires conn ires conn ires conn'
    print '#ONNECT |-----|----|---------|----|----|----|----|----|----|----|----|----|----|'
    for i_line in range(0, len(fin)):
        #01234567890123456789012345678901234567890
        #RESIDUE   RET     49
        #CONECT      C2     4 C1   C3  1H2  2H2  
        #ONNECT   conf atom  orbital  ires conn ires conn ires conn ires conn
        #ONNECT |-----|----|---------|----|----|----|----|----|----|----|----|----|----|
        #CONNECT  RSB+1  C19 sp3       0     C9  0    1H19 0    2H19 0    3H19
        
        if (fin[i_line][:6] == 'CONECT'):
            atomname = fin[i_line][11:15]
            n_connect = int(fin[i_line][18:20])
            if (n_connect == 1): orbital = '  s'
            elif (n_connect == 2): orbital = 'sp3'
            elif (n_connect == 3): orbital = 'sp2'
            elif (n_connect == 4): orbital = 'sp3'
            else: orbital = 'UNK'
            
            connect = []
            for i_connect in range(0, n_connect):
                connect.append(fin[i_line][20+i_connect*5:25+i_connect*5])
                
            print 'CONNECT ','%s01' %resname,atomname,'     ',orbital, '   0 %s' * n_connect %tuple(connect)

