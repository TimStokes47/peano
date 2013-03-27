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
     * simulation only. In this case, the third entry should equals 0
     * due to the overloaded function below.
     */
    virtual void addWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<3,double>&  boundingBoxOffset,
      const tarch::la::Vector<3,double>&  boundingBoxSize
    ) = 0;

    virtual void removeWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<3,double>&  boundingBoxOffset,
      const tarch::la::Vector<3,double>&  boundingBoxSize
    ) = 0;

    /**
     * 2d wrapper
     */
    virtual void addWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<2,double>&  boundingBoxOffset,
      const tarch::la::Vector<2,double>&  boundingBoxSize
    ) {
      tarch::la::Vector<3,double> boundingBoxOffset3d;
      tarch::la::Vector<3,double> boundingBoxSize3d;

      tarch::la::assignList(boundingBoxOffset3d) = boundingBoxOffset(0), boundingBoxOffset(1), 0.0;
      tarch::la::assignList(boundingBoxSize3d)   = boundingBoxSize(0),   boundingBoxSize(1),   0.0;

      addWorker( workerRank, level, boundingBoxOffset3d, boundingBoxSize3d );
    }

    /**
     * 2d wrapper
     */
    virtual void removeWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<2,double>&  boundingBoxOffset,
      const tarch::la::Vector<2,double>&  boundingBoxSize
    ) {
      tarch::la::Vector<3,double> boundingBoxOffset3d;
      tarch::la::Vector<3,double> boundingBoxSize3d;

      tarch::la::assignList(boundingBoxOffset3d) = boundingBoxOffset(0), boundingBoxOffset(1), 0.0;
      tarch::la::assignList(boundingBoxSize3d)   = boundingBoxSize(0),   boundingBoxSize(1),   0.0;

      removeWorker( workerRank, level, boundingBoxOffset3d, boundingBoxSize3d );
    }
};


#endif
