#!/bin/sh

if test $# -ne 1
then echo "FCP.sh usage: $1 is the absolute name of a directory"
exit 1
fi

case $1 in 
/*) ;;
*) echo "FCP.sh usage: $1 is the absolute name of a directory"
exit 2 ;;
esac

if test ! -d $1 -o !  -x $1
then echo "$1 not a directory or not traversable"
exit 3
fi

PATH=`pwd`
export PATH

file-dir.sh $1
echo "Execution has finished"

 
