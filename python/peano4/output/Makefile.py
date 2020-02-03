# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Helper import Overwrite
from .Helper import writeFile
from enum import Enum


import os
import re


class CompileMode(Enum):
  Debug = 0
  Trace = 1
  Release = 2
  

class Makefile(object):
  """ Represents the created makefile of a Peano4 project """
  default_overwrite = True
   
  
  def __init__(self):
    self.cppfiles = []
    self.d = {}
    self.d["CXX"]           = ""
    self.d["CXXFLAGS"]      = ""
    self.d["LDFLAGS"]       = ""
    self.d["LIBS"]          = ""
    self.d["DIM"]           = "2"
    self.d["CONFIGUREPATH"] = "."
    self.set_mode( CompileMode.Debug )

    
  def set_dimension(self,dimension):
    self.d["DIM"] = str(dimension)


  def get_configure_path(self):
    """
    Returns the directory where the ./configure script is located
    """
    return self.d["CONFIGUREPATH"]


  def get_source_path(self):
    """
    Returns the directory where the ./configure script is located
    """
    return self.get_configure_path() + "/src"


  def add_header_search_path(self, path ):
    self.d["CXXFLAGS"] += " -I" + path


  def add_library(self, library_name, library_path="" ):
    """
    If you want to link against a library from Peano, feel free to use
    get_Peano4_source_directory() and hand in a concatenation of this 
    path plus a subpath. Otherwise, specify the absolute path where to 
    search for. By default, Peano's src directory is in the search 
    path.
    
    A popular invocation including one of Peano's toolboxes is 
    
    project.output.makefile.add_library( "ToolboxFiniteElements2d_trace", project.output.makefile.get_source_path() + "/toolbox/finiteelements" )

    """
    if library_path!="":
      self.d["LIBS"] = "-L" + library_path + " " + self.d["LIBS"]
    self.d["LIBS"] += " -l" + library_name


  def set_mode( self, mode ):
    """
      mode should be of type CompileMode. Pass in 

      peano4.output.CompileMode.Debug
      
      for example
    """
    if mode==CompileMode.Debug:
      self.d["CXX_MODE_FLAGS"]  = "-g -O0 -DPeanoDebug=4"
      self.d["LIBRARY_POSTFIX"] = "_debug"
    elif mode==CompileMode.Trace:
      self.d["CXX_MODE_FLAGS"]  = "-g -O2 -DPeanoDebug=1"
      self.d["LIBRARY_POSTFIX"] = "_trace"
    elif mode==CompileMode.Release:
      self.d["CXX_MODE_FLAGS"]  = "-O2 -DPeanoDebug=0"
      self.d["LIBRARY_POSTFIX"] = ""
    else:
      assert(False)      


  def parse_configure_script_outcome(self,directory):
    """
    directory should point to the directory which holds the ./configure script.
    It furthermore has to be invoked after configure has passed successfully. 
    This script does not accept relative paths.
    """
    input_file = directory + "/Makefile"
    input = open( input_file, "r" )
    print( "parse configure outcome " + input_file + " to extract compile settings" )
    for line in input:
      if re.match( "CXX *=", line):
        compiler = line.split("=")[-1].strip()
        print( "used compiler is " + compiler )
        self.d["CXX"] = compiler
      if re.search( "CXXFLAGS *=", line):
        flags = line.split("=",1)[1].strip()
        self.d["CXXFLAGS"] += flags
        self.d["CXXFLAGS"] += " "
      if re.search( "LDFLAGS *=", line):
        flags = line.split("=",1)[1].strip()
        self.d["LDFLAGS"] += flags
        self.d["LDFLAGS"] += " "
      if re.search( "LIBS *=", line):
        flags = line.split("=",1)[1].strip()
        self.d["LIBS"] += flags
        self.d["LIBS"] += " "
    self.d["CONFIGUREPATH"] = directory

 
  def add_cpp_file(self,filename):
    self.cppfiles.append(filename)


  def generate(self,overwrite,directory):
    filename = directory + "/Makefile";
    if writeFile(overwrite,self.default_overwrite,filename):
      print( "write " + filename )
      
      # files
      self.d[ 'SOURCES' ] = ""
      for i in self.cppfiles:
        self.d[ 'SOURCES' ] += " "
        self.d[ 'SOURCES' ] += i
     
      # We first eliminate the precompiled variant, and then we get rid of the
      # postfix in the case where it is a source file
      with open( os.path.realpath(__file__).replace( ".pyc", ".template" ).replace( ".py", ".template" ), "r" ) as input:
        template = input.read()
      with open( filename, "w" ) as output:
        output.write( template.format(**self.d) )
   
