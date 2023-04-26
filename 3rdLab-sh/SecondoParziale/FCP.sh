#!/bin/sh
#inizio del file comandi principale FCP.sh

#controllo sul numero dei parametri
case $# in
0|1|2) echo Pochi parametri: termino l'esecuzione; exit 1;;
*) echo DEBUG-OK: si inizia l'esecuzione con $# parametri;;
esac

#controllo sul tipo dei parametri
num=1
S=
params=
for i in $*
do
	if test num -eq $#
	then #controllo che sia una semplice stringa
		case $i in
			*[\*,\?]*) echo L'ultimo parametro non è una semplice stringa; exit 2;;
			*)S=$i;;
		esac
	else #controllo che sia un nome assoluto di directory
		case $i in
		/*) if test ! -d $i -o ! -x $i
		then
		echo $i non directory o non traversabile; exit 3
		fi
		params=$params $i;;
		*) echo $i non in forma assoluta; exit 4;;
		esac
	fi
num=`expr $num + 1`
done

#settaggio ed export della variabile PATH
PATH=`pwd`:$PATH
export PATH

#chiamata al file ricorsivo con ogni gerarchia
> /tmp/contafile$$
> /tmp/contacaratteri$$
for i in $params
do
	echo Chiamata al file ricorsivo con la gerarchia $i
	FCR.sh $i $S /tmp/contafile$$ /tmp/contacaratteri$$
done

echo Sono stati trovati `wl -l < /tmp/contafile$$` file rispettanti le specifiche
num=1
risp=
for i in `cat /tmp/contafile$$`
do
	echo head -$num < /tmp/contacaratteri$$ | tail -1
	echo $i
	num=`expr $num + 1`
	echo Vuoi ordinare il file $i?
	read risp
	case $risp in
	y|Y|s|S) sort -f $i
	*) echo Il file non è stato ordinato
done

rm /tmp/conta$$
echo HO FINITO TUTTO
