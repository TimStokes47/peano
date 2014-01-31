import sys
import os
import os.path
import re


inputFilename  = sys.argv[1]
outputFilename = "tmp.data" 

outFile        = open( outputFilename, "w" )


outFile.write( "Step Idle-time Busy-time \n" )


#busyTimeAverage = 0
#idleTimeAverage = 0



maxSteps = 0
for rank in range(0,int(sys.argv[2])):
  inFile  = open( inputFilename,  "r" )
  step    = 0     

  data = "   "
  while ( not data=="" ):
    data = inFile.readline()
    if (re.search( "tarch::mpianalysis::DefaultAnalyser::beginIteration", data ) and re.search( "rank:" + str(rank) + " ", data )):
      try:
        currentIdleTime = data.split( "=" )[1].split(",")[0]
      except:
        currentIdleTime = 0
    if (re.search( "tarch::mpianalysis::DefaultAnalyser::endIteration", data ) and re.search( "rank:" + str(rank) + " ", data )):
      try:
        currentBusyTime = data.split( "=" )[1].split(",")[0]
      except:
        currentBusyTime = 0
      step  = step + 1
      outFile.write( str(step) + " " + str(currentIdleTime) + " " + str(currentBusyTime) + "\n" )
  
  if (step>maxSteps):
    maxSteps = step
    
  if (step==0):
    for s in range(1,maxSteps+1):
       outFile.write( str(s) + " ?0 ?0 \n" )
  
  outFile.write( "\n" )
  #outFile.write( "\n" )
  inFile.close()