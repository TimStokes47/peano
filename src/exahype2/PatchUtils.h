// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _EXAHYPE2_PATCH_UTILS_H_
#define _EXAHYPE2_PATCH_UTILS_H_


#include <vector>

#include "tarch/la/Vector.h"

#include "peano4/grid/GridControlEvent.h"
#include "peano4/utils/Globals.h"


namespace exahype2 {
  /**
   * In ExaHyPE's Finite Volume setup, a cell hosts a patch of Finite Volumes.
   * When we iterate over these volumes, we typically have to know the centre
   * of the volume.
   *
   * @param x      Centre of the cell
   * @param h      Size of the cell
   * @param index  Index of Finite Volume (in lexicographic ordering)
   */
  tarch::la::Vector<Dimensions,double>  getVolumeCentre(
    const tarch::la::Vector<Dimensions,double>&  x,
    const tarch::la::Vector<Dimensions,double>&  h,
    const tarch::la::Vector<Dimensions,int>&     numberOfVolumesPerPatch,
    const tarch::la::Vector<Dimensions,int>&     index
  );

  tarch::la::Vector<Dimensions,double>  getVolumeSize(
    const tarch::la::Vector<Dimensions,double>&  h,
    const tarch::la::Vector<Dimensions,int>&     numberOfVolumesPerPatch
  );
}

#endif
