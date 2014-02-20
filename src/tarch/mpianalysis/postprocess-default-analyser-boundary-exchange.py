from PIL import Image, ImageDraw, ImageFont

import csv
import sys
import pydot
import re

class Pair:
  def __init__(self, sender, receiver):
    self.sender         = sender
    self.receiver       = receiver
    self.count          = 0
    self.maxCardinality = 0
    self.average        = 0

inputFilename  = sys.argv[1]
numberOfRanks  = int(sys.argv[2])


graph              = pydot.Dot(graph_type='digraph')
sparseAverageGraph = pydot.Dot(graph_type='digraph')
sparseMaxGraph     = pydot.Dot(graph_type='digraph')

myfontsize = 8.0

for rank in range(0,numberOfRanks):
  graph.add_node(pydot.Node( str(rank), style="filled", fillcolor="grey", fontcolor="blue" ))
  sparseAverageGraph.add_node(pydot.Node( str(rank), style="filled", fillcolor="grey", fontcolor="blue" ))
  sparseMaxGraph.add_node(pydot.Node( str(rank), style="filled", fillcolor="grey", fontcolor="blue" ))


totalMaxCardinality     = 0
totalCount              = 0
totalAverage            = 0

pairs = dict()

waitingForNeighborLine = "tarch::mpianalysis::DefaultAnalyser::dataWasNotReceivedInBackground.*rank had to wait for (\d+) record\(s\)" \
                         + " from (\d+) on tag (\d) with page size (\d+)"

#Extract data from input file
inFile  = open( inputFilename,  "r" )
for line in inFile:
  m = re.search( waitingForNeighborLine, line )
  if(m):
    sender = int(m.group(2))
    receiver = int(m.group(3))
    key = (sender, receiver)
    
    if not pairs.has_key(key):
      pairs[key] = Pair(sender, receiver)
    pair = pairs[key]
    
    cardinality = int(m.group(2))
    pair.count += 1
    totalCount += 1 
    
    pair.maxCardinality = max(pair.maxCardinality, cardinality)
    totalMaxCardinality = max(totalMaxCardinality, cardinality)

    pair.average += cardinality
    totalAverage += cardinality
    
    
#Create graphs
for receiver in range(0,numberOfRanks):
  for sender in range(0,numberOfRanks):
    key = (sender, receiver)
    
    if pairs.has_key(key):
      pair = pairs[key]
      
      if pair.count>2:
        edge = pydot.Edge(str(sender),str(receiver), label="(" + str(pair.count) + "," + str(pair.maxCardinality) + "," + str(float(pair.average) / float(pair.count)) + ")", fontsize=str(myfontsize), labelfontcolor="blue" )
        graph.add_edge(edge)
        
      if pair.count>2 and (float(pair.average) / float(pair.count) > float(totalAverage) / float(totalCount)):
        edge = pydot.Edge(str(sender),str(receiver), label="(" + str(pair.count) + "," + str(pair.maxCardinality) + "," + str(float(pair.average) / float(pair.count)) + ")", fontsize=str(myfontsize), labelfontcolor="blue" )
        sparseAverageGraph.add_edge(edge)
  
      if pair.count>2 and ( float(pair.maxCardinality) > 0.9 * float(totalMaxCardinality) ):
        edge = pydot.Edge(str(sender),str(receiver), label="(" + str(pair.count) + "," + str(pair.maxCardinality) + "," + str(float(pair.average) / float(pair.count)) + ")", fontsize=str(myfontsize), labelfontcolor="blue" )
        sparseMaxGraph.add_edge(edge)
  
graph.add_node(pydot.Node( "Summary=(" + str(totalCount) + "," + str(totalMaxCardinality) + "," + str(float(totalAverage) / float(totalCount)) + ")", labelfontcolor="blue" ))
graph.write_png(sys.argv[1]+'.boundary-exchange.png', prog='fdp')

if (  totalCount>0 ):
  sparseAverageGraph.add_node(pydot.Node( "Threshold on average graph weight=" + str(float(totalAverage) / float(totalCount))))
else:
  sparseAverageGraph.add_node(pydot.Node( "No late boundary alerts dumped", labelfontcolor="blue" ))
sparseAverageGraph.write_png(sys.argv[1]+'.boundary-exchange-sparse-average.png', prog='fdp')

sparseMaxGraph.add_node(pydot.Node( "Threshold on max graph weight, 10% of " + str(totalMax) ))
sparseMaxGraph.write_png(sys.argv[1]+'.boundary-exchange-sparse-max.png', prog='fdp')

#dot
#twopi
#neato
#fdp


