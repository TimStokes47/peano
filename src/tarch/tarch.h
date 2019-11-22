// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _TARCH_H_
#define _TARCH_H_


/**
 * @namespace tarch
 *
 * tarch denotes the Technical Architectures. It is used within Peano, but it
 * is designed in a way that it is more or less independent of Peano, i.e.
 * Peano uses the tarch but not the other way round.
 *
 * Please note that the tarch in the Peano4 repository/branch is not the same
 * as the tarch in previous Peano versions. I've cleaned it up quite a lot and
 * old features that are not used anymore are removed.
 *
 * <h1> Debug level </h2>
 *
 * - PeanoDebug = 1: Enable tracing
 * - PeanoDebug = 2: Enable assertions (includes level 1)
 * - PeanoDebug = 4: Enable debug messages (includes level 2)
 */
namespace tarch {
  void writeCopyrightMessage();
}


#endif
