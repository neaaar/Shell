#!/bin/sh
#inizio del file comandi principale FCP.sh

#controllo sul numero dei parametri
case $# in
0|1|2)	echo numero parametri sbagliato; exit 1;;
*)	echo DEBUG-OK: si inizia la esecuzione con $# parametri;;
esac

#controllo sul tipo  dei parametri
#controllo che il primo parametro sia numerico
case $1 in
*[!0-9]*) echo primo parametro non numerico o non positivo; exit 2;;
*) if test $1 -eq 0
   then
	echo il primo parametro deve essere strettamente positivo
	exit 3;
   fi;;
esac

#una volta controllato il primom parametro possiamo salvarlo nella variabile Y
Y=$1
#a questo punto possiamo fare lo shift della variabili
shift
#in questo modo ci rimangono solo i nomi di directory

for i in $*
do
	case $i in
	/*) #controllo che sia directory e che sia traversabile
	if test ! -d $i -o -x $i
	then echo $i non directory o non traversabile
	exit 4
	fi;;
	*) echo $i non nome assoluto di directory
	exit 5;;
	esac
done

#una volta controllato il tipo dei parametri possiamo aggiornare ed esportare la var PATH
PATH=`pwd`:$PATH
export PATH

#chiamata al file ricorsivo con ognuna delle N gerarchie
> /tmp/conta$$
for i in $*
do
	FCR.sh $i $Y /tmp/conta$$
done

echo numero totale di file creati: `wc -l < /tmp/conta$$`
for i in `cat /tmp/conta$$`
do
	echo si visualizza il file dal nome assoluto $i
	cat $i
done

rm /tmp/conta$$

echo FINITO TUTTO
