// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include "tarch/logging/Log.h"
#include "toolbox/loadbalancing/AbstractLoadBalancing.h"

#include <initializer_list>
#include <list>


namespace toolbox {
  namespace loadbalancing {
    namespace strategies {
      class Hardcoded;
    } // namespace strategies
  }   // namespace loadbalancing
} // namespace toolbox


/**
 * Hardcoded domain decomposition strategy
 *
 * Hardcoded load balancing is useful if you know exactly which domain
 * decomposition you want to build up, or if you have to debug a particular
 * (deterministic) setup. It also comes in handy whenever you can transform
 * your dynamic load balancing challenge into an offline problem. The load
 * balancing's Python API contains all the tools to derive good balancing
 * strategies for offline problems (see @ref page_toolbox_loadbalancing_api).
 *
 *
 * ## Enumerated splits
 *
 * The strategy holds an enumerated list of splits. The @f$ k @f$ th split
 * reads
 *
 * > Tree splittingTrees[k] hands numberOfCells[k] fine grid cells over to a
 * > new tree on rank destinationRanks[k], and it uses modes[k] to select
 * > these cells.
 *
 * The strategy does not say when a split happens. It hands out the splits
 * in the order given, and it hands out the @f$ k @f$ th split as soon as
 *
 * 1. the splitting tree exists on this rank,
 * 2. the splitting tree holds more than numberOfCells[k] unrefined cells,
 *    i.e. it keeps at least one cell after the split, and
 * 3. Peano accepts the split, i.e. the splitting tree is not busy with
 *    another split or join.
 *
 * If one of these conditions does not hold, the @f$ k @f$ th split and all
 * subsequent splits wait for the next mesh sweep. Several splits can go out
 * in the same mesh sweep. Cells that the splitting tree has already promised
 * to other trees in this sweep do not count towards the second condition.
 *
 * @image html Hardcoded.png
 *
 * The picture is generated from Hardcoded.dot via
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * dot -Tpng -Gdpi=110 Hardcoded.dot -o Hardcoded.png
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * As the splits wait until the splitting tree is large enough, they usually
 * fire once the mesh construction has reached the mesh level on which the
 * cells are to be found. For adaptive meshes, a split might hence fire before
 * the mesh construction has terminated.
 *
 *
 * ## Split modes
 *
 * peano4::SplitInstruction::Mode::AggressiveTopDown hands over whole refined
 * cells including all their children, while
 * peano4::SplitInstruction::Mode::BottomUp selects cells on the finest mesh
 * level. Peano applies the mode of the first split that a tree triggers in a
 * mesh sweep to all splits of this tree in this sweep. The strategy
 * therefore never lets one tree issue two splits with different modes in the
 * same mesh sweep. A split with a different mode waits for the next mesh
 * sweep.
 *
 *
 * ## Examples
 *
 * Split a regular mesh of @f$ 27 \times 27 @f$ cells into four trees on
 * one rank:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * new toolbox::loadbalancing::strategies::Hardcoded(
 *   {0, 0, 0},          // splitting trees
 *   {182, 182, 182},    // number of cells
 *   {0, 0, 0},          // destination ranks
 *   peano4::SplitInstruction::Mode::AggressiveTopDown
 * );
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Tree 0 holds 9 and then 81 cells while the mesh is built up, which is not
 * enough for the first split. Once tree 0 holds all 729 cells, all three
 * splits go out in the same mesh sweep: Tree 0 has 729, 547 and 365 cells
 * left over, respectively, before it hands out the next 182 cells.
 *
 * If you change the mode of the last split into BottomUp
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * new toolbox::loadbalancing::strategies::Hardcoded(
 *   {0, 0, 0},
 *   {182, 182, 182},
 *   {0, 0, 0},
 *   {
 *     peano4::SplitInstruction::Mode::AggressiveTopDown,
 *     peano4::SplitInstruction::Mode::AggressiveTopDown,
 *     peano4::SplitInstruction::Mode::BottomUp
 *   }
 * );
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * then the first two splits go out together, while the third split has to
 * wait: It is not allowed to go out in the same sweep as the top-down splits,
 * and Peano does not accept it while tree 0 is still busy with the first two
 * splits.
 *
 * Splits can also continue to split trees that earlier splits have created:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * new toolbox::loadbalancing::strategies::Hardcoded(
 *   {0, 1},             // split 0 creates tree 1, split 1 splits tree 1
 *   {364, 182},
 *   {0, 0},
 *   peano4::SplitInstruction::Mode::BottomUp
 * );
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Here, the second split waits until tree 1 exists and holds more than 182
 * cells.
 *
 *
 * ## MPI
 *
 * Each rank keeps only the splits whose splitting tree lives on this rank
 * (see peano4::parallel::Node::getRank()) and hands them out in the order
 * given. The ranks hence do not have to synchronise.
 *
 * The idea behind the hardcoded strategy is that users pipe in a particular
 * configuration tied to their specific choice of ranks. If a hardcoded setup
 * does not use all ranks, there's nothing we can do here. Indeed, there might
 * be cases where this makes sense, if the hardcoded strategy is used in
 * combination with another load balancing scheme. If the strategy is used with
 * fewer ranks than the configuration uses, i.e. if the hardcoded strategy wants
 * to deploy to rank 20 and there are only 10 ranks, then we write an error and
 * continue with a mock up. See Split::validateAndAmend() for details.
 *
 *
 * ## Support for files
 *
 * I originally thought it would be nice to have support for hard-coded
 * load-balancing being encoded in a file. However, this contradicts Peano's
 * philosophy that everything should be in the Python script. Therefore, this
 * strategy does **not** support file I/O but the load balancing's Python
 * interface does.
 *
 *
 * ## Trivia
 *
 * There's also a hardcoded orchestration strategy in
 * tarch::multicore::orchestration::Hardcoded. This is a totally different
 * notion of being hardcoded: There's the decision-making is hardcoded whereas
 * here the actual decisions are. In this sense, the present name might be
 * chosen inappropriately.
 */
