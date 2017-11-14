import sys
import re

import matplotlib

import pylab
import networkx 

from mpl_toolkits.mplot3d import Axes3D









#
# Use these pairs also for sender and receiver
#
class Pair:
  def __init__(self, master, worker):
    self.master         = master
    self.worker         = worker
    self.count          = 0
    self.maxTime        = 0.0
    self.average        = 0
    self.maxCardinality = 0











#
# Creates the one big trace picture where we see what different ranks do at 
# different times
#
# @writes A file inputFileName.mpi-phases.large.png
# @writes A file inputFileName.mpi-phases.large.pdf
# @writes A file inputFileName.mpi-phases.png
# @writes A file inputFileName.mpi-phases.pdf
#
#
def plotMPIPhases(numberOfRanks,inputFileName,plotDirectoryName):
  beforeInTraversalColor  = "#ff3434"
  inTraversalColor        = "#00ab00"
  afterInTraversalColor   = "#560000"
  afterBoundaryExchange   = "#0000ab"
  prepareAsynchronousBoundaryExchangeColor = "#ffff00"
  releaseAsynchronousBoundaryExchangeColor = "#abab00"

  pylab.clf()
  pylab.gcf().set_size_inches( DefaultSize[0]*4, DefaultSize[1] )
  pylab.title( "MPI phases overview" )
  ax = pylab.gca()
  
  timeStampPattern = "([0-9]+\.?[0-9]*)"
  floatPattern = "([0-9]\.?[0-9]*)"
  
  beginIterationPattern      = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::beginIteration"
  enterCentralElementPattern = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::enterCentralElementOfEnclosingSpacetree"
  leaveCentralElementPattern = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::leaveCentralElementOfEnclosingSpacetree.*t_central-tree-traversal=\(" + floatPattern
  endIterationPattern        = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::endIteration.*t_traversal=\(" + floatPattern
  endDataExchange            = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::endReleaseOfBoundaryData"
  prepareAsynchronousHeap    = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::endToPrepareAsynchronousHeapDataExchange"
  releaseAsynchronousHeap    = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::endToReleaseSynchronousHeapData"

  lastTimeStamp  = [0] * numberOfRanks

  def plotMPIPhasesBar(rank,timeStamp,color):
    if (lastTimeStamp[rank]==0):
      lastTimeStamp[rank] = timeStamp
    rectLength = timeStamp-lastTimeStamp[rank]
    rect = pylab.Rectangle([lastTimeStamp[rank],rank-0.5],rectLength,1,facecolor=color,edgecolor=color,alpha=Alpha)
    ax.add_patch(rect)
    lastTimeStamp[rank] = lastTimeStamp[rank] + rectLength
  
  Alpha = 0.5
  
  try:
    inputFile = open( inputFileName,  "r" )
    print "parse mpi phases",
    for line in inputFile:
      m = re.search( beginIterationPattern, line )
      if (m):
        rank = int( m.group(2) )
        timeStamp = float( m.group(1) )
        lastTimeStamp[rank] = timeStamp
        print ".",
        if (rank==0):
          pylab.plot((timeStamp, timeStamp), (-0.5, numberOfRanks+1), '--', color="#445544", alpha=Alpha)
        pylab.plot((timeStamp, timeStamp), (rank-0.5, rank+0.5), '-', color="#000000" )
      m = re.search( prepareAsynchronousHeap, line )
      if (m):
        rank = int( m.group(2) )
        timeStamp = float( m.group(1) )
        plotMPIPhasesBar(rank,timeStamp,prepareAsynchronousBoundaryExchangeColor)
      m = re.search( releaseAsynchronousHeap, line )
      if (m):
        rank = int( m.group(2) )
        timeStamp = float( m.group(1) )
        plotMPIPhasesBar(rank,timeStamp,releaseAsynchronousBoundaryExchangeColor)
      m = re.search( enterCentralElementPattern, line )
      if (m):
        rank = int( m.group(2) )
        timeStamp = float( m.group(1) )
        plotMPIPhasesBar(rank,timeStamp,beforeInTraversalColor)
      m = re.search( leaveCentralElementPattern, line )
      if (m):
        rank = int( m.group(2) )
        timeStamp = float( m.group(1) )
        plotMPIPhasesBar(rank,timeStamp,inTraversalColor)
      m = re.search( endIterationPattern, line )
      if (m):
        rank = int( m.group(2) )
        timeStamp = float( m.group(1) )
        plotMPIPhasesBar(rank,timeStamp,afterInTraversalColor)
      m = re.search( endDataExchange, line )
      if (m):
        rank = int( m.group(2) )
        timeStamp = float( m.group(1) )
        plotMPIPhasesBar(rank,timeStamp,afterBoundaryExchange)

    print " done"
  except Exception as inst:
    print "failed to read " + inputFileName
    print inst
  
  ax.invert_yaxis()
  ax.autoscale_view()
  pylab.xlabel('t')
  pylab.grid(False)
  plotPrefix = plotDirectoryName + "/" + inputFileName
  pylab.savefig( plotPrefix + ".mpi-phases.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( plotPrefix + ".mpi-phases.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  try:
    switchToLargePlot()
    if numberOfRanks<=16:
      pylab.yticks([i for i in range(0,numberOfRanks)]) 
    else:
      pylab.yticks([i*16 for i in range(0,numberOfRanks/16)]) 
    pylab.savefig( plotPrefix + ".mpi-phases.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".mpi-phases.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
    switchBackToStandardPlot()  
  except:
    print "ERROR: failed to generated large-scale plot"
    switchBackToStandardPlot()  
    pylab.savefig( plotPrefix + ".mpi-phases.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".mpi-phases.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )




  

def plotBoundaryLateSends(numberOfRanks,inputFileName,plotDirectoryName):
  pairs = dict()
  totalMaxCardinality     = 0
  totalCount              = 0
  totalAverage            = 0
  waitingForNeighborLine  = "rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::dataWasNotReceivedInBackground.*rank had to wait for (\d+) record\(s\)" \
                          + " from (\d+) on tag (\d) with page size (\d+)"

  print "search for late boundary answers ...",
  try:
    inputFile = open( inputFileName,  "r" )
    for line in inputFile:
      m = re.search( waitingForNeighborLine, line )
      if (m):
        master = int(m.group(1))
        worker = int(m.group(3))
        key = (master, worker)
        
        if not pairs.has_key(key):
          pairs[key] = Pair(master, worker)
        pair = pairs[key]
        
        cardinality = int(m.group(1))
        pair.count += 1
        totalCount += 1 
     
        pair.maxCardinality = max(pair.maxCardinality, cardinality)
        totalMaxCardinality = max(totalMaxCardinality, cardinality)

        pair.average += cardinality
        totalAverage += cardinality

    print "boundary analysis found " + str( len(pairs) ) + " entries of interest in trace file" 
  except Exception as inst:
    print "failed to read " + inputFileName
    print inst
  
  
  graph              = networkx.DiGraph()
  sparseAverageGraph = networkx.DiGraph()
  sparseMaxGraph     = networkx.DiGraph()
  #for rank in range(0,numberOfRanks):
  #  graph.add_node(str(rank))
  #  sparseAverageGraph.add_node(str(rank))
  #  sparseMaxGraph.add_node(str(rank))
  for master in range(0,numberOfRanks):
    for worker in range(0,numberOfRanks):
      key = (master, worker)
      if pairs.has_key(key):
        pair = pairs[key]
        if pair.count>2:
          graph.add_edge(str(worker),str(master))
          #edge = pydot.Edge(str(sender),str(receiver), label="(" + str(pair.count) + "," + str(pair.maxCardinality) + "," + str(float(pair.average) / float(pair.count)) + ")", fontsize=str(myfontsize), labelfontcolor="blue" )
        
        if totalCount>0 and pair.count>2 and (float(pair.average) / float(pair.count) > float(totalAverage) / float(totalCount)):
          #edge = pydot.Edge(str(sender),str(receiver), label="(" + str(pair.count) + "," + str(pair.maxCardinality) + "," + str(float(pair.average) / float(pair.count)) + ")", fontsize=str(myfontsize), labelfontcolor="blue" )
          sparseAverageGraph.add_edge(str(worker),str(master))
 
        if pair.count>2 and ( float(pair.maxCardinality) > 0.9 * float(totalMaxCardinality) ):
          #edge = pydot.Edge(str(sender),str(receiver), label="(" + str(pair.count) + "," + str(pair.maxCardinality) + "," + str(float(pair.average) / float(pair.count)) + ")", fontsize=str(myfontsize), labelfontcolor="blue" )
          sparseMaxGraph.add_edge(str(worker),str(master))
  pylab.clf()
  pylab.title( "Late sends from neighbours" )
  drawTreeGraph(graph)
  plotPrefix = plotDirectoryName + "/" + inputFileName
  pylab.savefig( plotPrefix + ".boundary-data-exchange.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( plotPrefix + ".boundary-data-exchange.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  try:
    switchToLargePlot()
    pylab.savefig( plotPrefix + ".boundary-data-exchange.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".boundary-data-exchange.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
    switchBackToStandardPlot()
  except:
    print "ERROR: failed to generated large-scale plot"
    switchBackToStandardPlot()
    pylab.savefig( plotPrefix + ".boundary-data-exchange.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".boundary-data-exchange.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )

  pylab.clf()
  pylab.title( "Late sends from neighbours (only edges with weight more than average)" )
  drawTreeGraph(sparseAverageGraph)
  pylab.savefig( plotPrefix + ".boundary-data-exchange.sparse-average.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( plotPrefix + ".boundary-data-exchange.sparse-average.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  try:
    switchToLargePlot()
    pylab.savefig( plotPrefix + ".boundary-data-exchange.sparse-average.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".boundary-data-exchange.sparse-average.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
    switchBackToStandardPlot()
  except:
    print "ERROR: failed to generated large-scale plot"
    switchBackToStandardPlot()
    pylab.savefig( plotPrefix + ".boundary-data-exchange.sparse-average.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".boundary-data-exchange.sparse-average.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  switchBackToStandardPlot()

  pylab.clf()
  pylab.title( "Late sends from neighbours (only 10% heaviest edges)" )
  drawTreeGraph(sparseMaxGraph)
  pylab.savefig( plotPrefix + ".boundary-data-exchange.sparse-max.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( plotPrefix + ".boundary-data-exchange.sparse-max.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  try:
    switchToLargePlot()
    pylab.savefig( plotPrefix + ".boundary-data-exchange.sparse-max.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".boundary-data-exchange.sparse-max.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
    switchBackToStandardPlot()
  except:
    print "ERROR: failed to generated large-scale plot"
    switchBackToStandardPlot()
    pylab.savefig( plotPrefix + ".boundary-data-exchange.sparse-max.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".boundary-data-exchange.sparse-max.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  switchBackToStandardPlot()


def plotMasterWorkerLateSends(numberOfRanks,inputFileName,plotDirectoryName):
  GlobalSynchronisationOnRank0 = False
  
  #  <a href=\"" + outputFileName + ".master-worker-data-exchange.large.png\" /><img src=\"" + outputFileName + ".master-worker-data-exchange.png\" />\
  pairs = dict()
  totalMaxTime        = 0.0
  totalAverage        = 0
  totalCount          = 0
  floatPattern = "[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?"
  waitingForWorkerLine = "rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::endToReceiveDataFromWorker.+rank had to wait for worker (\d+) for ("+ floatPattern + ")s"

  print "search for late worker answers ",
  try:
    inputFile = open( inputFileName,  "r" )
    print "parse fork/join statistics",
    for line in inputFile:
      m = re.search( waitingForWorkerLine, line )
      if (m):
        master = int(m.group(1))
        worker = int(m.group(2))
        key = (master, worker)
        
        if not pairs.has_key(key):
          pairs[key] = Pair(master, worker)
        pair = pairs[key]
        
        time = float(m.group(3))
     
        pair.maxTime = max(pair.maxTime, time)
        totalMaxTime = max(totalMaxTime, time)
        pair.average += time
        totalAverage += time
        
        pair.count += 1
        totalCount += 1 
    print " found " + str( len(pairs) ) + " entries of interest in trace file" 
  except Exception as inst:
    print "failed to read " + inputFileName
    print inst
  
  
  graph              = networkx.DiGraph()
  sparseAverageGraph = networkx.DiGraph()
  sparseMaxGraph     = networkx.DiGraph()
  #for rank in range(0,numberOfRanks):
  #  graph.add_node(str(rank))
  #  sparseAverageGraph.add_node(str(rank))
  #  sparseMaxGraph.add_node(str(rank))
  for master in range(0,numberOfRanks):
    for worker in range(0,numberOfRanks):
      key = (master, worker)
      if pairs.has_key(key):
        pair = pairs[key]
        if pair.count>2:
          graph.add_edge(str(worker),str(master))
          #edge = pydot.Edge(str(worker),str(master), label="(" + str(pair.count) + "," + str(pair.maxTime) + "," + str(float(pair.average) / float(pair.count)) + ")", fontsize=str(myfontsize), labelfontcolor="blue" )
        
        if totalCount > 0 and pair.count>2 and (float(pair.average) / float(pair.count) > float(totalAverage) / float(totalCount)):
          sparseAverageGraph.add_edge(str(worker),str(master))
          if master==0:
            GlobalSynchronisationOnRank0 = True
            print "detected very strong MPI synchronisation"
          #edge = pydot.Edge(str(worker),str(master), label="(" + str(pair.count) + "," + str(pair.maxTime) + "," + str(float(pair.average) / float(pair.count)) + ")", fontsize=str(myfontsize), labelfontcolor="blue" )
 
        if pair.count>2 and ( float(pair.maxTime) > 0.9 * float(totalMaxTime) ):
          sparseMaxGraph.add_edge(str(worker),str(master))
          if master==0:
            print "detected very strong MPI synchronisation"
            GlobalSynchronisationOnRank0 = True
          #edge = pydot.Edge(str(worker),str(master), label="(" + str(pair.count) + "," + str(pair.maxTime) + "," + str(float(pair.average) / float(pair.count)) + ")", fontsize=str(myfontsize), labelfontcolor="blue" )
          #sparseMaxGraph.add_edge(edge)      
  pylab.clf()
  pylab.title( "Late workers" )
  drawTreeGraph(graph)
  plotPrefix = plotDirectoryName + "/" + inputFileName
  pylab.savefig( plotPrefix + ".master-worker-data-exchange.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( plotPrefix + ".master-worker-data-exchange.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  try:
    switchToLargePlot()
    pylab.savefig( plotPrefix + ".master-worker-data-exchange.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".master-worker-data-exchange.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
    switchBackToStandardPlot()
  except:
    print "ERROR: failed to generated large-scale plot"
    switchBackToStandardPlot()
    pylab.savefig( plotPrefix + ".master-worker-data-exchange.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".master-worker-data-exchange.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )

  pylab.clf()
  pylab.title( "Late workers (only edges with weight more than average)" )
  drawTreeGraph(sparseAverageGraph)
  pylab.savefig( plotPrefix + ".master-worker-data-exchange.sparse-average.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( plotPrefix + ".master-worker-data-exchange.sparse-average.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  try: 
    switchToLargePlot()
    pylab.savefig( plotPrefix + ".master-worker-data-exchange.sparse-average.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".master-worker-data-exchange.sparse-average.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
    switchBackToStandardPlot()
  except:
    print "ERROR: failed to generated large-scale plot"
    switchBackToStandardPlot()
    pylab.savefig( plotPrefix + ".master-worker-data-exchange.sparse-average.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".master-worker-data-exchange.sparse-average.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )

  pylab.clf()
  pylab.title( "Late workers (only 10% heaviest edges)" )
  drawTreeGraph(sparseMaxGraph)
  pylab.savefig( plotPrefix + ".master-worker-data-exchange.sparse-max.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( plotPrefix + ".master-worker-data-exchange.sparse-max.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  try:
    switchToLargePlot()
    pylab.savefig( plotPrefix + ".master-worker-data-exchange.sparse-max.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".master-worker-data-exchange.sparse-max.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
    switchBackToStandardPlot()
  except:
    print "ERROR: failed to generated large-scale plot"
    switchBackToStandardPlot()
    pylab.savefig( plotPrefix + ".master-worker-data-exchange.sparse-max.large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( plotPrefix + ".master-worker-data-exchange.sparse-max.large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )

  return GlobalSynchronisationOnRank0





def plotStatisticsForRank(currentRank,numberOfRanks,inputFileName,plotDirectoryName):
  pylab.clf()
  pylab.gcf().set_size_inches( DefaultSize[0]*4, DefaultSize[1] )
  pylab.title( "Walltime" )
  pylab.plot(tTotal[0], tTraversal[0], '-',  markersize=10, color='#000066', label='time per traversal (global master)' )
  if len(tTotal[currentRank])==len(tTraversal[currentRank]):
    pylab.plot(tTotal[currentRank], tTraversal[currentRank], '-',  markersize=10, color='#550000', label='time per traversal (only inside local domain)' )
  else:
    print "WARNING: tTotal and tTraversal for rank " + str(currentRank) + " do not hold the same number of entries. Input file seems to be corrupted"
  startRank = 1
  if (numberOfRanks==1):
    startRank = 0
  for rank in range(startRank,numberOfRanks):
    if len(tTotal[rank])==len(tTraversal[rank]):
      pylab.plot(tTotal[rank], tTraversal[rank], 'o',  color='r', alpha=AlphaValue, markersize=10)
    else:
      print "WARNING: tTotal and tTraversal for rank " + str(rank) + " do not hold the same number of entries. Input file seems to be corrupted"
  setGeneralPlotSettings()
  plotPrefix = plotDirectoryName + "/" + inputFileName
  pylab.savefig( plotPrefix + ".walltime-rank-" + str(currentRank) + ".png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( plotPrefix + ".walltime-rank-" + str(currentRank) + ".pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )

  pylab.clf()
  pylab.title( "Cells" )
  
  if len(tTotal[currentRank]) == len(numberOfLocalCells[currentRank]):
    pylab.plot(tTotal[currentRank], numberOfLocalCells[currentRank], '-',  markersize=10, color='b', label='local cells' )
  else:
    print "WARNING: arrays tTotal and numberOfLocalCells of rank " + str(currentRank) + " have different sizes. Input file might be corrupted"
  
  startRank = 1
  if (numberOfRanks==1):
    startRank = 0
  for rank in range(startRank,numberOfRanks):
    if len(tTotal[rank]) == len(numberOfLocalCells[rank]):
      pylab.plot(tTotal[rank], numberOfLocalCells[rank], 'o',  color='r', alpha=AlphaValue, markersize=10)
    else:
      print "WARNING: arrays tTotal and numberOfLocalCells of rank " + str(rank) + " have different sizes. Input file might be corrupted"
  setGeneralPlotSettings()
  pylab.savefig( plotPrefix + ".local-cells-rank-" + str(currentRank) + ".png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( plotPrefix + ".local-cells-rank-" + str(currentRank) + ".pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  
  totalTimeCalendar            = []
  joinTimeCalendar             = []
  boundaryTimeCalendar         = []
  synchronousHeapDataCalendar  = []
  asynchronousHeapDataCalendar = []
  centralElementCalendar       = []
  workerWaitTimeCalendar       = []

  totalTimeCPU                 = []
  joinTimeCPU                  = []
  boundaryTimeCPU              = []
  synchronousHeapDataCPU       = []
  asynchronousHeapDataCPU      = []
  centralElementCPU            = []

  ttotalTimeCalendar            = []
  tjoinTimeCalendar             = []
  tboundaryTimeCalendar         = []
  tsynchronousHeapDataCalendar  = []
  tasynchronousHeapDataCalendar = []
  tcentralElementCalendar       = []
  tworkerWaitTimeCalendar       = []

  ttotalTimeCPU                 = []
  tjoinTimeCPU                  = []
  tboundaryTimeCPU              = []
  tsynchronousHeapDataCPU       = []
  tasynchronousHeapDataCPU      = []
  tcentralElementCPU            = []

  try:
    inputFile = open( inputFileName,  "r" )
    for line in inputFile:
      if (re.search( "DefaultAnalyser::endIteration", line ) and re.search( "rank:" + str(currentRank) + " ", line ) and re.search( "t_total", line )):
        totalTimeCalendar.append( float( line.split( "(" )[-1].split(",")[0] ))
        totalTimeCPU.append(      float( line.split( "," )[-1].split(")")[0] ))
        
        ttotalTimeCalendar.append( float( line.strip().split( " " )[0] ))
        ttotalTimeCPU.append(      float( line.strip().split( " " )[0] ))
      if (re.search( "DefaultAnalyser::endReleaseOfJoinData", line ) and re.search( "rank:" + str(currentRank) + " ", line )):
        joinTimeCalendar.append( float( line.split( "=" )[1].split(",")[0] ))
        joinTimeCPU.append(      float( line.split( "=" )[2].strip() ))

        tjoinTimeCalendar.append( float( line.strip().split( " " )[0] ))
        tjoinTimeCPU.append(      float( line.strip().split( " " )[0] ))
      if (re.search( "DefaultAnalyser::endReleaseOfBoundaryData", line ) and re.search( "rank:" + str(currentRank) + " ", line )):
        boundaryTimeCalendar.append( float( line.split( "=" )[-2].split(",")[0] ))
        boundaryTimeCPU.append(      float( line.split( "=" )[-1].strip() ))

        tboundaryTimeCalendar.append( float( line.strip().split( " " )[0] ))
        tboundaryTimeCPU.append(      float( line.strip().split( " " )[0] ))
      if (re.search( "DefaultAnalyser::endToReleaseSynchronousHeapData", line ) and re.search( "rank:" + str(currentRank) + " ", line )):
        synchronousHeapDataCalendar.append( float( line.split( "=" )[-2].split(",")[0] ))      
        synchronousHeapDataCPU.append(      float( line.split( "=" )[-1].strip() ))

        tsynchronousHeapDataCalendar.append( float( line.strip().split( " " )[0] )    )  
        tsynchronousHeapDataCPU.append(      float( line.strip().split( " " )[0] ))
      if (re.search( "DefaultAnalyser::endToPrepareAsynchronousHeapDataExchange", line ) and re.search( "rank:" + str(currentRank) + " ", line )):
        asynchronousHeapDataCalendar.append( float( line.split( "=" )[-2].split(",")[0] ))      
        asynchronousHeapDataCPU.append(      float( line.split( "=" )[-1].strip() ))

        tasynchronousHeapDataCalendar.append( float( line.strip().split( " " )[0] ))     
        tasynchronousHeapDataCPU.append(      float( line.strip().split( " " )[0] ))
      if (re.search( "DefaultAnalyser::leaveCentralElementOfEnclosingSpacetree", line ) and re.search( "rank:" + str(currentRank) + " ", line )):
        centralElementCalendar.append( float( line.split( "(" )[-1].split(",")[0] ))
        centralElementCPU.append(      float( line.split( "," )[-1].split(")")[0] ))

        tcentralElementCalendar.append( float( line.strip().split( " " )[0] ))
        tcentralElementCPU.append(      float( line.strip().split( " " )[0] ))
      if (re.search( "DefaultAnalyser::dataWasNotReceivedFromWorker", line ) and re.search( "rank:" + str(currentRank) + " ", line )):
        workerWaitTimeCalendar.append( float( line.split( "for " )[2].split("s")[0] ))      
  
        tworkerWaitTimeCalendar.append( float( line.strip().split( " " )[0] ) )     
        
        
  except Exception as inst:
    print "failed to read " + inputFileName
    print inst

  pylab.clf()
  pylab.title( "Runtime profile (calendar time)" )

  if len(ttotalTimeCalendar)==len(totalTimeCalendar):
    pylab.plot(ttotalTimeCalendar, totalTimeCalendar, 'x-', label='total', color='#FF0000', markersize=10)
  else:
    print "ERROR: ttotalTimeCalendar has lenght " + str(len(ttotalTimeCalendar)) + " while totalTimeCalendar has lenght " + str(len(totalTimeCalendar))

  if len(tjoinTimeCalendar)==len(joinTimeCalendar):
    pylab.plot(tjoinTimeCalendar, joinTimeCalendar, 'o-', label='join', color='#00FF00', markersize=10)
  else:
    print "ERROR: tjoinTimeCalendar has lenght " + str(len(tjoinTimeCalendar)) + " while joinTimeCalendar has lenght " + str(len(joinTimeCalendar))

  if len(tboundaryTimeCalendar)==len(boundaryTimeCalendar):
    pylab.plot(tboundaryTimeCalendar, boundaryTimeCalendar, '^-', label='boundary exchange', color='#0000FF', markersize=10)
  else:
    print "ERROR: tboundaryTimeCalendar has lenght " + str(len(tboundaryTimeCalendar)) + " while boundaryTimeCalendar has lenght " + str(len(boundaryTimeCalendar))

  if len(tsynchronousHeapDataCalendar)==len(synchronousHeapDataCalendar):
    pylab.plot(tsynchronousHeapDataCalendar, synchronousHeapDataCalendar, '+-', label='synchronous heap', color='#aaaa00', markersize=10)
  else:
    print "ERROR: tsynchronousHeapDataCalendar has lenght " + str(len(tsynchronousHeapDataCalendar)) + " while synchronousHeapDataCalendar has lenght " + str(len(synchronousHeapDataCalendar))

  if len(tasynchronousHeapDataCalendar)==len(asynchronousHeapDataCalendar):
    pylab.plot(tasynchronousHeapDataCalendar, asynchronousHeapDataCalendar, '1-', label='asynchronous heap', color='#aa00aa', markersize=10)
  else:
    print "ERROR: tasynchronousHeapDataCalendar has lenght " + str(len(tasynchronousHeapDataCalendar)) + " while asynchronousHeapDataCalendar has lenght " + str(len(asynchronousHeapDataCalendar))

  #
  # If multiple ranks plot to one terminal, lines can be mixed up and parsing can fail. 
  #
  if (len(tcentralElementCalendar)==len(centralElementCalendar)):
    pylab.plot(tcentralElementCalendar, centralElementCalendar, 'v--', label='local elements traversal', color='#00aaaa', markersize=10)
  else:
    print "ERROR: tcentralElementCalendar has lenght " + str(len(tcentralElementCalendar)) + " while centralElementCalendar has lenght " + str(len(centralElementCalendar))

  pylab.plot(tworkerWaitTimeCalendar, workerWaitTimeCalendar, '3-', label='wait for worker', color='#667766', markersize=10)

  setGeneralPlotSettings()

  pylab.savefig( plotPrefix + ".runtime-profile-calendar-rank-" + str(currentRank) + ".png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( plotPrefix + ".runtime-profile-calendar-rank-" + str(currentRank) + ".pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )



  pylab.clf()
  pylab.title( "Runtime profile (cpu time)" )

  pylab.plot(ttotalTimeCPU, totalTimeCPU, 'x-', label='total', color='#FF0000', markersize=10)

  pylab.plot(tjoinTimeCPU, joinTimeCPU, 'o-', label='join', color='#00FF00', markersize=10)

  pylab.plot(tboundaryTimeCPU, boundaryTimeCPU, '^-', label='boundary exchange', color='#0000FF', markersize=10)

  pylab.plot(tsynchronousHeapDataCPU, synchronousHeapDataCPU, '+-', label='synchronous heap', color='#aaaa00', markersize=10)

  pylab.plot(tasynchronousHeapDataCPU, asynchronousHeapDataCPU, '1-', label='asynchronous heap', color='#aa00aa', markersize=10)

  if (len(tcentralElementCPU)==len(centralElementCPU)):
    pylab.plot(tcentralElementCPU, centralElementCPU, 'v--', label='local elements traversal', color='#00aaaa', markersize=10)
  else:
    print "- error in centralElementCPU"

  setGeneralPlotSettings()
  pylab.savefig( plotPrefix + ".runtime-profile-cpu-rank-" + str(currentRank) + ".png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( plotPrefix + ".runtime-profile-cpu-rank-" + str(currentRank) + ".pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
