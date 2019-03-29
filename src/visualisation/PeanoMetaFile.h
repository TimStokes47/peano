#ifndef _VISUALISATION_PEANOMETAFILE_H_
#define _VISUALISATION_PEANOMETAFILE_H_

#include <string>
#include <vector>
#include <map>


#include "visualisation/input/PatchFileReader.h"
#include "tarch/logging/Log.h"


namespace visualisation {
  namespace input {
    class PeanoTextPatchFileReader;
    class PeanoTextMetaFileReader;
  }
}


/**
 * Meta files are files that hold time series or reduced resolution images.
 */
class PeanoMetaFile {
  public:
	static std::string RawData;

	PeanoMetaFile();
	virtual ~PeanoMetaFile();

	/**
	 * @return Set of readers tied to a particular model. These readers haven't
	 *         parsed their files yet, i.e. you have to call parse() manually.
	 */
	std::vector<visualisation::input::PatchFileReader*> createReaders( int dataSetNumber, const std::string& selectors ) const;

	/**
	 * @return All the selectors (data representations) stored for a time step
	 */
	std::vector<std::string> getStoredDataRepresentations( int dataSetNumber ) const;

	int getNumberOfDataSets() const;

  private:
	friend class visualisation::input::PeanoTextMetaFileReader;

	static tarch::logging::Log  _log;

	struct PeanoDataSet {
	  std::map< std::string, std::vector<visualisation::input::PatchFileReader*> > data;
	};

	std::vector<PeanoDataSet>  _dataSets;
};

#endif
