#!/bin/sh
#inizio del file comandi ricorsivo FCR.sh

cd $1

for file in * #per tutti i file nella directory corrente
do
	if test -f $file -a -r $file #se file è effettivamente un file ed è leggibile
	then
		case $file in
		*.$2) #se il file termina con str1 si scrive il suo nome assoluto su /tmp/absoluteDirs-conta$$-str1
			echo `pwd`/$file >> $4;;
		*.$3) #se il file termina con str2 si scrive il suo nome assoluto su /tmp/absoluteDirs-conta$$-str2
			echo `pwd`/$file >> $5;;
		*) #altrimenti non facciamo niente
			;;
		esac
	fi
done

#esplorazione ricorsiva della gerarchia
for i in *
do
	if test -d $i -a -x $i #se directory e traversabile
	then
		FCR.sh `pwd`/$i $2 $3 $4 $5
	fi
done
