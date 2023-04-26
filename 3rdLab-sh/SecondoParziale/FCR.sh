#!/bin/sh
#inizio del file comandi ricorsivo FCR.sh

cd $1

for i in $*
do
	if test -w $i -a -r $i -a $i = $2.txt
	then
		echo $i >> $3
		echo `wc -c < $i` >> $4
	fi
done

for i in $*
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3 $4
	fi
done
