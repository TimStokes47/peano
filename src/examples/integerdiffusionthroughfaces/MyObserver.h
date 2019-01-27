// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _EXAMPLES_INTEGER_DIFFUSION_THROUGH_FACES_MYOBSERVER_H_
#define _EXAMPLES_INTEGER_DIFFUSION_THROUGH_FACES_MYOBSERVER_H_


#include "peano4/grid/TraversalObserver.h"
#include "peano4/stacks/STDVectorStack.h"


//#include <pair>
#include <map>


namespace examples {
  namespace integerdiffusionthroughfaces {
    class MyObserver;
  }
}


class examples::integerdiffusionthroughfaces::MyObserver: public peano4::grid::TraversalObserver {
  private:
	static tarch::logging::Log  _log;

	const int _spacetreeId;
	/**
	 * We use this counter to steer/hardcode dynamic AMR
	 */
	int _iterationCounter;

	/**
	 * To look up the right stack, we use a combination of tree number (id) and
	 * stack number.
	 */
	typedef std::pair<int,int>  DataKey;

	/**
	 * In this simple example, we store exactly the same thing both in cells
	 * and on faces. For most applications, that's not the case obviously.
	 */
	struct Data {
      #if PeanoDebug>0
      int                                    level;
      tarch::la::Vector<Dimensions,double>   x;
      #endif
	  int                                    value;
	};

	typedef peano4::stacks::STDVectorStack< Data >   FaceDataContainer;
	typedef peano4::stacks::STDVectorStack< Data >   CellDataContainer;

	std::map< DataKey, FaceDataContainer >   _faceData;
	std::map< DataKey, CellDataContainer >   _cellData;
  public:
	MyObserver(int spacetreeId = -1, int counter=0);

	virtual void beginTraversal() override;
	virtual void endTraversal() override;

	virtual void enterCell(
      const peano4::grid::GridTraversalEvent&  event
    ) override;


	virtual void leaveCell(
	  const peano4::grid::GridTraversalEvent&  event
    ) override;

	/**
	 * I use the clone to create one observer object per traversal thread. So
	 * between different spacetrees of one spacetree set, there can be no race
	 * condition. Yet, the clone() itself could be called in parallel.
	 */
	virtual TraversalObserver* clone(int spacetreeId) override;

	virtual std::vector< peano4::grid::GridControlEvent > getGridControlEvents() override;

	void createPersistentVertexAndPushOnStack(
      const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level,
      int                                          stackNumber
	) override;

	void createHangingVertexAndPushOnStack(
      const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level,
      int                                          stackNumber
	) override;

	void destroyPersistentVertexAndPopFromStack(
      const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level,
      int                                          stackNumber
	) override;

	void destroyHangingVertexAndPopFromStack(
      const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level,
      int                                          stackNumber
	) override;

	void createPersistentFaceAndPushOnStack(
      const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level,
	  int                                          normal,
      int                                          stackNumber
	) override;

	void createHangingFaceAndPushOnStack(
      const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level,
	  int                                          normal,
      int                                          stackNumber
	) override;

	void destroyPersistentFaceAndPopFromStack(
      const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level,
	  int                                          normal,
      int                                          stackNumber
	) override;

	void destroyHangingFaceAndPopFromStack(
      const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level,
	  int                                          normal,
      int                                          stackNumber
	) override;

	void createCellAndPushOnStack(
      const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level,
      int                                          stackNumber
    ) override;

	void destroyCellAndPopFromStack(
      const tarch::la::Vector<Dimensions,double>&  x,
	  int                                          level,
      int                                          stackNumber
    ) override;
};

#endif
