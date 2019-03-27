#ifndef _VISUALISATION_INPUT_PEANO_TEXT_PATCH_FILE_READER_H_
#define _VISUALISATION_INPUT_PEANO_TEXT_PATCH_FILE_READER_H_



#include <string>
#include <vector>

#include "PeanoPatch.h"
#include "PeanoVariable.h"


#include "tarch/logging/Log.h"


namespace visualisation {
  namespace input {
    class PeanoTextPatchFileReader;
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
class visualisation::input::PeanoTextPatchFileReader {
  private:
	static tarch::logging::Log _log;

	const std::string  _file;

	void    parsePatch( const std::vector<std::string>& patchDescription );
	double* parseMapping( const std::vector<std::string>& patchDescription );
  public:
	/**
	 * Read in one file.
	 *
	 * <h2> Parse process </h2>
	 *
	 * - Load the whole files into a vector called lines, i.e. we load line by line.
	 * -
	 */
	PeanoTextPatchFileReader();
	virtual ~PeanoTextPatchFileReader();

	void parse(const std::string &file);
	bool isEmpty();

	int dimensions = -1;
	int cells = -1;
	int vertices = -1;
	int* patchSize;

	std::vector<PeanoVariable*> variables;
	std::vector<PeanoPatch*> patches;
};

#endif /* PEANOREADER_H_ */
