import sys
import re
import matplotlib
matplotlib.use('Agg') # Must be before importing matplotlib.pyplot or pylab!
import pylab
import networkx 
import datetime
import argparse
import performanceanalysisroutines


from argparse import RawTextHelpFormatter




########################################################################
# START OF THE PROGRAM
########################################################################
# For the meaning of the command line arguments, see the documentation
# of the function 'plot_multithreading_adapter_scaling' above.
help = '''
Creates a brief overview of the domain decomposition
\n\n
Sample usages:\n
python domain-decomposition-analysis.py -file 112x16-0.results -dimension 2 -domainoffset 0.0 0.0 -domainsize 15.0 15.0\n
'''

parser = argparse.ArgumentParser(description=help,formatter_class=RawTextHelpFormatter)
parser.add_argument('-file',required=True,help="Input file")
parser.add_argument('-dimension',required=True,help="Dimension of problem. Either 2 or 3.")
parser.add_argument('-domainoffset',nargs="+",required=True,help="Offset of bounding box.")
parser.add_argument('-domainsize',nargs="+",required=True,help="Size of domain's bounding box.")
args   = parser.parse_args();


numberOfRanks   = performanceanalysisroutines.getNumberOfRanks(args.file)
numberOfThreads = performanceanalysisroutines.getNumberOfThreads(args.file)

inputFileName   = args.file
outputFileName  = args.file + ".html"

print "start to process input file " + inputFileName + " with " + str(numberOfRanks) + " rank(s) and " + str(numberOfThreads) + " thread(s)" 


outFile        = open( outputFileName, "w" )
  
    


#
# Now all the images are created and the data is analysed.
# I use this data to generate some problem-specific hints, 
# so it has to be done first
#  
performanceanalysisroutines.parseInputFile(numberOfRanks,inputFileName)

print "plot walltime overview"
performanceanalysisroutines.plotWalltimeOverview()

print "plot global grid overview"
performanceanalysisroutines.plotGlobalGridOverview()

if (numberOfThreads>0):      
  print "plot concurrency levels"
  if (numberOfRanks>0):
    for rank in range(0,numberOfRanks):
      print "plot concurrency levels for rank " + str(rank)
      performanceanalysisroutines.plotConcurrency(rank)
  else:
    print "plot concurrency levels"
    performanceanalysisroutines.plotConcurrency(0)
  
if (numberOfRanks>0):      
  print "plot logical grid topology"
  performanceanalysisroutines.plotLogicalTopology()

  print "extract fork and join statistics"
  performanceanalysisroutines.plotForkJoinStatistics()

  print "plot mpi phases"
  performanceanalysisroutines.plotMPIPhases()

  print "master-worker data exchange"
  performanceanalysisroutines.plotMasterWorkerLateSends()

  print "boundary data exchange"
  performanceanalysisroutines.plotBoundaryLateSends()


#
# Header of report
#
outFile.write( 
  "<html>\
     <body>\
     <h1>Peano Performance Report</h1>\
     <p>Report time stamp: " + str(datetime.datetime.utcnow()) + "</p>\
     <p>Peano's default performance analysis</p>\
     <p>Author: Tobias Weinzierl</p>\
     <p>Data file: " + inputFileName + "</p>\
  ")
     
if (numberOfRanks==0):      
  outFile.write( "\
     <p>Ranks: no MPI used</p>\
  ")
else:      
  outFile.write( "\
     <p>Ranks: " + str(numberOfRanks) + "</p>\
  ")
     
if (numberOfThreads==0):      
  outFile.write( "\
     <p>Threads: no multithreading used</p>\
  ")
else:      
  outFile.write( "\
     <p>Threads: " + str(numberOfThreads) + "</p>\
  ")
     
outFile.write( "\
     <center><img src=\"http://www.peano-framework.org/logo.png\" />\
     <br /><a href=\"http://www.peano-framework.org\" >www.peano-framework.org</a></center>\
  ")

