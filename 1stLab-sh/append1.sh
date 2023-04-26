case $# in
0) echo numero parametri passati non valido
exit 1;;
1)echo numero parametri passati non valido
exit 0;;
2)case $1 in
	/*) exit 1;;
	*/*) exit 1;;
	*) case $2 in
		/*) exit 1;;
		*/*) exit 1;;
		*) if test -f $1 -a -r $1
			then if ls $2 >/dev/null 2>&1
			then if test -f $2 -a -w $2
			then cat <$1 >>$2
			fi
			else if test -w `pwd`
			then touch $2
			cat <$1 >>$2
			fi
			fi
		fi
		exit 0;;
		esac
		exit 0;;
		esac
exit 1;;
*) echo me so sburato
exit 1;;
esac
