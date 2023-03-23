#!/bin/sh
if test $# -ne 1 #numero parametri non corretto
then echo numero parametri non corretto; exit 1
fi

if test -f $1 #parametro corretto, controllo sia file
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
