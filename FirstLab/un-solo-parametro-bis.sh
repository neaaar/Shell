case $# in
0) echo Numero parametri non corretto; exit 1;;

1)case $1 in
	/*) echo Il nome passato è un nome assoluto
	if test -f $1
	then echo Il nome passato appartiene ad un file
	else if test -d $1 -a -x $1
		then echo Il nome passato appartiene ad una directory traversabile
	else echo Il nome passato appartiene ad una directory non traversabile
	fi
	fi
	exit 0;;
	*/*) echo Il nome passato è un nome relativo; exit 0;;
	*) echo Il nome passato è un nome relativo semplice; exit 0;;
esac
exit ;;
*) echo Numero parametri non corretto; exit 1;;
esac
