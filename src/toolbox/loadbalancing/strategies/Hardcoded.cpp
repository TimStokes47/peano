#include "toolbox/loadbalancing/strategies/Hardcoded.h"

#include "peano4/parallel/Node.h"
#include "peano4/parallel/SpacetreeSet.h"
#include "tarch/Assertions.h"
#include "tarch/multicore/Core.h"
#include "toolbox/loadbalancing/loadbalancing.h"

#include <map>


tarch::logging::Log toolbox::loadbalancing::strategies::Hardcoded::_log(
  "toolbox::loadbalancing::strategies::Hardcoded"
);


toolbox::loadbalancing::strategies::Hardcoded::Hardcoded(
  std::initializer_list<int>                            splittingTrees,
  std::initializer_list<int>                            numberOfCells,
  std::initializer_list<int>                            destinationRanks,
  std::initializer_list<peano4::SplitInstruction::Mode> modes
):
  AbstractLoadBalancing(nullptr, nullptr),
  _hasKeptAndValidatedLocalSplits(false) {
  assertionEquals3(
    splittingTrees.size(),
    numberOfCells.size(),
    splittingTrees.size(),
    numberOfCells.size(),
    destinationRanks.size()
  );
  assertionEquals3(
    destinationRanks.size(),
    numberOfCells.size(),
    splittingTrees.size(),
    numberOfCells.size(),
    destinationRanks.size()
  );
  assertionEquals3(
    modes.size(),
    numberOfCells.size(),
    splittingTrees.size(),
    numberOfCells.size(),
    destinationRanks.size()
  );

  auto splittingTree   = splittingTrees.begin();
  auto cells           = numberOfCells.begin();
  auto destinationRank = destinationRanks.begin();
  auto mode            = modes.begin();

  while (splittingTree != splittingTrees.end()) {
    _splits.push_back(Split(*splittingTree, *cells, *destinationRank, *mode));
    splittingTree++;
    cells++;
    destinationRank++;
    mode++;
  }
}


toolbox::loadbalancing::strategies::Hardcoded::Hardcoded(
  std::initializer_list<int>     splittingTrees,
  std::initializer_list<int>     numberOfCells,
  std::initializer_list<int>     destinationRanks,
  peano4::SplitInstruction::Mode mode
):
  AbstractLoadBalancing(nullptr, nullptr),
  _hasKeptAndValidatedLocalSplits(false) {
  assertionEquals3(
    splittingTrees.size(),
    numberOfCells.size(),
    splittingTrees.size(),
    numberOfCells.size(),
    destinationRanks.size()
  );
  assertionEquals3(
    destinationRanks.size(),
    numberOfCells.size(),
    splittingTrees.size(),
    numberOfCells.size(),
    destinationRanks.size()
  );

  auto splittingTree   = splittingTrees.begin();
  auto cells           = numberOfCells.begin();
  auto destinationRank = destinationRanks.begin();

  while (splittingTree != splittingTrees.end()) {
    _splits.push_back(Split(*splittingTree, *cells, *destinationRank, mode));
    splittingTree++;
    cells++;
    destinationRank++;
  }
}


void toolbox::loadbalancing::strategies::Hardcoded::Split::validateAndAmend() {
  const int MaxRanks = tarch::mpi::Rank::getInstance().getNumberOfRanks();

  if (destinationRank >= MaxRanks) {
    int oldRank     = destinationRank;
    destinationRank = oldRank % MaxRanks;
    logWarning(
      "validateAndAmend()",
      "split instruction tried to assign trees to rank "
        << oldRank << " even though there are only " << MaxRanks
        << " ranks. Altered entry into " << toString()
    );
  }
}


toolbox::loadbalancing::strategies::Hardcoded::Split::Split(
  int                            splittingTree_,
  int                            numberOfCells_,
  int                            destinationRank_,
  peano4::SplitInstruction::Mode mode_
):
  splittingTree(splittingTree_),
  numberOfCells(numberOfCells_),
  destinationRank(destinationRank_),
  mode(mode_) {}


void toolbox::loadbalancing::strategies::Hardcoded::keepAndValidateLocalSplits(
) {
  const int localRank = tarch::mpi::Rank::getInstance().getRank();

  _splits.remove_if([&](const Split& split) {
    return peano4::parallel::Node::getInstance().getRank(split.splittingTree)
           != localRank;
  });

  for (auto& split : _splits) {
    split.validateAndAmend();
  }

  _hasKeptAndValidatedLocalSplits = true;
}


void toolbox::loadbalancing::strategies::Hardcoded::issueSplits() {
  std::map<int, int>                            cellsHandedOutInThisSweep;
  std::map<int, peano4::SplitInstruction::Mode> modeOfThisSweep;

  while (not _splits.empty()) {
    Split& split = _splits.front();

    if (not peano4::parallel::SpacetreeSet::getInstance().isLocalSpacetree(
          split.splittingTree
        )) {
      break;
    }

    const int remainingCells = peano4::parallel::SpacetreeSet::getInstance()
                                 .getGridStatistics(split.splittingTree)
                                 .getNumberOfLocalUnrefinedCells()
                               - cellsHandedOutInThisSweep[split.splittingTree];
    if (remainingCells <= split.numberOfCells) {
      break;
    }

    if (
      modeOfThisSweep.count(split.splittingTree) > 0
      and
      modeOfThisSweep.at(split.splittingTree) != split.mode
    ) {
      break;
    }

    if (not peano4::parallel::SpacetreeSet::getInstance().split(
          split.splittingTree,
          peano4::SplitInstruction{split.numberOfCells, split.mode},
          split.destinationRank
        )) {
      break;
    }

    logInfo("issueSplits()", "issued split " << split.toString());
    cellsHandedOutInThisSweep[split.splittingTree] += split.numberOfCells;
    modeOfThisSweep[split.splittingTree] = split.mode;
    _splits.pop_front();
  }
}


void toolbox::loadbalancing::strategies::Hardcoded::finishStep() {
  if (not _hasKeptAndValidatedLocalSplits) {
    keepAndValidateLocalSplits();
  }

  if (isEnabled(false)) {
    issueSplits();

    if (_splits.empty()) {
      logInfo(
        "finishStep()",
        "all split instructions have been handed out, so shutdown hardcoded load balancing now"
      );
      enable(false);
    }
  }

  _statistics.updateGlobalView();
  _blacklist.update();

  dumpStatistics();
}


void toolbox::loadbalancing::strategies::Hardcoded::finishSimulation() {}


std::string toolbox::loadbalancing::strategies::Hardcoded::Split::toString(
) const {
  std::ostringstream msg;
  msg
    << "(splittingTree=" << splittingTree << ",numberOfCells=" << numberOfCells
    << ",destinationRank=" << destinationRank
    << ",mode=" << peano4::SplitInstruction::toString(mode) << ")";
  return msg.str();
}
