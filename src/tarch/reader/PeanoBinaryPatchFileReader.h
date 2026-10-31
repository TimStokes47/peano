// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include "PeanoTextPatchFileReader.h"

namespace tarch {
  namespace reader {
    class PeanoBinaryPatchFileReader;
  } // namespace reader
} // namespace tarch


/**
 * Read a Peano patch file with binary quantities
 *
 * Counterpart of
 * @ref tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter.
 * All lines of the file are text as in a file with format ASCII. Only the
 * line between `begin cell-values` and `end cell-values` is replaced by the
 * raw doubles of the patch followed by one line break. The reader therefore
 * knows how many bytes to read from the metadata and the dimensions, and the
 * bytes may hold any value, including line breaks.
 *
 * The doubles are stored in the byte order of the machine that wrote them. The
 * reader assumes that it runs on a machine with the same byte order.
 */
class tarch::reader::PeanoBinaryPatchFileReader: public tarch::reader::PeanoTextPatchFileReader {
  protected:
    void readValues(
      std::istream&        in,
      int                  numberOfValues,
      std::vector<double>& values
    ) const override;

    Format getFormat() const override;
};
