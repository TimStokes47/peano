import sys
import re
import pylab
import networkx 




searchPatternNumberOfCells         = ".*peano::performanceanalysis::DefaultAnalyser::endIteration.*cells=\("
searchPatternNumberOfLocalVertices = ".*peano::performanceanalysis::DefaultAnalyser::endIteration.*local-vertices="



numberOfInnerLeafCells  = {}
numberOfInnerCells      = {}
numberOfOuterLeafCells  = {}
numberOfOuterCells      = {}
numberOfLocalCells      = {}
numberOfLocalVertices   = {}
  
  
  
#
# Display constants. Might be reset later
#
AlphaValue = 1.0



def parseInputFile():
  #data = {k: [] for k in range(2)}

  print "prepare data structures",
  for rank in range(0,numberOfRanks):
    numberOfInnerLeafCells[rank]  = []
    numberOfInnerCells[rank]      = []
    numberOfOuterLeafCells[rank]  = []
    numberOfOuterCells[rank]      = []
    numberOfLocalCells[rank]      = []
    numberOfLocalVertices[rank]   = []
    print ".",
  print " done"
   
  try:
    inputFile = open( inputFileName,  "r" )
    print "parse ",
    for line in inputFile:
      for rank in range(0,numberOfRanks):
        if ("DefaultAnalyser" in line):
          m = re.search( "rank:" + str(rank) + " +" + searchPatternNumberOfCells, line )
          if (m):
            token = line.replace("(",",").replace(")","").strip().split(",")
            numberOfInnerLeafCells[rank].append(float(token[-5]))
            numberOfOuterLeafCells[rank].append(float(token[-4]))
            numberOfInnerCells[rank].append(float(token[-3]))
            numberOfOuterCells[rank].append(float(token[-2]))
            numberOfLocalCells[rank].append(float(token[-1]))
            print ".",
          m = re.search( "rank:" + str(rank) + " +" + searchPatternNumberOfLocalVertices, line )
          if (m):
            token = line.strip().split("=")
            numberOfLocalVertices[rank].append(float(token[-1]))
            print ".",
          
    print " done"
  except Exception as inst:
    print "failed to read " + inputFileName
    print inst



def plotForkJoinStatistics():
  numberOfWorkingNodes = []
  numberOfIdleNodes    = []
  numberOfForks        = []
  numberOfJoins        = []
  numberOfWorkingNodes.append(1)
  numberOfIdleNodes.append(numberOfRanks-1)
  numberOfForks.append(0)
  numberOfJoins.append(0)
  try:
    inputFile = open( inputFileName,  "r" )
    print "parse fork/join statistics",
    for line in inputFile:
      searchPatternFork    = "peano::performanceanalysis::DefaultAnalyser::addWorker"
      searchPatternJoin    = "peano::performanceanalysis::DefaultAnalyser::removeWorker"
      searchEndIteration   = "rank:0 .*peano::performanceanalysis::DefaultAnalyser::endIteration"
      m = re.search( searchPatternFork, line )
      if (m):
        numberOfWorkingNodes.append(numberOfWorkingNodes[-1]+1)
        numberOfIdleNodes.append(numberOfIdleNodes[-1]-1)
        numberOfForks.append(numberOfForks[-1]+1)
        numberOfJoins.append(numberOfJoins[-1]+0)
      m = re.search( searchPatternJoin, line )
      if (m):
        numberOfWorkingNodes.append(numberOfWorkingNodes[-1]-1)
        numberOfIdleNodes.append(numberOfIdleNodes[-1]+1)
        numberOfForks.append(numberOfForks[-1]+0)
        numberOfJoins.append(numberOfJoins[-1]+1)
      m = re.search( searchEndIteration, line )
      if (m):
        numberOfWorkingNodes.append(numberOfWorkingNodes[-1])
        numberOfIdleNodes.append(numberOfIdleNodes[-1])
        numberOfForks.append(numberOfForks[-1])
        numberOfJoins.append(numberOfJoins[-1])
    print " done"
  except Exception as inst:
    print "failed to read " + inputFileName
    print inst
  
  
  pylab.clf()
  pylab.title( "Global cells" )
  x = pylab.arange(0, len(numberOfWorkingNodes), 1.0)
  pylab.plot(x, numberOfWorkingNodes, 'o-',  markersize=10, color='#ffaa00', label='working nodes' )
  pylab.plot(x, numberOfIdleNodes,    '+-',  markersize=10, color='#00ffaa', label='idle nodes' )
  pylab.plot(x, numberOfForks,        '.-',  markersize=10, color='#aa00ff', label='total forks' )
  pylab.plot(x, numberOfJoins,        'x-',  markersize=10, color='#ff00aa', label='total joins' )
  setGeneralPlotSettings()
  pylab.savefig( outputFileName + ".fork-join-statistics.png" )
  pylab.savefig( outputFileName + ".fork-join-statistics.pdf" )


