// This file is part of the ExaHyPE2 project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include "peano4/grid/GridControlEvent.h"
#include "tarch/la/Vector.h"
#include "tarch/logging/Log.h"
#include "tarch/multicore/BooleanSemaphore.h"

#include <array>
#include <map>
#include <memory>
#include <string>
#include <vector>


namespace exahype2 {
  class CheckpointReader;
} // namespace exahype2


/**
 * Read a checkpoint back
 *
 * A checkpoint is a set of Peano patch files as written by
 * exahype2.api.fv.WriteCheckpoint or exahype2.api.rkfd4.WriteCheckpoint:
 * An index file lists one dataset per checkpoint, and each dataset includes
 * one patch file per spacetree. Every patch holds the data of one mesh cell
 * plus its halo, written over the cell's box enlarged by the halo. The
 * meta-data block in the header of each file holds the solver's state
 * (@ref exahype2::Solver::toCheckpointMetaData()).
 *
 * ## File formats
 *
 * The files are either ASCII or binary files. The reader is format-agnostic:
 * It looks at the header of each patch file
 * (@ref tarch::reader::PeanoTextPatchFileReader::readHeader()), instantiates
 * the matching reader from tarch::reader and lets it parse the file. This
 * class only maps the patches that come back onto the cells of the mesh.
 *
 * The reader parses all files of one dataset. It then
 *
 * - yields refinement events which reconstruct the checkpoint's mesh
 *   (getRefinementEvents()),
 * - hands out the data of the patch belonging to a cell (readPatch()), and
 * - provides the solver state (getMetaData()).
 *
 * ## Matching patches and cells
 *
 * The reader identifies each patch by the level and the integer index of its
 * cell within the regular grid of that level, both derived from the domain's
 * offset and size. The index is the rounded quotient of the cell's offset and
 * size, i.e. there is no tolerance involved, and a coarse cell and the fine
 * cell in its centre never get mixed up.
 *
 * ## Reconstructing the mesh
 *
 * Peano refines a cell if it overlaps a refinement event, and if it is at
 * least as big as the event's mesh size. For every cell of the checkpoint on
 * level two or higher, the reader hence yields one event which covers the
 * middle third of the cell's parent, with the parent's size as mesh size.
 * This event refines the parent and all of its ancestors, but none of the
 * parent's neighbours. Cells on level one result from the refinement of the
 * root, which Peano always carries out. Events of cells with the same parent
 * are identical, so the reader yields each one only once.
 *
 * ## Parallelisation
 *
 * Every rank reads all files of the dataset. The memory footprint per rank
 * hence is the size of the whole checkpoint.
 */
class exahype2::CheckpointReader {
public:
  /**
   * Reader of a checkpoint dataset
   *
   * The routine reads the files once and hands out the same reader to all
   * later calls with the same index file, until release() frees it. It is
   * thread-safe.
   *
   * @param indexFileName Index file of the checkpoint, i.e. the file
   *   checkpoint-SolverName.peano-patch-file. The paths of the patch files
   *   in there are relative to the directory of the index file.
   * @param timeStamp Time stamp of the dataset to read. If it is negative,
   *   the reader reads the latest dataset. Otherwise, it reads the dataset
   *   whose time stamp is closest to timeStamp. Datasets without files are
   *   ignored.
   * @param haloWidth Number of volumes or grid cells of the halo per side,
   *   i.e. the difference between the number of entries per axis in the
   *   file and in the solver's patch, divided by two.
   * @param domainOffset Offset of the computational domain.
   * @param domainSize Size of the computational domain.
   */
  static CheckpointReader& getInstance(
    const std::string&                           indexFileName,
    double                                       timeStamp,
    int                                          haloWidth,
    const tarch::la::Vector<Dimensions, double>& domainOffset,
    const tarch::la::Vector<Dimensions, double>& domainSize
  );

  /**
   * Free the reader of an index file, if there is one
   */
  static void release(const std::string& indexFileName);

  CheckpointReader(
    const std::string&                           indexFileName,
    double                                       timeStamp,
    int                                          haloWidth,
    const tarch::la::Vector<Dimensions, double>& domainOffset,
    const tarch::la::Vector<Dimensions, double>& domainSize
  );

  /**
   * Refinement events which reconstruct the checkpoint's mesh
   */
  const std::vector<peano4::grid::GridControlEvent>& getRefinementEvents(
  ) const;

  /**
   * Copy the data of a cell's patch including its halo
   *
   * @param cellCentre Centre of the cell, i.e. marker.x().
   * @param cellH Size of the cell, i.e. marker.h().
   * @param numberOfEntries Number of doubles the patch with halo holds,
   *   i.e. the number of unknowns times the number of entries per axis to
   *   the power of Dimensions. The routine checks it against the file.
   * @param patchWithHalo Destination.
   * @return Whether the checkpoint holds a patch for the cell. If not, the
   *   routine leaves patchWithHalo unchanged.
   */
  bool readPatch(
    const tarch::la::Vector<Dimensions, double>& cellCentre,
    const tarch::la::Vector<Dimensions, double>& cellH,
    int                                          numberOfEntries,
    double*                                      patchWithHalo
  ) const;

  /**
   * Content of the meta-data block of the dataset's files, i.e. the solver's
   * state (@ref exahype2::Solver::fromCheckpointMetaData()). One key-value
   * pair per line, without indentation.
   */
  const std::string& getMetaData() const;

  /**
   * Time stamp of the dataset as given in the index file
   */
  double getTimeStamp() const;

  int getNumberOfPatches() const;

private:
  /**
   * Integer index of the cell per axis, followed by its level
   */
  using Key = std::array<long long, Dimensions + 1>;

  static tarch::logging::Log _log;

  static tarch::multicore::BooleanSemaphore                       _semaphore;
  static std::map<std::string, std::unique_ptr<CheckpointReader>> _instances;

  const int                                   _haloWidth;
  const tarch::la::Vector<Dimensions, double> _domainOffset;
  const tarch::la::Vector<Dimensions, double> _domainSize;

  double                                      _timeStamp;
  std::string                                 _metaData;
  std::vector<std::vector<double>>            _patchData;
  std::map<Key, int>                          _patchIndex;
  std::vector<peano4::grid::GridControlEvent> _refinementEvents;

  /**
   * Level and integer index of a cell within the regular grid of its level
   */
  Key toKey(
    const tarch::la::Vector<Dimensions, double>& cellOffset,
    const tarch::la::Vector<Dimensions, double>& cellH
  ) const;

  /**
   * Files of the selected dataset in the index file
   */
  std::vector<std::string> readIndexFile(
    const std::string& indexFileName,
    double             timeStamp
  );

  void readPatchFile(const std::string& fileName);

  void createRefinementEvents();
};
