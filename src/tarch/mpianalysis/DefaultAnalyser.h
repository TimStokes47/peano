// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MPIANALYIS_DEFAULT_ANALYSER_H_
#define _TARCH_MPIANALYIS_DEFAULT_ANALYSER_H_


#include "tarch/mpianalysis/Analyser.h"


namespace tarch {
  namespace mpianalysis {
    class DefaultAnalyser;
  }
}



/**
 * Analyser Interface
 *
 * @author Roland Wittmann, Tobias Weinzierl
 */
class tarch::mpianalysis::DefaultAnalyser: public tarch::mpianalysis::Analyser {
  public:
    DefaultAnalyser();
    virtual ~DefaultAnalyser();

    virtual void beginIteration();

    virtual void endIteration();

    virtual void addWorker(
      int                                 workerRank,
      int                                 level,
      const tarch::la::Vector<3,double>&  boundingBoxOffset,
      const tarch::la::Vector<3,double>&  boundingBoxSize
    );
};


#endif