def plotLogicalTopology():
  topologyGraph = networkx.MultiDiGraph()
  for rank in range(0,numberOfRanks):
    topologyGraph.add_node(str(rank))
    #topologyGraph.node[rank]['data']  = True
    #topologyGraph.node[rank]['label']  = str(rank)
  try:
    inputFile = open( inputFileName,  "r" )
    print "parse topology",
    for line in inputFile:
      searchPatternAddFork    = "peano::performanceanalysis::DefaultAnalyser::addWorker.*\d+->\d+\+\d+"
      searchPatternAddJoin    = "peano::performanceanalysis::DefaultAnalyser::removeWorker.*\d+\+\d+->d+"
      if ("DefaultAnalyser" in line):
          m = re.search( searchPatternAddFork, line )
          if (m):
            parent = m.group(0).split("->")[0].split(" ")[-1] 
            child  = m.group(0).split("+")[-1].split(" ")[-1]
            topologyGraph.add_edge(child,parent)
          m = re.search( searchPatternAddJoin, line )
          if (m):
            child  = m.group(0).split("+")[1].split("->")[0] 
            parent = m.group(0).split("->")[-1] 
            topologyGraph.add_edge(child,parent)
    print " done"
  except Exception as inst:
    print "failed to read " + inputFileName
    print inst
  pylab.clf()
  pylab.title( "Logical topology" )
  #pos = networkx.draw_circular(topologyGraph)
  #pos = networkx.draw_circular(topologyGraph)
  #pos = networkx.spring_layout(topologyGraph)
  #networkx.draw_networkx_nodes(
  #  topologyGraph,pos,
  #  node_color='#667766',
  #  node_size=10,
  #  alpha=0.2
  #)
  #networkx.draw_networkx_edges(topologyGraph,pos,
  #  alpha=0.2
  #)
  #networkx.draw_networkx_labels(topologyGraph,pos,
  #  font_size=16
  #)
  #networkx.draw_networkx(topologyGraph,with_labels=True)
  networkx.draw_networkx(
    topologyGraph,
    with_labels=True,
    node_color='#667766',
    node_size=10,
    alpha=0.2
  )
  pylab.savefig( outputFileName + ".topology.png" )
  pylab.savefig( outputFileName + ".topology.pdf" )
  DefaultSize = pylab.gcf().get_size_inches()
  pylab.gcf().set_size_inches( (DefaultSize[0]*10, DefaultSize[1]*10) )
  pylab.savefig( outputFileName + ".topology.large.png" )
  pylab.savefig( outputFileName + ".topology.large.pdf" )
  DefaultSize = pylab.gcf().get_size_inches()
  pylab.gcf().set_size_inches( (DefaultSize[0]/10, DefaultSize[1]/10) )


def setGeneralPlotSettings():
  pylab.xlabel('t')
  pylab.grid(True)
  try:
    pylab.legend(fontsize=9, framealpha=0.5)
  except:
    # old pylab version
    l = pylab.legend(prop={'size':9})
    l.get_frame().set_alpha(0.5)



def plotGlobalGridOverview():
  pylab.clf()
  pylab.title( "Global cells" )
  x = pylab.arange(0, len(numberOfInnerLeafCells[0]), 1.0)
  pylab.plot(x, numberOfInnerLeafCells[0], 'o-',  markersize=10, color='r', label='#inner leaf cells' )
  pylab.plot(x, numberOfOuterLeafCells[0], '+-',  markersize=10, color='g', label='#outer leaf cells' )
  pylab.plot(x, numberOfInnerCells[0],     '.-',  markersize=10, color='r', label='#inner cells' )
  pylab.plot(x, numberOfOuterCells[0],     'x-',  markersize=10, color='g', label='#outer cells' )
  setGeneralPlotSettings()
  pylab.savefig( outputFileName + ".grid-overview.png" )
  pylab.savefig( outputFileName + ".grid-overview.pdf" )

  pylab.clf()
  pylab.title( "Local cells" )
  startRank = 1
  if (numberOfRanks==1):
    startRank = 0
  for rank in range(startRank,numberOfRanks):
    x = pylab.arange(0, len(numberOfLocalCells[rank]), 1.0)
    pylab.plot(x, numberOfLocalCells[rank], 'o',  color='#000000', alpha=AlphaValue, markersize=10)
  pylab.xlabel('t')
  pylab.savefig( outputFileName + ".local-cells.png" )
  pylab.savefig( outputFileName + ".local-cells.pdf" )

  pylab.clf()
  pylab.title( "Local vertices" )
  startRank = 1
  if (numberOfRanks==1):
    startRank = 0
  for rank in range(startRank,numberOfRanks):
    x = pylab.arange(0, len(numberOfLocalVertices[rank]), 1.0)
    pylab.plot(x, numberOfLocalVertices[rank], 'o',  color='#000000', alpha=AlphaValue, markersize=10)
  pylab.xlabel('t')
  pylab.savefig( outputFileName + ".local-vertices.png" )
  pylab.savefig( outputFileName + ".local-vertices.pdf" )



