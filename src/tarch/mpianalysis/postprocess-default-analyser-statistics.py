import sys
import os
import os.path
import re


inputFilename  = sys.argv[1]
outputFilename = "report-" + inputFilename + ".html"

outFile        = open( outputFilename, "w" )


forks = 0
joins = 0
workingRanks = 1
sumOfWorkingRanks = 1
maxRanks     = 1
data = "   "
forksPerLevel = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ]

inFile         = open( inputFilename,  "r" )
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
inFile.close()


outFile.write( "<html>" )
outFile.write( "<body>" )


outFile.write( "<h1>Report on " + inputFilename + "</h1>" )
outFile.write( "<p>Peano mpianalysis default postprocessing<br />" )
outFile.write( "   Authors: Kristof Unterweger, Tobias Weinzierl, Roland Wittmann</p>" )

outFile.write( "<p>\
The following data are extracted from the file " + inputFilename + ".\
If you don't see any output, ensure that you have the tracing\
enabled and that you do not filter out info information from the\
tarch::mpianalysis component.\
Some postprocessing steps also require the pydot python module, i.e. \
you have to ensure that the pydot path is set via PYTHONPATH.\
</p>" )



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


outFile.write( "<p>\
<i>Performance hint: </i>\
If the maximal number of working ranks is significantly smaller than the total number of ranks, your application might \
have ran into a weak scaling issue. The problem then was too small. If the problem is sufficiently big, and the number \
if idle ranks still remains high, it might had happened that Peano was not able to fork on a coarse enough level. \
See the section on MPI tuning in the wiki that discusses multiscale concurrency. In such a case, it often helps \
to make the computational grid more regular to some degree, i.e. to prescribe a finer maximum mesh size. \
</p>" )


outFile.write( "<h2>Fork-join evaluation</h2>" )
outFile.write( "<img src=\"" + inputFilename + ".joins-and-forks.png\" />" )

outFile.write( "<center>" )
outFile.write( "<table border=\"1\">" )
outFile.write( "<tr> <td><b>Event type</b></td> <td><b>Master rank</b></td> <td><b>Worker rank</b></td> <td><b>Worker's domain</b></td> </td>" )
inFile         = open( inputFilename,  "r" )
data           = "  "
while ( not data=="" ):
  data = inFile.readline()
  if (re.search( "tarch::mpianalysis::DefaultAnalyser::addWorker", data )):
     outFile.write( "<tr>" )
     outFile.write( "<td>fork</td>" )
     outFile.write( "<td>" )
     outFile.write( data.split( "->" )[0].split( ")" )[-1] )
     outFile.write( "</td>" )
     outFile.write( "<td>" )
     outFile.write( data.split( "+" )[1].split( "[" )[0] )
     outFile.write( "</td>" )
     outFile.write( "<td>" )
     outFile.write( data.split( "worker's domain:" )[1] )
     outFile.write( "</td>" )
     outFile.write( "</tr>" )
inFile.close()
outFile.write( "</table>" )
outFile.write( "</center>" )


outFile.write( "<p>\
<i>Performance hint: </i>\
Joins and forks are expensive operations in terms of walltime. Furthermore, they hinder involved ranks to benefit \
from multithreading as Peano switches off multithreading temporarily whenever a node does rebalancing. As a \
consequence, it often does make sense to switch off rebalancing for compute-intense algorithmic steps and do \
the rebalancing throughout steps that are bandwidth-\data-bound. Those typically cannot benefit from multithreading \
that much anyway. Whenever the partitioning remains invariant, it futhermore does make sense to exchange the \
MPI oracle and work with no balancing. If an oracle tells Peano right from the beginning of a traversal that \
no rebalancing may happen, Peano can omit many checks throughout this traversal. \
</p>" )



outFile.write( "<h2>Worker tree</h2>" )
outFile.write( "<p>The following diagram gives an overview of your logical topology. Snapshots at multiple time-steps are merged, i.e. it is a static representation. Image might not be available if pydot is not available to your Python installation.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".workertree.png\" />" )


outFile.write( "<p>\
<i>Performance hint: </i>\
Study this graph together with a visualisation of the real grid decomposition (such as the graph \
below for 2d). Ensure that your decomposition is balanced any take the wiki's remarks on \
load balancing and node weights into account. \
</p>" )

