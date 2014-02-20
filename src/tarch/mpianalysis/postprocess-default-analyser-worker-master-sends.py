from PIL import Image, ImageDraw, ImageFont

import csv
import sys
import pydot
import re

class Pair:
  def __init__(self, master, worker):
    self.master         = master
    self.worker         = worker
    self.count          = 0
    self.maxTime        = 0.0
    self.average        = 0

inputFilename  = sys.argv[1]
numberOfRanks  = int(sys.argv[2])

myfontsize = 8.0


graph              = pydot.Dot(graph_type='digraph')
sparseMaxGraph     = pydot.Dot(graph_type='digraph')
sparseAverageGraph = pydot.Dot(graph_type='digraph')

for rank in range(0,numberOfRanks):
  graph.add_node(pydot.Node( str(rank), style="filled", fillcolor="grey", fontcolor="blue" ))
  sparseMaxGraph.add_node(pydot.Node( str(rank), style="filled", fillcolor="grey", fontcolor="blue" ))
  sparseAverageGraph.add_node(pydot.Node( str(rank), style="filled", fillcolor="grey", fontcolor="blue" ))


totalMaxTime        = 0.0
totalAverage        = 0
totalCount          = 0

pairs = dict()

floatPattern = "[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?"
waitingForWorkerLine = "rank:(\d+) tarch::mpianalysis::DefaultAnalyser::dataWasNotReceivedFromWorker.*rank had to wait for worker (\d+) for ("+ floatPattern + ")s"

#Extract data from input file
inFile  = open( inputFilename,  "r" )
for line in inFile:
  m = re.search( waitingForWorkerLine, line )
  if(m):
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

#Create graphs    
for master in range(0,numberOfRanks):
  for worker in range(0,numberOfRanks):
    key = (master, worker)
    
    if pairs.has_key(key):
      pair = pairs[key]
    
      if pair.count>2:
       edge = pydot.Edge(str(worker),str(master), label="(" + str(pair.count) + "," + str(pair.maxTime) + "," + str(float(pair.average) / float(pair.count)) + ")", fontsize=str(myfontsize), labelfontcolor="blue" )
       graph.add_edge(edge)
       
      if totalCount > 0 and pair.count>2 and (float(pair.average) / float(pair.count) > float(totalAverage) / float(totalCount)):
        edge = pydot.Edge(str(worker),str(master), label="(" + str(pair.count) + "," + str(pair.maxTime) + "," + str(float(pair.average) / float(pair.count)) + ")", fontsize=str(myfontsize), labelfontcolor="blue" )
        sparseAverageGraph.add_edge(edge)
 
      if pair.count>2 and ( float(pair.maxTime) > 0.9 * float(totalMaxTime) ):
        edge = pydot.Edge(str(worker),str(master), label="(" + str(pair.count) + "," + str(pair.maxTime) + "," + str(float(pair.average) / float(pair.count)) + ")", fontsize=str(myfontsize), labelfontcolor="blue" )
        sparseMaxGraph.add_edge(edge)      
        
graph.write_png(sys.argv[1]+'.worker-master.png', prog='fdp')
sparseMaxGraph.write_png(sys.argv[1]+'.worker-master-sparse-max.png', prog='fdp')
sparseAverageGraph.write_png(sys.argv[1]+'.worker-master-sparse-average.png', prog='fdp')
