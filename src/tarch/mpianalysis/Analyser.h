// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MPIANALYIS_ANALYSER_H_
#define _TARCH_MPIANALYIS_ANALYSER_H_


#include "tarch/la/Vector.h"


namespace tarch {
  namespace mpianalysis {
    class Analyser;
  }
}



/**
 * Analyser Interface
 *
 * @author Roland Wittmann, Tobias Weinzierl
 */
class tarch::mpianalysis::Analyser {
  public:
    virtual ~Analyser() {};

    // indicates start of a new global iteration run
    virtual void beginIteration() = 0;

    // indicates end of an running global iteration
    virtual void endIteration() = 0;

    /**
     * Inform analysis device about a worker and its associated domain
     * information. This information is 3d though it might be a 2d
     * simulation only. In this case, the third entry should equal 0.
     */
    virtual void addWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<3,double>&  boundingBoxOffset,
      const tarch::la::Vector<3,double>&  boundingBoxSize
    ) = 0;
};


#endif
