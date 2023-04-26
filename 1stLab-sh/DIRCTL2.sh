#!/bin/sh
case $# in
0) echo Nessun parametro passato, stampo tutte le info
echo SONO DIR.sh
echo "DEBUG-Ora eseguo ls -ld `pwd`"
ls -ld `pwd`/* | more
exit ;;

1)if test -f $1
then echo SONO DIR.sh
echo 'Il valore di $0 ===>' $0
echo 'Il valore di $1 ===>' $1
echo "DEBUG-Ora eseguo ls -l $1"
ls -l $1
else if test -d $1 #parametro corretto, controllo sia directory
then echo SONO DIR.sh
echo 'Il valore di $0 ===>' $0
echo 'Il valore di $1 ===>' $1
echo "DEBUG-Ora eseguo ls -ld $1"
ls -ld $1
fi
fi
exit ;;
*) echo sei un minchione
exit ;;
esac
