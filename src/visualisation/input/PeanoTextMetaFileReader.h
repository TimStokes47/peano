#ifndef _VISUALISATION_INPUT_PEANO_TEXT_META_FILE_READER_H_
#define _VISUALISATION_INPUT_PEANO_TEXT_META_FILE_READER_H_



#include <string>
#include <vector>

#include "PeanoPatch.h"
#include "PeanoVariable.h"
#include "PeanoMetaFile.h"

#include "MetaFileReader.h"

#include "tarch/logging/Log.h"


class PeanoMetaFile;

namespace visualisation {
  namespace input {
    class PeanoTextMetaFileReader;
  }
}


/**
 * Represent a Peano reader
 *
 * A reader is an object which basically takes one file and gives the content
 * of this file.
 *
 * If you just wanna in one file, open it with this file and pass the field
 * patches into the PeanoConverter.
 *
 * @author Dan Tuthill-Jones, Tobias Weinzierl
 */
class visualisation::input::PeanoTextMetaFileReader: public visualisation::input::MetaFileReader {
  private:
	static tarch::logging::Log  _log;

	const std::string  _filename;
	PeanoMetaFile*     _file;

	void parseDataset( const std::vector<std::string> lines );

  public:
	PeanoTextMetaFileReader( const std::string& filename );
	virtual ~PeanoTextMetaFileReader();

	void parse() override;

	PeanoMetaFile* getFile() override;
};

#endif
