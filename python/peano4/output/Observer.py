# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Helper import Overwrite
from .Helper import writeFile


import os
import re


class Observer(object):
  default_overwrite = True
  
  
  def __init__(self,classname,namespace,subdirectory,included_mappings,vertices,faces,cells):
    """
      Included mappings is a list of qualified mappings which are used
      
      vertices is a list of names. They are not qualified
      faces is a list of names. They are not qualified
      cells is a list of names. They are not qualified
    """
    self.classname    = classname
    self.namespace    = namespace
    self.subdirectory = subdirectory 
    
    self.included_mappings = included_mappings
    self.vertices          = vertices
    self.faces             = faces
    self.cells             = cells
    
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
    self.d[ "INCLUDES" ]                  = ""
    self.d[ "ATTRIBUTES" ]                = ""

    for mapping in self.included_mappings:
      self.d[ "INCLUDES" ] += "#include \""
      self.d[ "INCLUDES" ] += mapping.replace( "::", "/" )
      self.d[ "INCLUDES" ] += ".h\"\n"
      self.d[ "ATTRIBUTES" ] += mapping
      self.d[ "ATTRIBUTES" ] += "   _mapping"
      self.d[ "ATTRIBUTES" ] += str( self.included_mappings.index(mapping) )
      self.d[ "ATTRIBUTES" ] += "; \n"

    for vertex in vertices:
      self.d[ "ATTRIBUTES" ] += "   peano4::stacks::STDVectorStack< DataRepository::Vertices"
      self.d[ "ATTRIBUTES" ] += vertex
      self.d[ "ATTRIBUTES" ] += ">  _vertices"
      self.d[ "ATTRIBUTES" ] += vertex
      self.d[ "ATTRIBUTES" ] += "CallStack; \n"

    for face in faces:
      self.d[ "ATTRIBUTES" ] += "   peano4::stacks::STDVectorStack< DataRepository::Faces"
      self.d[ "ATTRIBUTES" ] += face
      self.d[ "ATTRIBUTES" ] += ">  _faces"
      self.d[ "ATTRIBUTES" ] += face
      self.d[ "ATTRIBUTES" ] += "CallStack; \n"
      
    self.d[ "MAPPING_SIGNATURE_FINE_GRID_CELL_ARGUMENTS" ]       = ""
    self.d[ "MAPPING_SIGNATURE_FINE_GRID_FACES_ARGUMENTS" ]      = ""
    self.d[ "MAPPING_SIGNATURE_FINE_GRID_VERTICES_ARGUMENTS" ]   = ""
    self.d[ "MAPPING_SIGNATURE_COARSE_GRID_CELL_ARGUMENTS" ]     = ""
    self.d[ "MAPPING_SIGNATURE_COARSE_GRID_FACES_ARGUMENTS" ]    = ""
    self.d[ "MAPPING_SIGNATURE_COARSE_GRID_VERTICES_ARGUMENTS" ] = ""


  def __generate_header(self,overwrite,full_qualified_filename):
    if writeFile(overwrite,self.default_overwrite,full_qualified_filename):
      print( "write " + full_qualified_filename )
      template_file = os.path.realpath(__file__).replace( ".pyc", ".h.template" ).replace( ".py", ".h.template" )
      with open( os.path.realpath(template_file), "r" ) as input:
        template = input.read()
      with open( full_qualified_filename, "w" ) as output:
        output.write( template.format(**self.d) )



  TemplateConstructor = """
  
{FULL_QUALIFIED_CLASSNAME}::{CLASSNAME}(int spacetreeId):
  _spacetreeId( spacetreeId ) {MAPPING_INITIALISATION_LIST}
{{
}}
  

  """


  def __generate_constructor(self,output_file):
    #
    # Constructor
    #
    self.d[ "MAPPING_INITIALISATION_LIST" ]                = ""
    for mapping in self.included_mappings:
      self.d[ "MAPPING_INITIALISATION_LIST" ]  += ", _mapping"
      self.d[ "MAPPING_INITIALISATION_LIST" ]  += str( self.included_mappings.index(mapping) )
      self.d[ "MAPPING_INITIALISATION_LIST" ]  += "(spacetreeId)"
    output_file.write( self.TemplateConstructor.format(**self.d) )


  TemplateClone = """
  
peano4::grid::TraversalObserver* {FULL_QUALIFIED_CLASSNAME}::clone(int spacetreeId) {{
  return new {CLASSNAME}(spacetreeId);
}}
  
  """


  TemplateGetGridControlEvents = """
  
std::vector< peano4::grid::GridControlEvent > {FULL_QUALIFIED_CLASSNAME}::getGridControlEvents() {{
  std::vector< peano4::grid::GridControlEvent > result;

{MAPPING_GET_GRID_CONTROL_EVENT_CALLS}
        
  return result;
}}
  
  """
  

  TemplateBeginTraversal = """
  
void {FULL_QUALIFIED_CLASSNAME}::beginTraversal( const tarch::la::Vector<Dimensions,double>&  x, const tarch::la::Vector<Dimensions,double>&  h ) {{
{MAPPING_BEGIN_TRAVERSAL_CALLS}

  int outStack = peano4::grid::PeanoCurve::CallStack;

{INITIAL_PUSH_TO_OUTPUT_STREAMS}
}}
  
  """

    
  def __generate_beginTraversal(self,output_file):
    self.d[ "MAPPING_BEGIN_TRAVERSAL_CALLS" ]     = ""
    for mapping in self.included_mappings:
      self.d[ "MAPPING_BEGIN_TRAVERSAL_CALLS" ]  += "  _mapping"
      self.d[ "MAPPING_BEGIN_TRAVERSAL_CALLS" ]  += str( self.included_mappings.index(mapping) )
      self.d[ "MAPPING_BEGIN_TRAVERSAL_CALLS" ]  += ".beginTraversal();\n"
      
    self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    = ""
    for cell in self.cells:
      self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    += "  DataRepository::_CellData" + cell + "[ DataRepository::DataKey(_spacetreeId,outStack) ].push( celldata::" + cell + "() );\n"

    for face in self.faces:
      self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    += "  _faces" + face + "CallStack.push(DataRepository::Faces" + face + "(x,h));\n"

    for vertex in self.vertices:
      self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    += "  _vertices" + vertex + "CallStack.push(DataRepository::Vertices" + vertex + "(x,h));\n"

    output_file.write( self.TemplateBeginTraversal.format(**self.d) )


  TemplateEndTraversal = """
  
void {FULL_QUALIFIED_CLASSNAME}::endTraversal( const tarch::la::Vector<Dimensions,double>&  x, const tarch::la::Vector<Dimensions,double>&  h ) {{
{MAPPING_END_TRAVERSAL_CALLS}
  int inStack = peano4::grid::PeanoCurve::CallStack;
{FINAL_POP_FROM_INPUT_STREAMS}
}}
  
  """

    
  def __generate_endTraversal(self,output_file):
    self.d[ "MAPPING_END_TRAVERSAL_CALLS" ]                = ""
    for mapping in self.included_mappings:
      self.d[ "MAPPING_END_TRAVERSAL_CALLS" ]  += "  _mapping"
      self.d[ "MAPPING_END_TRAVERSAL_CALLS" ]  += str( self.included_mappings.index(mapping) )
      self.d[ "MAPPING_END_TRAVERSAL_CALLS" ]  += ".endTraversal();\n"

    self.d[ "FINAL_POP_FROM_INPUT_STREAMS" ]    = ""
    for cell in self.cells:
      self.d[ "FINAL_POP_FROM_INPUT_STREAMS" ]    += "  DataRepository::_CellData" + cell + "[ DataRepository::DataKey(_spacetreeId,inStack) ].pop();\n"

    for face in self.faces:
      self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    += "  _faces" + cell + "CallStack.pop();\n"

    for vertex in self.vertices:
      self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    += "  _vertices" + vertex + "CallStack.pop();\n"
    
    output_file.write( self.TemplateEndTraversal.format(**self.d) )
    
    
  def __generate_getGridControlEvents(self,output_file):
    self.d[ "MAPPING_GET_GRID_CONTROL_EVENT_CALLS" ]                = ""
    for mapping in self.included_mappings:
      self.d[ "MAPPING_GET_GRID_CONTROL_EVENT_CALLS" ]  += "  const std::vector< peano4::grid::GridControlEvent > result"
      self.d[ "MAPPING_GET_GRID_CONTROL_EVENT_CALLS" ]  += str( self.included_mappings.index(mapping) )
      self.d[ "MAPPING_GET_GRID_CONTROL_EVENT_CALLS" ]  += " = _mapping"
      self.d[ "MAPPING_GET_GRID_CONTROL_EVENT_CALLS" ]  += str( self.included_mappings.index(mapping) )
      self.d[ "MAPPING_GET_GRID_CONTROL_EVENT_CALLS" ]  += ".getGridControlEvents();\n"
      self.d[ "MAPPING_GET_GRID_CONTROL_EVENT_CALLS" ]  += "  result.insert( result.begin(), result"
      self.d[ "MAPPING_GET_GRID_CONTROL_EVENT_CALLS" ]  += str( self.included_mappings.index(mapping) )
      self.d[ "MAPPING_GET_GRID_CONTROL_EVENT_CALLS" ]  += ".begin(), result"
      self.d[ "MAPPING_GET_GRID_CONTROL_EVENT_CALLS" ]  += str( self.included_mappings.index(mapping) )
      self.d[ "MAPPING_GET_GRID_CONTROL_EVENT_CALLS" ]  += ".end() );\n"
    output_file.write( self.TemplateGetGridControlEvents.format(**self.d) )


  def __generate_clone(self,output_file):
    output_file.write( self.TemplateClone.format(**self.d) )


  TemplateEnterCell_Preamble = """  
void {FULL_QUALIFIED_CLASSNAME}::enterCell( const peano4::grid::GridTraversalEvent&  event ) {{
"""


  TemplateEnterCell_FaceLoad = """  
  // Handle face {FACE_UNKNOWN_NAME}
  DataRepository::Faces{FACE_UNKNOWN_NAME}     fineGridFaces{FACE_UNKNOWN_NAME}(event.getX(),event.getH());
  DataRepository::Faces{FACE_UNKNOWN_NAME}     coarseGridFaces{FACE_UNKNOWN_NAME}(event.getX(),event.getH());
  {{
    DataRepository::FaceData{FACE_UNKNOWN_NAME}::PushBlockVertexStackView faceView = DataRepository::_FaceData{FACE_UNKNOWN_NAME}[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].pushBlock(Dimensions*2);
  }}
"""


  TemplateEnterCell_CellLoad = """  
  // Handle cell {CELL_UNKNOWN_NAME}
  {{
    int inCellStack  = event.getCellData();
    int outCellStack = peano4::grid::PeanoCurve::CallStack;
    logDebug("enterCell(...)", "cell " << inCellStack << "->pos-" << outCellStack );

    if (inCellStack==peano4::grid::TraversalObserver::CreateOrDestroyPersistentGridEntity) {{
      celldata::{CELL_UNKNOWN_NAME} data;

      // @todo
      Ich weiss noch net, wie ich das hier mache, dass ich mehrere Mappings benutze. Eigentlich
      waere ein Array nett.
          _mapping->createCell(
      event.getX(),event.getH(),data,
        _facesCallStack.top(0),
        _cellData[ DataKey(_spacetreeId,outCellStack) ].top(1),
        _facesCallStack.top(1)
    );

      
      DataRepository::_CellData{CELL_UNKNOWN_NAME}[ DataRepository::DataKey(_spacetreeId,outCellStack) ].push(data);
    }}
    else {{
      celldata::{CELL_UNKNOWN_NAME} data;
      data = DataRepository::_CellData{CELL_UNKNOWN_NAME}[ DataRepository::DataKey(_spacetreeId,inCellStack) ].pop();
      DataRepository::_CellData{CELL_UNKNOWN_NAME}[ DataRepository::DataKey(_spacetreeId,outCellStack) ].push(data);
    }}
  }}
  celldata::{CELL_UNKNOWN_NAME}& fineCell{CELL_UNKNOWN_NAME}   = DataRepository::_CellData{CELL_UNKNOWN_NAME}[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].top(0);
  celldata::{CELL_UNKNOWN_NAME}& coarseCell{CELL_UNKNOWN_NAME} = DataRepository::_CellData{CELL_UNKNOWN_NAME}[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].top(1);
"""



  TemplateEnterCell_MappingInvocation = """  
  {{
    peano4::datamanagement::CellMarker marker(event.getIsRefined(),false);
    {ACTIVE_MAPPING}.touchCellFirstTime(
      event.getX(), event.getH(),
      {MAPPING_SIGNATURE_FINE_GRID_CELL_ARGUMENTS}
      {MAPPING_SIGNATURE_FINE_GRID_FACES_ARGUMENTS}
      {MAPPING_SIGNATURE_FINE_GRID_VERTICES_ARGUMENTS}
      {MAPPING_SIGNATURE_COARSE_GRID_CELL_ARGUMENTS}
      {MAPPING_SIGNATURE_COARSE_GRID_FACES_ARGUMENTS}
      {MAPPING_SIGNATURE_COARSE_GRID_VERTICES_ARGUMENTS}
      marker
    );
  }}
"""



  TemplateEnterCell_Epilogue = """  
}}
"""

  
  def __generate_enterCell(self,output_file):
    """
      Generates enter cell
      Befills the argument lists for the mapping calls
    """
    output_file.write( self.TemplateEnterCell_Preamble.format(**self.d) )
          
    # @todo Das ist noch net fertig
    for vertices in self.vertices:
      output_file.write( "  DataRepository::Vertices" + vertices + " fineGridVertices" + face + "(event.getX(),event.getH());\n" )
      output_file.write( "  DataRepository::Vertices" + vertices + " coarseGridVertices" + face + "(event.getX(),event.getH());\n" )

    for face in self.faces:
      self.d[ "FACE_UNKNOWN_NAME" ]  = face
      self.d[ "MAPPING_SIGNATURE_FINE_GRID_FACES_ARGUMENTS" ]   += "fineGridFaces" + face + ", "
      self.d[ "MAPPING_SIGNATURE_COARSE_GRID_FACES_ARGUMENTS" ] += "fineGridFaces" + face + ", "
      output_file.write( self.TemplateEnterCell_FaceLoad.format(**self.d) )

    for cell in self.cells:
      self.d[ "CELL_UNKNOWN_NAME" ]  = cell
      self.d[ "MAPPING_SIGNATURE_FINE_GRID_CELL_ARGUMENTS" ]   += "fineCell" + cell + ", "
      self.d[ "MAPPING_SIGNATURE_COARSE_GRID_CELL_ARGUMENTS" ] += "coarseCell" + cell + ", "
      output_file.write( self.TemplateEnterCell_CellLoad.format(**self.d) )

    for mapping in self.included_mappings:
      self.d[ "ACTIVE_MAPPING" ]  = "_mapping" + str( self.included_mappings.index(mapping) )
      output_file.write( self.TemplateEnterCell_MappingInvocation.format(**self.d) )
        
    output_file.write( self.TemplateEnterCell_Epilogue.format(**self.d) )



  def __generate_leaveCell(self,output_file):
    output_file.write( "void " + self.d[ "FULL_QUALIFIED_CLASSNAME" ] + "::leaveCell( const peano4::grid::GridTraversalEvent&  event ) { \n" )
    output_file.write( "}\n\n\n" )


  TemplateImplementationFilePrologue = """
#include "{CLASSNAME}.h"
#include "DataRepository.h"
#include "peano4/grid/PeanoCurve.h"



tarch::logging::Log {FULL_QUALIFIED_CLASSNAME}::_log( "{FULL_QUALIFIED_CLASSNAME}" );

"""


  def __generate_implementation(self,overwrite,full_qualified_filename):
    if writeFile(overwrite,self.default_overwrite,full_qualified_filename):
      print( "write " + full_qualified_filename )
      output_file = open( full_qualified_filename, "w" )
      output_file.write( self.TemplateImplementationFilePrologue.format(**self.d) )
      
      self.__generate_constructor(output_file)
      self.__generate_clone(output_file)
      self.__generate_getGridControlEvents(output_file)
      self.__generate_beginTraversal(output_file)
      self.__generate_endTraversal(output_file)
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

