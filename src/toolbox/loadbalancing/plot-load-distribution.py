# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import argparse
import os


import matplotlib.pyplot as plt


#
# @todo Wir sollten nach Ranks unterscheiden und fuer jeden Rank einmal plotten (mit anderer Farbe)
#
def parse_file( filename ):
  """
    Returns tuple of (x_data,y_data_local,y_data_remote)
    
    x_data is a series of time stamps
    y_data is a series of corresponding entries. Each entry is an array again as there might be many trees
    
  """
  file = open( filename, "r" )
  x_data        = []
  y_data_local  = []
  y_data_remote = []
  for line in file:
    if "toolbox::loadbalancing::dumpStatistics" in line:
      x_data.append( float( line.strip().split(" ")[0] ) )
      y_data_local.append( [] )
      y_data_remote.append( [] )
      for entry in line.split( "(#")[1:-1]:
        y_data_local[-1].append( float(entry.split(":")[1].split("/")[0]) )
        y_data_remote[-1].append( float(entry.split("/")[1].split(")")[0]) )
  return ( x_data, y_data_local, y_data_remote )


if __name__ == "__main__":
  parser = argparse.ArgumentParser(description='Peano 4 load balancing plotter')
  parser.add_argument("file",   help="filename of the file to parse (should be a text file)")
  parser.add_argument("-v", "--verbose", help="increase output verbosity", default=False)
  args = parser.parse_args()

  plt.clf()

  ( x_data, y_data_local, y_data_remote ) = parse_file( args.file )

  #
  # Should be one colour per rank
  #
  Colours = [ "#0000ff" ]
  Symbols = [ "o" ]
  for i in range(0,len(x_data)):
    one_snapshot_x_data = [ x_data[i] for j in y_data_local[i]]
    if i==0:
      plt.scatter(one_snapshot_x_data, y_data_local[i], marker=Symbols[0], color=Colours[0],  alpha=0.2, label="Rank 1" )  
    else:
      plt.scatter(one_snapshot_x_data, y_data_local[i], marker=Symbols[0], color=Colours[0],  alpha=0.2)  
    plt.scatter(one_snapshot_x_data, y_data_remote[i], marker=Symbols[0], color=Colours[0], alpha=0.2, facecolors='none')  


  plt.xlabel( "time" )
  plt.ylabel( "cells per tree" )
  plt.yscale( "log", basey=2 )
  #plt.yscale( "log" )
  plt.legend()
  plt.savefig( args.file + ".pdf" )
  plt.savefig( args.file + ".png" )
  
