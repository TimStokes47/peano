# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import os
import peano4.output.InvokeExternalTool



class DaStGenToLegacyTool(object):
  DaStGenInvocationPrefix = "java -jar ~/git/DaStGen/DaStGen.jar --plugin PeanoSnippetGenerator --naming Peano4NameTranslator "
    
    
  """
  This converter requires my good old DaStGen Java tool, writes a spec file for 
  DaStGen and finally invokes the tool.
  """
  def __init__(self,patch):
    self.data = patch


  def get_stack_container(self):
    return "peano4::stacks::STDVectorStack< " + self.data.get_full_qualified_type() + " >";

    
  def get_header_file_include(self):
    return "#include \"peano4/stacks/STDVectorStack.h\" \n \
            #include \"" + self.data.namespace[-1] + "/" + self.data.name + ".h\""

  __Template_Prologue = """  
#include "peano4/utils/Globals.h"

// have to know whether we are using MPI
#include "config.h"

Packed-Type: short int;
"""

  def __get_file_name(self):
    return self.data.name + ".def"

  def __get_full_qualified_file_name(self):
    return self.data.namespace[-1] + "/" + self.__get_file_name()


  def __generate_dastgen_input_file(self):
    file = open( self.__get_full_qualified_file_name(), "w" )
    file.write( self.__Template_Prologue )
    file.write( "class " )
    for i in self.data.namespace:
      file.write( i )
      file.write( "::" )
    file.write( self.data.name )
    file.write( " {\n" )

    for i in self.data.attributes_double:
      file.write( "  persistent parallelise double " )
      file.write( i )
      file.write( ";\n" )

    file.write( "};\n\n\n" )


  def construct_output(self,output):
    """
      Pass in a version of output
    """
    self.__generate_dastgen_input_file()
    output.makefile.add_cpp_file( self.data.namespace[-1] + "/" + self.data.name + ".cpp" )
    output.add( peano4.output.InvokeExternalTool( self.DaStGenInvocationPrefix + " " + self.__get_file_name() + " .", self.data.namespace[-1] ) )

