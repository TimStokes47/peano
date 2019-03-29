#include "PeanoMetaFile.h"


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>


std::string  PeanoMetaFile::RawData( "raw" );
tarch::logging::Log  PeanoMetaFile::_log( "PeanoMetaFile" );


PeanoMetaFile::PeanoMetaFile() {
}



/*
	std::cout << "\nSaving metadeta file " << fileName << "\n";
	std::ofstream file;
	file.open(fileName.c_str(), std::ofstream::out | std::ofstream::trunc);


	file << "#\n";
	file << "# Peano metadata output file\n";
	file << "# Version 0.2\n";
	file << "#\n";
	file << "format ASCII\n\n";

	for(uint i = 0; i < dataSets->size(); i++) {
		PeanoDataSet* dataset = dataSets->at(i);
		std::vector<std::string> lines = dataset->toString();
		for(std::string line: lines)
			file << line << "\n";
		file << "\n";
	}

	file << "end patch\n";
	file.close();
*/


PeanoMetaFile::~PeanoMetaFile() {
	// @todo Das brauchen wir eigentlich schon
/*
  for(uint i = 0; i < _dataSets.size(); i++) {
	delete _dataSets.at(i);
*/
}


int PeanoMetaFile::getNumberOfDataSets() const {
  return _dataSets.size();
}


std::vector<std::string> PeanoMetaFile::getStoredDataRepresentations( int dataSetNumber ) const {
  std::vector<std::string> result;
  for (auto p: _dataSets[dataSetNumber].data) {
	result.push_back(p.first);
  }
  return result;
}


std::vector<visualisation::input::PatchFileReader*> PeanoMetaFile::createReaders( int dataSetNumber, const std::string& identifier ) const {
  if ( _dataSets[dataSetNumber].data.count(identifier)==0 ) {
	 logError( "createReaders(...)", "no data set with identifier " << identifier << " in data set no " << dataSetNumber );
	 return std::vector<visualisation::input::PatchFileReader*>();
  }
  return _dataSets[dataSetNumber].data.at(identifier);
}
