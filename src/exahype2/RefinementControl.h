// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _EXAHYPE2_REFINEMENT_CONTROL_H_
#define _EXAHYPE2_REFINEMENT_CONTROL_H_


#include <vector>
#include "peano4/grid/GridControlEvent.h"


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
 * This class is a very simple container which collects a point set of refinement
 * and coarsening commands. It then constructs bounding boxes from these guys.
 */
class exahype2::RefinementControl {
  public:
    RefinementControl();

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
  private:
	  static tarch::logging::Log  _log;

	  std::vector< peano4::grid::GridControlEvent >  _events;
};

#endif
