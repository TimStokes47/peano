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
 * One individual data set which typically is one time step. In the text file,
 * one reader corresponds to one begin dataset ... end dataset piece. A dataset
 * on disc can be the merger of many datasets (patch sets) written by different
 * ranks.
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

	std::vector<std::string> toString();
  private:
	friend class visualisation::input::PeanoTextMetaFileReader;

	std::vector<std::string>* fullData;
	std::string directory;
};

#endif /* PEANODATASET_H_ */
