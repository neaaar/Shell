case $# in
0) echo Numero parametri non corretto; exit 1;;

1)case $1 in
	/*) echo Parametro sbagliato, passare nome relativo semplice; exit 0;;
	*/*) echo Parametro sbagliato, passare nome relativo semplice; exit 0;;
	*) echo Il nome passato è un nome relativo semplice
	if ls -l $1 > /dev/null
	then if test -f $1
	then echo Stampo nome assoluto: `pwd`/$1
	else echo File non trovato
	fi
	fi
	exit 0;;
esac
exit ;;
*) echo Numero parametri non corretto; exit 1;;
esac
