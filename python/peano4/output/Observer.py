# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Helper import Overwrite
from .Helper import writeFile


import os
import re
from gi._gi import BaseInfo


class Observer(object):
  default_overwrite = True
  
  
  def __init__(self,classname,namespace,subdirectory,included_mappings,vertices,faces,cells):
    """
      Included mappings is a list of qualified mappings which are used
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
      self.d[ "ATTRIBUTES" ] += "    "
      self.d[ "ATTRIBUTES" ] += mapping
      self.d[ "ATTRIBUTES" ] += "   _mapping"
      self.d[ "ATTRIBUTES" ] += str( self.included_mappings.index(mapping) )
      self.d[ "ATTRIBUTES" ] += "; \n"

    self.d[ "MAPPING_SIGNATURE_FINE_GRID_CELL_ARGUMENTS" ]       = ""
    self.d[ "MAPPING_SIGNATURE_COARSE_GRID_CELL_ARGUMENTS" ]     = ""
    for cell in cells:
      if cells.index(cell)!=0:
        self.d[ "MAPPING_SIGNATURE_FINE_GRID_CELL_ARGUMENTS" ]   += ",";
        self.d[ "MAPPING_SIGNATURE_COARSE_GRID_CELL_ARGUMENTS" ] += ",";
      self.d[ "MAPPING_SIGNATURE_FINE_GRID_CELL_ARGUMENTS" ]     += "peano4::datamanagement::CellWrapper<" + cell.get_full_qualified_type() + ">( event.getX(), event.getH(), &DataRepository::_" + cell.get_logical_type_name() + "Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].top(0) )";
      self.d[ "MAPPING_SIGNATURE_COARSE_GRID_CELL_ARGUMENTS" ]   += "peano4::datamanagement::CellWrapper<" + cell.get_full_qualified_type() + ">( event.getX(), event.getH(), event.getRelativePositionToFather(), &DataRepository::_" + cell.get_logical_type_name() + "Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].top(1) )";
      
    if len(cells)==0:
      self.d[ "MAPPING_SIGNATURE_FINE_GRID_CELL_ARGUMENTS" ]       = "peano4::datamanagement::CellWrapper<void>(event.getX(), event.getH())"
      self.d[ "MAPPING_SIGNATURE_COARSE_GRID_CELL_ARGUMENTS" ]     = "peano4::datamanagement::CellWrapper<void>(event.getX(), event.getH(),event.getRelativePositionToFather())"
          
     
    self.d[ "MAPPING_SIGNATURE_FINE_GRID_FACES_ARGUMENTS" ]      = ""
    self.d[ "MAPPING_SIGNATURE_COARSE_GRID_FACES_ARGUMENTS" ]    = ""
    for face in faces:
      if faces.index(face)!=0:
        self.d[ "MAPPING_SIGNATURE_FINE_GRID_FACES_ARGUMENTS" ]   += ","
        self.d[ "MAPPING_SIGNATURE_COARSE_GRID_FACES_ARGUMENTS" ] += ","
      self.d[ "MAPPING_SIGNATURE_FINE_GRID_FACES_ARGUMENTS" ]     += "peano4::datamanagement::FaceEnumerator<" + face.get_full_qualified_type() + ">( event.getX(),event.getH(), &DataRepository::_" + face.get_logical_type_name() + "Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].top(TwoTimesD-1) )";
      self.d[ "MAPPING_SIGNATURE_COARSE_GRID_FACES_ARGUMENTS" ]   += "peano4::datamanagement::FaceEnumerator<" + face.get_full_qualified_type() + ">( event.getX(),event.getH(), event.getRelativePositionToFather(), &DataRepository::_" + face.get_logical_type_name() + "Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].top(TwoTimesD*2-1) )";
      
    self.d[ "MAPPING_SIGNATURE_FINE_GRID_VERTICES_ARGUMENTS" ]   = ""
    self.d[ "MAPPING_SIGNATURE_COARSE_GRID_VERTICES_ARGUMENTS" ] = ""
    for vertex in vertices:
      if vertices.index(vertex)!=0:
        self.d[ "MAPPING_SIGNATURE_FINE_GRID_VERTICES_ARGUMENTS" ]   += ","
        self.d[ "MAPPING_SIGNATURE_COARSE_GRID_VERTICES_ARGUMENTS" ] += ","
      self.d[ "MAPPING_SIGNATURE_FINE_GRID_VERTICES_ARGUMENTS" ]     += "peano4::datamanagement::VertexEnumerator<" + vertex.get_full_qualified_type() + ">( event.getX(),event.getH(), &DataRepository::_" + face.get_logical_type_name() + "Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].top(TwoPowerD-1) )";
      self.d[ "MAPPING_SIGNATURE_COARSE_GRID_VERTICES_ARGUMENTS" ]   += "peano4::datamanagement::VertexEnumerator<" + vertex.get_full_qualified_type() + ">( event.getX(),event.getH(), event.getRelativePositionToFather(), &DataRepository::_" + face.get_logical_type_name() + "Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].top(TwoPowerD*2-1) )";

    #
    # Create powersets of arguments
    #
    for i in range(0,2):
      new_dictionary_entries = {}
      for keyA in self.d:
        for keyB in self.d:
          new_entry = ""
          if self.d[keyA]!="" and self.d[keyB]!="":
            new_entry = self.d[keyA] + "," + self.d[keyB]
          else:
            new_entry = self.d[keyA] + self.d[keyB]
          new_dictionary_entries[keyA+","+keyB] = new_entry
      for key in new_dictionary_entries:
        self.d[key] = new_dictionary_entries[key]
    
    #
    # Create variants with leading and tailing comma
    #
    new_dictionary_entries = {}
    for key in self.d:
      if self.d[key]!="":
        new_dictionary_entries[ key+","     ] = self.d[key] + ","
        new_dictionary_entries[ ","+key     ] = "," + self.d[key] 
        new_dictionary_entries[ ","+key+"," ] = "," + self.d[key] + ","
      else:
        new_dictionary_entries[ key+"," ]       = ""
        new_dictionary_entries[ "," + key ]     = ""
        new_dictionary_entries[ "," + key+"," ] = ""
        
    for key in new_dictionary_entries:
      self.d[key] = new_dictionary_entries[key]


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


  TemplateBeginTraversal = """
  
