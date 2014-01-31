#!/bin/bash

echo Create report of Peano run from Peano\'s DefaultAnalyser
echo \(C\) 2014 Kristof Unterweger, Tobias Weinzierl, Roland Wittmann
echo Usage: ./create-report.sh peano-output-file number-of-ranks
echo   You have to switch the tracing information on to make the script work
echo   You have to ensure that pydot is in your path due to environment 
echo     variable PYTHONPATH 





script_dir=$(dirname $0)


grep -o "tarch::mpianalysis::.*::addWorker.*" $1 | gawk -F" " '{ print $2 }' | sed 's|[0-9]*+||g' | sed 's|\->|,|g'  > $1.workertree
python $script_dir/postprocess-default-analyser-workertree.py $1.workertree

grep -o "tarch::mpianalysis::.*::addWorker.*" $1 | gawk -F" " '{ print $4 }' | gawk -F":" '{ print $2 }'  | sed -e 's|,level||g' | sed -e 's|x|,|g' | sed -e 's|\[||g;s|\]||g'  > $1.2d-dd
python $script_dir/postprocess-default-analyser-2d-dd.py $1.2d-dd

python $script_dir/postprocess-default-analyser-forks-and-joins.py $1
gnuplot $script_dir/postprocess-default-analyser-forks-and-joins.gnuplot
mv tmp.joins-and-forks.png $1.joins-and-forks.png

python $script_dir/postprocess-default-analyser-runtimes.py $1 $2
gnuplot $script_dir/postprocess-default-analyser-runtimes.gnuplot
mv tmp.runtimes.png $1.runtimes.png
mv tmp.runtimes.log.png $1.runtimes.log.png
mv tmp.runtimes.master.png $1.runtimes.master.png
mv tmp.runtimes.master.log.png $1.runtimes.master.log.png

for (( r=2; r<$2; r++ ))
do
  echo Create communication profile for rank $r
  echo "RANK=$r; load '$script_dir/postprocess-default-analyser-runtimes-per-rank.gnuplot'" | gnuplot  
  mv tmp.runtimes.png $1.runtimes.rank-$r.png
  mv tmp.runtimes.log.png $1.runtimes.log.rank-$r.png
done


python $script_dir/postprocess-default-analyser-statistics.py $1 $2

%rm tmp.*
