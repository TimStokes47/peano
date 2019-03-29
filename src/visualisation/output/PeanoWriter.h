#ifndef _VISUALISATION_OUTPUT_PEANO_WRITER_H_
#define _VISUALISATION_OUTPUT_PEANO_WRITER_H_

#include "PeanoPatch.h"
#include "PeanoMetaFile.h"
#include "Writer.h"

#include "../config.h"

#include "tarch/logging/Log.h"

#include <vector>
#include <fstream>

namespace visualisation {
  namespace output {
    class PeanoWriter;
  }

  namespace input {
    /**
     * Forward declaration
     */
    class PeanoTextPatchFileReader;
  }
}


class visualisation::output::PeanoWriter: public visualisation::output::Writer {
  public:
	PeanoWriter(const std::string&  directory, const std::string& outputFileWithoutExtention);
	~PeanoWriter();

	/**
	 * Meta file write
	 */
	void writeFile(const PeanoMetaFile& metaFile, const std::string& selector) override;

	/**
	 * This is a special variant of the writer which dumps all the different
	 * file representations that we find in a Peano file. Therefore, it is not
	 * a routine found in the super interface.
	 */
	void writeFile(const PeanoMetaFile& metaFile);

	static void writeFile(const std::string& outputFileWithoutExtension, const std::vector<PeanoPatch*>& patches);
  private:
	static tarch::logging::Log  _log;

	static const std::string _FileExtension;
	static const std::string _Header;

	const std::string& _outputFileWithoutExtension;
	const std::string& _directory;
	std::ofstream      _metaFile;
};

#endif
