#include "Node.h"


#include "peano4/grid/Spacetree.h"



#include "tarch/Assertions.h"
// @todo Node should be called Rank
// @todo parallel should be called mpi as namespace
#include "tarch/parallel/Node.h"


tarch::logging::Log  peano4::parallel::Node::_log("peano4::parallel::Node");


peano4::parallel::Node::Node() {
}


peano4::parallel::Node::~Node() {
}


bool peano4::parallel::Node::isGlobalMaster(int rank, int threadId) const {
  return rank==0 and threadId==0;
}


peano4::parallel::Node& peano4::parallel::Node::getInstance() {
  static Node singleton;
  return singleton;
}


int peano4::parallel::Node::getId(int rank, int threadId) const {
  const int numberOfRanks = tarch::parallel::Node::getInstance().getNumberOfNodes();
  return numberOfRanks * threadId + rank;
}


int peano4::parallel::Node::getRank(int id) const {
  const int numberOfRanks = tarch::parallel::Node::getInstance().getNumberOfNodes();
  return id % numberOfRanks;
}


int peano4::parallel::Node::getNextFreeLocalId() const {
  int localThread = 0;
  const int localRank = tarch::parallel::Node::getInstance().getRank();
  while (true) {
	if ( _bookedLocalThreads.count( getId(localRank,localThread) )==0 ) return getId(localRank,localThread);
	localThread++;
  }
  return -1;
}


void peano4::parallel::Node::registerId(int id) {
  assertion( _bookedLocalThreads.count(id)==0 );
  _bookedLocalThreads.insert(id);
}


int peano4::parallel::Node::getOutputStackNumberOfBoundaryExchange(int id) const {
  return peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance + id * 2;
}


int peano4::parallel::Node::getInputStackNumberOfBoundaryExchange(int id) const {
  return peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance + id * 2 + 1;
}


/*
bool peano4::parallel::Node::isInputStackNumber(int id) const {
  return id>=peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance
     and ( (id-peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance) % 2 == 1 );
}
*/


bool peano4::parallel::Node::isBoundaryExchangeOutputStackNumber(int id) const {
  return id>=peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance
     and ( (id-peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance) % 2 == 0 );
}


int peano4::parallel::Node::getIdOfBoundaryExchangeOutputStackNumber(int number) const {
  assertion( isBoundaryExchangeOutputStackNumber(number) );
  return (number-peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance) / 2;
}
