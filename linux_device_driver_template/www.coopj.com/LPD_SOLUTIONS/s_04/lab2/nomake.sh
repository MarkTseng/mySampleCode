#/* **************** LPD:1.0 s_04/nomake.sh **************** */
#/*
# * The code herein is: Copyright Jerry Cooperstein, 2009
# *
# * This Copyright is retained for the purpose of protecting free
# * redistribution of source.
# *
# *     URL:    http://www.coopj.com
# *     email:  coop@coopj.com
# *
# * The primary maintainer for this code is Jerry Cooperstein
# * The CONTRIBUTORS file (distributed with this
# * file) lists those known to have contributed to the source.
# *
# * This code is distributed under Version 2 of the GNU General Public
# * License, which you should have received with the source.
# *
# */
#!/bin/bash

echo Doing Lab 1 .....

make -f lab_simple_Makefile.sh

echo Doing Lab 2 ..... first way....

make -f lab_complicated_Makefile1.sh

echo cleaning up

make -f lab_complicated_Makefile1.sh clean

echo Doing Lab 2 ..... second way....

make -f lab_complicated_Makefile2.sh
