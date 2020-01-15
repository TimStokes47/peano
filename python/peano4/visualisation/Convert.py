# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import subprocess


class Convert(object):
  """
    Convert is a file which wraps around one Peano block file output. Output can 
    be a whole data set or simply one data file.
  """
  
  def __init__(self,file_name):
    self.visualisation_tools_path = "."
    self.file_name = file_name
    self.extension = ".peano-patch-file"
    self.output_path = "."


  def set_visualisation_tools_path(self, path):
    """
      Set path where tool can find the convert script. 
    """
    self.visualisation_tools_path = path


  def set_output_path(self, path):
    """
      Set path where tool drop output. If you set an output different to .,
      every filter application will take the input and overwrite the output.
      If you use ., then the input data is enriched
    """
    self.output_path = path


  def __get_selectors(self):
    print( "inspect file " + self.file_name + " ... ")
    convert_result = ""
    result = []
    try:
      convert_result = subprocess.check_output([
        self.visualisation_tools_path + "/convert", 
        "inspect", 
        self.file_name + self.extension
      ])
      for line in convert_result.splitlines(False):
        if "variable" in line and not "hosts" in line:
          new_entry = line.split("variable")[-1].strip()
          result.append( new_entry ) 
      result = set(result)
      print( "complete. Found selectors " + str(result) )
    except Exception as e:
      print( "failed (" + str(e) + ")" )  
    return result


  def extract_fine_grid(self):
    """
      Set path where tool can find the convert script. 
    """
    selectors = self.__get_selectors()

    print( "extract fine grid from " + self.file_name + " ... ")
    try:
      for selector in selectors:
        subprocess.check_call([
          self.visualisation_tools_path + "/convert", 
          "apply-filter",
          self.file_name + self.extension,
          selector,
          self.output_path,
          "extract-fine-grid",
          selector + "-fine-grid"
        ])
      print( "complete" )
    except Exception as e:
      print( "failed (" + str(e) + ")" )


  def convert_to_vtk(self):
    """
      Set path where tool can find the convert script. 
    """
    selectors = self.__get_selectors()

    print( "convert file " + self.file_name + " ... ")
    try:
      for selector in selectors:
        subprocess.check_call([
          self.visualisation_tools_path + "/convert", 
          "convert-file",
          self.file_name + self.extension,
          selector,
          self.output_path,
          "vtu"
        ])
      print( "complete" )
    except Exception as e:
      print( "failed (" + str(e) + ")" )

