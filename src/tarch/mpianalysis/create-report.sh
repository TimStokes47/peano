#!/bin/bash

echo Create report of Peano run from Peano\'s DefaultAnalyser
echo \(C\) 2014 Kristof Unterweger, Tobias Weinzierl, Roland Wittmann
echo Usage: ./create-report.sh peano-output-file number-of-ranks
echo   You have to switch the tracing information on to make the script work
echo   You have to ensure that pydot is in your path due to environment 
echo     variable PYTHONPATH 





script_dir=$(dirname $0)



echo Create main html page
python $script_dir/postprocess-default-analyser-statistics.py $1 $2

echo Create overview statistics
grep -o "tarch::mpianalysis::.*::addWorker.*" $1 | gawk -F" " '{ print $2 }' | sed 's|[0-9]*+||g' | sed 's|\->|,|g'  > $1.workertree
python $script_dir/postprocess-default-analyser-workertree.py $1.workertree
convert -resize 1024x768 $1.workertree.png $1.workertree.small.png

grep -o "tarch::mpianalysis::.*::addWorker.*" $1 | gawk -F" " '{ print $4 }' | gawk -F":" '{ print $2 }'  | sed -e 's|,level||g' | sed -e 's|x|,|g' | sed -e 's|\[||g;s|\]||g'  > $1.2d-dd
python $script_dir/postprocess-default-analyser-2d-dd.py $1.2d-dd

python $script_dir/postprocess-default-analyser-forks-and-joins.py $1
gnuplot $script_dir/postprocess-default-analyser-forks-and-joins.gnuplot
mv tmp.joins-and-forks.png $1.joins-and-forks.png

echo Create global communication profile
python $script_dir/postprocess-default-analyser-runtimes.py $1 $2
gnuplot $script_dir/postprocess-default-analyser-runtimes.gnuplot
mv tmp.runtimes.cpu.png $1.runtimes.cpu.png
mv tmp.runtimes.calendar.png $1.runtimes.calendar.png
mv tmp.runtimes.log.cpu.png $1.runtimes.log.cpu.png
mv tmp.runtimes.log.calendar.png $1.runtimes.log.calendar.png



for (( r=2; r<$2; r++ ))
do
  echo Create communication profile for rank $r
  echo "RANK=$r; load '$script_dir/postprocess-default-analyser-runtimes-per-rank.gnuplot'" | gnuplot  
  mv tmp.runtimes.cpu.png $1.runtimes.rank-$r.cpu.png
  mv tmp.runtimes.calendar.png $1.runtimes.rank-$r.calendar.png
  mv tmp.runtimes.log.cpu.png $1.runtimes.log.rank-$r.cpu.png
  mv tmp.runtimes.log.calendar.png $1.runtimes.log.rank-$r.calendar.png
done




echo Create late worker send analysis - might last long
python $script_dir/postprocess-default-analyser-worker-master-sends.py $1 $2
convert -resize 1024x768 $1.worker-master.png $1.worker-master.small.png
convert -resize 1024x768 $1.worker-master-sparse-average.png $1.worker-master-sparse-average.small.png
convert -resize 1024x768 $1.worker-master-sparse-max.png $1.worker-master-sparse-max.small.png



echo Create late neighbour send analysis - might last long
python $script_dir/postprocess-default-analyser-boundary-exchange.py $1 $2
convert -resize 1024x768 $1.boundary-exchange.png $1.boundary-exchange.small.png
convert -resize 1024x768 $1.boundary-exchange-sparse-average.png $1.boundary-exchange-sparse-average.small.png
convert -resize 1024x768 $1.boundary-exchange-sparse-max.png $1.boundary-exchange-sparse-max.small.png

rm tmp.*
