#!/bin/sh
#inizio del file comandi ricorsivo FCR.sh

cd $1

for i in *
do
	if test -f $i
	then
		numcaratteri=`wc -c < $i`
		if test $numcaratter -eq $2
		then
			echo trovato il seguente file che rispetta la specifica: `pwd`$i
			echo $i >> $3
		fi
	fi
done

for i in *
do
	if test -d $i -a $x $i
	then FCR.sh `pwd`/$i $2 $3
	fi
done
