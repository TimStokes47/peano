#!/bin/bash

echo Postprocess terminal output of Peano\'s DefaultAnalyser
echo \(C\) 2013 Roland Wittmann, Kristof Unterweger, Tobias Weinzierl
echo Usage: ./postprocess-default-analyser.sh output-file
echo You have to switch the tracing information on to make the script work

script_dir=$(dirname $0)

#echo Assume Python script to reside in $script_dir

grep -o "tarch::mpianalysis::.*::addWorker.*" $1 | gawk -F" " '{ print $2 }' | sed 's|[0-9]*+||g' | sed 's|\->|,|g'  > $1.workertree
echo You find a brief topology description in file $1.workertree


python $script_dir/postprocess-default-analyser-workertree.py $1.workertree

echo You find a visualisation in $1.workertree.png
