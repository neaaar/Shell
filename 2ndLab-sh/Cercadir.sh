#!/bin/sh
#file comandi Cercafile.sh
#ricerca in breadth-first
cd $1		#ci spostiamo nella directory in cui dobbiamo fare la ricerca per questa invocazione
cd ..
case $1 in
`pwd`/$2) echo la directory $2 è la directory corrente `pwd`/$2;;
*) ;;
esac
cd $1

if test -d $2	#verifichiamo se esiste una directory con il nome fornito dall'utente
then
echo la directory $2 si trova in `pwd`	#trovato file
fi

#passiamo ora alla parte ricorsiva
for i in *
do
if test -d $i -a -x $i
then
	#stampa di debugging
	echo Stiamo per esplorare la directory `pwd`/$i
	#invocazione ricorsiva: ATTENZIONE NOME ASSOLUTO dir!!!
	Cercafile.sh `pwd`/$i $2
fi
done
