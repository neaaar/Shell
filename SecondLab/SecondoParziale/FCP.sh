#!/bin/sh
#inizio del file comandi principale FCP.sh

#controllo sul numero di parametri
case $# in
0|1|2)	echo numero parametri sbagliato, usage is posnumber dirass dirass [dirass]
	exit 1;;
*)	echo DEBUG OK: numero di parametri giusto, si prosegue con l'esecuzione del file FCP.sh
esac

#controllo sul tipo dei parametri
num=1
params=
for i in $*
do
	if $num -eq 1
	then #individuato parametro numerico
		case $1 in
		*[!0-9])echo $1 non numerico o non positivo
	 	exit 2;;
		*)if test $1 -eq 0
	 	then echo $1 deve essere strettamente positivo
	 	exit 3
	 	fi;;
		esac
	else
		case $i in
		/*)if test ! -d $i -o ! -x $i
		then	echo $i non directory o non traversabile
		fi
		params=$params $i;;
		*)echo $i non nome assoluto di directory
		exit 4;;
	fi
	num=`expr $num + 1`
done
H=$1

#settaggio ed export variabile PATH
PATH=`pwd`:$PATH
export PATH

#chiamata al file ricorsivo
for i in  $params
do
	> /tmp/conta$$
	echo invocazione del file ricorsivo con la directory $i
	FCR.sh $i $H /tmp/conta$$
	numfile=`wc -l < /tmp/conta$$`
	echo nella gerarchia $i sono stati trovati $numfile file rispettanti la specifica
	if test `expr $numfile % 2` -eq 0
	then echo si invoca la parte C con i seguenti parametri: `cat /tmp/conta$$`
	fi
done

rm /tmp/conta$$
echo FINITO TUTTO
