folders=(pdt src visualisation)
files=(README.md)a


date=`date +%Y-%m-%d`

peanoVersion="Peano-v3-$date-$(git log --format="%h" -n 1 .)"

tarName="$peanoVersion.tar.gz"

echo $tarName

tar --exclude-vcs --exclude=*.o -czvf  $tarName ${folders[*]} ${files[*]}

