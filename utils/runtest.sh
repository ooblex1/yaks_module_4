#!/bin/bash
if [ $# != 1 ]; then
		echo 'usage: runtest "command arg1 ... argN"'
		exit 
fi
# execute the command passed as the first argument
CMD=$1
# run command discarding output from command & shell
{ ${CMD} >& /dev/null ; } >& /dev/null
# get the result from the last command
RESVAL=$?
# decide what to print based on the result
if [ ${RESVAL} == 0 ] ; then
		echo -e "Success  : ${CMD}"
elif [ ${RESVAL} == 139 ] ; then
		echo -e "\033[93mSegFault\033[0m : ${CMD}"
else
		echo -e "\033[91mFailed\033[0m   : ${CMD}"
fi
