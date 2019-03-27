/*
 * PeanoMetaReader.h
 *
 *  Created on: 26 Nov 2017
 *      Author: dan
 */

#ifndef PEANOMETAFILE_H_
#define PEANOMETAFILE_H_

#include <string>
#include <vector>

#include "PeanoDataSet.h"

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
	PeanoMetaFile();
	virtual ~PeanoMetaFile();

	std::vector<PeanoDataSet*>* getDataSets();
	PeanoDataSet* getDataSet(int index);
	int numberOfDataSets();
	void save();
  private:
	friend class visualisation::input::PeanoTextMetaFileReader;

	std::string fileName;
	std::vector<PeanoDataSet*>* dataSets;
	std::string directory;
};

#endif /* PEANOMETAFILE_H_ */
