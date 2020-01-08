# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Mapping import Mapping


# @todo getter einfuehren, so dass man sieht was gen Observer geht und was net
# @todo Konstanten fuer ops

class PlotGridInPeanoBlockFormat(Mapping):
  def __init__(self,filename):
    self.filename = filename


  def get_constructor_body(self):
    return """
    static int counter = 0;
    static tarch::multicore::BooleanSemaphore booleanSemaphore;
    tarch::multicore::Lock lock(booleanSemaphore);
    
    _writer      = nullptr;
    _dataWriter  = nullptr;
    _treeNumber  = treeNumber;

    // An MPI lock (critical section) would be important!
    
    if (_treeNumber>=0) {
      const bool newFile = tarch::mpi::Rank::getInstance().isGlobalMaster() and _counter==0;
      logDebug( "PlotGrid2PlotGridInPeanoBlockFormat1()", "created tree instance for " << treeNumber << ". Create new file=" << newFile);
      _writer = new tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter(
        Dimensions,\"""" + self.filename + """\", not newFile
      );
      _dataWriter = _writer->createCellDataWriter( "cell-data", 1, 1 );
      _counter = counter;
      counter++;
    }
"""


  def get_destructor_body(self):
    return """
if (_dataWriter!=nullptr) {
  _dataWriter->close();

  std::ostringstream filename;
  filename << \"""" + self.filename + """\" << "-tree-" << _treeNumber << "-" << _counter;
  _writer->writeToFile( filename.str() );

  delete _dataWriter;
  delete _writer;

  _dataWriter = nullptr;
  _writer     = nullptr;
}
   """
    

  def get_body_of_getGridControlEvents(self):
    return "  return std::vector< peano4::grid::GridControlEvent >();\n" 


  def get_mapping_name(self):
    return "PlotGridInPeanoBlockFormat"


  def user_should_modify_template(self):
    return False


  def get_body_of_operation(self,operation_name):
    return "\n"

#void examples::integerdiffusionthroughfaces::PeanoFormatCellDataPlotter::plotCell(
#  const tarch::la::Vector<Dimensions,double>&  center,
#  const tarch::la::Vector<Dimensions,double>&  h,
#  CellData&                                    data
#) {
#  int vertexIndices[TwoPowerD];

#  // As we have only one index per patch, we don't have to convert anything
#  int indices = _writer->plotPatch(
#    center - h * 0.5,
#    h
#  );
#
#  assertion( _dataWriter!=nullptr );
#  assertion( _oldDataWriter!=nullptr );
#  _dataWriter->plotCell(indices,data.value);
#  _oldDataWriter->plotCell(indices,data.oldValue);
#}


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
