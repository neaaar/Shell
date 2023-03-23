if test $# -eq 0
then exit 1
fi

echo $*

a=1
for i in $*
do
echo \$$a: $i
a=`expr $a + 1`
done