#
# main
#
if ( len(sys.argv)<=2 ):
  print "please specify input file name and number of ranks"
else:
  inputFileName   = sys.argv[1]
  outputFileName  = sys.argv[1] + ".html"
  numberOfRanks   = int(sys.argv[2])

  outFile        = open( outputFileName, "w" )
  
  AlphaValue     = 1.0/numberOfRanks
  if (AlphaValue<0.01):
    AlphaValue   = 0.01
  
  print "read input file " + inputFileName + " for " + str(numberOfRanks) + " rank(s)"
  parseInputFile()
  
  #
  # Header of report
  #
  print "write report header"
  outFile.write( 
    "<html>\
       <body>\
       <h1>Peano Performance Report</h1>\
       <p>Peano's default performance analysis</p>\
       <p>Author: Tobias Weinzierl</p>\
       <p>Data file: " + inputFileName + "</p>\
       <p>Ranks: " + str(numberOfRanks) + "</p>\
       <center><img src=\"http://www.peano-framework.org/logo.png\" />\
       <br /><a href=\"http://www.peano-framework.org\" >www.peano-framework.org</a></center>\
    ")

  outFile.write( "\
     <h2 id=\"table-of-contents\">Table of contents</h2>\
     <ul>\
       <li><a href=\"#global-grid-overview\">Global grid overview</a></li>\
       <li><a href=\"#logical-topology\">Logical topology</a></li>\
       <li><a href=\"#fork-join-statistics\">Fork and join statistics</a></li>\
     </ul>\
   ")


  #
  # Grid information
  #
  print "plot global grid overview"
  plotGlobalGridOverview()
  outFile.write( "\
    <h2 id=\"global-grid-overview\">Global grid overview</h2>\
    <img src=\"" + outputFileName + ".grid-overview.png\" />\
    <img src=\"" + outputFileName + ".local-cells.png\" />\
    <img src=\"" + outputFileName + ".local-vertices.png\" />\
    <br /><br />\
    <p>\
    The darker the plots, the more ranks exhibit a certain characteristics. \
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
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")

  #
  # Grid information
  #
  print "plot logical grid topology"
  plotLogicalTopology()
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
    See the section on MPI tuning in the <a href=\"http://sourceforge.net/p/peano/wiki\" target=\"_blank\">wiki</a> that discusses multiscale concurrency. In such a case, it often helps \
    to make the computational grid more regular to some degree, i.e. to prescribe a finer maximum mesh size. \
    </p>\
    <p>\
    <i>Performance hint: </i>\
    If you find this graph to become a multigraph in many places, lots of joins and forks are conducted. \
    Joins and forks are expensive operations in terms of walltime. Evaluating the load balancing \
    information also is not for free. Hence, try to reduce the number of joins and forks as you \
    switch on rebalancing only from time to time, and reduce the load balancing overhead. See \
    the section on 'Disable load balancing' in the <a href=\"http://sourceforge.net/p/peano/wiki\" target=\"_blank\">wiki</a>. \
    </p>\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")


  #
  # Fork and join statistics
  #
  print "extract fork and join statistics"
  plotForkJoinStatistics()
  outFile.write( "\
    <h2 id=\"fork-join-statistics\">Fork and join statistics</h2>\
    <img src=\"" + outputFileName + ".fork-join-statistics.png\" />\
    <br /><br />\
    <i>Performance hint: </i>\
    <p>\
    Joins and forks are expensive operations in terms of walltime. Evaluating the load balancing \
    information also is not for free. Hence, try to reduce the number of joins and forks as you \
    switch on rebalancing only from time to time, and reduce the load balancing overhead. See \
    the section on 'Disable load balancing' in the <a href=\"http://sourceforge.net/p/peano/wiki\" target=\"_blank\">wiki</a>. \
    </p>\
    <a href=\"#table-of-contents\">To table of contents</a>\
    ")

  #
  # Trailor of report
  #
  print "write report trailer"
  outFile.write( "</body>" )
  outFile.write( "</html>" )
  outFile.close()
  