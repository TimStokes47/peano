#include "PeanoMetaFile.h"

/*
#include "boost/algorithm/string/predicate.hpp"
#include "boost/algorithm/string/erase.hpp"
#include "boost/algorithm/string/trim.hpp"
*/

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>


PeanoMetaFile::PeanoMetaFile() {}



void PeanoMetaFile::save() {
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
}

/*
std::vector<PeanoReader*>* PeanoMetaFile::createReadersFull(int index) {
	return getDataSet(index)->createReadersFull();
}

std::vector<PeanoReader*>* PeanoMetaFile::createReadersResolution(int index, int resolution) {
	return getDataSet(index)->createReadersResolution(resolution);
}
*/


std::vector<PeanoDataSet*>* PeanoMetaFile::getDataSets() {
	return this->dataSets;
}


PeanoDataSet* PeanoMetaFile::getDataSet(int index) {
	return this->dataSets->at(index);
}


int PeanoMetaFile::numberOfDataSets() {
	return dataSets->size();
}


PeanoMetaFile::~PeanoMetaFile() {
	for(uint i = 0; i < dataSets->size(); i++) {
		delete dataSets->at(i);
	}
	delete dataSets;
}
