# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from peano4.solversteps.Mapping import Mapping



class ProjectPatchOntoFaces(Mapping):
  """
  This class assumes that you have NxNxN patch within your block. It also assumes that 
  you have an 2MxNxN patch on your faces. M<N. The code interprets these face-associated
  data as overlap with the patch, hooks into touchCellLastTime, and projects the cell's
  patch data onto the overlap (simple copy). 
  
  
  patch          Instance of peano4.datamodel.Patch
  patch_overlap  Instance of peano4.datamodel.Patch. Consult remark above about how the
                 dimensions of this overlap patch have to match. 
  """
  
  
  def __init__(self,patch,patch_overlap):
    self.d = {}
    if patch_overlap.dim[0] % 2 != 0:
      print( "Error: Patch associated to face has to have even number of cells. Otherwise, it is not a symmetric overlap." )
      assert( patch_overlap.dim[0] % 2 == 0 )
    if patch.dim[0] != patch.dim[1]:
      print( "Error: Can only handle square patches." )
      assert( patch.dim[0] == patch.dim[1] )
    if patch_overlap.dim[1] != patch.dim[0]:
      print( "Error: Patch of overlap and patch of cell have to match" )
      assert( patch_overlap.dim[1] == patch.dim[0] )
      
    #self.d[ "FILENAME" ]           = filename



  def get_constructor_body(self):
    return ""

  
  def get_destructor_body(self):
    return ""


  def get_body_of_getGridControlEvents(self):
    return "  return std::vector< peano4::grid::GridControlEvent >();\n" 


  def get_mapping_name(self):
    return __name__.split(".")[-1].replace(".py", "").replace(".", "_")
    #return __name__.split(".")[-1].replace(".py", "")


  def user_should_modify_template(self):
    return False


  __Template_TouchCellFirstTime = """ 
  int vertexIndices[TwoPowerD];
  
  const double PatchScaling = 0.95;

  assertion( _writer!=nullptr );
  assertion( _dataWriter!=nullptr );
  
  const int patchIndex = _writer->plotPatch(
    fineGridCell{NAME}.centre() - fineGridCell{NAME}.h() * PatchScaling * 0.5,
    fineGridCell{NAME}.h() * PatchScaling
  );
 
  int cellIndex  = _dataWriter->getFirstCellWithinPatch(patchIndex);
  int currentDoF = 0;
  
  dfor(k,{DOFS_PER_AXIS}) {{
    double* data = fineGridCell{NAME}.data().value + currentDoF;
    _dataWriter->plotCell( cellIndex, data );
    cellIndex++;
    currentDoF += {UNKNOWNS};
  }}
"""


  def get_body_of_operation(self,operation_name):
    result = "\n"
    if operation_name==Mapping.OPERATION_TOUCH_CELL_FIRST_TIME:
      #result = self.__Template_TouchCellFirstTime.format(**self.d)
      pass 
    return result


  def get_attributes(self):
    return ""


  def get_includes(self):
    return """
#include "tarch/plotter/griddata/blockstructured/PeanoTextPatchFileWriter.h"
#include "tarch/multicore/BooleanSemaphore.h"
#include "tarch/multicore/Lock.h"
#include "tarch/mpi/Rank.h"
#include "peano4/utils/Loop.h"
"""
