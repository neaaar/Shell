#!/bin/sh
#inizio del file comandi ricorsivo FCR.sh

cd $1

numlinee=
for i in *
do
	if test -f $i -a -r $i
	then
		numlinee=`wc -l < $i`
		if test numtotlinee -ge $2
		then
			if test numlinee -ge 5
			then
				`head -5 < $i | tail -1` > $i.quinta
				echo `pwd`/$i.quinta >> $3
			else
				 > $i.NOquinta
				echo `pwd`/$i.NOquinta >> $3
			fi
		fi
	fi
done

#esploriamo ricorsivamente la gerarchia
for i in *
do
	if test -d $i -a -x $i
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done
