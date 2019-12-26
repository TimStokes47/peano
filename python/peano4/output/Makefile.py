# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Helper import Overwrite
from .Helper import writeFile

import os
import re


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
    
  def set_dimension(self,dimension):
    self.d["DIM"] = str(dimension)

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
   
