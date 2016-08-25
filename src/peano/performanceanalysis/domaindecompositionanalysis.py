import argparse
from argparse import RawTextHelpFormatter

import pylab
import networkx
import datetime
import performanceanalysisroutines


########################################################################
# START OF THE PROGRAM
########################################################################
# For the meaning of the command line arguments, see the documentation
# of the function 'plot_multithreading_adapter_scaling' above.
help = '''
Creates a brief overview of the domain decomposition
\n\n
Sample usages:\n
python domain-decomposition-analysis.py -file 112x16-0.results -dimension 2 -domainoffset 0.0 0.0 -domainsize 15.0 15.0\n
'''

parser = argparse.ArgumentParser(description=help,formatter_class=RawTextHelpFormatter)
parser.add_argument('-file',required=True,help="Input file")
parser.add_argument('-dimension',required=True,help="Dimension of problem. Either 2 or 3.")
parser.add_argument('-domainoffset',nargs="+",required=True,help="Offset of bounding box.")
parser.add_argument('-domainsize',nargs="+",required=True,help="Size of domain's bounding box.")
args   = parser.parse_args();


outputFileName  = args.file + ".html"
outFile        = open( outputFileName, "w" )
outFile.write( 
  "<html>\
     <body>\
     <h1>Peano Domain Decomposition Report</h1>\
     <p>Report time stamp: " + str(datetime.datetime.utcnow()) + "</p>\
     <p>Author: Tobias Weinzierl</p>\
     <p>Data file: " + args.file + "</p>\
  ")


dim = int(args.dimension)


numberOfRanks = performanceanalysisroutines.getNumberOfRanks(args.file)

print "prepare data structures ",
(volume,offset,level) = performanceanalysisroutines.prepareDomainDecompositionDataStructures(numberOfRanks,dim)
print "done"


performanceanalysisroutines.plotLogicalTopology(args.file,numberOfRanks);
#pylab.clf()
#pylab.title( "Logical topology" )
#
#topologyGraph = networkx.MultiDiGraph()
#for rank in range(0,numberOfRanks):
#  topologyGraph.add_node(str(rank))##
#
#print "parse input file ",
#parents  = [-1 for x in range(0,numberOfRanks)]
#maxLevel = 0
#try:
#  inputFile = open( args.file,  "r" )#
#
#  for line in inputFile:
#    if "start node for subdomain" in line:
#      print ".",
#      child = int(line.split( "rank:" )[1].split( " " )[0])
#      parent = int(line.split( "with master" )[1])
#      topologyGraph.add_edge(child,parent)
#      parents[child]=parent
#      fragments = performanceanalysisroutines.getBoundingBox(line) 
#      if dim==2:
#        offset[child] = ( float(fragments[0]),float(fragments[1]) )
#        volume[child] = ( float(fragments[2]),float(fragments[3]) )
#      else:
#        offset[child] = ( float(fragments[0]),float(fragments[1]),float(fragments[2]) )
#        volume[child] = ( float(fragments[3]),float(fragments[4]),float(fragments[5]) )
#      level[child] = int(line.split( "on level" )[1].split( " with" )[0])
#      maxLevel = max(maxLevel,level[child])
#  print " done"
#except Exception as inst:
#  print "failed to read " + args.file
#  print inst
##
#
#print "Plot logical topology ",
#try:
#  pos=networkx.graphviz_layout(topologyGraph,prog='twopi',args='')
#  #pos=networkx.graphviz_layout(topologyGraph,prog='dot',args='')
#except:
#  pos=networkx.spring_layout(topologyGraph)
#networkx.draw(
#  topologyGraph,
#  pos,
#  with_labels=True,
#  node_color='#667766',
#  node_size=10,
#  alpha=0.2
#)
#pylab.savefig( args.file + ".topology.png" )
#pylab.savefig( args.file + ".topology.pdf" )
outFile.write( "<h2>Logical topology</h2>" )
outFile.write( "<img src=\"" + args.file + ".topology.png\" />" )
print " done "

@todo Links auf die grosse Version


print "compute volumes ",
volumes = [0.0 for x in offset]
for i in range(0,numberOfRanks):
  print ".",
  if dim==2:
    volumes[i] = volume[i][0]*volume[i][1]
  else:
    volumes[i] = volume[i][0]*volume[i][1]*volume[i][2]
print " done "


print "compute volume overlap with domain ",
overlaps = [0.0 for x in offset]
for i in range(0,numberOfRanks):
  print ".",
  overlaps[i] = 1.0
  for d in range(0,dim):
    left  = max(offset[i][d],float(args.domainoffset[d]))
    right = min(offset[i][d]+volume[i][d],float(args.domainoffset[d])+float(args.domainsize[d]))
    delta = right-left
    if delta<0:
      print "ERROR for rank " + str(i) +  \
            ": region=" + str(offset[i]) + "x" + str(volume[i]) + \
            ", bonding box=" + str(args.domainoffset) + "x" + str(args.domainsize) + \
            ", delta= " + str(delta) 
    overlaps[i] = overlaps[i] * delta
  if overlaps[i]>volume[i]:
    print "ERROR for rank " + str(i) +  \
          ": region=" + str(offset[i]) + "x" + str(volume[i]) + \
          ", bounding box=" + str(args.domainoffset) + "x" + str(args.domainsize) + \
          ", volume= " + str(volumes[i]) + ", overlaps=" + str(overlaps[i]) 
