import pylab


def getNodeColor(c,nodes):
 NodeColors = [ 
  "#ff0000", "#00ff00", "#0000ff", 
  "#ffff00", "#ff00ff", "#00ffff", 
  "#ff6767", "#67ff67", "#6767ff", 
  "#ffff67", "#ff67ff", "#67ffff" 
  "#ababab", 
  "#ab55ab", "#abab55", 
  "#5555ab", "#55ab55", "#ab5555",
  "#cc9966", "#99cc66", "#66cc99",  
  "#cc6699", "#9966cc", "#6699cc"
 ]
 return NodeColors[nodes.index(nodes[c])%len(NodeColors)]
    

   
    
def plot2dDomainDecompositionOnLevel(l,numberOfRanks,domainoffset,domainsize,offset,volume,levels,nodes,outputFileName):
  print "plot domain decomposition on level " + str(l),
  pylab.clf()
  pylab.figure(figsize=(float(domainsize[0]),float(domainsize[1])))
  for i in range(0,numberOfRanks):
    if levels[i]==l:
      print ".",
      
      pylab.gca().add_patch(pylab.Rectangle(offset[i]+(volume[i][0]*0.01,volume[i][0]*0.01), volume[i][0]*0.98, volume[i][1]*0.98, color=getNodeColor(i,nodes)))
      
      chosenFontSize = int(round(float(domainsize[0])*12.0/(l)))
      
      pylab.text(
        offset[i][0] + volume[i][0]/2,
        offset[i][1] + volume[i][1]/2,
        str(i), fontsize=chosenFontSize
      )
  print ".",
  pylab.xlim( float(domainoffset[0]), float(domainoffset[0])+float(domainsize[0]) )
  pylab.ylim( float(domainoffset[1]), float(domainoffset[1])+float(domainsize[1]) )
  pylab.savefig( outputFileName + ".level" + str(l) + ".png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( outputFileName + ".level" + str(l) + ".pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  print "done"
  
     
def plot2dDomainDecomposition(numberOfRanks,domainoffset,domainsize,offset,volume,levels,nodes,outputFileName):
  print "plot domain decomposition ",
  pylab.clf()
  pylab.figure(figsize=(float(domainsize[0]),float(domainsize[1])))
  for l in range(0,128):
   for i in range(0,numberOfRanks):
    if levels[i]==l:
      print ".",
      
      pylab.gca().add_patch(pylab.Rectangle(offset[i]+(volume[i][0]*0.01,volume[i][0]*0.01), volume[i][0]*0.98, volume[i][1]*0.98, color=getNodeColor(i,nodes)))
  print ".",
  pylab.xlim( float(domainoffset[0]), float(domainoffset[0])+float(domainsize[0]) )
  pylab.ylim( float(domainoffset[1]), float(domainoffset[1])+float(domainsize[1]) )
  pylab.savefig( outputFileName + ".png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( outputFileName + ".pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  print "done"
    

def plot3dDomainDecompositionOnLevel(l,numberOfRanks,domainoffset,domainsize,offset,volume,levels,nodes,outputFileName):
  print "plot domain decomposition on level " + str(l),
  pylab.clf()
     
  #fig = pylab.figure()
  #ax = fig.add_subplot(111, projection='3d')
  
  NumberOfPartitionsPerPlot = len(Colors)
  NumberOfDifferentPlots    = int(round(numberOfRanks/NumberOfPartitionsPerPlot+0.5))
  numberOfPartitionsPlottedInThisSubfigure = 0
  currentSubPlot                           = 0
  
  pylab.figure(figsize=(float(domainsize[0]),float(domainsize[1])*NumberOfDifferentPlots))
  
  try:
   ax = pylab.subplot2grid((NumberOfDifferentPlots,1), (0,0), projection='3d')
   ax.set_xlim3d( float(domainoffset[0]), float(domainoffset[0])+float(domainsize[0]) )
   ax.set_ylim3d( float(domainoffset[1]), float(domainoffset[1])+float(domainsize[1]) )
   ax.set_zlim3d( float(domainoffset[2]), float(domainoffset[2])+float(domainsize[2]) )
  except:
   pass
  for i in range(0,numberOfRanks):
    if (numberOfPartitionsPlottedInThisSubfigure>NumberOfPartitionsPerPlot):
      numberOfPartitionsPlottedInThisSubfigure = 0
      try:
       ax = pylab.subplot2grid((NumberOfDifferentPlots,1), (currentSubPlot,0), projection='3d')
       ax.set_xlim3d( float(domainoffset[0]), float(domainoffset[0])+float(domainsize[0]) )
       ax.set_ylim3d( float(domainoffset[1]), float(domainoffset[1])+float(domainsize[1]) )
       ax.set_zlim3d( float(domainoffset[2]), float(domainoffset[2])+float(domainsize[2]) )
      except:
       pass
      currentSubPlot  = currentSubPlot + 1
   
    if levels[i]==l:
      print ".",
  
      myColor      = getNodeColor(i,nodes)
      
      NumberOfLines = 20
      for xCount in range(0,NumberOfLines):
        x = offset[i][0] + volume[i][0] * xCount / NumberOfLines 
        ax.plot3D([x,x],[offset[i][1]+volume[i][1], offset[i][1]], [offset[i][2]+volume[i][2]/2,offset[i][2]+volume[i][2]/2],color=myColor)
      ## bars along z
      ax.plot3D([offset[i][0],             offset[i][0]],             [offset[i][1], offset[i][1]], [offset[i][2],offset[i][2]+volume[i][2]],color=myColor, linewidth=2.0)
      ax.plot3D([offset[i][0]+volume[i][0],offset[i][0]+volume[i][0]],[offset[i][1], offset[i][1]], [offset[i][2],offset[i][2]+volume[i][2]],color=myColor, linewidth=2.0)
      ax.plot3D([offset[i][0],             offset[i][0]],             [offset[i][1]+volume[i][1], offset[i][1]+volume[i][1]], [offset[i][2],offset[i][2]+volume[i][2]],color=myColor, linewidth=2.0)
      ax.plot3D([offset[i][0]+volume[i][0],offset[i][0]+volume[i][0]],[offset[i][1]+volume[i][1], offset[i][1]+volume[i][1]], [offset[i][2],offset[i][2]+volume[i][2]],color=myColor, linewidth=2.0)
      ## bars along x
      ax.plot3D([offset[i][0],     offset[i][0]+volume[i][0]], [offset[i][1],              offset[i][1]],              [offset[i][2],              offset[i][2]],              color=myColor, linewidth=2.0)
      ax.plot3D([offset[i][0],     offset[i][0]+volume[i][0]], [offset[i][1],              offset[i][1]],              [offset[i][2]+volume[i][2], offset[i][2]+volume[i][2]], color=myColor, linewidth=2.0)
      ax.plot3D([offset[i][0],     offset[i][0]+volume[i][0]], [offset[i][1]+volume[i][1], offset[i][1]+volume[i][1]], [offset[i][2],              offset[i][2]],              color=myColor, linewidth=2.0)
      ax.plot3D([offset[i][0],     offset[i][0]+volume[i][0]], [offset[i][1]+volume[i][1], offset[i][1]+volume[i][1]], [offset[i][2]+volume[i][2], offset[i][2]+volume[i][2]], color=myColor, linewidth=2.0)
      ## bars along y
      ax.plot3D([offset[i][0],              offset[i][0]],              [offset[i][1],  offset[i][1]+volume[i][1]], [offset[i][2],              offset[i][2]],              color=myColor, linewidth=2.0)
      ax.plot3D([offset[i][0],              offset[i][0]],              [offset[i][1],  offset[i][1]+volume[i][1]], [offset[i][2]+volume[i][2], offset[i][2]+volume[i][2]], color=myColor, linewidth=2.0)
      ax.plot3D([offset[i][0]+volume[i][0], offset[i][0]+volume[i][0]], [offset[i][1],  offset[i][1]+volume[i][1]], [offset[i][2],              offset[i][2]],              color=myColor, linewidth=2.0)
      ax.plot3D([offset[i][0]+volume[i][0], offset[i][0]+volume[i][0]], [offset[i][1],  offset[i][1]+volume[i][1]], [offset[i][2]+volume[i][2], offset[i][2]+volume[i][2]], color=myColor, linewidth=2.0)

      chosenFontSize = 12
      ax.text(offset[i][0]+volume[i][0]/2, offset[i][1]+volume[i][1]/2, offset[i][2]+volume[i][2]/2, str(i), "y", fontsize=chosenFontSize)      

      numberOfPartitionsPlottedInThisSubfigure = numberOfPartitionsPlottedInThisSubfigure+1      
  
  pylab.savefig( outputFileName + ".level" + str(l) + ".png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( outputFileName + ".level" + str(l) + ".pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  print "done"
    
        

def printNodeTable(outputFile,numberOfRanks,parents,nodes):
 outputFile.write( "<table border=\"1\">" )

 outputFile.write( "<tr>" )
 outputFile.write( "<td><b>Rank (master)</b></td>" )
 outputFile.write( "<td><b>Node</b></td>" )
 outputFile.write( "<td><b>Children (worker)</b></td>" )
 outputFile.write( "</tr>" )

 for nodeNumber in range(0,numberOfRanks):
  outputFile.write( "<tr>" )
  outputFile.write( "<td bgcolor=\"" + getNodeColor(nodeNumber,nodes) + "\">" + str(nodeNumber) + "</td>" )
  outputFile.write( "<td bgcolor=\"" + getNodeColor(nodeNumber,nodes) + "\">" + nodes[nodeNumber] + "</td>" )
  for c in range(0,numberOfRanks):
   if parents[c]==nodeNumber:
     outputFile.write( "<td bgcolor=\"" + getNodeColor(c,nodes) + "\">->" + str(c) + "</td>" )
  outputFile.write( "</tr>" )
 outputFile.write( "</table>" )

 