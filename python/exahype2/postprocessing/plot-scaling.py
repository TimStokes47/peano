# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import argparse
import tarfile
import os
import exahype2


import matplotlib.pyplot as plt


if __name__ == "__main__":
  parser = argparse.ArgumentParser(description='ExaHyPE2 scaling plotter')
  parser.add_argument("file",   help="filename of the file to parse (should be a tar.gz file)")
  parser.add_argument("-v", "--verbose", help="increase output verbosity", default=False)
  args = parser.parse_args()

  open_mode = ""  
  if args.file.endswith( "tar.gz"):
    open_mode = "r:gz"
  elif args.file.endswith( "tar" ):
    open_mode = "r:"
  else:
    print( "Warning: passed file should end with tar.gz or tar" )

  tar = tarfile.open( args.file, "r:gz" )
  data_files = tar.getnames()
  
  data_points = []
  for data_file in data_files:
    print( "parse " + data_file )
    tar.extract( data_file )
    new_data = exahype2.postprocessing.PerformanceDataPoint(data_file,args.verbose)
    if new_data.valid:
      data_points.append( new_data ) 
    os.remove( data_file )

  plt.clf()

  (x_data, y_data) = exahype2.postprocessing.extract_grid_construction_times( data_points )    
  plt.plot( x_data, y_data, label="total grid construction (incl. initial lb)" )

  (x_data, y_data) = exahype2.postprocessing.extract_times_per_step( data_points )    
  plt.plot( x_data, y_data, label="time per time step" )

  plt.plot( x_data, exahype2.postprocessing.linear_runtime_trend_line(x_data,y_data), "--", label="linear trend" )
  
  plt.xlabel( "Threads" )
  plt.ylabel( "Time [t]=s" )
  plt.xscale( "log", basex=2 )
  #plt.yscale( "log", basey=2 )
  #plt.xscale( "log" )
  plt.yscale( "log" )
  xtics   = [ 1 ]
  xlabels = [ "serial" ]
  while xtics[-1] < x_data[-1]:
    xtics.append( xtics[-1]*2 )
    xlabels.append( str(xtics[-1]) )
  plt.xticks( xtics, xlabels )
  plt.legend()
  filename = args.file.replace( ".tar.gz", "").replace( ".tar", "" )
  plt.savefig( filename + ".pdf" )
  plt.savefig( filename + ".png" )
  
  tar.close()
