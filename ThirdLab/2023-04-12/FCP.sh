#!/bin/sh
#inizio del file comandi principale FCP.sh

#controllo sul numero dei parametri
case $# in
0|1|2)
	echo ERRORE: usage is str1 str2 dirAss [dirAss]; exit 1;;
*)
	echo DEBUG-OK: si continua la esecuzione del file comandi con $# parametri;;
esac

#controllo sul tipo dei parametri
#il primo parametro deve essere una semplice stringa
case $1 in
*/*) #se contiene uno slash non è una semplice stringa
	echo ERRORE: $1 non semplice stringa; exit 2;;
*)
	;;
esac

#salviamo la prima stringa in una variabile di nome str1 e facciamo lo shift
str1=$1
shift

#il secondo parametro deve essere una semplice stringa
case $1 in
*/*) #se contiene uno slash non è una semplice stringa
	echo ERRORE: $1 non semplice stringa; exit 3;;
*)
	;;
esac

#salviamo anche la seconda stringa e facciamo un altro shift
str2=$1
shift

#a questo punto nei parametri passati al file comandi dovrebbero rimanere solo nomi assoluti di directory
for i in $*
do
	case $i in
	/*) #se $i è nome assoluto controlliamo che sia una directory e che sia traversabile
		if test ! -d $i -o ! -x $i #se $i non directory o non traversabile
		then
			echo ERRORE: $i non directory o non traversabile; exit 4
		fi;;
	*) #se $i non è un nome assoluto
		echo ERROR: $i non nome assoluto; exit 5;;
	esac
done

#abbiamo ora controllato tutti i parametri, possiamo quindi passare al settaggio e all'export della variabile path
PATH=`pwd`:$PATH
export PATH

#inizializiamo i file temporanei che useremo per tenere traccia dei file rispettanti le specifiche trovati
> /tmp/absoluteDirs-conta$$-str1
> /tmp/absoluteDirs-conta$$-str2

#possiamo ora passare all'esecuzione delle Z fasi
for dirAss in $*
do
	echo Fase per $dirAss
	FCR.sh $dirAss $str1 $str2 /tmp/absoluteDirs-conta$$-str1 /tmp/absoluteDirs-conta$$-str2
done

#finite le Z fasi:
#1.1) stampa numero di file con estensione str1 trovati
SOMMA1=`wc -l < /tmp/absoluteDirs-conta$$-str1`
echo Numero totale di file aventi estensione $str1 trovati: $SOMMA1

#1.2) stampa numero di file con estensione str2 trovati
SOMMA2=`wc -l < /tmp/absoluteDirs-conta$$-str2`
echo Numero totale di file aventi estensione $str2 trovati: $SOMMA2

#preparazione al punto 2: ottenimento di uno dei nomi dell'utente corrente
for i in `whoami`
do
	nome=$i
	break
done

#se SOMMA2=0 non eseguiamo il pezzo dopo (verrebbe chiesto all'utente di inserire un numero tra 1 e 0)
if test $SOMMA2 -eq 0
then
	echo ERRORE: SOMMA2 = 0, risulta quindi impossibile inserire un numero compreso tra 1 e SOMMA2; exit 6
fi

#2) se SOMMA2 <= SOMMA1
if test $SOMMA2 -le $SOMMA1
then
	echo $nome, inserisca un numero compreso fra 1 e $SOMMA2
	read N
	#se N < 1 o N > SOMMA2 -> errore
	if test $N -lt 1 -o $N -gt $SOMMA2
	then
		echo ERRORE: $N non compreso tra 1 e $SOMMA2; exit 7
	fi
	#se invece N compreso tra 1 e SOMMA2
	echo Nome N-esimo file assoluto con estensione $str1: `head -$N < /tmp/absoluteDirs-conta$$-str1 | tail -1`
	echo Nome N-esimo file assoluto con estensione $str2: `head -$N < /tmp/absoluteDirs-conta$$-str2 | tail -1`
fi

#rimozione dei file temporanei
rm /tmp/absoluteDirs-conta$$-str1
rm /tmp/absoluteDirs-conta$$-str2
echo FINITO TUTTO
