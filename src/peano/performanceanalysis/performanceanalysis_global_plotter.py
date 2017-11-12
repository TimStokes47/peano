import matplotlib
matplotlib.use('Agg') # Must be before importing matplotlib.pyplot or pylab!


import pylab
import networkx 


import performanceanalysis_output


def drawTreeGraph(myGraph):
  try:
    from networkx import graphviz_layout
    pos=networkx.graphviz_layout(myGraph,prog='twopi',args='')
  except:
    try:
      pos=networkx.graphviz_layout(myGraph,prog='dot',args='')
    except:
      pos=networkx.spring_layout(myGraph)
      print "fall back to spring layout. Nicer graphs might result if either compatible PyGraphviz or Pydot packages were available"
  networkx.draw_networkx(
    myGraph,
    pos,
    with_labels=True,
    arrows=True,
    node_color='#667766',
    node_size=10,
    alpha=0.2
  )


def plotLogicalTopology(numberOfRanks,inputFileNamePlusPath,parents,levels,offset,volume):
  topologyGraph = networkx.DiGraph()
  for c in range(1,numberOfRanks):
    topologyGraph.add_edge(c,parents[c])

  pylab.clf()
  pylab.title( "Logical topology" )
  drawTreeGraph(topologyGraph)
  pylab.savefig( inputFileNamePlusPath + ".png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( inputFileNamePlusPath + ".pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )

    
def plotWorkloadAndResponsibilityDistributionPerRank(numberOfRanks,outputFileName,volumes,overlaps,work):
 print "plot workload distribution per rank",
 pylab.clf()
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
 pylab.fill_between(ranks, work, color='#0000bb', alpha=0.4)

 maxLocalWorkToGetExtraLabel = 0
 oldRanksWithLabels  = -1
 ranksWithLabels     = len(work)
 while ranksWithLabels > numberOfRanks/10 and oldRanksWithLabels != ranksWithLabels:
   oldRanksWithLabels = ranksWithLabels
   maxLocalWorkToGetExtraLabel = 0.5 * maxLocalWorkToGetExtraLabel + 0.5 * max(work)
   ranksWithLabels = sum( i>maxLocalWorkToGetExtraLabel for i in work )
 
 for i in range(0,numberOfRanks):
   if work[i]>maxLocalWorkToGetExtraLabel:
     pylab.text(i,volumes[i]+10,str(i))
     pylab.plot([i,i], [0, volumes[i]], '--', color="#000000") 

 ranksWithZeroResponsibility = 0
 while len(volumes)>ranksWithZeroResponsibility and volumes[ranksWithZeroResponsibility]==0.0:
   ranksWithZeroResponsibility = ranksWithZeroResponsibility + 1

 if ranksWithZeroResponsibility>2 and len(volumes)>ranksWithZeroResponsibility:
   pylab.text(ranksWithZeroResponsibility,volumes[ranksWithZeroResponsibility]+20,"ranks_per_node")
   pylab.plot([ranksWithZeroResponsibility,ranksWithZeroResponsibility], [0, volumes[ranksWithZeroResponsibility]], '-', color="#000000") 
 
 try:
   pylab.legend(fontsize=9, framealpha=0.5)
 except:
   # old pylab version
   l = pylab.legend(prop={'size':9})
   l.get_frame().set_alpha(0.5)
 ax = pylab.gca()
 ax.autoscale_view()
 ax.set_yscale('symlog', basey=10)

 pylab.xlabel('rank')  
 pylab.ylabel('$\Omega $')
 pylab.savefig( outputFileName + ".symlog.png", transparent = True )
 pylab.savefig( outputFileName + ".symlog.pdf", transparent = True )


 ax.set_yscale('symlog', basey=10)

 #if numberOfRanks>32:
 # pylab.figure(figsize=(numberOfRanks/10,4))

 ax.set_yscale('linear' )
 pylab.xlabel('rank')  
 pylab.ylabel('$\Omega $')
 pylab.savefig( outputFileName + ".png" )
 pylab.savefig( outputFileName + ".pdf" )
 print "done"



    
def plotWorkloadAndResponsibilityDistributionPerNode(numberOfRanks,outputFileName,work,nodes):
 print "plot workload distribution per node",
 pylab.clf()

 data = {nodes[i]:0 for i in range(0,numberOfRanks)}
 
 for i in range(0,numberOfRanks):
   print ".",
   data[ nodes[i] ] = data[ nodes[i] ] + work[i]
   
 pylab.bar(range(len(data)), data.values(), align='center')
 pylab.xticks(range(len(data)), data.keys())

 ax = pylab.gca()
 ax.autoscale_view()
 ax.set_yscale('symlog', basey=10)

 pylab.xlabel('rank')  
 pylab.ylabel('$\Omega $')
 pylab.savefig( outputFileName + ".symlog.png", transparent = True )
 pylab.savefig( outputFileName + ".symlog.pdf", transparent = True )


 ax.set_yscale('linear' )
 pylab.xlabel('rank')  
 pylab.ylabel('$\Omega $')
 pylab.savefig( outputFileName + ".png" )
 pylab.savefig( outputFileName + ".pdf" )
 print "done"

