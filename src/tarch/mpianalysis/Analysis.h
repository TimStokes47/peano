// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_MPIANALYIS_ANALYSIS_H_
#define _TARCH_MPIANALYIS_ANALYSIS_H_


#include "tarch/mpianalysis/Analyser.h"


namespace tarch {
  namespace mpianalysis {
    class Analysis;
  }
}



/**
 * Analyser Singleton
 *
 * This type/component results from the Peano project however is of more
 * general nature. It yields performance analysis and load balancing data with
 * respect to mpi. By default, the tarch forwards such information to the
 * logging facilities. However, you can also redirect them to your own types
 * to connect to postprocessing tools, e.g.
 *
 * Analysis is a plain singleton that holds an instance of Analyser. While
 * there is a default Analyser forwarding data to the info log level, you can
 * switch the device yourself.
 *
 * @author Roland Wittmann, Tobias Weinzierl
 */
class tarch::mpianalysis::Analysis {
  private:
    Analysis();

    Analyser*  _device;

    void deleteDevice();
  public:
    ~Analysis();

    static Analysis& getInstance();

    /**
     * Set new device. The Analysis object becomes responsible for the pointer,
     * i.e. will delete it automatically at shutdown (or if you set another
     * device again).
     */
    void setDevice( Analyser* device );

    Analyser& getDevice();
};


#endif
