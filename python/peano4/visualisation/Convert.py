# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import subprocess
import os


class Convert(object):
  """
    Convert is a file which wraps around one Peano block file output. Output can 
    be a whole data set or simply one data file.
    
    silent Tell the convert tool to shut up. Doesn't omit all outputs though, as 
      the Python wrapper still write some stats
  """
  
  def __init__(self,file_name, silent = False):
    self.visualisation_tools_path = "."
    self.file_name       = file_name
    self.extension       = ".peano-patch-file"
    self.output_path     = "."
    self.mpi_prefix      = ""
    self.silent          = silent


  def set_input_file_name(self,file_name):
    self.file_name = file_name


  def set_visualisation_tools_path(self, path, mpi_prefix="" ):
    """
      Set path where tool can find the convert script. Convert is part of 
      the Peano4 installation if you have configured your build to use
      VTK (see ./configure script and the option --with-vtk in there).
      
      invocation_prefix If you built your code with MPI, you might have to 
        set an explicit invocation prefix. Otherwise, just leave it as it 
        is. Don't add something alike -np 1. The code will add this 
        automatically.
    """
    self.visualisation_tools_path = path
    self.mpi_prefix               = mpi_prefix


  def set_output_path(self, path):
    """
      Set path where tool drop output. If you set an output different to .,
      every filter application will take the input and overwrite the output.
      If you use ., then the input data is enriched
    """
    self.output_path = path
    

  def __invoke(self):
    """
    Returns the invocation call. This is a set of strings.
    """
    if self.mpi_prefix!="":
      return [self.mpi_prefix, "-n", "1", self.visualisation_tools_path + "/convert"]
    else:
      return [self.visualisation_tools_path + "/convert"]


  def __get_selectors(self):
    if not self.silent:
      print( "inspect file " + self.file_name + " ... " )
    convert_result = ""
    result = []
    invocation = self.__invoke() + [ "inspect", self.file_name + self.extension ]
    try:
      convert_result = subprocess.check_output(invocation)
      for line in convert_result.decode("utf-8").splitlines(False):
        #if "variable" in line:
        #  print( line )
        #if "variable" in line and not "hosts" in line and not "held" in line:
        #  new_entry = line.split("variable")[-1].strip()
        if "variable" in line:
          new_entry = line.split("variable")[-1].strip()
          result.append( new_entry ) 
      result = set(result)
      if not self.silent:
        print( "found selectors " + str(result) )
    except Exception as e:
      print( "failed to inspect (" + str(e) + ") with " + str(invocation) )  
    return result


  def extract_fine_grid(self):
    """
      Set path where tool can find the convert script. 
    """
    selectors = self.__get_selectors()

    print( "extract fine grid from " + self.file_name + " ... ")
    for selector in selectors:
      invocation =         self.__invoke() + [
          "apply-filter",
          self.file_name + self.extension,
          selector,
          self.output_path,
          "extract-fine-grid",
          selector + "-fine-grid"
        ]
      try:
        if self.silent:
          subprocess.check_call(invocation, stdout=subprocess.PIPE)
        else:
          subprocess.check_call(invocation)
          print( "complete" )
      except Exception as e:
        print( "failed to extract grid (" + str(e) + ") with " + str(invocation) )  


  def plot_domain_decomposition(self):
    """
      This wrapper plots/extracts the domain decomposition. If you are interested in the
      domain decomposition, then you have to apply this filter as the very first step. The 
      convert tool tends to flatten all IO data and thus loses the tree information after 
      the very first conversion - unless you make this the first step. 
    """
    selectors = self.__get_selectors()

    print( "extract fine grid from " + self.file_name + " ... ")
    for selector in selectors:
      invocation =         self.__invoke() + [
          "apply-filter",
          self.file_name + self.extension,
          selector,
          self.output_path,
          "plot-domain-decomposition",
          selector + "-domain-decomposition"
        ]
      try:
        if self.silent:
          subprocess.check_call(invocation, stdout=subprocess.PIPE)
        else:
          subprocess.check_call(invocation)
          print( "complete" )
      except Exception as e:
        print( "failed to extract grid (" + str(e) + ") with " + str(invocation) )  
        

  def convert_to_vtk(self):
    """
      Set path where tool can find the convert script. 
    """
    selectors = self.__get_selectors()

    print( "convert file " + self.file_name + " into vtk (vtu binary format) ... ")
    for selector in selectors:
      invocation =         self.__invoke() + [
          "convert-file",
          self.file_name + self.extension,
          selector,
          self.output_path,
          "vtu"
        ]
      try:
        if self.silent:
          subprocess.check_call(invocation, stdout=subprocess.PIPE)
        else:
          subprocess.check_call(invocation)
          print( "complete" )
      except Exception as e:
        print( "failed to convert to vtk (" + str(e) + ") with " + str(invocation) )  

