# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.solversteps.ActionSet import ActionSet



class PlotScalarNodalFieldInPeanoBlockFormat(ActionSet):
  def __init__(self,filename,vertex_unknown,getter):
    """
      Plot only the grid structure
      
      filename         Name of the output file
      vertex_unknown   The object you have associated with a vertex and that you want to print
      getter           Getter acting on the vertex. Could be something alike getU() for example.
                       If there's no getter but you want to directly access the data, remove 
                       any brackets from the passed string.
    """
    self.d = {}
    self.d[ "FILENAME" ]            = filename
    self.d[ "VERTEX_UNKNOWN_NAME" ] = vertex_unknown.name
    self.d[ "GETTER" ]              = getter
        

  __Template_Constructor = """
  static int counter = 0;
  static tarch::multicore::BooleanSemaphore booleanSemaphore;
  tarch::multicore::Lock lock(booleanSemaphore);
    
  _writer      = nullptr;
  _dataWriter  = nullptr;
  _treeNumber  = treeNumber;

  // An MPI lock (critical section) would be important!
    
  if (_treeNumber>=0) {{
    const bool newFile = tarch::mpi::Rank::getInstance().isGlobalMaster() and _counter==0;
    logDebug( "PlotGrid2PlotGridInPeanoBlockFormat1()", "created tree instance for " << treeNumber << ". Create new file=" << newFile);
    _writer = new tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter(
      Dimensions,"{FILENAME}", not newFile
    );
    _dataWriter = _writer->createVertexDataWriter( "{VERTEX_UNKNOWN_NAME}", 2, 1 );
    _counter = counter;
    counter++;
  }}
"""


  def get_constructor_body(self):
    return self.__Template_Constructor.format(**self.d)


  __Template_Destructor = """
  if (_dataWriter!=nullptr) {{
    _dataWriter->close();

    std::ostringstream filename;
    filename << "{FILENAME}" << "-tree-" << _treeNumber << "-" << _counter;
    _writer->writeToFile( filename.str() );

    delete _dataWriter;
    delete _writer;

    _dataWriter = nullptr;
    _writer     = nullptr;
  }}
"""

    
  def get_destructor_body(self):
    return self.__Template_Destructor.format(**self.d)


  def get_body_of_getGridControlEvents(self):
    return "  return std::vector< peano4::grid::GridControlEvent >();\n" 


  def get_action_set_name(self):
    return __name__.replace(".py", "").replace(".", "_")


  def user_should_modify_template(self):
    return False


  __Template_TouchCellFirstTime = """ 
  int vertexIndices[TwoPowerD];

  int patchIndex = _writer->plotPatch(
    fineGridVertices{VERTEX_UNKNOWN_NAME}.x(0),
    fineGridVertices{VERTEX_UNKNOWN_NAME}.h()
  );

  assertion( _dataWriter!=nullptr );
  int vertexIndex  = _dataWriter->getFirstVertexWithinPatch(patchIndex);
  dfor2(k)
    auto data = fineGridVertices{VERTEX_UNKNOWN_NAME}(kScalar).{GETTER};
    _dataWriter->plotVertex( vertexIndex, data );
    vertexIndex++;
  enddforx
"""


  def get_body_of_operation(self,operation_name):
    result = "\n"
    if operation_name==ActionSet.OPERATION_TOUCH_CELL_FIRST_TIME:
      result = self.__Template_TouchCellFirstTime.format(**self.d) 
    return result


  def get_attributes(self):
    return """
    int                _treeNumber;
    int                _counter;

    tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter*                    _writer;
    tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::VertexDataWriter*  _dataWriter;
"""


  def get_includes(self):
    return """
#include "tarch/plotter/griddata/blockstructured/PeanoTextPatchFileWriter.h"
#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/Lock.h"
#include "tarch/mpi/Rank.h"

#include "peano4/utils/Loop.h"
"""
