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
outFile.write( "<td>Ranks:</td><td>" + sys.argv[2] + "</td>" )
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
outFile.write( "<p>The following diagram gives an overview of your logical topology. Snapshots at multiple time-steps are merged, i.e. it is a static representation. Image might not be available if pydot is not available to your Python installation.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".workertree.png\" />" )

outFile.write( "<h2>Domain decomposition</h2>" )
outFile.write( "<p>The image below is a snapshot of the domain decomposition. In 2d it " )
outFile.write( "illustrates the exact domain decomposition. For higher dimensions it shows "  )
outFile.write( "a 2d cut.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".2d-dd.png\" />" )

outFile.write( "<h2>Boundary exchange statistics - late senders</h2>" )
outFile.write( "<p>The follow diagrams display an edge whenever a node A had to wait for a neighbouring node B. The first entry of the label counters how often it had to work, the second the maximum data cardinality, i.e. vertices, and the last one the average wait cardinality. If A points to B, B hat to wait for data coming from A. The directed edges illustrate the data flow.</p>" )
outFile.write( "<p>In all diagrams, singular events, i.e. events waits only once or twice, are omitted.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".boundary-exchange.png\" />" )
outFile.write( "<p>The graph below holds only those edges that have an average that is bigger than the average of averages.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".boundary-exchange-sparse-average.png\" />" )
outFile.write( "<p>The last graph illustrates those late senders that have max values that fall into the upper 10% of max values.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".boundary-exchange-sparse-max.png\" />" )

outFile.write( "<h2>Master-worker statistics - late worker</h2>" )
outFile.write( "<p>If an edge points from a to b, it means that master b had to wait for its worker a.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".worker-master.png\" />" )
outFile.write( "<p>The following graph holds only edges whose average is beyond the average of averages.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".worker-master-sparse-average.png\" />" )
outFile.write( "<p>The following graph holds only edges whose maximum weight is with 10% of the total maximum weight.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".worker-master-sparse-max.png\" />" )

outFile.write( "<h2>Runtime profiles</h2>" )
outFile.write( "<p>The diagram below gives a summative overview over all busy and idle times." )
outFile.write( "It is once given with standard y-axis, and once with a logarithmically scaled axis.</p>" )
outFile.write( "<p>As the rank iteration numbers are not synchronised (each worker counts locally how many traversals have been done), the individual measurements can be slightly translated to each other.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".runtimes.png\" />" )
outFile.write( "<img src=\"" + inputFilename + ".runtimes.log.png\" />" )
outFile.write( "<img src=\"" + inputFilename + ".runtimes.master.png\" />" )
outFile.write( "<img src=\"" + inputFilename + ".runtimes.master.log.png\" />" )

outFile.write( "<p>The diagrams below give a history of the times where each individual node spent " )
outFile.write( "its time. Nodes that remain idle throughout the computation are not enlisted.</p>" )

for i in range(1,int(sys.argv[2])):
  outFile.write( "<img src=\"" + inputFilename + ".runtimes.rank-" + str(i) + ".png\" />" )
  outFile.write( "<img src=\"" + inputFilename + ".runtimes.log.rank-" + str(i) + ".png\" />" )

outFile.write( "</body>" )
outFile.write( "</html>" )