print " done "


print "compute actual work ",
work = [x for x in overlaps]
for i in range(1,numberOfRanks):
  work[ parents[i] ] = work[ parents[i] ] - overlaps[i]
print " done "

work[0]=0.0

print "validate data ",
for i in range(0,numberOfRanks):
  print ".",
  if overlaps[i]<0.0:
    print "\nERROR for rank " + str(i) +  \
          ": region=" + str(offset[i]) + "x" + str(volume[i]) + \
          ", bounding box=" + str(args.domainoffset) + "x" + str(args.domainsize) + \
          ", volume= " + str(volumes[i]) + ", overlaps=" + str(overlaps[i]) 
    print "overlaps may not be negative"
  if volumes[i]<0.0:
    print "\nERROR for rank " + str(i) +  \
          ": region=" + str(offset[i]) + "x" + str(volume[i]) + \
          ", bounding box=" + str(args.domainoffset) + "x" + str(args.domainsize) + \
          ", volume= " + str(volumes[i]) + ", overlaps=" + str(overlaps[i]) 
    print "volumes may not be negative"
  if overlaps[i]>volume[i]:
    print "\nERROR for rank " + str(i) +  \
          ": region=" + str(offset[i]) + "x" + str(volume[i]) + \
          ", bounding box=" + str(args.domainoffset) + "x" + str(args.domainsize) + \
          ", volume= " + str(volumes[i]) + ", overlaps=" + str(overlaps[i]) 
    print "overlaps have to be bigger than volume"
  if work<0:
    print "\nERROR for rank " + str(i) +  \
          ": region=" + str(offset[i]) + "x" + str(volume[i]) + \
          ", bounding box=" + str(args.domainoffset) + "x" + str(args.domainsize) + \
          ", volume= " + str(volumes[i]) + ", overlaps=" + str(overlaps[i]) + ", work=" + str(work[i])
    print "work has to be positive"
print " done "





print "plot ",
pylab.clf()
if numberOfRanks>32:
  pylab.figure(figsize=(numberOfRanks/10,4))
ranks = [x for x in range(0,numberOfRanks)]

print ".",
_markevery = numberOfRanks
if numberOfRanks>11:
  _markevery = _markevery / 11
pylab.plot(ranks, volumes, '-o', label="area of responsibility", markevery=_markevery, color='#000000', markersize=10)

print ".",
_markevery = numberOfRanks
if numberOfRanks>11:
  _markevery = _markevery / 11
pylab.plot(ranks, overlaps, '-s', label="area of responsibility $\cap \Omega$", markevery=_markevery, color='#ff0000', markersize=10)

print ".",
#pylab.plot(ranks, work, '-v', label="work", markevery=numberOfRanks/12, color='#0000bb', markersize=10)
pylab.fill_between(ranks, work, color='#0000bb', alpha=0.4)

maxLocalWork = max(work)
for i in range(0,numberOfRanks):
  if work[i]>maxLocalWork/10:
    pylab.text(i,volumes[i]+10,str(i))
    pylab.plot([i,i], [0, volumes[i]], '--', color="#000000")

pylab.xlabel('rank')
pylab.ylabel('$\Omega $')
try:
  pylab.legend(fontsize=9, framealpha=0.5)
except:
  # old pylab version
  l = pylab.legend(prop={'size':9})
  l.get_frame().set_alpha(0.5)
ax = pylab.gca()
ax.autoscale_view()
ax.set_yscale('symlog', basey=10)
pylab.savefig( args.file + ".work.png" )
pylab.savefig( args.file + ".work.pdf" )
outFile.write( "<h2>Work statistics</h2>" )
outFile.write( "<img src=\"" + args.file + ".work.png\" />" )
outFile.write( "<p>The filled region is the actual local work volume of a rank. It has to be smaller than the region of responsibility that might overlap the actual domain.</p>" )
print "done"


if dim==2:
 outFile.write( "<h2>Domain decomposition (level by level)</h2>" )
 for l in range(1,maxLevel+1):
  print "plot domain decomposition on level " + str(l),
  pylab.clf()
  pylab.figure(figsize=(float(args.domainsize[0]),float(args.domainsize[1])))
  for i in range(0,numberOfRanks):
    if level[i]==l:
      print ".",
      pylab.gca().add_patch(pylab.Rectangle(offset[i]+(volume[i][0]*0.01,volume[i][0]*0.01), volume[i][0]*0.98, volume[i][1]*0.98, color="#aabbaa"))
      
      pylab.text(
        offset[i][0] + volume[i][0]/2,
        offset[i][1] + volume[i][1]/2,
        str(i)
      )
      #print "\noffset=" + str(offset[i]) + ", volume=" + str(volume[i]),
  print ".",
  pylab.xlim( float(args.domainoffset[0]), float(args.domainoffset[0])+float(args.domainsize[0]) )
  pylab.ylim( float(args.domainoffset[1]), float(args.domainoffset[1])+float(args.domainsize[1]) )
  pylab.savefig( args.file + ".level" + str(l) + ".png" )
  pylab.savefig( args.file + ".level" + str(l) + ".pdf" )
  outFile.write( "<img src=\"" + args.file + ".level" + str(l) + ".png\" />" )
  print "done"
 
 
outFile.write( "</html>" )

  