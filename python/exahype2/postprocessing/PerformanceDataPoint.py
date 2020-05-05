# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
from fileinput import filename




class PerformanceDataPoint(object):
  def __init__(self,file_name,verbose):
    self._file_name            = file_name
    self._verbose              = verbose 
    self._start_time_stepping  = -1
    self._end_time_stepping    = -1
    self._threads              = 1
    self._number_of_time_steps = 0
    self.valid                 = False
    self.parse()
    pass

  def convert_to_seconds(self,data):
    """
   
     Hand in a string in nanoseconds and return double as s
    
    """
    result = float(data)
    return result / 1000 / 1000 / 1000


  def parse(self):
    file       = open(self._file_name, "r")
    self.valid = True
    try:
      for line in file:
        if "build:" in line:
          if "2d" in line:
            self._d = 2
          if "3d" in line:
            self._d = 3
          if "tbb" in line or "omp" in line:
            self._threads = int( line.split( "threads" )[0].split( "(")[-1] )
            if self._verbose:
              print( "- threads = " + str(self._threads))
        if "run TimeStep" in line and self._start_time_stepping<0:
          self._start_time_stepping = self.convert_to_seconds(line.strip().split( " " )[0])
          if self._verbose:
            print( "- grid construction finished after " + str(self._start_time_stepping))
        elif "run" in line:
          self._end_time_stepping = self.convert_to_seconds(line.strip().split( " " )[0])
        
        if "run TimeStep" in line:
          self._number_of_time_steps += 1
      self._end_time_stepping = self._end_time_stepping - self._start_time_stepping
      if self._verbose:
        print( "- time stepping finished after " + str(self._end_time_stepping))
    except:
      print( "parsing failed")
      self.valid = False
    
    if self._number_of_time_steps<=0:
      print( "file " + self._file_name + " is invalid as number of time steps equals zero" )
      self.valid = False
      
      
def extract_grid_construction_times(performance_data_points):
  """
     
   Returns a tuple of arrays to be plotted
    
  """
  x_data = []
  y_data = []
    
  for point in performance_data_points:
    x_value = point._threads
    insert_at_position = 0
    while insert_at_position<len(x_data) and x_data[insert_at_position]<x_value:
      insert_at_position += 1
    x_data.insert( insert_at_position, x_value )
    y_data.insert( insert_at_position, point._start_time_stepping )
    
  return (x_data,y_data)


def extract_times_per_step(performance_data_points):
  """
     
   Returns a tuple of arrays to be plotted
    
  """
  x_data = []
  y_data = []
    
  for point in performance_data_points:
    x_value = point._threads
    insert_at_position = 0
    while insert_at_position<len(x_data) and x_data[insert_at_position]<x_value:
      insert_at_position += 1
    x_data.insert( insert_at_position, x_value )
    y_data.insert( insert_at_position, point._end_time_stepping/point._number_of_time_steps )
    
  return (x_data,y_data)


