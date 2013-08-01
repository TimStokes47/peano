#!/bin/bash

echo Postprocess terminal output of Peano\'s DefaultAnalyser
echo \(C\) 2013 Roland Wittmann
echo Usage: ./postprocess-default-analyser.sh output-file

script_dir=$(dirname $0)

#echo Assume Python script to reside in $script_dir

grep addWorker $1 | gawk -F" " '{ print $5 }' | gawk -F":" '{ print $2 }'  | sed -e 's|,level||g' | sed -e 's|x|,|g' | sed -e 's|\[||g;s|\]||g'  > $1.tdout
python $script_dir/postprocess-default-analyser.py $1.tdout

echo You find a visualisation in $1.tdout.png
