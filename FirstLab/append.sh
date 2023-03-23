case $# in
0) echo numero parametri passati non valido 
exit 1;;
1) if test -f $1
then cat > $1
fi
exit 0;;
2)if test -f $1 -a -f $2
then cat < $1 >>$2
fi
exit 0;;
*) echo me so sburato
exit 1;;
esac
