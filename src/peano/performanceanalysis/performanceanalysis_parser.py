import sys
import re


def getNumberOfRanks(filename):
  print "parse input file header ",
  try:
    inputFile = open( filename,  "r" )
    for line in inputFile:
      print ".",
      if "processes:" in line:
        numberOfRanks = int(line.split( "processes:" )[1].split( "," )[0])
        print str(numberOfRanks) + " MPI ranks used"
        return numberOfRanks

  except Exception as inst:
    print "failed to read " + filename
    print inst
  return 0


def getNumberOfThreads(filename):
  print "parse input file header ",
  try:
    inputFile = open( filename,  "r" )
    for line in inputFile:
      print ".",
      if "threads:" in line:
        numberOfThreads = int(line.split( "threads:" )[1].split( "(per process)" )[0])
        print str(numberOfThreads) + " threads ranks used"
        return numberOfThreads

  except Exception as inst:
    print "failed to read " + filename
    print inst
  return 0


def getBoundingBox(line):
  line    = line.replace( "]x[", "," )
  results = line.split( "node for subdomain [")[1].split( "] on level" )[0].split( "," )
  return results

  
def getLogicalTopology(numberOfRanks,dim,inputFileName,plotDirectoryName):
  parents  = [-1 for x in range(0,numberOfRanks)]
  nodes    = ["global master" for x in range(0,numberOfRanks)]
  levels   = [-1 for x in range(0,numberOfRanks)]
  if dim==2:
    volume = [(0.0,0.0) for x in range(0,numberOfRanks)]
    offset = [(0.0,0.0) for x in range(0,numberOfRanks)]
  else:
    volume = [(0.0,0.0,0.0) for x in range(0,numberOfRanks)]
    offset = [(0.0,0.0,0.0) for x in range(0,numberOfRanks)]
  level = [ 0 for x in range(0,numberOfRanks)]
  
  try:
    inputFile = open( inputFileName,  "r" )
    print "parse topology",
    for line in inputFile:
      if "start node for subdomain" in line:
        print ".",
        child = int(line.split( "rank:" )[1].split( " " )[0])
        parent = int(line.split( "with master" )[1])
        node = line.split( ",rank:" )[0].split(" ")[-1]
        parents[child]=parent
        nodes[child]=node
        fragments = getBoundingBox(line) 
        if dim==2:
          offset[child] = ( float(fragments[0]),float(fragments[1]) )
          volume[child] = ( float(fragments[2]),float(fragments[3]) )
        else:
          offset[child] = ( float(fragments[0]),float(fragments[1]),float(fragments[2]) )
          volume[child] = ( float(fragments[3]),float(fragments[4]),float(fragments[5]) )
        levels[child] = int(line.split( "on level" )[1].split( " with" )[0])
    print " done"
  except Exception as inst:
    print "failed to read " + inputFileName
    print inst
  return (parents,levels,offset,volume,nodes)

