# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Helper import Overwrite
from .Helper import writeFile

import os
import re


class Observer(object):
  default_overwrite = True
  
  
  def __init__(self,classname,namespace,subdirectory):
    self.classname    = classname
    self.namespace    = namespace
    self.subdirectory = subdirectory 
    
    self.d = {}
    self.d[ "OPEN_NAMESPACE" ]           = ""
    self.d[ "CLOSE_NAMESPACE" ]          = ""
    self.d[ "FULL_QUALIFIED_CLASSNAME" ] = ""
    for i in namespace:
      self.d[ "OPEN_NAMESPACE" ]           += "namespace " + i + "{\n"
      self.d[ "CLOSE_NAMESPACE" ]          += "}\n"
      self.d[ "FULL_QUALIFIED_CLASSNAME" ] += i + "::"
    self.d[ "CLASSNAME" ]                 = classname
    self.d[ "FULL_QUALIFIED_CLASSNAME" ] += classname
    self.d[ "INCLUDE_GUARD" ]             = "_" + self.d[ "FULL_QUALIFIED_CLASSNAME" ].replace( "::", "_" ).upper() + "_H_"


  def __generate_header(self,overwrite,full_qualified_filename):
    if writeFile(overwrite,self.default_overwrite,full_qualified_filename):
      print( "write " + full_qualified_filename )
      template_file = os.path.realpath(__file__).replace( ".pyc", ".h.template" ).replace( ".py", ".h.template" )
      with open( os.path.realpath(template_file), "r" ) as input:
        template = input.read()
      with open( full_qualified_filename, "w" ) as output:
        output.write( template.format(**self.d) )



  TemplateConstructor = """
  
  {FULL_QUALIFIED_CLASSNAME}::{CLASSNAME}(int spacetreeId)
  //:
  //  _mapping(spacetreeId) {{
  {{
  }}
  

  """


  TemplateBeginTraversal = """
  
  void {FULL_QUALIFIED_CLASSNAME}::beginTraversal( const tarch::la::Vector<Dimensions,double>&  x, const tarch::la::Vector<Dimensions,double>&  h ) {{
    //_mapping.beginTraversal();
  }}
  
  """


  TemplateEndTraversal = """
  
  void {FULL_QUALIFIED_CLASSNAME}::endTraversal( const tarch::la::Vector<Dimensions,double>&  x, const tarch::la::Vector<Dimensions,double>&  h ) {{
    //_mapping.endTraversal();
  }}
  
  """


  TemplateClone = """
  
  peano4::grid::TraversalObserver* {FULL_QUALIFIED_CLASSNAME}::clone(int spacetreeId) {{
    return new {CLASSNAME}(spacetreeId);
  }}
  
  """


  TemplateGetGridControlEvents = """
  
  std::vector< peano4::grid::GridControlEvent > {FULL_QUALIFIED_CLASSNAME}::getGridControlEvents() {{
    std::vector< peano4::grid::GridControlEvent > result;
    
    //const std::vector< peano4::grid::GridControlEvent > userData = _mapping.getGridControlEvents();
    //result.insert(result.begin(),userData.begin(),userData.end());
    
    return result;
  }}
  
  """
  
  

  def __generate_implementation_glue_code(self,output_file):
    output_file.write( self.TemplateConstructor.format(**self.d) )
    output_file.write( self.TemplateBeginTraversal.format(**self.d) )
    output_file.write( self.TemplateEndTraversal.format(**self.d) )
    output_file.write( self.TemplateClone.format(**self.d) )
    output_file.write( self.TemplateGetGridControlEvents.format(**self.d) )


  def __generate_enterCell(self,output_file):
    output_file.write( "void " + self.d[ "FULL_QUALIFIED_CLASSNAME" ] + "::enterCell( const peano4::grid::GridTraversalEvent&  event ) { \n" )
    output_file.write( "}\n\n\n" )

  def __generate_leaveCell(self,output_file):
    output_file.write( "void " + self.d[ "FULL_QUALIFIED_CLASSNAME" ] + "::leaveCell( const peano4::grid::GridTraversalEvent&  event ) { \n" )
    output_file.write( "}\n\n\n" )
    
    #if writeFile(overwrite,self.default_overwrite,full_qualified_filename):
    #  print( "write " + full_qualified_filename )
    #  output_file = open( full_qualified_filename, "w" )
    #  output_file.write( "#include \"" + self.d[ "CLASSNAME" ] + ".h\"\n\n\n" )
    # 
    #  self.__generate_implementation_glue_code(output_file)


  def __generate_implementation(self,overwrite,full_qualified_filename):
    if writeFile(overwrite,self.default_overwrite,full_qualified_filename):
      print( "write " + full_qualified_filename )
      output_file = open( full_qualified_filename, "w" )
      output_file.write( "#include \"" + self.d[ "CLASSNAME" ] + ".h\"\n\n\n" )
      
      self.__generate_implementation_glue_code(output_file)
      self.__generate_enterCell(output_file)
      self.__generate_leaveCell(output_file)


  def get_header_file_name(self):
    return self.subdirectory + "/" + self.classname + ".h"


  def get_cpp_file_name(self):
    return self.subdirectory + "/" + self.classname + ".cpp"


  def generate(self,overwrite,directory):
    if not os.path.exists( directory + "/" + self.subdirectory ):
      os.mkdir(directory + "/" + self.subdirectory)
    
    header_filename = directory + "/" + self.get_header_file_name()
    cpp_filename    = directory + "/" + self.get_cpp_file_name()
    
    self.__generate_header(overwrite,header_filename)
    self.__generate_implementation(overwrite,cpp_filename)