class toolbox::loadbalancing::strategies::Hardcoded:
  public toolbox::loadbalancing::AbstractLoadBalancing {
public:
  /**
   * Construct hard-coded load balancing strategy
   *
   * The splits are passed as four initialiser lists. Entry @f$ k @f$ of
   * each list describes the @f$ k @f$ th split. So if you pass
   *
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * {2,2,2}, {11,12,13}, {22,23,24}, {BottomUp,BottomUp,AggressiveTopDown}
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *
   * then tree 2 will split three times. It will deploy 11, 12 and 13 cells
   * to the ranks 22, 23 and 24, respectively. See the class documentation
   * for when the splits happen.
   *
   *
   * ## Implementation
   *
   * We do not work with initialiser lists internally. Instead, the
   * constructor takes the list and transforms it into a sequence of
   * Split objects which are then held internally.
   *
   * I originally wanted the constructor to dumb some info on what
   * type of load balancing exactly had been constructed. However, some codes
   * use the load balancing object as static variable, so it might come up
   * before the logging infrastructure is properly configured. This means
   * that the logInfo will crash. So better stay silent and avoid logInfo()
   * calls here.
   *
   *
   * ## Pitfall
   *
   * At this point, I may not yet call validateAndAmend() or filter out the
   * splits of other ranks, as this requires MPI to be properly initialised.
   * If the hardcoded strategy is used as kind of a global variable - a
   * typical application case - then the initialisation consequently breaks
   * down. Both steps therefore happen in the first finishStep().
   */
  Hardcoded(
    std::initializer_list<int>                            splittingTrees,
    std::initializer_list<int>                            numberOfCells,
    std::initializer_list<int>                            destinationRanks,
    std::initializer_list<peano4::SplitInstruction::Mode> modes
  );

  /**
   * Shortcut around other constructor
   *
   * In this variant, all split modes are of the same type and there's a
   * default. See the other constructor for details.
   */
  Hardcoded(
    std::initializer_list<int>     splittingTrees,
    std::initializer_list<int>     numberOfCells,
    std::initializer_list<int>     destinationRanks,
    peano4::SplitInstruction::Mode mode = peano4::SplitInstruction::Mode::
      BottomUp
  );

  /**
   * Finish the current sweep
   *
   *
   * ## Issue the split instructions
   *
   * This routine hands out the splits that are due according to the rules
   * in the class documentation. Upon the first call, it also drops all
   * splits whose splitting tree is hosted by another rank, and it validates
   * the remaining ones.
   *
   * ## Disable strategy
   *
   * If all the splits have been issued, the strategy disables
   * itself. Usually, it is up to the user to disable a load balancing
   * scheme if they want this to happen. For the hard-coded variant, this
   * makes no sense: If there are no split instructions left over, we can
   * as well just disable the strategy straightaway. This helps to speed up
   * other things potentially, where a decision depends on the load balancing
   * being deactivated already.
   *
   * ## Update statistics
   *
   * Standard step of any load balancing: update the global view of the
   * statistics, and then also the blacklist. Eventually, we dump the
   * statistics to the terminal.
   */
  virtual void finishStep() override;

  virtual void finishSimulation() override;

private:
  static tarch::logging::Log _log;

  struct Split {
    int                            splittingTree;
    int                            numberOfCells;
    int                            destinationRank;
    peano4::SplitInstruction::Mode mode;
    Split(
      int                            splittingTree_,
      int                            numberOfCells_,
      int                            destinationRank_,
      peano4::SplitInstruction::Mode mode_
    );

    std::string toString() const;

    /**
     * Validate a split instruction
     *
     * This operation validates the split operation and amends its state if
     * required. That means notably that it looks if the target tree does
     * exist in an MPI environment. If not, it uses a module division to fix
     * the assignment.
     *
     *
     * ## Implementation pitfalls
     *
     * I originally called validateAndAmend immediately when I built up the
     * split lists. However, this happens at startup time for many codes. At
     * this point MPI_Init might not have been called, i.e. the rank might
     * not yet be properly configured.
     */
    void validateAndAmend();
  };

  /**
   * Drop the splits of other ranks and validate the remaining ones
   *
   * Is called by the first finishStep(). See the pitfall discussion in the
   * constructor.
   */
  void keepAndValidateLocalSplits();

  /**
   * Hand out the splits that are due
   *
   * Runs through the splits in the order given and stops at the first split
   * which is not due yet. See the class documentation for the rules.
   */
  void issueSplits();

  std::list<Split> _splits;

  bool _hasKeptAndValidatedLocalSplits;
};