outFile.write( "<h2>Domain decomposition</h2>" )
outFile.write( "<p>The image below is a snapshot of the domain decomposition. In 2d it " )
outFile.write( "illustrates the exact domain decomposition. For higher dimensions it shows "  )
outFile.write( "a 2d cut. </p>" )
outFile.write( "<img src=\"" + inputFilename + ".2d-dd.png\" />" )


outFile.write( "<h2>Boundary exchange statistics - late senders</h2>" )

outFile.write( "<p>\
The follow diagrams display an edge whenever a node A had to wait for a neighbouring node B. \
The first entry of the label counters how often it had to work, the second the maximum data \
cardinality, i.e. vertices, and the last one the average wait cardinality. If A points to B, \
B hat to wait for data coming from A. The directed edges illustrate the data flow. \
</p>" )

outFile.write( "<p>In all diagrams, singular events, i.e. events waits only once or twice, are omitted.</p>" )

outFile.write( "<img src=\"" + inputFilename + ".boundary-exchange.png\" />" )
outFile.write( "<p>The graph below holds only those edges that have an average that is bigger than the average of averages.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".boundary-exchange-sparse-average.png\" />" )
outFile.write( "<p>The last graph illustrates those late senders that have max values that fall into the upper 10% of max values.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".boundary-exchange-sparse-max.png\" />" )


outFile.write( "<p>\
Peano is designed to exchange all boundary data in the background of the computation. \
In the ideal case, the above graph hence should be empty or very sparse. \
If it is pretty dense, your algorithm is bandwidth-bound. \
</p>" )


outFile.write( "<p>\
<i>Performance hint: </i>\
The global rank 0 should not have adjacent edges. If it has, ensure you've followed the 'Avoid communications \
with rank 0' recipes from the wiki. \
</p>" )

outFile.write( "<p>\
<i>Performance hint: </i>\
If this graph is a clique, your mpi buffer sizes might be too small or too big. Cf. \
SendReceiveBufferPool's setBufferSize(). Its default values might be ill-suited for your \
application's memory footprint. \
</p>" )

outFile.write( "<p>\
<i>Performance hint: </i>\
If single nodes are the hot-spots making the others wait (only few nodes are in the centre), those nodes might either have \
a significantly higher load than others (cf. balancing remark above) or they might have a significant higher surface to \
other nodes that has to be exchanged. \
</p>" )

outFile.write( "<p>\
<i>Performance hint: </i>\
If edges point from workers to their direct parents, the local master-worker balancing might be ill-balanced. Such \
edges are in the first place not that important - any node might have a severe workload where it can only deploy \
smaller parts of the work. The involved worker than has, by definition, to be smaller than its master (it got only \
a fragment of the total work) and then waits for the master's boundary data for the subsequent iteration. A \
critical situation (from a performance point of view) would occur, if the master had to wait for the worker's \
finished message. Cf. Master-worker statistics below. \
</p>" )

outFile.write( "<h2>Master-worker statistics - late worker</h2>" )
outFile.write( "<p>If an edge points from a to b, it means that master b had to wait for its worker a.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".worker-master.png\" />" )
outFile.write( "<p>The following graph holds only edges whose average is beyond the average of averages.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".worker-master-sparse-average.png\" />" )
outFile.write( "<p>The following graph holds only edges whose maximum weight is with 10% of the total maximum weight.</p>" )
outFile.write( "<img src=\"" + inputFilename + ".worker-master-sparse-max.png\" />" )


outFile.write( "<p>\
Peano makes the global rank 0 become a pure administrative rank, i.e. it deploys all workload to another mpi \
rank and afterwards handles load balancing, global algorithm control, and so forth. As a consequence \
there is usually always one edge pointing towards rank 0. This edge does not illustrate a performance issue. \
</p>" )


outFile.write( "<p>\
<i>Performance hint: </i>\
The edges here illustrate the critical communication path for one traversal, i.e. long graphs here indicate \
that along these graphs the Peano traversal is partially serialised. Eliminate these edges by reducing the \
workload of involved nodes. \
</p>" )


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

