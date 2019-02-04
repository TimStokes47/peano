// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _EXAMPLES_INTEGER_DIFFUSION_THROUGH_FACES_MYMAPPING_H_
#define _EXAMPLES_INTEGER_DIFFUSION_THROUGH_FACES_MYMAPPING_H_


#include "peano4/utils/Globals.h"
#include "tarch/la/Vector.h"
#include "tarch/logging/Log.h"


namespace examples {
  namespace integerdiffusionthroughfaces {
    class MyMapping;
  }
}


/**
 * This is basically an abstract from the observer. The observer is
 * informed about actions of the automata inside the grid. It then
 * translates the observed activities into user data movements and
 * invocations of this interface. On the long run, I want to generate
 * these mappings. Well, at least the header.
 */
class examples::integerdiffusionthroughfaces::MyMapping {
  private:
	static tarch::logging::Log   _log;
  public:
	void createPersistentFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      int                                          normal,
	  int&                                         data
    );

	void createHangingFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      int                                          normal,
	  int&                                         data
    );

	void destroyPersistentFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      int                                          normal,
	  int&                                         data
    );

	void destroyHangingFace(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      int                                          normal,
	  int&                                         data
    );

	void touchFaceFirstTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      int                                          normal,
	  int&                                         data
    );

	void touchFaceLastTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
      int                                          normal,
	  int&                                         data
    );

	void createCell(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  int&                                         data
    );

	void destroyCell(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  int&                                         data
    );

	/**
	 * @todo Jetzt kommen aber die Enumeratoren rein hier fuer die Faces und Vertices
	 */
	void touchCellFirstTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  int&                                         data
    );

	void touchCellLastTime(
      const tarch::la::Vector<Dimensions,double>&  center,
      const tarch::la::Vector<Dimensions,double>&  h,
	  int&                                         data
    );
};

#endif
