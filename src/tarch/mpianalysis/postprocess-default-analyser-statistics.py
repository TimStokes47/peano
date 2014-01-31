import sys
import os
import os.path
import re


inputFilename  = sys.argv[1]
outputFilename = "report-" + inputFilename + ".html"

inFile         = open( inputFilename,  "r" )
outFile        = open( outputFilename, "w" )


forks = 0
joins = 0
workingRanks = 1
sumOfWorkingRanks = 1
maxRanks     = 1
data = "   "
forksPerLevel = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]

while ( not data=="" ):
  data = inFile.readline()
  if (re.search( "tarch::mpianalysis::DefaultAnalyser::addWorker", data )):
    forks             = forks + 1
    workingRanks      = workingRanks + 1
    sumOfWorkingRanks = sumOfWorkingRanks + workingRanks
    level             = int(data.split("level:")[-1].split("]")[0])
    forksPerLevel[level] = forksPerLevel[level] + 1
    if workingRanks > maxRanks: 
      maxRanks = workingRanks
  if (re.search( "tarch::mpianalysis::DefaultAnalyser::removeWorker", data )):
    joins = joins + 1
    workingRanks = workingRanks - 1
    sumOfWorkingRanks = sumOfWorkingRanks + workingRanks



outFile.write( "<html>" )
outFile.write( "<body>" )


outFile.write( "<h1>Report on " + inputFilename + "</h1>" )
outFile.write( "<p>Peano mpianalysis default postprocessing<br />" )
outFile.write( "   Authors: Kristof Unterweger, Tobias Weinzierl, Roland Wittmann</p>" )

outFile.write( "<p>" )
outFile.write( "The following data are extracted from the file " + inputFilename )
outFile.write (". If you don't see any output, ensure that you have the tracing ")
outFile.write( "enabled and that you do not filter out info information from the " )
outFile.write( "tarch::mpianalysis component. " )
outFile.write( "Some postprocessing steps also require the pydot python module, i.e. " )
outFile.write( "you have to ensure that the pydot path is set via PYTHONPATH. ")
outFile.write( "</p>" )



outFile.write( "<center>" )
outFile.write( "<table border=\"1\">" )
outFile.write( "<tr>" )
outFile.write( "<td>Ranks:</td><td>" + sys.argv[1] + "</td>" )
outFile.write( "<tr>" )
outFile.write( "<td>Forks total:</td><td>" + str(forks) + "</td>" )
outFile.write( "</tr><tr>" )
outFile.write( "<td>Joins total:</td><td>" + str(joins) + "</td>" )
outFile.write( "</tr><tr>" )
outFile.write( "<td>Max. working ranks:</td><td>" + str(maxRanks) + "</td>" )
outFile.write( "</tr><tr>" )
outFile.write( "<td>Average working ranks after each reblancing:</td><td>" + str(sumOfWorkingRanks/(forks+joins)) + "</td>" )
outFile.write( "</tr>" )
outFile.write( "</table>" )


outFile.write( "<table border=\"1\">" )
outFile.write( "<tr>" )
for i in range(0,10):
  outFile.write( "</tr><tr>" )
  outFile.write( "<td>Forks on level " + str(i) + ":</td><td>" )
  outFile.write( str(forksPerLevel[i]) )
  outFile.write( "</td>" )

outFile.write( "</tr>" )
outFile.write( "</table>" )
outFile.write( "</center>" )


outFile.write( "<h2>Fork-join evoluation</h2>" )
outFile.write( "<img src=\"" + inputFilename + ".joins-and-forks.png\" />" )


outFile.write( "<h2>Worker tree</h2>" )
outFile.write( "<img src=\"" + inputFilename + ".workertree.png\" />" )

outFile.write( "<h2>Domain decomposition</h2>" )
outFile.write( "<p>The image below is a snapshot of the domain decomposition. In 2d it " )
outFile.write( "illustrates the exact domain decomposition. For higher dimensions it shows "  )
outFile.write( "a 2d cut.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".2d-dd.png\" />" )

outFile.write( "</body>" )
outFile.write( "</html>" )

