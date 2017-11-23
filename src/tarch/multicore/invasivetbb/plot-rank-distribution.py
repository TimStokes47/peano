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


try:
  inputFile = open( args.file,  "r" )
  for line in inputFile:
    for rank in range(firstRank,firstRank+noOfRanks):
      if re.search( "rank:" + str(rank) + ".*tarch::multicore::Core::configure", line):
        timeStamps[rank-firstRank].append(  float( line.strip().split( " " )[0] ) )
        coresBooked[rank-firstRank].append( float( line.split( "had " )[1].split("threads")[0] ))
        coresWanted[rank-firstRank].append( float( line.split( "and got " )[1].split("(")[0] ))

except Exception as inst:
  print "failed to read " + filename
  print inst
  
  
for rank in range(noOfRanks):
  print str(timeStamps[rank])
  print str(coresBooked[rank])
  pylab.plot( timeStamps[rank], coresBooked[rank] )

pylab.xlabel('Runtime [t]=s')  
pylab.ylabel('Cores used')
pylab.savefig( args.output + ".png", transparent = True )
pylab.savefig( args.output + ".pdf", transparent = True ) 
