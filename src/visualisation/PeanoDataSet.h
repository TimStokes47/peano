/*
 * PeanoDataSet.h
 *
 *  Created on: 29 Jan 2018
 *      Author: dan
 */

#ifndef PEANODATASET_H_
#define PEANODATASET_H_

#include <string>
#include "PeanoPatch.h"
#include "input/PeanoTextPatchFileReader.h"


namespace visualisation {
  namespace input {
    class PeanoTextPatchFileReader;
    class PeanoTextMetaFileReader;
  }
}


/**
 * One individual data set which typically is one time step with its different
 * resolutions.
 *
 * @author Dan Tuthill-Jones, Tobias Weinzierl
 */
class PeanoDataSet {
  public:
	PeanoDataSet(std::string directory);
	virtual ~PeanoDataSet();

	/**
	 * @return Set of readers tied to the full resolution model
	 */
	std::vector<visualisation::input::PeanoTextPatchFileReader*>* createReadersForRawData();
//	std::vector<PeanoReader*>* createReadersResolution(int res);

    // std::string getDirectory();
	std::vector<std::string>* getFullData();
	std::vector<std::vector<int>>* getResolutions();
	std::string getResolution(int index);
	std::vector<std::string> toString();

	// @todo remove
	std::string getSimpleName();


//	PeanoPatch* createSubSample(int x, int y, int z, bool saveToFile);
private:
	friend class visualisation::input::PeanoTextMetaFileReader;

	std::vector<std::string>* fullData;
	std::vector<std::string>* resolutionData;
	std::vector<std::vector<int>>* resolutions;
	std::string directory;
};

#endif /* PEANODATASET_H_ */
