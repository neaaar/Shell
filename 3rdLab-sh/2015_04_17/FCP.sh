#!/bin/sh
#inizio del file comandi principale FCP.sh

#controllo sul numero dei parametri
case $# in
0|1|2) echo Pochi parametri passati, usage is num dirass dirass; exit 1;;
*) echo DEBUG-OK: si procede alla esecuzione di FCP.sh con $# parametri;;
esac

#controllo sul tipo dei parametri
#il primo parametro deve essere un numero strettamente positivo
X= #inizializzo la variabile per salvare il numero strettamente positivo
case $1 in
*[!0-9]*) echo $1 non numerico o non positivo; exit 2;;
*) if test $1 -eq 0
then
	echo Prima variabile uguale a zero; exit 3
fi;;
esac

#se il primo parametro è numerico e strettamente positivo lo posso salvare
X=$1
#a questo punto posso fare lo shift delle variabili
shift
#e continuo il controllo sui parametri rimasti (devono essere tutti nomi assoluti di gerarchie)
for i in $*
do
	case $i in
	/*) #è nome assoluto, controllo che sia directory
		if test ! -d $i -o -! -x $i
		then
			echo $i non directory o non traversabile; exit 4;;
		fi;;
	*) #non è nome assoluto
		echo $i non nome assoluto; exit 5;;
	esac
done

#finiti controlli tipo parametri, si passa al settaggio ed export della variabile PATH
PATH=`pwd`:$PATH
export PATH

#si chiama il file comandi ricorsivo con ognuna delle gerarchie passate come parametri
> /tmp/conta$$ #creazione file temporaneo per contare globalmente i file rispettanti le specifiche
for i in $*
do
	echo Chiamata a FCR.sh con la gerarchia $i
	FCR.sh $i /tmp/conta$$ $X
done

#finite le esplorazioni delle diverse gerarchie
echo Numero totale di file trovati: `wc -l < /tmp/conta$$`
K=
for i in `cat /tmp/conta$$`
do
	echo $i, inserire numero K (strettamente positivo e strettamente minore di X) per visualizzare la Kesima linea del file appena menzionato
	read K
	case $K in
	*[!0-9]*) echo K non numerico o non positivo; continue;;
	*)if test $K -eq 0
	then
		echo K deve essere strettamente maggiore di zero; continue
	fi
	if test $K -ge $X
		echo K deve essere strettamente minore di X; continue
	fi
	head -$K | tail -1;;
	esac
done

rm /tmp/conta$$
echo FINITO TUTTO
