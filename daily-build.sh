folders=(pdt src visualisation)
files=(README.md)

peanoVersion="Peano-3.0-$(git log --format="%h" -n 1 .)"

tarName="$peanoVersion.tar.gz"

echo $tarName

tar --exclude-vcs --exclude=*.o -czvf  $tarName ${folders[*]} ${files[*]}

