#include "loadbalancing.h"

#include "tarch/logging/Log.h"

#include "peano4/parallel/SpacetreeSet.h"



void toolbox::loadbalancing::dumpStatistics() {
  tarch::logging::Log _log( "toolbox::loadbalancing" );

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

