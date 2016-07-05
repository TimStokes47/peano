#include "petsc/runners/Runner.h"


#include "petsc/repositories/Repository.h"
#include "petsc/repositories/RepositoryFactory.h"

#include "peano/utils/UserInterface.h"

#include "tarch/Assertions.h"

#include "tarch/parallel/Node.h"
#include "tarch/parallel/NodePool.h"



#include "tarch/parallel/FCFSNodePoolStrategy.h"
#include "peano/parallel/loadbalancing/OracleForOnePhaseWithGreedyPartitioning.h"


// @todo Remove this include as soon as you've created your real-world geometry
#include "peano/geometry/Hexahedron.h" 


petsc::runners::Runner::Runner() {
  // @todo Insert your code here
}


petsc::runners::Runner::~Runner() {
  // @todo Insert your code here
}


int petsc::runners::Runner::run() {



  // @todo Insert your geometry generation here and adopt the repository 
  //       generation to your needs. There is a dummy implementation to allow 
  //       for a quick start, but this is really very dummy (it generates 
  //       solely a sphere computational domain and basically does nothing with 
  //       it).
  
  // Start of dummy implementation
  peano::geometry::Hexahedron geometry(
    tarch::la::Vector<DIMENSIONS,double>(1.0),
    tarch::la::Vector<DIMENSIONS,double>(0.0)
   );
  petsc::repositories::Repository* repository = 
    petsc::repositories::RepositoryFactory::getInstance().createWithSTDStackImplementation(
      geometry,
      tarch::la::Vector<DIMENSIONS,double>(1.0),   // domainSize,
      tarch::la::Vector<DIMENSIONS,double>(0.0)    // computationalDomainOffset
    );
  

  // This is new because of PETSc:
  if (tarch::parallel::Node::getInstance().isGlobalMaster()) {
    tarch::parallel::NodePool::getInstance().setStrategy(
      new tarch::parallel::FCFSNodePoolStrategy()
    );
  }
  tarch::parallel::NodePool::getInstance().restart();
  tarch::parallel::NodePool::getInstance().waitForAllNodesToBecomeIdle();
  peano::parallel::loadbalancing::Oracle::getInstance().setOracle(
    new peano::parallel::loadbalancing::OracleForOnePhaseWithGreedyPartitioning(false)
  );




  int result = 0;
  if (tarch::parallel::Node::getInstance().isGlobalMaster()) {
    result = runAsMaster( *repository );
  }
  #ifdef Parallel
  else {
    result = runAsWorker( *repository );
  }
  #endif
  
  delete repository;
  
  return result;
}


int petsc::runners::Runner::runAsMaster(petsc::repositories::Repository& repository) {
  peano::utils::UserInterface::writeHeader();

  // @todo Insert your code here
  
  // Start of dummy implementation
  
  repository.switchToCreateGrid(); repository.iterate();
  repository.switchToAssemble(); repository.iterate();
  repository.switchToPlot(); repository.iterate();

 
 
  repository.logIterationStatistics( true );
  repository.terminate();
  // End of dummy implementation

  return 0;
}
