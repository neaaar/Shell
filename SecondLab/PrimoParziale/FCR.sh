#!/bin/sh

cd $1

echo debug
>/tmp/contafile$$

for i in *
do
	if test -f $i
	then echo $i >> /tmp/contafile$$
	fi
done

if test `wc -l < /tmp/contafile$$` -eq $2
then
	>/tmp/contarighe$$
	for i in `cat /tmp/contafile$$`
	do
		echo $i >/tmp/contarighe$$
	done

	if test `wl -l < /tmp/contarighe$$` -eq $3
	then
		if grep $4 /tmp/contarighe$$
		then echo Carattere trovato all'interno di un file della directory `pwd`
		fi
	fi
	rm /tmp/contarighe$$
fi

rm /tmp/contafile$$

for i in *
do
	if test -d $i -a -x $i
	then
		echo Iniziamo la esplorazione della directory `pwd`/$i
		FCR.sh `pwd`/$i $2 $3 $4
	fi
done
