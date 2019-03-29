#include "PeanoMetaFile.h"


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>


std::string  PeanoMetaFile::RawData( "" );


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


std::vector<visualisation::input::PatchFileReader*> PeanoMetaFile::createReaders( int dataSetNumber, const std::string& identifier ) const {
  return _dataSets[dataSetNumber].data.at(identifier);
}
