// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO_MAPPING_SPECIFICATION_H_
#define _PEANO_MAPPING_SPECIFICATION_H_


#include <string>


namespace peano {
  struct MappingSpecification;
}


/**
 * Specification for a mapping operation
 *
 * Gives Peano hints on the behaviour and, hence, supports a more aggressive
 * on-the-fly optimisation.
 *
 *
|| manipulates || Nop | Operation is empty, and the algorithm's semantics is preserved, if Peano kicks out the whole function call.
||             || OnlyLeaves | Algorithm's semantics is preserved, if Peano kicks out the whole function call for refined vertices of cells that have solely refined vertices around them.
||             || WholeTree  | No events are eliminated.
|| multithreading || Serial | Do not run these operation in parallel on a shared memory machine. Application hence doesn't need semaphores.
||                || RunConcurrentlyOnFineGrid | Peano runs the events in parallel on the finest grid, i.e. doesn't care about any data dependencies.
||                || AvoidFineGridRaces  | Peano can try to speed up the application due to multithreading. It ensures that events are invoked such that the coarse grid data (of enterCell, e.g.) is not shared with another thread. Is stronger than AVOID_COARSE_GRID_RACES, i.e. ensures this data consistency as well.
||                || AvoidCoarseGridRaces  | Peano can try to speed up the application due to multithreading. However, Peano ensures that events are invoked such that the fine grid data (of enterCell, e.g.) is not shared with another thread.
|| restartable    || false | May not try to recover if core fails.
||                || true  | If the operation is deployed to a thread of its own and this thread creashes, Peano may reexecute this operation. Consequently, some operations might be called multiple times due to a hardware failure, but the code is resiliency safe, i.e. does not crash due to simple hardware errors.
 *
 *
 *
 * @author Tobias Weinzierl
 */
struct peano::MappingSpecification {
  enum Manipulates  {
    Nop, OnlyLeaves, WholeTree
  };

  enum Multithreading {
    Serial, RunConcurrentlyOnFineGrid, AvoidCoarseGridRaces, AvoidFineGridRaces
  };

  Manipulates     manipulates;
  Multithreading  multithreading;
  bool            restartable;

  MappingSpecification(Manipulates manipulates_, Multithreading _multithreading_, bool _restartable_);

  /**
   * Most general specification
   *
   * Is used by the adapters to merge multiple specifications.
   */
  static MappingSpecification getMostGeneralSpecification();

  std::string toString() const;
};

/**
 * Combine two specifications. The resulting specification has weaker or equal
 * constraints. If one specification work for example only on the leaves and
 * supports multicore parallelism and the other one works on the whole tree
 * serially, then the result specification works serially on the whole tree.
 */
peano::MappingSpecification operator&(const peano::MappingSpecification& lhs, const peano::MappingSpecification& rhs);


#endif
