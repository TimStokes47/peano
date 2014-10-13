import sys
import re
import pylab 


searchPatternNumberOfCells = ".*peano::performanceanalysis::DefaultAnalyser::endIteration.*cells=\("



numberOfInnerLeafCells  = {}
numberOfInnerCells      = {}
numberOfOuterLeafCells  = {}
numberOfOuterCells      = {}
  



if ( len(sys.argv)<=2 ):
  print "please specify input file name and number of ranks"
else:
  inputFileName   = sys.argv[1]
  outputFileName  = sys.argv[1] + ".html"
  numberOfRanks   = int(sys.argv[2])

  outFile        = open( outputFileName, "w" )
  
  #
  # Header of report
  #
  print "read input file " + inputFileName + " for " + str(numberOfRanks) + " rank(s)"
  outFile.write( "<html>" )
  outFile.write( "<body>" )
  
  outFile.write( "<h1>Peano Performance Report</h1>" )
  outFile.write( "<p>Peano's default performance analysis</p>" )
  outFile.write( "<p>Author: Tobias Weinzierl</p>" )
  outFile.write( "<p>Data file: " + inputFileName + "</p>" )
  outFile.write( "<p>Ranks: " + str(numberOfRanks) + "</p>" )

  outFile.write( "<center><img src=\"http://www.peano-framework.org/logo.png\" />" )
  outFile.write( "<br /><a href=\"http://www.peano-framework.org\" >www.peano-framework.org</a></center>" )

  outFile.write( "<h2 id=\"table-of-contents\">Table of contents</h2>" )
  outFile.write( "<ul>" )
  outFile.write( "<li><a href=\"#number-of-cells\">Number of cells</a></li>" )
  outFile.write( "</ul>" )


  #
  # Grid information
  #
  for rank in range(0,numberOfRanks):
    numberOfInnerLeafCells[rank]  = []
    numberOfInnerCells[rank]      = []
    numberOfOuterLeafCells[rank]  = []
    numberOfOuterCells[rank]      = []
    try:
      inputFile = open( inputFileName,  "r" )
      totalVertexUpdates = 0
      for line in inputFile:
        m = re.search( "rank:" + str(rank) + searchPatternNumberOfCells, line )
        if (m):
          token = line.replace("(",",").replace(")","").strip().split(",")
          numberOfInnerLeafCells[rank].append(float(token[-4]))
          numberOfOuterLeafCells[rank].append(float(token[-3]))
          numberOfInnerCells[rank].append(float(token[-2]))
          numberOfOuterCells[rank].append(float(token[-1]))
    except Exception as inst:
      print "failed to read " + inputFileName
      print inst
  
  pylab.clf()
  pylab.title( "Cells" )
  pylab.xlabel('t')
  x = pylab.arange(0, len(numberOfInnerLeafCells[0]), 1.0)
  pylab.plot(x, numberOfInnerLeafCells[0], 'o',  markersize=10, label='#inner leaf cells' )
  pylab.plot(x, numberOfOuterLeafCells[0], '^',  markersize=10, label='#outer leaf cells' )
  pylab.plot(x, numberOfInnerCells[0],     '.',  markersize=10, label='#inner cells' )
  pylab.plot(x, numberOfOuterCells[0],     'x',  markersize=10, label='#outer cells' )
  for rank in range(1,numberOfRanks):
    x = pylab.arange(0, len(numberOfInnerLeafCells[rank]), 1.0)
    pylab.plot(x, numberOfInnerLeafCells[rank], '--',  markersize=10)
    pylab.plot(x, numberOfOuterLeafCells[rank], '--',  markersize=10)
    pylab.plot(x, numberOfInnerCells[rank],     '--',  markersize=10)
    pylab.plot(x, numberOfOuterCells[rank],     '--',  markersize=10)

  pylab.grid(True)
  try:
    pylab.legend(fontsize=9, framealpha=0.5)
  except:
    # old pylab version
    #pylab.legend()

    l = pylab.legend(prop={'size':9})
    l.get_frame().set_alpha(0.5)
  pylab.savefig( "number-of-cells.png")
  pylab.savefig( "number-of-cells.pdf")

  outFile.write( "<h2 id=\"number-of-cells\">Number of cells</h2>" )
  outFile.write( "<img src=\"number-of-cells.png\" />" )

  pylab.clf()
  for rank in range(0,numberOfRanks):
    x = pylab.arange(0, len(numberOfInnerLeafCells[rank]), 1.0)
    pylab.plot(x, numberOfInnerLeafCells[rank], 'o-',  markersize=10, label='#inner leaf cells',     markevery=5)
    pylab.plot(x, numberOfOuterLeafCells[rank], '^-',  markersize=10, label='#outer leaf cells',     markevery=7)
    pylab.plot(x, numberOfInnerCells[rank],     '.-',  markersize=10, label='#inner cells',          markevery=11)
    pylab.plot(x, numberOfOuterCells[rank],     'x-',  markersize=10, label='#outer cells',          markevery=13)
    pylab.grid(True)
    try:
      pylab.legend(fontsize=9, framealpha=0.5)
    except:
      # old pylab version
      #pylab.legend()
      l = pylab.legend(prop={'size':9})
      l.get_frame().set_alpha(0.5)
    pylab.savefig( "number-of-cells-rank-" + str(rank) + ".png")
    pylab.savefig( "number-of-cells-rank-" + str(rank) + ".pdf")
    outFile.write( "<h3>Rank " + str(rank) + "</h3>" )
    outFile.write( "<img src=\"number-of-cells-rank-" + str(rank) + ".png\" />" )


  outFile.write( "<br /><br /><a href=\"#table-of-contents\">To table of contents</a>" )


  #
  # Trailor of report
  #
  print "done"
  outFile.write( "</body>" )
  outFile.write( "</html>" )
  outFile.close()
  