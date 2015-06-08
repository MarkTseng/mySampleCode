#/* **************** LPD:1.0 s_27/nomake.sh **************** */
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

# is this a kernel class or not?
if [ ! "$(grep KROOT ../genmake )" ]; then

cat <<EOF >Makefile

allofit:  userprogs

userprogs:
	@\$(MAKE) CFLAGS="-O2 -Wall -pedantic" \\
         lab1_nl_routing lab2_nl_receive_test

clean:
	rm -rf  lab1_nl_routing lab2_nl_receive_test

EOF
echo "Ask your instructor for help in compiling the kernel module" > /dev/stderr
else
    ../genmake
fi
make
