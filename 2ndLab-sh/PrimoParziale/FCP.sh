case $# in
4) ;;
*) echo NUMERO PARAMETRI SBAGLIATO: usage is $0 directory strictlyPositiveNumber strictlyPositiveNumber singleChracter; exit 1 ;;
esac

case $1 in
/*) ;;
*) echo Il primo parametro deve essere un nome assoluto di directory; exit 2 ;;
esac
G=$1

case $2 in
*[!0-9]*) echo Il secondo parametro deve essere un numero; exit 3 ;;
*)	if test $2 -eq 0
	then echo Il secondo parametro deve essere un numero strettamente positivo; exit 4
	fi;;
esac
N=$2

case $3 in
*[!0-9]*) echo Il terzo parametro deve essere un numero; exit 5 ;;
*)	if test $3 -eq 0
	then echo Il terzo parametro deve essere un numero strettamente positivo; exit 6
	fi;;
esac
H=$3

case $4 in
?) ;;
*) echo Il quarto parametro deve essere un singolo carattere; exit 7 ;;
esac
Cx=$4

PATH=`pwd`:$PATH
export PATH

FCR.sh $G $N $H $Cx
