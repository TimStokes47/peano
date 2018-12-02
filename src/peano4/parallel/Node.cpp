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


int peano4::parallel::Node::getGlobalId(int rank, int threadId) const {
  const int numberOfRanks = tarch::parallel::Node::getInstance().getNumberOfNodes();
  return numberOfRanks * peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance * threadId
	   + rank          * peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance;
}


int peano4::parallel::Node::getRank(int id) const {
  const int numberOfRanks = tarch::parallel::Node::getInstance().getNumberOfNodes();
  return id % (numberOfRanks * peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance);
}


int peano4::parallel::Node::getThreadId( int id ) const {
  const int numberOfRanks = tarch::parallel::Node::getInstance().getNumberOfNodes();
  return id / (numberOfRanks * peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance);
}


int peano4::parallel::Node::getTag( int id ) const {
  assertionMsg(false, "not implemented ye");
  return 0;
}
