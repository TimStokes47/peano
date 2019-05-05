#ifndef _VISUALISATION_INPUT_PEANO_TEXT_PATCH_FILE_READER_H_
#define _VISUALISATION_INPUT_PEANO_TEXT_PATCH_FILE_READER_H_



#include <string>
#include <vector>

#include "../data/PatchData.h"
#include "PatchFileReader.h"


#include "visualisation/data/DataSet.h"
#include "visualisation/data/Variable.h"


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
class visualisation::input::PeanoTextPatchFileReader: public visualisation::input::PatchFileReader {
  private:
	static tarch::logging::Log _log;

	const std::string  _file;

	void    parsePatch( int dataSetCounter, const std::vector<std::string>& patchDescription );
	void    parseVariablesDeclaration( int dataSetCounter, const std::vector<std::string>& patchDescription, const std::string&  name, visualisation::data::PeanoDataType type );

    void    addDataToPatch( int dataSetCounter, const std::string& variableName, double* offset, double* size, const std::vector< std::string >& textData );

	int                                          _dimensions;
	std::vector< visualisation::data::DataSet >  _data;
  public:
	/**
	 * Read in one file.
	 *
	 * <h2> Parse process </h2>
	 *
	 * - Load the whole files into a vector called lines, i.e. we load line by line.
	 * -
	 */
	PeanoTextPatchFileReader(const std::string &file);
	virtual ~PeanoTextPatchFileReader();

	void parse() override;
	std::vector< visualisation::data::DataSet >  getData() const override;
};

#endif
