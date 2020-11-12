#!/bin/bash
if [ $# != 2 ]; then
		echo 'usage: runtest "command arg1 ... argN" "error msg"'
		exit 
fi
# execute the command passed as the first argument
CMD=$1
MSG=$2
# run command discarding output from command & shell
{ ${CMD} >& /dev/null ; } >& /dev/null
# get the result from the last command
RESVAL=$?
# decide what to print based on the result
if [ ${RESVAL} == 0 ] ; then
		echo -e "Success  : ${CMD} -- ${MSG}"
elif [ ${RESVAL} == 139 ] ; then
		echo -e "\033[93mSegFault\033[0m : ${CMD} -- ${MSG}"
else
		echo -e "\033[91mFailed\033[0m   : ${CMD} -- ${MSG}"
fi
