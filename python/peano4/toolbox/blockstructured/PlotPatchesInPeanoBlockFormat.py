# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.solversteps.Mapping import Mapping


# @todo getter einfuehren, so dass man sieht was gen Observer geht und was net
# @todo Konstanten fuer ops

class PlotPatchesInPeanoBlockFormat(Mapping):
  """
  Very simple plotter that should be used in combination with Patches.
  At the moment, I only support plain double patches, i.e. you have to 
  use the default source code generator for your stuff. This plotter 
  works only for cell patches, i.e. patches associated with faces or 
  vertices are not supported.
  
  At the moment, I can only plot one dataset (patch type) per plotter.
  In theory, the underlying Peano plotter however could dump multiple
  patches at once.
  
  Also, this dump expects incoming data to be AoS.
  """
  
  
  def __init__(self,filename,patch,dataset_name):
    self.d = {}
    self.d[ "FILENAME" ]           = filename
    self.d[ "UNKNOWNS" ]           = str(patch.no_of_unknowns)
    self.d[ "DOFS_PER_AXIS" ]      = str(patch.dim[0])
    self.d[ "NAME" ]               = dataset_name
        
    for i in patch.dim:
      if i!=patch.dim[0]:
        print( "Error: patch plotter requires patch to have same dimension along all coordinate axes")

    #self.dim       = dim
    #self.no_of_unknowns = no_of_unknowns

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
    _dataWriter = _writer->createCellDataWriter( "{NAME}", {DOFS_PER_AXIS}, {UNKNOWNS} );
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


  def get_mapping_name(self):
    return "PlotPatchesInPeanoBlockFormat"


  def user_should_modify_template(self):
    return False


  __Template_TouchCellFirstTime = """ 
  int vertexIndices[TwoPowerD];
  
  const double PatchScaling = 0.95;

  assertion( _writer!=nullptr );
  assertion( _dataWriter!=nullptr );
  
  const int patchIndex = _writer->plotPatch(
    center - h * PatchScaling * 0.5,
    h * PatchScaling
  );
 
  int cellIndex  = _dataWriter->getFirstCellWithinPatch(patchIndex);
  int dofCounter = 0;
  
  dfor(k,{DOFS_PER_AXIS}) {{
    for( int i=0; i<{DOFS_PER_AXIS}; i++) {{
    }}
    _dataWriter->plotCell( cellIndex, k(0) * 0.001 );
    cellIndex++;
  }}
"""


  def get_body_of_operation(self,operation_name):
    result = "\n"
    if operation_name==Mapping.OPERATION_TOUCH_CELL_FIRST_TIME:
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
#include "peano4/utils/Loop.h"
"""
