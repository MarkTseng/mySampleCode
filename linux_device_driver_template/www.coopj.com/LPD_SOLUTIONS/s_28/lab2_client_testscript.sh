#/* **************** LPD:1.0 s_28/lab2_client_testscript.sh **************** */
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

# Copyright 3/2006, J Cooperstein, coop@coopj.com, (GPLv2)

HOST="localhost"


NCONS=500
NWRITES=2000

NCONS=1500
NWRITES=2000

# testing values to make sure script ok; above ones more useful
NCONS=100
NWRITES=200


#ulimit -n $(( $NCONS + 16))  # makes ure they are enough open filedesciptors
#                             # must be root if > 1024
#echo The number of file descriptors per process set to: $(ulimit -n)

NLOOP=2

[ -f ./lab1_client ] && LABNO=1
[ -f ./lab2_client ] && LABNO=2

CLIENT=./lab$LABNO\_client

# Any crap left from previous?

echo Cleaning up any previous incarnations .....
for SERVER in \
    ./lab$LABNO\_server_epoll \
    ./lab$LABNO\_server_fork \
    ./lab$LABNO\_server_pthread \
    ./lab$LABNO\_server_select \
    ./lab$LABNO\_server_poll \
    ; do killall -w $SERVER ;
done

for SERVER in \
    ./lab$LABNO\_server_epoll \
    ./lab$LABNO\_server_fork \
    ./lab$LABNO\_server_pthread \
    ./lab$LABNO\_server_select \
    ./lab$LABNO\_server_poll \
    ; do
  echo ""
  echo =======================================================================

  j=1
    while [ $j -le $NLOOP ] ; do
	echo ""
	echo "             RUN $j  of CLIENT=$CLIENT with SERVER=$SERVER "
	$SERVER $NCONS> /dev/null &
# give it a couple of seconds to set up before running client
	sleep 2
	$CLIENT $HOST $NCONS $NWRITES > /dev/null
	killall -w $SERVER
	j=$(( $j + 1 ))
    done

done

echo =======================================================================




