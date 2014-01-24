import sys
import os
import os.path
import re


inputFilename  = sys.argv[1]
outputFilename = "tmp.data"

inFile         = open( inputFilename,  "r" )
outFile        = open( outputFilename, "w" )



outFile.write( "Step Forks Joins Working-ranks \n" )


step  = 0
forks = 0
joins = 0
ranks = 1

data = "   "
while ( not data=="" ):
  data = inFile.readline()
  step  = step + 1
  if (re.search( "tarch::mpianalysis::DefaultAnalyser::addWorker", data )):
    forks = forks + 1
    ranks = ranks + 1
  if (re.search( "tarch::mpianalysis::DefaultAnalyser::removeWorker", data )):
    joins = joins + 1
    ranks = ranks - 1
    step  = step + 1
  outFile.write( str(step) + " " + str(forks) + " " + str(joins) + " " + str(ranks) + "\n" )
  