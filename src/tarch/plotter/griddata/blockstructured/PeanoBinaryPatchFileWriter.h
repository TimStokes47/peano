// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once

#include "tarch/logging/Log.h"
#include "PatchWriter.h"

#include <fstream>
#include <vector>

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <filesystem>

namespace tarch {
  namespace plotter {
    namespace griddata {
      namespace blockstructured {
        class PeanoBinaryPatchFileWriter;
      }
    }
  }
}

/**
 * Plot Peano's mesh data in Peano's patch file format with binary quantities
 *
 * This writer is the sibling of
 * @ref tarch::plotter::griddata::blockstructured::PeanoTextPatchFileWriter. It
 * writes exactly the same file, with two differences:
 *
 * - The header holds the line `format Binary` instead of `format ASCII`.
 * - The quantities of a patch, i.e. the lines between `begin cell-values` and
 *   `end cell-values` (or the vertex counterparts), are not written as text.
 *   They are dumped as raw doubles, 8 bytes each in the byte order of the
 *   machine, followed by one line break. The surrounding lines (patch offset
 *   and size, metadata, mappings, markers) remain plain text. The reader knows
 *   from the metadata how many bytes to read.
 *
 * Binary files are smaller and faster to write, and they hold the doubles
 * bit-exact. The index file is a plain text file as before, as it holds no
 * quantities. Use @ref tarch::reader::PeanoBinaryPatchFileReader to read the
 * data back.
 *
 * As the doubles are written as they are, there is no notion of precision of
 * the quantities. The precision passed to the constructor still determines the
 * digits of the text entries (patch offsets and sizes, mappings).
 *
 * Everything below on index file modes applies to this writer as well.
 *

 * Consult the @ref tarch_plotting "general plotting" remarks on rationale and
 * patterns behind the realisation of this class. This is a technical view. If
 * you are interested in the file format that is written, you have to read the
 * @ref documentation_tarch_visualisation "documentation of the Peano patch file format".
 *
 *
 * ## Meta file construction modes
 *
 * There are different configuration modes. They are passed as an argument to
 * the constructor:
 *
 * - IndexFileMode::CreateNew Create a new index file. The old one is thrown
 *   away.
 * - IndexFileMode::NoIndexFile Ignore the whole index file stuff. Every
 *   instance of the plotter dumps its own data and they do not synchronise in
 *   any way.
 * - IndexFileMode::AppendNewData Try to append a new data set to a given
 *   index file.
 *
 * The last one is the most sophisticated variant and deserves further
 * explanation. First of all, the code searches for the index (or meta) file.
 * If there is none, the code basically falls back to IndexFileMode::CreateNew,
 * i.e. creates a new index file.
 *
 * If there is a file, we open it and look what the latest time stamp in
 * there is. For this, we call getLatestTimeStepInIndexFile(). If that time
 * stamp is bigger than the current time stamp, we know that this index file is
 * from a previous run. We create a backup (some people do appreciate if we
 * do so) and then again create a new index file.
 *
 * If our own time stamp matches the last time stamp in the index file, we know
 * that the current dump is one (of many) dumps by a spacetree which contributes
 * towards the overall picture. That is, we add our own data set to the existing
 * time stamp.
 *
 * If our own time stamp is bigger than the last one, we know that we are the
 * first plotter of all the plotters out there which started to dump the data of
 * the next time stamp. We append a new section for a new dump and add our own
 * file there.
 */
class tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter: public tarch::plotter::griddata::blockstructured::PatchWriter {
  protected:
    static tarch::logging::Log _log;
    static const std::string HEADER;

    std::string  _fileName;
    std::string  _indexFile;

    bool _writtenToFile;

    int  _dimensions;

    int _patchCounter;

    std::stringstream _snapshotFileOut;
    bool              _haveWrittenAtLeastOnePatch;

    void writeMetaData(const std::string& metaData);
    void writeMapping(int totalEntries, double* values);

    void createBackupOfMetaFile();
    void createEmptyIndexFile();
    void addNewDatasetToIndexFile(double timestamp);
    void addNewFileToCurrentDataSetInIndexFile( const std::string&  filename );

    /**
     * Find latest time step in index file
     *
     * This routine runs through the index file and searches for the latest
     * entry in there which looks similar to
     *
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * timestamp  0.000000000000000000000000e+00
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     *
     * It will then return the value of this time stamp.
     */
    double getLatestTimeStepInIndexFile() const;

  public:
    class CellDataWriter: public tarch::plotter::griddata::blockstructured::PatchWriter::CellDataWriter {
      protected:
        tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter& _writer;

        const std::string _identifier;
        const int         _numberOfCellsPerAxis;
        const int         _numberOfUnknowns;
        int               _entryCounter;
        std::vector<double> _out;

        void flushIfPatchIsComplete();
      public:
        CellDataWriter(
          const std::string& identifier,
          int                unknownsPerAxis,
          int                numberOfUnknowns, const std::string& description,
          const std::string& metaData,
          double*            mapping,
          tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter& writer
        );
        virtual ~CellDataWriter();
        
        /**
         * Has no effect, as the quantities are written as raw doubles.
         */
        void setPrecision(int precision);

        int getCellsPerPatch() const;

        /**
          * Write data for one cell.
          *
          * @param index Index of the cell. This index has to equal the index
          *              used for the cell within the VTKWriter class
          *              interface.
          * @param value Value for the cell.
          */
        virtual void plotCell( int index, double value ) override;
        virtual void plotCell( int index, double* values ) override;

        virtual void close() override;

        virtual void assignRemainingCellsDefaultValues() override;

        virtual int getFirstCellWithinPatch(int index) const override;
     };

    class VertexDataWriter: public tarch::plotter::griddata::blockstructured::PatchWriter::VertexDataWriter {
       protected:
         tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter& _writer;

