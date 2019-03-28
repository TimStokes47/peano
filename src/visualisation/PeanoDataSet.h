/*
 * PeanoDataSet.h
 *
 *  Created on: 29 Jan 2018
 *      Author: dan
 */

#ifndef PEANODATASET_H_
#define PEANODATASET_H_

#include <string>
#include <map>

#include "PeanoPatch.h"


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
 * Datasets do not have a filename themselves, but the dataset itself is only a
 * wrapper around existing files on disc. It holds the data in multiple
 * resolutions/postprocessing steps if there are multiple ones.
 *
 * @author Dan Tuthill-Jones, Tobias Weinzierl
 */
class PeanoDataSet {
  public:
	static std::string RawData;

	PeanoDataSet(const std::string& directory);
	virtual ~PeanoDataSet();

	/**
	 * @return Set of readers tied to a particular model
	 */
	std::vector<visualisation::input::PeanoTextPatchFileReader*>* createReaders( const std::string& identifier );

	std::vector<std::string> toString();
  private:
	friend class visualisation::input::PeanoTextMetaFileReader;

	std::string  _directory;

	/**
	 * A data set has a unique set of variables. If there are multiple variable
	 * (files), they all have to agree on those.
	 */
	std::vector<PeanoVariable*> variables;

	std::map< std::string, std::vector<std::string> > _data;
};

#endif /* PEANODATASET_H_ */
