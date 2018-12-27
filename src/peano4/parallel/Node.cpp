#include "Node.h"


#include "peano4/grid/Spacetree.h"


#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds



#include "tarch/Assertions.h"
// @todo Node should be called Rank
// @todo parallel should be called mpi as namespace
#include "tarch/parallel/Node.h"


#include "tarch/multicore/Lock.h"


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



void peano4::parallel::Node::deregisterId(int id) {
  assertion( _bookedLocalThreads.count(id)==1 );
  _bookedLocalThreads.erase(id);
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


bool peano4::parallel::Node::isBoundaryExchangeInputStackNumber(int id) const {
  return id>=peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance
     and ( (id-peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance) % 2 == 1 );
}


int peano4::parallel::Node::getIdOfBoundaryExchangeOutputStackNumber(int number) const {
  assertion( isBoundaryExchangeOutputStackNumber(number) );
  assertion2( (number-peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance)%2==0, number, peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance );
  return (number-peano4::grid::Spacetree::MaxNumberOfStacksPerSpacetreeInstance) / 2;
}


void peano4::parallel::Node::sendVertexSynchronously( const peano4::grid::GridVertex& vertex, int fromId, int toId ) {
  tarch::multicore::Lock lock( _semaphore );

  std::pair<int,int> key( fromId, toId );

  _sendReceiveBuffer[key].push( vertex );

  logDebug( "sendVertexSynchronously(...)", "tree " << fromId << " pushes vertex " << vertex.toString() << " to buffer for tree " << toId );
}


peano4::grid::GridVertex peano4::parallel::Node::getVertexSynchronously( int fromId, int toId ) {
  std::pair<int,int> key( fromId, toId );

  tarch::multicore::Lock lock( _semaphore );
  bool isEmpty = _sendReceiveBuffer[key].empty();
  lock.free();

  while ( isEmpty ) {
    std::this_thread::sleep_for (std::chrono::seconds(1));
    lock.lock();
    isEmpty = _sendReceiveBuffer[key].empty();
	lock.free();
  }

  lock.lock();
  peano4::grid::GridVertex result = _sendReceiveBuffer[key].front();
  _sendReceiveBuffer[key].pop();

  logDebug( "getVertexSynchronously(int,int)", "deploy " << result.toString() << " from " << fromId << " to tree " << toId );

  return result;
}
