#!/bin/sh

cd $1
nfile=0
ndir=0

for i in *
do
if test -f $i
then echo [F] `pwd`/$i
nfile=`expr $nfile + 1`
fi

if test -d $i
then echo [D] `pwd`/$i
ndir=`expr $ndir + 1`
fi
done

echo Total file number: $nfile
echo Total directory number: $ndir