outFile.write( "\
   <h2 id=\"table-of-contents\">Table of contents</h2>\
   <ul>\
   ")

outFile.write( "\
     <li><a href=\"#walltime-overview\">Walltime overview</a></li>\
     <li><a href=\"#global-grid-overview\">Global grid overview</a></li>\
    ")


if (numberOfThreads>0):      
  outFile.write( "<li><a href=\"#concurrency\">Multithreading concurrency</a></li>")
  
  
if (numberOfRanks>0):      
  outFile.write( "\
     <li><a href=\"#logical-topology\">Logical topology</a></li>\
     <li><a href=\"#fork-join-statistics\">Fork and join statistics</a></li>\
     <li><a href=\"#fork-history\">Fork history</a></li>\
     <li><a href=\"#mpi-phases\">MPI phases</a></li>\
     <li><a href=\"#master-worker-data-exchange\">Master-worker data exchange</a></li>\
     <li><a href=\"#boundary-data-exchange\">Boundary data exchange</a></li>\
     <li><a href=\"#individual-ranks\">Individual ranks</a></li>\
    ")

outFile.write( "\
   </ul>\
   ")






#
# Walltime information
#
outFile.write( "\
    <h2 id=\"walltime-overview\">Walltime overview</h2>\
     <img src=\"" + outputFileName + ".walltime.png\" />\
    <br /><br />\
    <p>\
    The fuzzy dots summarise the local runtimes of the individual ranks, \
    i.e. the time spent within the local traversals. \
    Cf. shift discussion in the next section. \
    For non-reducing algorithms that do not synchronise their boundaries, \
    local compute times can exceed the global time per traversal. \
    </p>\
    <p>\
    The solid line is the time per traversal on the global master rank. \
    If you run without MPI, this equals your application's runtime. \
    </p>\
    <p>\
    If your code does not synchronise the ranks with the global master, the solid line does not give a good \
    overview of your runtime. If the code synchronises only everh kth grid sweep, k sweeps are always summarised \
    by one linear graph. \
    </p>\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")

#
# Grid information
#
outFile.write( "\
    <h2 id=\"global-grid-overview\">Global grid overview</h2>\
    <img src=\"" + outputFileName + ".grid-overview.png\" />\
    <img src=\"" + outputFileName + ".grid-overview-global-master.png\" />\
    <br /><br />\
    ")
    
    
if (numberOfRanks>0):      
  outFile.write( "\
    <img src=\"" + outputFileName + ".local-cells.png\" />\
    <img src=\"" + outputFileName + ".local-vertices.png\" />\
    <br /><br />\
    ")

outFile.write( "\
    <p>\
    <b>Remarks on the global cell/vertex and the master plots:</b> \
    If you are implementing a code that allows ranks to send up their state to the \
    master before they've completed the whole traversal (see communicator object in \
    the mappings) or even to skip reduction, you will see oscillations and non-smooth \
    behaviour for some data. This is not a bug, but has to do with the communication scheme. \
    </p>\
    <p>\
    <b>Remarks on the local cell/vertex plots:</b> The darker the plots the more ranks exhibit a certain characteristics. \
    If the points spread out, this is an indicator that your load balancing is \
    inadequate. \
    If you run Peano on multiple ranks, rank 0 typically degenerates to a pure \
    administrative rank, i.e. it is o.k. if there is close to 0 vertices and cells \
    for one particular rank. \
    Please note that the outputs of the individual ranks are not synchronised \
    with each other. \
    Ranks that start to work later yield shorter histograms that are all aligned \
    to the left. \
    </p>\
    <p>\
    <i>Performance hint:</i></p><p>\
    In Peano's spacetree world, the coarse grid ranks are the most critical ranks. \
    If your global master holds many cells, e.g., almost for sure you run into poor-scaling \
    code. \
    <p>\
    <i>Performance hint:</i></p><p>\
    If you identify ranks whose local load decreases incrementally, these are ranks \
    that step by step fork more of their work to other ranks. In this case, it might \
    make sense to switch from an aggressive refinement into an iterative grid refinement \
    strategy (one refinement level per step, e.g.) to allow the rank to deploy work \
    throughout the grid construction and thus build up the grid in parallel and avoid \
    the transfer of whole grid blocks due to rebalancing. \
    </p>\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")



