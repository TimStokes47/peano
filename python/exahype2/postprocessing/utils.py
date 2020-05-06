# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org



def linear_runtime_trend_line(x_data, y_data):
  """
    We assume this is performance data, i.e. x_data denotes the 
    number of cores or ranks, and y_data are runtimes. We give
    only calibrated y data back. x_data has to be ordered.
  """
  result = []
  for i in range(0,len(x_data)):
    result.append( y_data[0]/x_data[i] )
  return result
