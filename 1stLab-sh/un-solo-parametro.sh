case $# in
0) echo Numero parametri non corretto; exit 1;;

1)case $1 in
	/*) echo Il nome passato è un nome assoluto; exit 0;;
	*/*) echo Il nome passato è un nome relativo; exit 0;;
	*) echo Il nome passato è un nome relativo semplice; exit 0;;
esac
exit ;;
*) echo Numero parametri non corretto; exit 1;;
esac
