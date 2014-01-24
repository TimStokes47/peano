#!/bin/bash

echo Postprocess terminal output of Peano\'s DefaultAnalyser
echo \(C\) 2013 Roland Wittmann, Tobias Weinzierl
echo Usage: ./postprocess-default-analyser.sh output-file
echo You have to switch the tracing information on to make the script work

script_dir=$(dirname $0)

#echo Assume Python script to reside in $script_dir

grep -o "tarch::mpianalysis::.*::addWorker.*" $1 | gawk -F" " '{ print $4 }' | gawk -F":" '{ print $2 }'  | sed -e 's|,level||g' | sed -e 's|x|,|g' | sed -e 's|\[||g;s|\]||g'  > $1.tdout
python $script_dir/postprocess-default-analyser-2d-dd.py $1.2d-dd

echo You find a visualisation in $1.2d-dd.png