void {FULL_QUALIFIED_CLASSNAME}::beginTraversal( const tarch::la::Vector<Dimensions,double>&  x, const tarch::la::Vector<Dimensions,double>&  h ) {{
  //
  // Invoke beginTraversal() on the mappings
  //
{MAPPING_BEGIN_TRAVERSAL_CALLS}

  //
  // Fill call stacks with dummies which represent level -1 such that we can 
  // call standard mapping routines on level 0 with parents. Without these 
  // statements, a top(1) call would raise an assertion. 
  //
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
      self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    += "  DataRepository::_" + cell.get_logical_type_name() + "Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].push( " + cell.get_full_qualified_type() + "() );\n"
      pass

    for face in self.faces:
      self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    += "  for (int i=0; i<TwoTimesD; i++) {\n"
      self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    += "    DataRepository::_" + face.get_logical_type_name() + "Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].push( " + face.get_full_qualified_type() + "() );\n"
      self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    += "  }\n"
      pass

    for vertex in self.vertices:
      self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    += "  for (int i=0; i<TwoPowerD; i++) {\n"
      self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    += "    DataRepository::_" + vertex.get_logical_type_name() + "Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].push( " + vertex.get_full_qualified_type() + "() );\n"
      self.d[ "INITIAL_PUSH_TO_OUTPUT_STREAMS" ]    += "  }\n"
      pass

    output_file.write( self.TemplateBeginTraversal.format(**self.d) )


  TemplateEndTraversal = """
  
void {FULL_QUALIFIED_CLASSNAME}::endTraversal( const tarch::la::Vector<Dimensions,double>&  x, const tarch::la::Vector<Dimensions,double>&  h ) {{
{MAPPING_END_TRAVERSAL_CALLS}
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
      self.d[ "FINAL_POP_FROM_INPUT_STREAMS" ]    += "  DataRepository::_" + cell.get_logical_type_name() + "Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].pop();\n"
      pass

    for face in self.faces:
      self.d[ "FINAL_POP_FROM_INPUT_STREAMS" ]    += "  for (int i=0; i<TwoTimesD; i++) {\n"
      self.d[ "FINAL_POP_FROM_INPUT_STREAMS" ]    += "    DataRepository::_" + face.get_logical_type_name() + "Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].pop();\n"
      self.d[ "FINAL_POP_FROM_INPUT_STREAMS" ]    += "  }\n"
      pass

    for vertex in self.vertices:
      self.d[ "FINAL_POP_FROM_INPUT_STREAMS" ]    += "  for (int i=0; i<TwoPowerD; i++) {\n"
      self.d[ "FINAL_POP_FROM_INPUT_STREAMS" ]    += "    DataRepository::_" + vertex.get_logical_type_name() + "Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].pop();\n"
      self.d[ "FINAL_POP_FROM_INPUT_STREAMS" ]    += "  }\n"
      pass

    
    output_file.write( self.TemplateEndTraversal.format(**self.d) )
    


  def __format_template_per_mapping(self,output_file,template,reverse_order=False):
    """
     Takes the specified template file, iterates over self.mappings and pastes
     the template into the output file once per mapping. Per mapping, the dictionary's
     entries are updated. Otherwise, the dictionary remains unchanged.
    """ 
    local_mappings = [x for x in self.included_mappings]
    if reverse_order:
      local_mappings.reverse()
        
    for mapping in local_mappings:
      self.d[ "ACTIVE_MAPPING" ]  = "_mapping" + str( self.included_mappings.index(mapping) )
      self.d[ "ACTIVE_MAPPING_FULL_QUALIFIED_NAME" ]  = mapping
      output_file.write( template.format(**self.d) )
        

  TemplateGetGridControlEvents_Prologue = """
  
std::vector< peano4::grid::GridControlEvent > {FULL_QUALIFIED_CLASSNAME}::getGridControlEvents() {{
  std::vector< peano4::grid::GridControlEvent > result;
"""

  TemplateGetGridControlEvents_MappingCall = """
  {{
    const std::vector< peano4::grid::GridControlEvent > mappingResult = {ACTIVE_MAPPING}.getGridControlEvents();
    result.insert(result.begin(),mappingResult.begin(),mappingResult.end());
  }}
"""        

  
  TemplateGetGridControlEvents_Epilogue = """
  return result;
}}
  
"""
  
        
  def __generate_getGridControlEvents(self,output_file):
    output_file.write( self.TemplateGetGridControlEvents_Prologue.format(**self.d) )
    self.__format_template_per_mapping(output_file,self.TemplateGetGridControlEvents_MappingCall)
    output_file.write( self.TemplateGetGridControlEvents_Epilogue.format(**self.d) )


  def __generate_clone(self,output_file):
    output_file.write( self.TemplateClone.format(**self.d) )


  TemplateEnterCell_Prologue = """  
void {FULL_QUALIFIED_CLASSNAME}::enterCell( const peano4::grid::GridTraversalEvent&  event ) {{
  logTraceInWith1Argument( "enterCell(peano4::grid::GridTraversalEvent)", event.toString() );
"""


  TemplateEnterCell_VertexLoad_Prologue = """
"""


  TemplateEnterCell_VertexLoad_MappingCall = """
"""


  TemplateEnterCell_FaceLoad_Prologue = """  
  // Load face {name}
  {{
    auto view = DataRepository::_{logical_type_name}Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].pushBlock( TwoTimesD );
    for (int i=0; i<TwoTimesD; i++) {{
      int inFaceStack          = event.getFaceDataFrom(i);
      int outFaceStackPosition = event.getFaceDataTo(i);
      {full_qualified_type} data ;
      if (
        inFaceStack!=peano4::grid::TraversalObserver::CreateOrDestroyPersistentGridEntity
        and
        inFaceStack!=peano4::grid::TraversalObserver::CreateOrDestroyHangingGridEntity
      ) {{
        data = DataRepository::_{logical_type_name}Stack[ DataRepository::DataKey(_spacetreeId,inFaceStack) ].pop();
      }}
      view.set(outFaceStackPosition,data);
    }}
  }}
"""


  TemplateEnterCell_FaceLoad_MappingCall = """  
  // Handle face {name}
  {{
    peano4::datamanagement::FaceEnumerator<{full_qualified_type}> view(
      event.getX(), event.getH(),
      &DataRepository::_{logical_type_name}Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].top( TwoTimesD-1 )
    );
    for (int i=0; i<TwoTimesD; i++) {{
      int inFaceStack        = event.getFaceDataFrom(i);
      int outFaceStackNumber = event.getFaceDataTo(i);

      logDebug("enterCell(...)", "face " << inFaceStack << "->pos-" << outFaceStackNumber );
      
      if (inFaceStack==peano4::grid::TraversalObserver::CreateOrDestroyPersistentGridEntity) {{
        /*
        {ACTIVE_MAPPING}.createPersistentFace(
          view.x(outFaceStackNumber)
         ,event.getH()
         ,view.normal(outFaceStackNumber)
         ,view(outFaceStackNumber)
         {,MAPPING_SIGNATURE_FINE_GRID_VERTICES_ARGUMENTS,MAPPING_SIGNATURE_COARSE_GRID_VERTICES_ARGUMENTS,MAPPING_SIGNATURE_COARSE_GRID_FACES_ARGUMENTS}
        );
        */
        //_mapping->touchFaceFirstTime(fineGridFaces.x(outFaceStack),event.getH(),fineGridFaces.normal(outFaceStack),data);
      }}
      else if (inFaceStack==peano4::grid::TraversalObserver::CreateOrDestroyHangingGridEntity) {{
        //_mapping->createHangingFace(fineGridFaces.x(outFaceStack),event.getH(),fineGridFaces.normal(outFaceStack),data);
        //_mapping->touchFaceFirstTime(fineGridFaces.x(outFaceStack),event.getH(),fineGridFaces.normal(outFaceStack),data);
      }}
      else if (peano4::grid::PeanoCurve::isInOutStack(inFaceStack)) {{
        //_mapping->touchFaceFirstTime(fineGridFaces.x(outFaceStack),event.getH(),fineGridFaces.normal(outFaceStack),data);
      }}
    }}
  }}
"""


  TemplateEnterCell_CellLoad_Prologue = """  
  // Load cell {name}
  {{
    const int inCellStack  = event.getCellData();
    const int outCellStack = peano4::grid::PeanoCurve::CallStack;
    logDebug("enterCell(peano4::grid::GridTraversalEvent)", "cell " << inCellStack << "->pos-" << outCellStack );

    {full_qualified_type} data;
    if (inCellStack!=peano4::grid::TraversalObserver::CreateOrDestroyPersistentGridEntity) {{
      data = DataRepository::_{logical_type_name}Stack[ DataRepository::DataKey(_spacetreeId,inCellStack) ].pop();
    }}
    DataRepository::_{logical_type_name}Stack[ DataRepository::DataKey(_spacetreeId,outCellStack) ].push(data);
  }}
"""


  TemplateEnterCell_CellLoad_MappingCall = """  
  // Invoke creational events on cell {name}
  if (event.getCellData()==peano4::grid::TraversalObserver::CreateOrDestroyPersistentGridEntity) {{
    {ACTIVE_MAPPING}.createCell(
        event.getX(),
        event.getH(),
        {MAPPING_SIGNATURE_FINE_GRID_VERTICES_ARGUMENTS,}
        {MAPPING_SIGNATURE_FINE_GRID_FACES_ARGUMENTS,}
        DataRepository::_{logical_type_name}Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].top(0)
        {,MAPPING_SIGNATURE_COARSE_GRID_VERTICES_ARGUMENTS}
        {,MAPPING_SIGNATURE_COARSE_GRID_FACES_ARGUMENTS}
        {,MAPPING_SIGNATURE_COARSE_GRID_CELL_ARGUMENTS}
    );
  }}
"""


  TemplateEnterCell_MappingCall = """  
  {{
    {ACTIVE_MAPPING}.touchCellFirstTime(
       {MAPPING_SIGNATURE_FINE_GRID_VERTICES_ARGUMENTS,MAPPING_SIGNATURE_FINE_GRID_FACES_ARGUMENTS,MAPPING_SIGNATURE_FINE_GRID_CELL_ARGUMENTS}
      {,MAPPING_SIGNATURE_COARSE_GRID_VERTICES_ARGUMENTS,MAPPING_SIGNATURE_COARSE_GRID_FACES_ARGUMENTS,MAPPING_SIGNATURE_COARSE_GRID_CELL_ARGUMENTS}
    );
  }}
"""


  TemplateEnterCell_Epilogue = """  
  logTraceOut( "enterCell(peano4::grid::GridTraversalEvent)" );
}}
"""

  
  def __generate_enterCell(self,output_file):
    """
      Generates enter cell
    """
    output_file.write( self.TemplateEnterCell_Prologue.format(**self.d) )
          
    for vertex in self.vertices:
      self.d[ "name" ]                 = vertex.name
      self.d[ "enumeration_type" ]     = vertex.get_enumeration_type()
      self.d[ "logical_type_name" ]    = vertex.get_logical_type_name()
      self.d[ "full_qualified_type" ]  = vertex.get_full_qualified_type()
      output_file.write( self.TemplateEnterCell_VertexLoad_Prologue.format(**self.d) )
      self.__format_template_per_mapping(output_file, self.TemplateEnterCell_VertexLoad_MappingCall, False)

    for face in self.faces:
      self.d[ "name" ]                 = face.name
      self.d[ "enumeration_type" ]     = face.get_enumeration_type()
      self.d[ "logical_type_name" ]    = face.get_logical_type_name()
      self.d[ "full_qualified_type" ]  = face.get_full_qualified_type()
      output_file.write( self.TemplateEnterCell_FaceLoad_Prologue.format(**self.d) )
      self.__format_template_per_mapping(output_file, self.TemplateEnterCell_FaceLoad_MappingCall, False)

    for cell in self.cells:
      self.d[ "name" ]                 = cell.name
      #self.d[ "enumeration_type" ]     = cell.get_enumeration_type()
      self.d[ "logical_type_name" ]    = cell.get_logical_type_name()
      self.d[ "full_qualified_type" ]  = cell.get_full_qualified_type()
      output_file.write( self.TemplateEnterCell_CellLoad_Prologue.format(**self.d) )
      self.__format_template_per_mapping(output_file, self.TemplateEnterCell_CellLoad_MappingCall, False)


    self.__format_template_per_mapping(output_file, self.TemplateEnterCell_MappingCall, False)
    output_file.write( self.TemplateEnterCell_Epilogue.format(**self.d) )


  TemplateLeaveCell_Prologue = """  
