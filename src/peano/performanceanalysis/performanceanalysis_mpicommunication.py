import pylab
import re



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
  DefaultSize = pylab.gcf().get_size_inches()
  pylab.gcf().set_size_inches( DefaultSize[0]*4, DefaultSize[1] )
  pylab.title( "MPI trace of activities" )
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

  pylab.gcf().set_size_inches( DefaultSize[0], DefaultSize[1] )


