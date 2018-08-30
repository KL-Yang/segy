#/bin/bash

#cflag="-O2"
cflag="-ggdb -W -Wall"
gcc -c $cflag libsegy.c
gcc -c $cflag unit_common.c
gcc -o unit01 $cflag unit01.c libsegy.o unit_common.o -lm -lpng
./unit01 vel_z6.25m_x12.5m_exact.segy
