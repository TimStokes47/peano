from PIL import Image, ImageDraw, ImageFont

import csv
import sys
import pydot
import re


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


totalMax     = 0
totalCount   = 0
totalAverage = 0

for receiver in range(0,numberOfRanks):
  for sender in range(0,numberOfRanks):
    count   = 0
    max     = 0
    average = 0
    inFile  = open( inputFilename,  "r" )
    data = "   "
    while ( not data=="" ):
      data = inFile.readline()
      if (
        re.search( "tarch::mpianalysis::DefaultAnalyser::dataWasNotReceivedInBackground", data ) and 
        re.search( "rank:" + str(receiver) + " ", data ) and
        re.search( "from " + str(sender) + " ", data ) 
      ):
        cardinality = int(data.split( " record(s)" )[0].split("for")[1])
        count      = count + 1
        totalCount = totalCount + 1 
        if ( cardinality > max ):
          max      = cardinality
        if ( cardinality > totalMax ):
          totalMax = cardinality
        average      = average + cardinality
        totalAverage = totalAverage + cardinality

    if count>2:
      edge = pydot.Edge(str(sender),str(receiver), label="(" + str(count) + "," + str(max) + "," + str(float(average) / float(count)) + ")", fontsize=myfontsize, labelfontcolor="blue" )
      graph.add_edge(edge)
        
  
graph.add_node(pydot.Node( "Summary=(" + str(totalCount) + "," + str(totalMax) + "," + str(float(totalAverage) / float(totalCount)) + ")", labelfontcolor="blue" ))
graph.write_png(sys.argv[1]+'.boundary-exchange.png', prog='fdp')



for receiver in range(0,numberOfRanks):
  for sender in range(0,numberOfRanks):
    count   = 0
    max     = 0
    average = 0
    inFile  = open( inputFilename,  "r" )
    data = "   "
    while ( not data=="" ):
      data = inFile.readline()
      if (
        re.search( "tarch::mpianalysis::DefaultAnalyser::dataWasNotReceivedInBackground", data ) and 
        re.search( "rank:" + str(receiver) + " ", data ) and
        re.search( "from " + str(sender) + " ", data ) 
      ):
        cardinality = int(data.split( " record(s)" )[0].split("for")[1])
        count      = count + 1
        if ( cardinality > max ):
          max      = cardinality
        average      = average + cardinality

    if count>2 and (float(average) / float(count) > float(totalAverage) / float(totalCount)):
      edge = pydot.Edge(str(sender),str(receiver), label="(" + str(count) + "," + str(max) + "," + str(float(average) / float(count)) + ")", fontsize=myfontsize, labelfontcolor="blue" )
      sparseAverageGraph.add_edge(edge)

    if count>2 and ( float(max) > 0.9 * float(totalMax) ):
      edge = pydot.Edge(str(sender),str(receiver), label="(" + str(count) + "," + str(max) + "," + str(float(average) / float(count)) + ")", fontsize=myfontsize, labelfontcolor="blue" )
      sparseMaxGraph.add_edge(edge)
      

sparseAverageGraph.add_node(pydot.Node( "Threshold on average graph weight=" + str(float(totalAverage) / float(totalCount))))
sparseAverageGraph.write_png(sys.argv[1]+'.boundary-exchange-sparse-average.png', prog='fdp')

sparseMaxGraph.add_node(pydot.Node( "Threshold on max graph weight, 10% of " + str(totalMax) ))
sparseMaxGraph.write_png(sys.argv[1]+'.boundary-exchange-sparse-max.png', prog='fdp')

#dot
#twopi
#neato
#fdp