#
# Concurrency analysis
#
if (numberOfThreads>0):      
  outFile.write( "<h2 id=\"concurrency\">Multithreading concurrency</h2>" )
  if numberOfRanks==0:
    outFile.write( "<img src=\"" + outputFileName + "-rank-0.concurrency.png\" />" )
    outFile.write( "<br /><a href=\"" + outputFileName + "-rank-0.concurrency.large.png\">Big version</a>" )

  for rank in range(0,numberOfRanks):
    outFile.write( "<h3>Rank " + str(rank) + "</h2>" )
    outFile.write( "<img src=\"" + outputFileName + "-rank-" + str(rank) + ".concurrency.png\" />" )
    outFile.write( "<br /><a href=\"" + outputFileName + "-rank-" + str(rank) + ".concurrency.large.png\">Big version</a>" )
    
  outFile.write("\
    <br /><br />\
    <p>\
    Legend:\
    <ul>\
     <li>Dotted black line: Concurrency level is one</li> \
     <li>Solid black line: Real concurrency level of current code.</li> \
     <li>Solid green line: Maximum concurrency that could be used in the code if all grain sizes were set to one. Ignores background tasks.</li> \
     <li>Dotted dark green line: Maximum algorithmic concurrency level introduced by the code. Takes not into account that background tasks (blue) might be handled in the background, i.e. real concurrency level could be higher.</li> \
     <li>Light red bar: Average-case algorithmic concurrency level that could be obtained if the code selected grain size one everywhere. Ignores background tasks.</li> \
     <li>Dark red bar: Average-case algorithmic concurrency observed for selected grain sizes. Takes not into account that background tasks (blue) might be handled in the background, i.e. real concurrency level could be higher.</li> \
     <li>Blue bar: Additional tasks that are spawned into the background. They might remain there quite long (if the system is busy), but the diagram only tracks when they are first spawned.</li> \
    </ul>\
    If your events internally are multithreaded, this multithreading is not tracked by the performance analysis. Use a real performance analysis tool to get statements on your actual core usage. \
    </p>\
    <i>Performance hint: </i>\
    <p>\
      If your real concurrency falls below one, there has to be some IO or OS swapping that should be removed. \
      If your real concurrency level falls below the number of cores available, your multicore usage is poor. Try to improve concurrency by reducing grain sizes in the oracle, e.g. \
    </p>\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")
  if (numberOfRanks>0):      
    pass
    
    

