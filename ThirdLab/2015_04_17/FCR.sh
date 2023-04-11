#!/bin/sh
#inizio del file comandi ricorsivo FCR.sh

cd $1

#si controlla se i file nella directory corrente rispettano le specifiche
for i in *
do
	if test -f $i -a $r $i
	then
		NLt=`grep 't$' $i | wc -l`
		if test NLt -ge $3
		then
			echo Trovato file che rispetta le specifiche: `pwd`/$i
			echo `pwd`/$i >> $2
		fi
	fi
done

#esplorazione ricorsiva della gerarchia
for i in *
do
	if test -d $i -a $x $i #se directory e traversabile
	then
		FCR.sh `pwd`/$i $2 $3
	fi
done

