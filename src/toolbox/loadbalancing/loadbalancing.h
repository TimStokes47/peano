// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TOOLBOX_LOAD_BALANCING_LOAD_BALANCING_
#define _TOOLBOX_LOAD_BALANCING_LOAD_BALANCING_



namespace toolbox {
  /**
   * @namespace toolbox::loadbalancing
   *
   * Suite of generic load balancing algorithms for Peano 4.
   */
  namespace loadbalancing {
    /**
     * Dump the stats of the lb to the terminal (info device). It is invoked
     * around once per time step/grid sweep and runs through all the spacetrees
     * held on this particular rank.
     */
    void dumpStatistics();
  }
}


#endif
