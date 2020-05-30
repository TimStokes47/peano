// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _EXAHYPE2_REFINEMENT_CONTROL_H_
#define _EXAHYPE2_REFINEMENT_CONTROL_H_


#include <vector>

#include "peano4/grid/GridControlEvent.h"

#include "tarch/multicore/BooleanSemaphore.h"

namespace exahype2 {
  enum class RefinementCommand {
    Keep, Refine, Coarsen
  };

  /**
   * The default is coarsen as this is the lowest priority command.
   */
  RefinementCommand getDefaultRefinementCommand();

  class RefinementControl;
}


/**
 * If one of the flags says refine, then refine. If one of the two flags says keep and
 * noone says refine, return keep. Coarsen if and only if both say coarsen.
 */
exahype2::RefinementCommand operator&&( exahype2::RefinementCommand lhs, exahype2::RefinementCommand rhs);

std::string toString( exahype2::RefinementCommand value );

/**
 * Manage refine/erase requests within ExaHyPE 2
 *
 * This class is a very simple container which collects a point set of
 * refinement and coarsening commands. It then constructs bounding boxes from
 * these guys and returns the bounding boxes to Peano 4.
 *
 * <h2> Multithreading </h2>
 *
 * As we use the class within a multithreaded environment, things become a
 * little bit more complicated. Peano 4 creates an observer object per
 * spacetree through a clone from observer no -1 (per rank, so this observer
 * is not really an observer. It is rather an observer prototype) and then
 * asks this observer how to refine. So we design the following pattern:
 *
 * - When an observer is created, it creates its own local refinement control.
 * - We hand out refine/erase commands from the global one as this is our
 *   globally valid repository.
 * - We accumulate the refine/erase instructions locally. So we clear() our
 *   local copy when we start to run through the grid and then pipe all
 *   commands into this local object.
 * - When we are done, we merge the local copy back.
 *
 * So far, everything is simple and straightforward. The only tricky part is
 * when to decide when to kick out old refine/coarsen commands. The prototype
 * refinement control object is kind of a repository where we take copies from,
 * but we never know how many copies will be created in the end. So we make
 * each local copy notify the global guy that it will accumulate something, i.e.
 * eventually merge() its content into the global object. As a result, the
 * global can work with two buffers - an accumulation buffer and a valid one.
 * Whenever we know that all copies have merged their stuff back, we swap/update
 * the buffers.
 *
 *
 */
class exahype2::RefinementControl {
  public:
    RefinementControl(double tolerance = 0.1);

    void startToAccumulateLocally();

    void clear();

    std::vector< peano4::grid::GridControlEvent >  getGridControlEvents() const;

    /**
	   * @param x Centre of the cell for which this command is triggered
	   * @param h Size of this cell
	   */
    void addCommand(
      const tarch::la::Vector<Dimensions,double>&  x,
      const tarch::la::Vector<Dimensions,double>&  h,
      exahype2::RefinementCommand                  command,
      bool                                         invokedByGridConstruction
    );

    void merge( const RefinementControl& control );
  private:
	  static tarch::logging::Log  _log;

	  static tarch::multicore::BooleanSemaphore  _semaphore;

	  /**
	   * We blow up the region around refinement criterion slightly.
	   */
	  const double _Tolerance;

	  int _accumulatingCopiesOfGlobalObject;

	  std::vector< peano4::grid::GridControlEvent >  _events;
	  std::vector< peano4::grid::GridControlEvent >  _validEventsFromPreviousSweeps;
};

#endif
