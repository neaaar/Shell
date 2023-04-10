#!/bin/sh
#inizio del file comandi ricorsivo FCR.sh

cd $1

case $1 in
*/$2?$2)
	pwd >> $3;;
*)
	;;
esac

#esplorazione ricorsiva della gerarchia
for i in *
do
	if test -d $i -a -x $i #se directory e traversabile
	FCR.sh `pwd`/$i $2 $3
done

