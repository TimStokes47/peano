import pylab


def plot2dDomainDecompositionOnLevel(l,numberOfRanks,domainoffset,domainsize,offset,volume,levels,outputFileName):
  Colors = [ "#ddeedd", "#ccbbcc", "#aabbaa", "#887788" ]
  print "plot domain decomposition on level " + str(l),
  pylab.clf()
  pylab.figure(figsize=(float(domainsize[0]),float(domainsize[1])))
  colorCounter = 0
  for i in range(0,numberOfRanks):
    if levels[i]==l:
      print ".",
      
      pylab.gca().add_patch(pylab.Rectangle(offset[i]+(volume[i][0]*0.01,volume[i][0]*0.01), volume[i][0]*0.98, volume[i][1]*0.98, color=Colors[colorCounter % len(Colors)]))
      
      chosenFontSize = int(round(float(domainsize[0])*12.0/(l)))
      colorCounter   = colorCounter + 1
      
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
  

def plot3dDomainDecompositionOnLevel(l,numberOfRanks,domainoffset,domainsize,offset,volume,levels,outputFileName):
  Colors = [ "#ff1122", "#11ff22", "#1122ff", "#ffee11", "#ff11cc", "#11ddff" ]
  print "plot domain decomposition on level " + str(l),
  pylab.clf()
     
  #fig = pylab.figure()
  #ax = fig.add_subplot(111, projection='3d')
  
  NumberOfPartitionsPerPlot = len(Colors)
  NumberOfDifferentPlots    = int(round(numberOfRanks/NumberOfPartitionsPerPlot+0.5))
  numberOfPartitionsPlottedInThisSubfigure = 0
  currentSubPlot                           = 0
  
  pylab.figure(figsize=(float(domainsize[0]),float(domainsize[1])*NumberOfDifferentPlots))
  
  
  colorCounter = 0
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
  
      myColor      = Colors[colorCounter % len(Colors)]
      
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
      colorCounter = colorCounter + 1
  
  pylab.savefig( outputFileName + ".level" + str(l) + ".png", transparent = True, bbox_inches = 'tight', pad_inches = 0, dpi=80 )
  pylab.savefig( outputFileName + ".level" + str(l) + ".pdf", transparent = True, bbox_inches = 'tight', pad_inches = 0 )
  print "done"
    

def printNodeTable(outputFile,numberOfRanks,parents,nodes):
 NodeColors = [ 
  "#ff0000", "#00ff00", "#6767ff", 
  "#ffff00", "#ff00ff", "#00ffff",
  "#ababab", 
  "#ab55ab", "#abab55", 
  "#5555ab", "#55ab55", "#ab5555",
  "#121212",
  "#cc9966", "#99cc66", "#66cc99",  
  "#cc6699", "#9966cc", "#6699cc" 
 ]

 outputFile.write( "<table border=\"1\">" )

 outputFile.write( "<tr>" )
 outputFile.write( "<td><b>Rank (master)</b></td>" )
 outputFile.write( "<td><b>Node</b></td>" )
 outputFile.write( "<td><b>Children (worker)</b></td>" )
 outputFile.write( "</tr>" )

 for nodeNumber in range(0,numberOfRanks):
  outputFile.write( "<tr>" )
  outputFile.write( "<td bgcolor=\"" + NodeColors[nodeNumber%len(NodeColors)] + "\">" + str(nodeNumber) + "</td>" )
  outputFile.write( "<td bgcolor=\"" + NodeColors[nodeNumber%len(NodeColors)] + "\">" + nodes[nodeNumber] + "</td>" )
  for c in range(0,numberOfRanks):
   if parents[c]==nodeNumber:
     outputFile.write( "<td bgcolor=\"" + NodeColors[c%len(NodeColors)] + "\">->" + str(c) + "</td>" )
  outputFile.write( "</tr>" )
 outputFile.write( "</table>" )