void {FULL_QUALIFIED_CLASSNAME}::leaveCell( const peano4::grid::GridTraversalEvent&  event ) {{
  logTraceInWith1Argument( "leaveCell(peano4::grid::GridTraversalEvent)", event.toString() );
"""



  TemplateLeaveCell_MappingCall = """  
  {{
    {ACTIVE_MAPPING}.touchCellLastTime(
      {MAPPING_SIGNATURE_FINE_GRID_VERTICES_ARGUMENTS,MAPPING_SIGNATURE_FINE_GRID_FACES_ARGUMENTS,MAPPING_SIGNATURE_FINE_GRID_CELL_ARGUMENTS,MAPPING_SIGNATURE_COARSE_GRID_VERTICES_ARGUMENTS,MAPPING_SIGNATURE_COARSE_GRID_FACES_ARGUMENTS,MAPPING_SIGNATURE_COARSE_GRID_CELL_ARGUMENTS}
    );
  }}
"""


  TemplateLeaveCell_CellStore_MappingCall = """  
  if (event.getCellData()==peano4::grid::TraversalObserver::CreateOrDestroyPersistentGridEntity) {{
    {ACTIVE_MAPPING}.destroyCell(
         event.getX()
        ,event.getH()
        {,MAPPING_SIGNATURE_FINE_GRID_VERTICES_ARGUMENTS,MAPPING_SIGNATURE_FINE_GRID_FACES_ARGUMENTS}
        ,DataRepository::_{logical_type_name}Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].top(0)
        {,MAPPING_SIGNATURE_COARSE_GRID_VERTICES_ARGUMENTS,MAPPING_SIGNATURE_COARSE_GRID_FACES_ARGUMENTS,MAPPING_SIGNATURE_COARSE_GRID_CELL_ARGUMENTS}
    );
  }}
"""


  TemplateLeaveCell_CellStore_Epilogue = """  
  // Handle cell {name}
  {{
    const int inCellStack   = peano4::grid::PeanoCurve::CallStack;
    const int outCellStack  = event.getCellData();
    logDebug("leaveCell(peano4::grid::GridTraversalEvent)", "cell " << inCellStack << "->pos-" << outCellStack );

    {full_qualified_type} data = DataRepository::_{logical_type_name}Stack[ DataRepository::DataKey(_spacetreeId,inCellStack) ].pop();
    if (outCellStack!=peano4::grid::TraversalObserver::CreateOrDestroyPersistentGridEntity) {{
      DataRepository::_{logical_type_name}Stack[ DataRepository::DataKey(_spacetreeId,outCellStack) ].push(data);
    }}
  }}
"""


  TemplateLeaveCell_FaceStore_MappingCall = """
"""

  TemplateLeaveCell_FaceStore_Epilogue = """
  // Store face {name}
  {{
    auto view = DataRepository::_{logical_type_name}Stack[ DataRepository::DataKey(_spacetreeId,peano4::grid::PeanoCurve::CallStack) ].popBlock( TwoTimesD );
    for (int i=0; i<TwoTimesD; i++) {{
      int inFaceStackPosition  = event.getFaceDataFrom(i);
      int outFaceStack         = event.getFaceDataTo(i);
      {full_qualified_type} data = view.get(inFaceStackPosition);
      if (
        outFaceStack!=peano4::grid::TraversalObserver::CreateOrDestroyPersistentGridEntity
        and
        outFaceStack!=peano4::grid::TraversalObserver::CreateOrDestroyHangingGridEntity
      ) {{
        DataRepository::_{logical_type_name}Stack[ DataRepository::DataKey(_spacetreeId,outFaceStack) ].push(data);
      }}
    }}
  }}
"""

  TemplateLeaveCell_VertexStore_MappingCall = """
  {{
  }}
"""

  TemplateLeaveCell_VertexStore_Epilogue = """
"""


  TemplateLeaveCell_Epilogue = """  
  logTraceOut( "leaveCell(peano4::grid::GridTraversalEvent)" );
}}
"""


  def __generate_leaveCell(self,output_file):
    """
      Generates enter cell
    """
    output_file.write( self.TemplateLeaveCell_Prologue.format(**self.d) )
          
    for cell in self.cells:
      self.d[ "name" ]                 = cell.name
      self.d[ "logical_type_name" ]    = cell.get_logical_type_name()
      self.d[ "full_qualified_type" ]  = cell.get_full_qualified_type()
      self.__format_template_per_mapping(output_file, self.TemplateLeaveCell_CellStore_MappingCall, False)
      output_file.write( self.TemplateLeaveCell_CellStore_Epilogue.format(**self.d) )

    for face in self.faces:
      self.d[ "name" ]                 = face.name
      self.d[ "enumeration_type" ]     = face.get_enumeration_type()
      self.d[ "logical_type_name" ]    = face.get_logical_type_name()
      self.d[ "full_qualified_type" ]  = face.get_full_qualified_type()
      self.__format_template_per_mapping(output_file, self.TemplateLeaveCell_FaceStore_MappingCall, False)
      output_file.write( self.TemplateLeaveCell_FaceStore_Epilogue.format(**self.d) )
        
    for vertex in self.vertices:
      self.d[ "name" ]                 = vertex.name
      self.d[ "enumeration_type" ]     = vertex.get_enumeration_type()
      self.d[ "logical_type_name" ]    = vertex.get_logical_type_name()
      self.d[ "full_qualified_type" ]  = vertex.get_full_qualified_type()
      self.__format_template_per_mapping(output_file, self.TemplateLeaveCell_VertexStore_MappingCall, False)
      output_file.write( self.TemplateLeaveCell_VertexStore_Epilogue.format(**self.d) )

    output_file.write( self.TemplateLeaveCell_Epilogue.format(**self.d) )


  TemplateImplementationFilePrologue = """
#include "{CLASSNAME}.h"
#include "DataRepository.h"

#include "peano4/grid/PeanoCurve.h"

#include "peano4/datamanagement/CellWrapper.h"
#include "peano4/datamanagement/FaceEnumerator.h"
#include "peano4/datamanagement/VertexEnumerator.h"



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

