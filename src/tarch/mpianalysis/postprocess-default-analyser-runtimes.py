import sys
import os
import os.path
import re


inputFilename  = sys.argv[1]
outputFilename = "tmp.data" 

outFile        = open( outputFilename, "w" )


outFile.write( "Step Busy-time Idle-time Join-time Boundary-data-wait-time Synchronous-heap-data-wait-time Asynchronous-heap-data-wait-time Time-spent-working-in-central-element  Accumulated-worker-wait-time \n" )


currentIdleTimeCalendar             = 0
currentIdleTimeCPU                  = 0
currentBusyTimeCalendar             = 0
currentBusyTimeCPU                  = 0
currentJoinTimeCalendar             = 0
currentJoinTimeCPU                  = 0
currentBoundaryTimeCalendar         = 0
currentBoundaryTimeCPU              = 0
currentSynchronousHeapDataCalendar  = 0
currentSynchronousHeapDataCPU       = 0
currentAsynchronousHeapDataCalendar = 0
currentAsynchronousHeapDataCPU      = 0
currentCentralElementCalendar       = 0
currentCentralElementCPU            = 0
currentWorkerWaitTimeCalendar       = 0
 
 
maxSteps = 0
for rank in range(0,int(sys.argv[2])):
  inFile  = open( inputFilename,  "r" )
  step    = 0     

  data = "   "
  while ( not data=="" ):
    data = inFile.readline()
    if (re.search( "tarch::mpianalysis::DefaultAnalyser::beginIteration", data ) and re.search( "rank:" + str(rank) + " ", data )):
      step  = step + 1
      outFile.write( 
        str(step) + " " + 
        str(currentBusyTimeCalendar) + " " + 
        str(currentBusyTimeCPU) + " " + 
        str(currentIdleTimeCalendar) + " " + 
        str(currentIdleTimeCPU) + " " + 
        str(currentJoinTimeCalendar) + " " + 
        str(currentJoinTimeCPU) + " " + 
        str(currentBoundaryTimeCalendar) + " " + 
        str(currentBoundaryTimeCPU) + " " + 
        str(currentSynchronousHeapDataCalendar) + " " + 
        str(currentSynchronousHeapDataCPU) + " " + 
        str(currentAsynchronousHeapDataCalendar) + " " +
        str(currentAsynchronousHeapDataCPU) + " " +
        str(currentCentralElementCalendar) + " " +
        str(currentCentralElementCPU) + " " + 
        str(currentWorkerWaitTimeCalendar) + " " +
        "\n" 
      )


      currentIdleTimeCalendar             = 0
      currentIdleTimeCPU                  = 0
      currentBusyTimeCalendar             = 0
      currentBusyTimeCPU                  = 0
      currentJoinTimeCalendar             = 0
      currentJoinTimeCPU                  = 0
      currentBoundaryTimeCalendar         = 0
      currentBoundaryTimeCPU              = 0
      currentSynchronousHeapDataCalendar  = 0
      currentSynchronousHeapDataCPU       = 0
      currentAsynchronousHeapDataCalendar = 0
      currentAsynchronousHeapDataCPU      = 0
      currentCentralElementCalendar       = 0
      currentCentralElementCPU            = 0
      currentWorkerWaitTimeCalendar       = 0
      
      try:
        currentIdleTimeCalendar = data.split( "=" )[1].split(",")[0]
      except:
        currentIdleTimeCalendar = 0
      try:
        currentIdleTimeCPU      = data.split( "=" )[2].strip()
      except:
        currentIdleTimeCPU      = 0

    if (re.search( "tarch::mpianalysis::DefaultAnalyser::endIteration", data ) and re.search( "rank:" + str(rank) + " ", data )):
      try:
        currentBusyTimeCalendar = data.split( "=" )[1].split(",")[0]
      except:
        currentBusyTimeCalendar = 0
      try:
        currentBusyTimeCPU      = data.split( "=" )[2].strip()
      except:
        currentBusyTimeCPU      = 0
        
    if (re.search( "tarch::mpianalysis::DefaultAnalyser::endReleaseOfJoinData", data ) and re.search( "rank:" + str(rank) + " ", data )):
      try:
        currentJoinTimeCalendar = data.split( "=" )[1].split(",")[0]
      except:
        currentJoinTimeCalendar = 0
      try:
        currentJoinTimeCPU      = data.split( "=" )[2].strip()
      except:
        currentJoinTimeCPU      = 0

    if (re.search( "tarch::mpianalysis::DefaultAnalyser::endReleaseOfBoundaryData", data ) and re.search( "rank:" + str(rank) + " ", data )):
      try:
        currentBoundaryTimeCalendar = data.split( "=" )[1].split(",")[0]
      except:
        currentBoundaryTimeCalendar = 0
      try:
        currentBoundaryTimeCPU      = data.split( "=" )[2].strip()
      except:
        currentBoundaryTimeCPU      = 0

    if (re.search( "tarch::mpianalysis::DefaultAnalyser::endToReleaseSynchronousHeapData", data ) and re.search( "rank:" + str(rank) + " ", data )):
      try:
        currentSynchronousHeapDataCalendar = data.split( "=" )[1].split(",")[0]
      except:
        currentSynchronousHeapDataCalendar = 0
      try:
        currentSynchronousHeapDataCPU      = data.split( "=" )[2].strip()
      except:
        currentSynchronousHeapDataCPU      = 0

    if (re.search( "tarch::mpianalysis::DefaultAnalyser::endToPrepareAsynchronousHeapDataExchange", data ) and re.search( "rank:" + str(rank) + " ", data )):
      try:
        currentAsynchronousHeapDataCalendar = data.split( "=" )[1].split(",")[0]
      except:
        currentAsynchronousHeapDataCalendar = 0
      try:
        currentAsynchronousHeapDataCPU      = data.split( "=" )[2].strip()
      except:
        currentAsynchronousHeapDataCPU      = 0

    if (re.search( "tarch::mpianalysis::DefaultAnalyser::leaveCentralElementOfEnclosingSpacetree", data ) and re.search( "rank:" + str(rank) + " ", data )):
      try:
        currentCentralElementCalendar = data.split( "=" )[1].split(",")[0]
      except:
        currentCentralElementCalendar = 0
      try:
        currentCentralElementCPU      = data.split( "=" )[2].strip()
      except:
        currentCentralElementCPU      = 0

    if (re.search( "tarch::mpianalysis::DefaultAnalyser::dataWasNotReceivedFromWorker", data ) and re.search( "rank:" + str(rank) + " ", data )):
      try:
        currentWorkerWaitTimeCalendar = currentWorkerWaitTimeCalendar+ float(data.split( "for " )[2].split("s")[0])
      except:
        currentWorkerWaitTimeCalendar = currentWorkerWaitTimeCalendar
  
  if (step>maxSteps):
    maxSteps = step
    
  if (step==0):
    for s in range(1,maxSteps+1):
       outFile.write( str(s) + " ?0 ?0 ?0 ?0 ?0 ?0 ?0 ?0 ?0 ?0 ?0 ?0 ?0 ?0 ?0 ?0 \n" )
  
  outFile.write( "\n" )
  #outFile.write( "\n" )
  inFile.close()