if (numberOfRanks>0):      
  #
  # Logical topology  
  #
  outFile.write( "\
    <h2 id=\"logical-topology\">Logical topology</h2>\
    <img src=\"" + outputFileName + ".topology.png\" />\
    <br /><br />\
    <p>\
    A bigger version of the file can be found \
    <a href=\"" + outputFileName + ".topology.large.png\">here</a>. \
    </p>\
    <i>Performance hint: </i>\
    <p>\
    If the maximal number of working ranks is significantly smaller than the total number of ranks, your application might \
    have ran into a weak scaling issue. The problem then was too small. If the problem is sufficiently big, and the number \
    if idle ranks still remains high, it might had happened that Peano was not able to fork on a coarse enough level. \
    See the remarks on MPI tuning in Peano's quick start guide discussing multiscale concurrency. In such a case, it often helps \
    to make the computational grid more regular to some degree, i.e. to prescribe a finer maximum mesh size. \
    </p>\
    <p>\
    <i>Performance hint: </i></p><p>\
    If you find this graph to become a multigraph in many places, lots of joins and forks are conducted. \
    Joins and forks are expensive operations in terms of walltime. Evaluating the load balancing \
    information also is not for free. Hence, try to reduce the number of joins and forks as you \
    switch on rebalancing only from time to time, and reduce the load balancing overhead. See \
    the section on 'Disable load balancing' in Peano's quick start guide. \
    </p>\
    <p>\
    <i>Performance hint:</i></p><p>\
    Peano scales best if the coarsest grids are decomposed. In turn, it performs good if the logical topology tree is not too \
    wide, i.e. if it is deep and its breadth is bounded by 3^d you typically have a good performance. If your tree is shallow, \
    few ranks grab all the workers which is often not a good sign. Often, such an unfair behaviour goes hand in hand with a \
    monotonic decrease of cells on one worker. See discussion in the Section 'Global grid overview'. \
    Also consult the notes on multiscale concurrency in Peano's quick start guide. The latter also discusses how to manually enforce a higher coarse grid \
    regularity and thus allow the load balancing to fork. \
    </p>\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")


  #
  # Fork and join statistics
  #
  outFile.write( "\
    <h2 id=\"fork-join-statistics\">Fork and join statistics</h2>\
    <img src=\"" + outputFileName + ".fork-join-statistics.png\" />\
    <br /><br />\
    <p>\
    The statistics use the node's timers. If the timers of nodes ran with MPI \
    are slightly non-synchronised or many forks and joins happen at the same time, \
    you might see vertical zig-zag patterns. They are not errors but measurement inaccuracies. \
    </p>\
    <i>Performance hint: </i>\
    <p>\
    Joins and forks are expensive operations in terms of walltime. Evaluating the load balancing \
    information also is not for free. Hence, try to reduce the number of joins and forks as you \
    switch on rebalancing only from time to time, and reduce the load balancing overhead. See \
    the section on load balancing disabling in Peano's quick start guide. \
    </p>\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")


  #
  # Fork history
  #
  outFile.write( "<h2 id=\"fork-history\">Fork/join history/histograms</h2>" )
  extractForkHistory()
  outFile.write( " \
    <p>\
    The table above is an overview over all forks and joins. It allows you to explicitly keep track of these repartitionings and to interpret other performance data. \
    </p>\
    <i>Performance hint: </i>\
    <p>\
    Most Peano applications suffer if the forks are not conducted on the coarsest grid levels. If you run into a red (empty) level above, the code does not fork on a \
    particular level while finer levels then continue to fork. This will, due to the dfs nature of the grid traversal, induce a serialisation of the code. Try to fork \
    more aggressively on coarser levels by using a more regular grid for the coarse resolutions, e.g. \
    </p>\
    <i>Performance hint: </i>\
    <p>\
    Master-worker communication is very critical for the runtime as this information is exchanged synchronously. If some columns in the first table above holds only \
    one entry, you should have a very shallow topology (cf. topology graph) and thus one master for many ranks. This is typically not a good idea as this master then \
    easily becomes a bottleneck. Try to distribute the master job (by forking on coarser levels, e.g.). \
    </p>\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")


  #
  # MPI Phases
  # 
  outFile.write( "\
    <h2 id=\"mpi-phases\">MPI Phases</h2>\
    <a href=\"" + outputFileName + ".mpi-phases.large.png\"> <img src=\"" + outputFileName + ".mpi-phases.png\" /> </a> \
    <br /><br />\
    <h3>Legend:</h3>\
    <ul>\
    <li>x-axis: simulation time</li>\
    <li>y-axis: ranks</li>\
    <li>vertical black bars: end of iteration on master rank</li>\
    <li>bright red: time spent outside of the domain prior to enter the actual local tree; comprises the time the node has to wait for data from the master if master-worker data exchange is not switched off.</li>\
    <li>green: time spent within local tree</li>\
    <li>dark red: time spent outside of the domain after local tree has been processed; comprises the time the node has to wait until data is successfully sent to the master if worker-master data exchange is not switched off. Usually this effect however is negligible as MPI buffers away the blocking messages</li>\
    <li>dark red on master: time spent to wait for first worker</li>\
    <li>blue: time required to exchange heap and boundary data; the heap data usually is for free as it overlaps into the subsequent traversal</li>\
    <li>white: idle time and time used to exchange boundary and heap data</li>\
    </ul>\
    <i>Visualisation remarks/performance hints: </i>\
    <ul>\
    <li>The sampling accuracy is low, i.e. if your code has a very low \
    runtime per traversal ratio, the measurements become inaccurate. </li>\
    <li>If you switch off reduction (workers do not send data back to their master), the end iteration bar is inserted though workers still might be working on the traversal.</li>\
    <li>If long green blocks assigned to nodes without any further workers introduce a critical path, your problem is ill-balanced, i.e. some ranks have signficiantly more work to do than others. You might want to use more ranks to give the load balancing more degrees of freedom to balance.</li>\
    <li>If all ranks suffer from long light red blocks, your setup suffers from latency. Ranks wait quite a while for their masters to give them a go. If you run a sequence of multiple sweeps in a row, you might be able map them onto one iterate. Skipping master-worker data exchange also might help.</li>\
    <li>If large dark redish blocks introduce your critical path, your problem either is decomposed into too small chunks. Reduce number of forks/ranks. Or you have to search for ways to skip worker-master data exchange.</li>\
    <li>If all ranks have large blue blocks that all end at the same time, then you have a very tight coupling of the ranks through boundary data. Change your bounding box, e.g. (see remarks in guide book on boundary data skips).</li>\
    <li>If the green bars are significantly smaller than the other bars, your problem is too small for this number of ranks. All your time is spent on communication.</li>\
    </ul>\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")


  #
  # Master-worker data exchange. In particular late senders
  #
  outFile.write( "\
    <h2 id=\"master-worker-data-exchange\">Master-worker data exchange</h2>\
    <p>If an edge points from a to b, it means that master b had to wait for its worker a. The labels are wait times in seconds. </p>\
    <a href=\"" + outputFileName + ".master-worker-data-exchange.large.png\" /><img src=\"" + outputFileName + ".master-worker-data-exchange.png\" /></a>\
    <p>The following graph holds only edges whose average is beyond the average of averages.</p>\
    <a href=\"" + outputFileName + ".master-worker-data-exchange.sparse-average.large.png\" /><img src=\"" + outputFileName + ".master-worker-data-exchange.sparse-average.png\" /></a>\
    <p>The following graph holds only edges whose maximum weight is with 10% of the total maximum weight.</p>\
    <a href=\"" + outputFileName + ".master-worker-data-exchange.sparse-max.large.png\" /><img src=\"" + outputFileName + ".master-worker-data-exchange.sparse-max.png\" /></a>\
    ")

  # This is one of the reasons why we have to generate the plots first    
  if  GlobalSynchronisationOnRank0:
    outFile.write( "<table bgcolor=\"#ff0000\"><tr><td>Attention: Code might suffer from strong synchronisation through rank 0.</td></tr></table>" ) 
  
  outFile.write( "\
    <br /><br />\
    <p>\
    If you obtain a warning no strong synchronisation with the master above but switched off all/most synchronisation with the master \
    please ignore the warning. In this case, it is natural that the global master waits for the workers to finish at some points. It \
    then is not an inefficiency pattern but an efficiency validation. \
    </p>\
    <i>Performance hints: </i>\
    <p>\
    If you use purely administrative ranks, i.e. ranks that hold only one cell of the spacetree and deploy all children to other ranks, such ranks always should have incoming \
    edges from all their children in the plot. \
    </p>\
    <p>\
    The edges here illustrate the critical communication path for one traversal, i.e. long graphs running from a node to rank 0 indicate \
    that along these graphs the Peano traversal is serialised. Eliminate these edges by reducing the \
    workload of the involved nodes. See remark in Peano's quick start guide on the optimisation of the worker-master communication or \
    reduction avoiding algorithms. \
    </p>\
    <p>\
    If you have late workers that you cannot explain, also try to scale up the problem. Late \
    workers often are an indicator for classic strong scaling issues, i.e. too small problem sizes. \
    </p>\
    <p>\
    If rank 0 synchronises all other ranks (see attention), try to reduce the synchronisation according to guide book. \
    </p>\
    <p>\
    If rank 0 continues to synchronises all other ranks and your MPI phases exhibit lots of dark red areas, your rank 0 might participate in too many boundary exchanges. Consult guide book how to adopt the domain boundaries accordingly. \
    </p>\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")



  #
  # Boundary data exchange. In particular late senders
  #
  outFile.write( "\
    <h2 id=\"boundary-data-exchange\">Boundary data exchange</h2>\
    <p>\
    The follow diagrams display an edge whenever a node A had to wait for a neighbouring node B. \
    The first entry of the label counters how often it had to work, the second the maximum data \
    cardinality, i.e. vertices, and the last one the average wait cardinality. If A points to B, \
    B hat to wait for data coming from A. The directed edges illustrate the data flow. As mentioned, \
    all figures enlist data cardinalities. They are not wait times but number of records missing. \
    </p>\
    <p>\
    In all diagrams, singular events, i.e. events waits only once or twice, are omitted.\
    </p>\
    <a href=\"" + outputFileName + ".boundary-data-exchange.large.png\" /><img src=\"" + outputFileName + ".boundary-data-exchange.png\" /></a>\
    <p>The following graph holds only edges whose average is beyond the average of averages.</p>\
    <a href=\"" + outputFileName + ".boundary-data-exchange.sparse-average.large.png\" /><img src=\"" + outputFileName + ".boundary-data-exchange.sparse-average.png\" /></a>\
    <p>The following graph holds only edges whose maximum weight is with 10% of the total maximum weight.</p>\
    <a href=\"" + outputFileName + ".boundary-data-exchange.sparse-max.large.png\" /><img src=\"" + outputFileName + ".boundary-data-exchange.sparse-max.png\" /></a>\
    ")

  outFile.write( "\
    <br /><br />\
    <i>Performance hint: </i>\
    <p>\
    Peano is designed to exchange all boundary data in the background of the computation. \
    In the ideal case, the above graph hence should be empty or very sparse. \
    If  it is pretty dense, your algorithm is bandwidth-bound. This is not a problem \
    per se, if the bandwidth-bound nodes do not slow down their masters, i.e. \
    the diagrams above have to be read in context with the worker-master graphs. \
    </p>\
    <p>\
    The global rank 0 should not have adjacent edges. If it has, ensure you've followed the avoid communications \
    with rank 0 recipes from Peano's quick start guide. \
    </p>\
    <p>\
    If this graph is a clique, your mpi buffer sizes might be too small or too big. Cf. \
    SendReceiveBufferPool's setBufferSize(). Its default values might be ill-suited for your \
    application's memory footprint. \
    </p>\
    <p>\
    If single nodes are the hot-spots making the others wait (many outgoing edges), those nodes might either have \
    a significantly higher load than others (cf. balancing remark above) or they might have a significant higher surface to \
    other nodes that has to be exchanged. Adopt load balancing and check the load balancing \
    as discussed in Peano's quick start guide. \
    </p>\
    <p>\
    If edges point from workers to their direct parents (cf. logical topology), the local master-worker balancing might be ill-balanced. Such \
    edges are in the first place not that important - any node might have a severe workload where it can only deploy \
    smaller parts of the work. The involved worker then has, by definition, to be smaller than its master (it got only \
    a fragment of the total work) and then waits for the master's boundary data for the subsequent iteration. A \
    critical situation (from a performance point of view) would occur, if the master had to wait for the worker's \
    finished message. Cf. Master-worker statistics above. \
    </p>\
    <p>\
    If nodes delay their masters but have no significant heavy edges in the boundary graph, study their individual runtime \
    profile carefully. If these profiles also indicate that the data exchange is not signficiant, your worker-master \
    data exchange suffers from worker-master latency. See Peano's quick start guide on the optimisation of worker-master communication. \
    </p><br /><br />\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")


  #
  # Boundary data exchange. In particular late senders
  #
  outFile.write( "\
    <h2 id=\"individual-ranks\">Individual ranks</h2>\
    <p>\
      Each Peano traversal consists of the following phases: \
      <ul> \
      <li> \
        <i>Busy time</i>: This is the time the node is actually traversing the tree, i.e. \
        the time between a start message received from its master and the time until the finished \
        message is sent back to the master. Busy times also comprise message exchanges, but these \
        message exchanges (boundary, load balancing, and so forth) run parallel to or are merged \
        into the grid traversal. If the workload is homogeneous (i.e. roughly the same number of \
        operations per spacetree node or at least leaf), the busy time usually directly correlates \
        to the number of cells, i.e. to the workload distribution. \
        The only fragments of the busy time that are illustrated separately is the time consumed by \
        the heap data exchange as well as the accumulated time a rank has to wait for the . \
      </li> \
      <li> \
        Definition <i>Work within domain</i>: Part of the busy time where the inner domain's bounding \
        box is traversed and not the embedding cells. Peano embeds all domains into a greater box to \
        simplify the boundary handling. This means, a traversal has to run through the actual domain \
        (work within domain time) and to run through boundary/shadow layers. Busy time comprises the \
        whole run while work within domain is only a subset. \
      </li> \
      <li> \
        Definition <i>Communication time</i>: As soon as a node has sent the information back to the master \
        that it has finished its traversal, it starts to clean up all the pending sends of boundary \
        data to the neighbours, receives the boundary data from all adjacent ranks, switches local \
        data structures, and so forth. It prepares all the data for the subsequent iteration. The \
        communication time comprises the \
        <ul> \
          <li> exchange of <i>join</i> data and the </li>  \
          <li> exchange of <i>boundary</i> vertices. </li> \
        </ul> \
      </li> \
      <li> \
        Definition <i>Idle time</i>: As soon as all data is communicated, the rank \
        waits for the next startup message from its master. This waiting \
        time is real idle time where nothing is done. \
      </li> \
    </p><br /><br />\
    <a href=\"#table-of-contents\">To table of contents</a>\
    " )

  for rank in range(0,numberOfRanks):
    outFile.write( "<a href=\"#runtime-rank-" + str(rank) + "\">Profile of rank " + str(rank) + "</a> - " )


  for rank in range(0,numberOfRanks):
    outFile.write( "<h3 id=\"runtime-rank-" + str(rank) + "\">Profile of rank " + str(rank) + "</h2>")
    createRankDetails(rank)  
    outFile.write( "\
      <img src=\"" + outputFileName + ".walltime-rank-" + str(rank) + ".png\" />\
      <img src=\"" + outputFileName + ".local-cells-rank-" + str(rank) + ".png\" />\
      <img src=\"" + outputFileName + ".runtime-profile-calendar-rank-" + str(rank) + ".png\" />\
      <img src=\"" + outputFileName + ".runtime-profile-cpu-rank-" + str(rank) + ".png\" />\
    <br /><br />\
    <a href=\"#individual-ranks\">To rank overview</a>\
    <br /><br />\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")


#
# Trailor of report
#
outFile.write( "</body>" )
outFile.write( "</html>" )
outFile.close()
print "html file written"



#
# Create individual plots - lasts long so I prefer to finish writing the HTML 
# file first.
#
if (numberOfRanks>0):      
  print "continue to write individual rank statistics ... html file however should be readable already"
  for rank in range(0,numberOfRanks):
    print "plot statistics for rank " + str(rank)
    plotStatisticsForRank(rank)
