// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _EXAMPLES_INTEGER_DIFFUSION_THROUGH_FACES_MAPPING_H_
#define _EXAMPLES_INTEGER_DIFFUSION_THROUGH_FACES_MAPPING_H_


#include "peano4/utils/Globals.h"
#include "tarch/la/Vector.h"


namespace examples {
  namespace integerdiffusionthroughfaces {
    class Mapping;
    struct CellData;
    struct FaceData;
  }
}


/**
 * This is basically an abstract from the observer. The observer is
 * informed about actions of the automata inside the grid. It then
 * translates the observed activities into user data movements and
 * invocations of this interface. On the long run, I want to generate
 * these mappings. Well, at least the header.
 */
class examples::integerdiffusionthroughfaces::Mapping {
  public:
	virtual void beginTraversal() = 0;
	virtual void endTraversal() = 0;

	virtual void createPersistentFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      int                                          normal,
	  FaceData&                                    data
    ) = 0;

	virtual void createHangingFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      int                                          normal,
	  FaceData&                                    data
    ) = 0;

	virtual void destroyPersistentFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      int                                          normal,
	  FaceData&                                    data
    ) = 0;

	virtual void destroyHangingFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      int                                          normal,
	  FaceData&                                    data
    ) = 0;

	virtual void touchFaceFirstTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      int                                          normal,
	  FaceData&                                    data
    ) = 0;

	virtual void touchFaceLastTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      int                                          normal,
	  FaceData&                                    data
    ) = 0;

	virtual void createCell(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  CellData&                                    data
    ) = 0;

	virtual void destroyCell(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  CellData&                                    data
    ) = 0;

	virtual void touchCellFirstTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  CellData&                                    data
    ) = 0;

	virtual void touchCellLastTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  CellData&                                    data
    ) = 0;
};

#endif
