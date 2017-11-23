import argparse
from argparse import RawTextHelpFormatter

import pylab

import gc
import re
import os


########################################################################
# START OF THE PROGRAM
########################################################################
# For the meaning of the command line arguments, see the documen\tation
# of the function 'plot_multithreading_adapter_scaling' above.
help = '''
Creates a brief overview of the domain decomposition
\n\n
Sample usages:\n
python plot-rank-distribution.py -file 112x16-0.results -firstrankonnode 12 -numberofranks 6 -output outfile \n
The script automatically appends .pdf and .png to outfile.
'''

parser = argparse.ArgumentParser(description=help,formatter_class=RawTextHelpFormatter)
parser.add_argument('-file',               required=True, help="Input file")
parser.add_argument('-firstrankonnode',    required=True, help="First rank to be studied")
parser.add_argument('-numberofranks',      required=True, help="Number of consecutive ranks to be studied")
parser.add_argument('-output',             required=True, help="Prefix of output file")
args   = parser.parse_args();

firstRank = int(args.firstrankonnode)
noOfRanks = int(args.numberofranks)

timeStamps  = [ [] for x in range(0,noOfRanks) ]
coresBooked = [ [] for x in range(0,noOfRanks) ]
coresWanted = [ [] for x in range(0,noOfRanks) ]

#
# =================
# Build up database
# =================
inputFile = open( args.file,  "r" )
try:
  for line in inputFile:
    for rank in range(firstRank,firstRank+noOfRanks):
      # The tricky thing here is the additional space in the expression. Otherwise 6 and 64 match
      if re.search( "rank:" + str(rank) + " .*start to collect information from individual ranks", line):
        newTimeStamp = float( line.strip().split( " " )[0] )
        timeStamps[rank-firstRank].append(  newTimeStamp )
        coresBooked[rank-firstRank].append( 1 )
        coresWanted[rank-firstRank].append( 1 )
      
      if re.search( "rank:" + str(rank) + " .*tarch::multicore::Core::configure", line):
        newTimeStamp = float( line.strip().split( " " )[0] )
        
        if len(timeStamps[rank-firstRank])>1 and timeStamps[rank-firstRank][-1]>newTimeStamp:
          print "error in line " + line
          print "series of previous time stamps for this rank: " + str(timeStamps[rank-firstRank])
          exit(-1)
        
        timeStamps[rank-firstRank].append(  newTimeStamp )
        coresBooked[rank-firstRank].append( float( line.split( "had " )[1].split("threads")[0] ))
        coresWanted[rank-firstRank].append( float( line.split( "and got " )[1].split("(")[0] ))
        

except Exception as inst:
  print "failed to read " + inputFile
  print inst
  
  
postprocessedTimeStamps  = [ ]
accumulatedCoresBooked   = { x:[] for x in range(-1,noOfRanks) }
accumulatedCoresWanted   = { x:[] for x in range(-1,noOfRanks) }
currentCoresBooked       = [ 1 for x in range(0,noOfRanks) ]
currentCoresWanted       = [ 1 for x in range(0,noOfRanks) ]
currentTimeStamp         = 0.0

entriesRemaining = True
while entriesRemaining:
  entriesRemaining = False
  Timeout          = 65536
  currentTimeStamp = Timeout
  nextActiveRank    = -1
  for rank in range(noOfRanks):
    if len(timeStamps[rank])>0 and timeStamps[rank][0]<currentTimeStamp:
      nextActiveRank    = rank
      currentTimeStamp  = timeStamps[rank][0]

  if  currentTimeStamp<Timeout:
    #print "extracted next smallest time stamp which is observed on rank " + str(nextActiveRank)
    #print "timestamp=" + str(currentTimeStamp)
    #for rank in range(noOfRanks):
    #  print "t(" + str(rank) + ")=" + str(timeStamps[rank])    
    #  print "booked(" + str(rank) + ")=" + str(coresBooked[rank])    
    #  print "wanted(" + str(rank) + ")=" + str(coresWanted[rank])    
      
    currentCoresBooked[nextActiveRank] = coresBooked[nextActiveRank][0]
    currentCoresWanted[nextActiveRank] = coresWanted[nextActiveRank][0]
    
    if coresBooked[nextActiveRank][0]<1 or coresWanted[nextActiveRank][0]<1:
      print "error in data, some rank counts 0 or negative"

    del timeStamps[nextActiveRank][0] 
    del coresBooked[nextActiveRank][0] 
    del coresWanted[nextActiveRank][0] 
      
    postprocessedTimeStamps.append( currentTimeStamp )
    accumulatedCoresWanted[-1].append(0)
    accumulatedCoresBooked[-1].append(0)
  
    for rank in range(noOfRanks):
      accumulatedCoresBooked[rank].append( 
        currentCoresBooked[rank] + accumulatedCoresBooked[rank-1][-1] )
      accumulatedCoresWanted[rank].append( 
        currentCoresWanted[rank] + accumulatedCoresWanted[rank-1][-1] )
        
      if accumulatedCoresBooked[rank][-1]<1:
        print "error in accumulated cores booked"
      if accumulatedCoresWanted[rank][-1]<1:
        print "error in accumulated cores wanted"
      
    
    entriesRemaining = True
 
for rank in range(0,noOfRanks):
  pylab.fill_between( postprocessedTimeStamps, accumulatedCoresBooked[rank], accumulatedCoresBooked[rank-1], alpha=0.5 )
  pylab.plot( postprocessedTimeStamps, accumulatedCoresWanted[rank] )

pylab.xlabel('Runtime [t]=s')  
pylab.ylabel('Cores used/requested')
pylab.savefig( args.output + ".png", transparent = True )
pylab.savefig( args.output + ".pdf", transparent = True ) 
