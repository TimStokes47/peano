// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include <iosfwd>
#include <string>
#include <vector>

namespace tarch {
  namespace reader {
    class PeanoTextPatchFileReader;
  } // namespace reader
} // namespace tarch


/**
 * Read a Peano patch file back into memory
 *
 * The reader complements
 * @ref tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter and
 * @ref tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter.
 * The file format is described in @ref documentation_tarch_visualisation.
 * This class reads files with `format ASCII`, i.e. files where the quantities
 * of a patch are written as text. tarch::reader::PeanoBinaryPatchFileReader
 * extends it for `format Binary`.
 *
 * ## Which reader to use
 *
 * Both formats share the whole structure of the file but the quantities of the
 * patches. Therefore, the routine read() is the same for both readers and the
 * subclass only overwrites readValues(). To find out which reader is the right
 * one, call readHeader() first. It looks at the first lines of the file only,
 * i.e. it is cheap.
 *
 * ## Scope
 *
 * The reader reads the cell data of a file, i.e. the `cell-values` of the
 * patches. It does not read vertex data and it ignores the mappings.
 */
class tarch::reader::PeanoTextPatchFileReader {
  public:
    enum class Format {
      ASCII,
      Binary
    };

    /**
     * Information from the first lines of a file
     */
    struct Header {
      Format format     = Format::ASCII;
      int    dimensions = -1;
    };

    /**
     * One patch of the file
     */
    struct Patch {
      /**
       * Offset and size of the patch. Always as many entries as the file has
       * dimensions.
       */
      std::vector<double> offset;
      std::vector<double> size;

      /**
       * Number of entries per axis, as stored in the file's cell metadata.
       */
      int numberOfDoFsPerAxis = 0;

      /**
       * Number of unknowns per entry, as stored in the file's cell metadata.
       */
      int numberOfUnknowns = 0;

      /**
       * All quantities of the patch in the order they are in the file.
       */
      std::vector<double> values;
    };

    /**
     * Whole content of a file
     */
    struct Content {
      Header header;

      /**
       * Content of the meta-data block of the file's header. One key-value
       * pair per line, without indentation.
       */
      std::string metaData;

      std::vector<Patch> patches;
    };

    virtual ~PeanoTextPatchFileReader() = default;

    /**
     * Read only the header of a file
     *
     * This is the light-touch routine: It runs through the first lines of the
     * file until it has found the dimensions and does not parse any patch.
     * Use it to find out which reader to instantiate.
     *
     * A file without a format line is an ASCII file. The routine terminates
     * the application if it cannot open the file.
     */
    static Header readHeader(const std::string& fileName);

    /**
     * Read the whole file
     *
     * The routine terminates the application if it cannot open the file, if
     * the file claims a format other than the one the reader supports, or if
     * the file is corrupted.
     */
    Content read(const std::string& fileName) const;

  protected:
    /**
     * Read the quantities of one patch
     *
     * The routine is called after the line `begin cell-values` has been
     * read. It has to leave the stream such that the next line to be read is
     * the line `end cell-values`.
     *
     * @param in Stream of the file, opened in binary mode.
     * @param numberOfValues Number of doubles the patch holds. Derived from
     *   the cell metadata and the dimensions.
     * @param values Output. Is cleared first.
     */
    virtual void readValues(
      std::istream&        in,
      int                  numberOfValues,
      std::vector<double>& values
    ) const;

    /**
     * Format this reader supports
     */
    virtual Format getFormat() const;
};
