#include "loadbalancing.h"

#include "tarch/logging/Log.h"

#include "peano4/parallel/SpacetreeSet.h"


namespace {
  // If I don't instantiate it here, i.e. outside of a function, then I run risk
  // that it is initialised late and thus has the wrong time stamp.
  tarch::logging::Log _log( "toolbox::loadbalancing" );
}


void toolbox::loadbalancing::dumpStatistics() {
  std::ostringstream msg;

  std::set<int> idsOfLocalSpacetrees = peano4::parallel::SpacetreeSet::getInstance().getLocalSpacetrees();
  for (auto p: idsOfLocalSpacetrees) {
    msg << "(#" << p << ":"
        << peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(p).getNumberOfLocalUnrefinedCells()
		<< "/"
		<< peano4::parallel::SpacetreeSet::getInstance().getGridStatistics(p).getNumberOfRemoteUnrefinedCells()
		<< ")";
  }

  logInfo( "dumpStatistics()", msg.str() );
}