         const std::string _identifier;
         const int         _numberOfVerticesPerAxis;
         const int         _numberOfUnknowns;

         /**
          * Number of entries written within a patch.
          */
         int               _entryCounter;
         std::vector<double> _out;

         void flushIfPatchIsComplete();

       public:
         VertexDataWriter(
           const std::string& identifier,
           int                unknownsPerAxis,
           int                numberOfUnknowns, const std::string& description,
           const std::string& metaData,
           double*            mapping,
           tarch::plotter::griddata::blockstructured::PeanoBinaryPatchFileWriter& writer
         );

         ~VertexDataWriter();

         /**
          * Has no effect, as the quantities are written as raw doubles.
          */
         void setPrecision(int precision);

         int getVerticesPerPatch() const;

         /**
          * Write data for one cell.
          *
          * @param index Index of the vertex. This index has to equal the index
          *              used for the cell within the VTKWriter class
          *              interface.
          * @param value Value for the cell.
          */
         virtual void plotVertex( int index, double value ) override;
         virtual void plotVertex( int index, double* values ) override;

         virtual void close() override;

         /**
          * @see close()
          */
         virtual void assignRemainingVerticesDefaultValues() override;

         virtual int getFirstVertexWithinPatch(int index) const override;
    };

    enum class IndexFileMode {
      CreateNew,
      AppendNewData,
      NoIndexFile
    };

    /**
     * Create a new Peano text file output
     *
     * An instance of this class has to be owned by each action set that wants
     * to dump data.
     *
     * ## AppendNewData and the order of operations
     *
     * If appendToIndexFile is IndexFileMode::AppendNewData and no index file
     * exists yet, the constructor has to both create that index file (which,
     * via createEmptyIndexFile(), already writes one dataset section for
     * timeStamp) and decide whether the current writer's own timeStamp is
     * "newer" than whatever is already in the index file, to decide if a
     * further, separate dataset section is required. That decision has to be
     * made by calling getLatestTimeStepInIndexFile() *after* the index file
     * has been (re)created if it had to be - not before. Comparing against
     * the "no data yet" state that getLatestTimeStepInIndexFile() reports for
     * a file that does not exist yet would make the writer for the very first
     * data set of a run believe it also has to open a second, empty dataset
     * section for its own (freshly written) time stamp, and every later
     * writer for that same time stamp would then register its data in that
     * spurious second section rather than the first, correct one. If you
     * touch the ordering of operations in the AppendNewData branch of the
     * implementation, preserve this "decide only after creating/recreating"
     * order.
     *
     * @param dimension     I make this a free parameter, even though most
     *   codes will pass in the compile time parameter Dimensions. However,
     *   there are codes which plot submanifolds for example or want to write
     *   3d data within a 2d code, as they implement a 2.5d model.
     * @param fileName      Name of the file to write. Has to be unique per
     *   dump.
     * @param indexFileName Name of the index file. Can we empty if you select
     *   NoIndexFile.
     * @param precision     Number of significant digits of the text entries
     *   of the file, i.e. patch offsets and sizes and mappings. The quantities
     *   are written as raw doubles and are not affected. Keep at least six
     *   digits on deep adaptive meshes, as the patch offsets otherwise become
     *   imprecise.
     */
    PeanoBinaryPatchFileWriter(int dimension, const std::string&  fileName, const std::string&  indexFileName, IndexFileMode appendToIndexFile, double timeStamp, int precision = 6);

    /**
     * Destructor
     *
     * The destructor is empty, i.e. if you destroy this object, your data are
     * lost. You have to dump the data beforehand manually by calling
     * writeToFile().
     */
    virtual ~PeanoBinaryPatchFileWriter();

    /**
     * Caller has to destroy this instance manually.
     */
    virtual CellDataWriter*    createCellDataWriter( const std::string& identifier, int unknownsPerAxis, int recordsPerCell, const std::string& description ) override;
    virtual CellDataWriter*    createCellDataWriter( const std::string& identifier, int unknownsPerAxis, int recordsPerCell, const std::string& description, const std::string& metaData ) override;
    virtual CellDataWriter*    createCellDataWriter( const std::string& identifier, int unknownsPerAxis, int recordsPerCell, const std::string& description, const std::string& metaData, double* mapping ) override;

    /**
     * Caller has to destroy this instance manually.
     */
    virtual VertexDataWriter*  createVertexDataWriter( const std::string& identifier, int unknownsPerAxis, int recordsPerVertex, const std::string& description ) override;
    virtual VertexDataWriter*  createVertexDataWriter( const std::string& identifier, int unknownsPerAxis, int recordsPerVertex, const std::string& description, const std::string& metaData  ) override;
    virtual VertexDataWriter*  createVertexDataWriter( const std::string& identifier, int unknownsPerAxis, int recordsPerVertex, const std::string& description, const std::string& metaData, double* mapping ) override;


    virtual int plotPatch(
      const tarch::la::Vector<2,double>& offset,
      const tarch::la::Vector<2,double>& size
    ) override;

    virtual int plotPatch(
      const tarch::la::Vector<3,double>& offset,
      const tarch::la::Vector<3,double>& size
    ) override;

    /**
     * @return Write has been successful
     */
    virtual bool writeToFile() override;

    /**
     * @return Whether writer is ready to accept data.
     */
    virtual bool isOpen() override;

    /**
     * Clear local data
     *
     * Clear the writer, i.e. erase all the data. However, as the writer does
     * not track how many vertex and cell writers you've created, it's up to
     * you to ensure that none of these instances is left. So we clear the core
     * data, but we do not clear the data within any data writer which you have
     * created by calling routines such as createCellDataWriter().
     */
    virtual void clear() override;
};

