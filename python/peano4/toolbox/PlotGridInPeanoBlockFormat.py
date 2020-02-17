# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.solversteps.ActionSet import ActionSet



class PlotGridInPeanoBlockFormat(ActionSet):
  def __init__(self,filename,cell_unknown):
    """
      Plot only the grid structure
      
      filename       Name of the output file
      cell_unknown   If you use cell unknowns, pass any unknown in. As we do not dump
                     any semantic information about unknowns, it does not matter which 
                     one you choose. If you don't have cell unknowns at all, pass in 
                     None 
    """
    self.d = {}
    self.d[ "FILENAME" ]     = filename
    self.d[ "CELL_WRAPPER" ] = "Cell"
    if cell_unknown!=None:
      self.d[ "CELL_WRAPPER" ] += cell_unknown.name
    

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
    _dataWriter = _writer->createCellDataWriter( "cell-marker", 1, 1 );
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

  int indices = _writer->plotPatch(
    marker.x() - marker.h() * 0.5,
    marker.h()
  );

  assertion( _dataWriter!=nullptr );
  // @todo Use marker data here
 _dataWriter->plotCell(indices,1.0);
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

    tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter*                  _writer;
    tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter::CellDataWriter*  _dataWriter;
"""


  def get_includes(self):
    return """
#include "tarch/plotter/griddata/blockstructured/PeanoTextPatchFileWriter.h"
#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/Lock.h"
#include "tarch/mpi/Rank.h"
"""
