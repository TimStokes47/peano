# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Helper import Overwrite
from .Helper import writeFile

import os
import re


class MappingInterface(object):
  default_overwrite = True
    
  def __init__(self,classname,namespace,subdirectory,is_interface):
    """
     If is_interface is set, then all routines are abstract 
    """
    self.classname    = classname
    self.namespace    = namespace
    self.subdirectory = subdirectory
    self.is_interface = is_interface
    self.operations   = [
      ("beginTraversal","void"),
      ("endTraversal","void")
    ]
    self.include_files = []
    self.typedefs = []

  def add_operation(self,name,signature):
    """
     signature is a long tuple. The first entry is the name of the routine,
     the second entry is the result type. From thereon, one entry gives the 
     name of an attribute, the second one the type.
    """
    self.operations.append(signature)    

  def __generate_includes(self,outputfile):
    for i in self.include_files:
      outputfile.write( "#include \"" )
      outputfile.write( i )
      outputfile.write( "\"\n")
      
  def __generate_operation(self,outputfile,operation):
      outputfile.write( "    virtual " )
      outputfile.write( operation[1] )
      outputfile.write( " ")
      outputfile.write( operation[0] )
      outputfile.write( "(")
      i = 2
      while i<len(operation):
        outputfile.write( operation[i+1] )
        outputfile.write( " ")
        outputfile.write( operation[i] )
        i+=2
        if i<len(operation):
          outputfile.write( ", ")
        
      outputfile.write( ")")
      if self.is_interface:
        outputfile.write( " = 0")
      outputfile.write( ";\n\n")

  
  def generate(self,overwrite,directory):
    if not os.path.exists( directory + "/" + self.subdirectory ):
      os.mkdir(directory + "/" + self.subdirectory)

    filename = directory + "/" + self.subdirectory + "/" + self.classname + ".h";
    if writeFile(overwrite,self.default_overwrite,filename):
      print( "write " + filename )
      outputfile = open( filename, "w" )

      full_qualified_classname = ""
      for i in self.namespace:
        full_qualified_classname += i
        full_qualified_classname += "::"
      full_qualified_classname += self.classname
      include_guard = "_" + full_qualified_classname.replace( "::", "_" ).upper() + "_H_"

      outputfile.write( "#ifndef " + include_guard + "\n" )
      outputfile.write( "#define " + include_guard + "\n\n\n" )
      
      outputfile.write( "#include \"peano4/utils/Globals.h\" \n" )
      outputfile.write( "#include \"peano4/datamanagement/FaceEnumerator.h\" \n" )
      outputfile.write( "#include \"peano4/datamanagement/VertexEnumerator.h\" \n" )
      outputfile.write( "#include \"peano4/datamanagement/CellMarker.h\" \n" )
      outputfile.write( "#include \"tarch/la/Vector.h\" \n\n\n" )

      self.__generate_includes(outputfile)
      
      for i in self.namespace:
        outputfile.write( "namespace " + i + "{\n" )

      outputfile.write( "  class " + self.classname + ";\n" )

      for i in self.namespace:
        outputfile.write( "}\n" )

      outputfile.write( "class " + full_qualified_classname + "{\n" )
      outputfile.write( "  public:\n" )
      
      for i in self.operations:
        self.__generate_operation(outputfile,i)       

      outputfile.write( "};\n\n\n" )

      outputfile.write( "#endif\n\n" )
