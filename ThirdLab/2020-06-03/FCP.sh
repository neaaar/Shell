#!/bin/sh
#inizio del file comandi principale FCP.sh

#controllo sul numero dei parametri
case $# in
0|1|2)
	echo Pochi parametri: usage is singlechar dirass dirass [dirass]; exit 1;;
*)
	echo DEBUG-OK: si procede alla esecuzione del programma con $# parametri;;
esac

#controllo sul tipo dei parametri
#il primo parametro deve essere un singolo carattere
case $1 in
?)
	;;
*)
	echo ERRORE: il primo parametro deve essere un singolo carattere; exit 2;;
esac

#se è un singolo carattere possiamo salvarlo e fare lo shift
C=$1
shift

#ora nei parametri rimangono solo i nomi assoluti di directory
for i in $*
do
	case $i in
	/*)
		if test ! -d $i -o ! -x $i
		then
			echo ERRORE: $i non directory o non traversabile; exit 3
		fi;;
	*)
		echo ERRORE: $i non nome assoluto; exit 4;;
done

#settaggio ed export della variabile PATH
PATH=`pwd`:$PATH
export PATH

#creazione del file temporaneo
> /tmp/nomiAssoluti

#chiamata al file comandi ricorsivo con ogni gerarchia
for i in $*
do
	echo Si inizia la esplorazione della gerarchia $i
	FCR.sh $i $C /tmp/nomiAssoluti
done

#finite le N fasi
echo Numero di file rispettanti le specifiche trovati: `wc -l < /tmp/nomiAssoluti`
for i in `cat /tmp/nomiAssoluti`
do
	echo $i
	echo Elena, vuole visualizzare il contenuto (compresi i file nascosti) della directory $i?
	read answer
	case $answer in
	S*|s*|Y*|y*)
			ls -a $i;;
	*)
			;;
	esac
done

rm /tmp/nomiAssoluti

echo FINITO TUTTO
