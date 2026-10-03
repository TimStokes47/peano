#include "Solver.h"

#include <algorithm>
#include <limits>

#include "tarch/multicore/Lock.h"
#include "tarch/mpi/Rank.h"
#include "tarch/services/ServiceRepository.h"


exahype2::Solver::Solver():
  _minTimeStamp(0.0),
  _maxTimeStamp(0.0),
  _minTimeStampUpdatedThisTimeStep(0.0),
  _maxTimeStampUpdatedThisTimeStep(0.0),
  _minVolumeH(std::numeric_limits<double>::max()),
  _maxVolumeH(0.0),
  _minVolumeHUpdatedThisTimeStep(std::numeric_limits<double>::max()),
  _maxVolumeHUpdatedThisTimeStep(0.0),
  _minTimeStepSize(0.0),
  _maxTimeStepSize(0.0),
  _minTimeStepSizeUpdatedThisTimeStep(std::numeric_limits<double>::max()),
  _maxTimeStepSizeUpdatedThisTimeStep(0.0),
  _cellUpdates(0) {
}


double exahype2::Solver::getMinTimeStamp() const {
  return _minTimeStamp;
}


double exahype2::Solver::getMaxTimeStamp() const {
  return _maxTimeStamp;
}


double exahype2::Solver::getMinTimeStepSize() const {
  return _minTimeStepSize;
}


double exahype2::Solver::getMaxTimeStepSize() const {
  return _maxTimeStepSize;
}


double exahype2::Solver::getMaxMeshSize() const {
  return _maxVolumeH;
}


double exahype2::Solver::getMinMeshSize() const {
  return _minVolumeH;
}


bool exahype2::Solver::cellCanUseStatelessPDETerms(
  [[maybe_unused]] const tarch::la::Vector<Dimensions, double>& cellCentre,
  [[maybe_unused]] const tarch::la::Vector<Dimensions, double>& cellH,
  [[maybe_unused]] double                                       t,
  [[maybe_unused]] double                                       dt
) const {
  return true;
}


void exahype2::Solver::startTimeStep(
  [[maybe_unused]] double globalMinTimeStamp,
  [[maybe_unused]] double globalMaxTimeStamp,
  [[maybe_unused]] double globalMinTimeStepSize,
  [[maybe_unused]] double globalMaxTimeStepSize
) {
  _minTimeStamp = _minTimeStampUpdatedThisTimeStep;
  _maxTimeStamp = _maxTimeStampUpdatedThisTimeStep;

  _minTimeStampUpdatedThisTimeStep = std::numeric_limits<double>::max();
  _maxTimeStampUpdatedThisTimeStep = std::numeric_limits<double>::min();

  _minTimeStepSize = _minTimeStepSizeUpdatedThisTimeStep;
  _maxTimeStepSize = _maxTimeStepSizeUpdatedThisTimeStep;

  _minTimeStepSizeUpdatedThisTimeStep = std::numeric_limits<double>::max();
  _maxTimeStepSizeUpdatedThisTimeStep = std::numeric_limits<double>::min();

  _minVolumeH = _minVolumeHUpdatedThisTimeStep;
  _maxVolumeH = _maxVolumeHUpdatedThisTimeStep;

  _minVolumeHUpdatedThisTimeStep = std::numeric_limits<double>::max();
  _maxVolumeHUpdatedThisTimeStep = 0.0;

  _cellUpdates = 0;
}


void exahype2::Solver::finishTimeStep() {
#ifdef Parallel
  double newMinTimeStamp    = _minTimeStampUpdatedThisTimeStep;
  double newMaxTimeStamp    = _maxTimeStampUpdatedThisTimeStep;
  double newMinVolumeH      = _minVolumeHUpdatedThisTimeStep;
  double newMaxVolumeH      = _maxVolumeHUpdatedThisTimeStep;
  double newMinTimeStepSize = _minTimeStepSizeUpdatedThisTimeStep;
  double newMaxTimeStepSize = _maxTimeStepSizeUpdatedThisTimeStep;
  int    newCellUpdates     = _cellUpdates;

  tarch::mpi::Rank::getInstance().allReduce(
    &newMinTimeStamp,
    &_minTimeStampUpdatedThisTimeStep,
    1, MPI_DOUBLE,
    MPI_MIN,
    [&]() -> void { tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }
  );
  tarch::mpi::Rank::getInstance().allReduce(
    &newMaxTimeStamp,
    &_maxTimeStampUpdatedThisTimeStep,
    1, MPI_DOUBLE,
    MPI_MAX,
    [&]() -> void { tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }
  );

  tarch::mpi::Rank::getInstance().allReduce(
    &newMinVolumeH,
    &_minVolumeHUpdatedThisTimeStep,
    1, MPI_DOUBLE,
    MPI_MIN,
    [&]() -> void { tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }
  );
  tarch::mpi::Rank::getInstance().allReduce(
    &newMaxVolumeH,
    &_maxVolumeHUpdatedThisTimeStep,
    1, MPI_DOUBLE,
    MPI_MAX,
    [&]() -> void { tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }
  );

  tarch::mpi::Rank::getInstance().allReduce(
    &newMinTimeStepSize,
    &_minTimeStepSizeUpdatedThisTimeStep,
    1, MPI_DOUBLE,
    MPI_MIN,
    [&]() -> void { tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }
  );
  tarch::mpi::Rank::getInstance().allReduce(
    &newMaxTimeStepSize,
    &_maxTimeStepSizeUpdatedThisTimeStep,
    1, MPI_DOUBLE,
    MPI_MAX,
    [&]() -> void { tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }
  );

  tarch::mpi::Rank::getInstance().allReduce(
    &newCellUpdates,
    &_cellUpdates,
    1, MPI_INT,
    MPI_SUM,
    [&]() -> void { tarch::services::ServiceRepository::getInstance().receiveDanglingMessages(); }
  );
#endif

  if (_minTimeStampUpdatedThisTimeStep >= std::numeric_limits<double>::max() / 10.0) {
    _minTimeStampUpdatedThisTimeStep = 0.0;
  }
}


void exahype2::Solver::updateCell(double timeStepSize, double timeStamp) {
  tarch::multicore::Lock lock(_semaphore);
  _minTimeStepSizeUpdatedThisTimeStep = std::min(_minTimeStepSizeUpdatedThisTimeStep, timeStepSize);
  _maxTimeStepSizeUpdatedThisTimeStep = std::max(_maxTimeStepSizeUpdatedThisTimeStep, timeStepSize);
  _minTimeStampUpdatedThisTimeStep    = std::min(_minTimeStampUpdatedThisTimeStep, timeStamp);
  _maxTimeStampUpdatedThisTimeStep    = std::max(_maxTimeStampUpdatedThisTimeStep, timeStamp);
  _cellUpdates++;
}
