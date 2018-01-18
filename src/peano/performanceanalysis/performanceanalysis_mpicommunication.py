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
def plotMPIPhases(numberOfRanks,inputFileName,fileName):
  ColorInsideTree                  = "#00ff00"
  ColorReceiveDataFromWorker       = "#ff0000"
  ColorReceiveDataFromMaster       = "#660000"
  ColorReleaseSynchronousHeapData  = "#0000ff"
  ColorReleaseAsynchronousHeapData = "#000066"
  ColorReleaseJoinData             = "#ffff00"
  ColorReleaseBoundaryData         = "#666600"
  
  pylab.clf()
  DefaultSize = pylab.gcf().get_size_inches()
  pylab.gcf().set_size_inches( DefaultSize[0]*4, DefaultSize[1] )
  pylab.title( "MPI trace of activities" )
  ax = pylab.gca()
  
  timeStampPattern = "([0-9]+\.?[0-9]*)"
  floatPattern = "([0-9]\.?[0-9]*)"
  
  beginIterationPattern              = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::beginIteration"
  leaveCentralElementPattern         = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::leaveCentralElementOfEnclosingSpacetree.*t_central-tree-traversal=\(" + floatPattern
  receiveDataFromWorkerPattern       = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::endToReceiveDataFromWorker.* for " + floatPattern
  receiveDataFromMasterPattern       = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::endToReceiveDataFromMaster.* for " + floatPattern
  releaseSynchronousHeapDataPattern  = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::endToReleaseSynchronousHeapData.*time=" + floatPattern
  releaseAsynchronousHeapDataPattern = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::endToPrepareAsynchronousHeapDataExchange.*time=" + floatPattern
  releaseJoinDataPattern             = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::endReleaseOfJoinData.*time=" + floatPattern
  releaseBoundaryDataPattern         = timeStampPattern + ".*rank:(\d+)*.*peano::performanceanalysis::DefaultAnalyser::endReleaseOfBoundaryData.*time=" + floatPattern

  def plotMPIPhasesBar( rank, start, end, color):
    if end>start:
      rect = pylab.Rectangle([start,rank-0.5],end-start,1,facecolor=color,edgecolor=color,alpha=Alpha)
      ax.add_patch(rect)
  
  Alpha = 0.5
  
  try:
    inputFile = open( inputFileName,  "r" )
    print "parse mpi phases",
    for line in inputFile:
      m = re.search( beginIterationPattern, line )
      if (m):
        rank = int( m.group(2) )
        timeStamp = float( m.group(1) )
        print ".",
        if (rank==0):
          pylab.plot((timeStamp, timeStamp), (-0.5, numberOfRanks+1), ':', color="#445544", alpha=Alpha)
        pylab.plot((timeStamp, timeStamp), (rank-0.5, rank+0.5), '-', color="#ababab" )
      m = re.search( leaveCentralElementPattern, line )
      if (m):
        timeStamp = float( m.group(1) )
        rank      = int( m.group(2) )
        duration  = float( m.group(3) )
        plotMPIPhasesBar(rank,timeStamp-duration,timeStamp,ColorInsideTree)
      m = re.search( receiveDataFromWorkerPattern, line )
      if (m):
        timeStamp = float( m.group(1) )
        rank      = int( m.group(2) )
        duration  = float( m.group(3) )
        plotMPIPhasesBar(rank,timeStamp-duration,timeStamp,ColorReceiveDataFromWorker)
      m = re.search( receiveDataFromMasterPattern, line )
      if (m):
        timeStamp = float( m.group(1) )
        rank      = int( m.group(2) )
        duration  = float( m.group(3) )
        plotMPIPhasesBar(rank,timeStamp-duration,timeStamp,ColorReceiveDataFromMaster)
      m = re.search( releaseSynchronousHeapDataPattern, line )
      if (m):
        timeStamp = float( m.group(1) )
        rank      = int( m.group(2) )
        duration  = float( m.group(3) )
        plotMPIPhasesBar(rank,timeStamp-duration,timeStamp,ColorReleaseSynchronousHeapData)
      m = re.search( releaseAsynchronousHeapDataPattern, line )
      if (m):
        timeStamp = float( m.group(1) )
        rank      = int( m.group(2) )
        duration  = float( m.group(3) )
        plotMPIPhasesBar(rank,timeStamp-duration,timeStamp,ColorReleaseAsynchronousHeapData)
      m = re.search( releaseJoinDataPattern, line )
      if (m):
        timeStamp = float( m.group(1) )
        rank      = int( m.group(2) )
        duration  = float( m.group(3) )
        plotMPIPhasesBar(rank,timeStamp-duration,timeStamp,ColorReleaseJoinData)
      m = re.search( releaseBoundaryDataPattern, line )
      if (m):
        timeStamp = float( m.group(1) )
        rank      = int( m.group(2) )
        duration  = float( m.group(3) )
        plotMPIPhasesBar(rank,timeStamp-duration,timeStamp,ColorReleaseBoundaryData)
        
        

    print " done"
  except Exception as inst:
    print "failed to read " + inputFileName
    print inst
  
  ax.invert_yaxis()
  ax.autoscale_view()
  pylab.xlabel('t')
  pylab.grid(False)
  pylab.savefig( fileName + ".png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( fileName + ".pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  try:
    pylab.gcf().set_size_inches( DefaultSize[0]*4*10, DefaultSize[1]*10 )
    if numberOfRanks<=16:
      pylab.yticks([i for i in range(0,numberOfRanks)]) 
    else:
      pylab.yticks([i*16 for i in range(0,numberOfRanks/16)]) 
    pylab.savefig( fileName + ".large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( fileName + ".large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  except:
    print "ERROR: failed to generated large-scale plot"
    pylab.savefig( fileName + ".large.png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
    pylab.savefig( fileName + ".large.pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )

  pylab.gcf().set_size_inches( DefaultSize[0], DefaultSize[1] )


