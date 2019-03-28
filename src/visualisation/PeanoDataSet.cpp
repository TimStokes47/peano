#include "PeanoDataSet.h"
#include "tarch/Assertions.h"

#include "input/PeanoTextPatchFileReader.h"

#include <vector>
#include <string>

#include "output/VTUWriter.h"


std::string PeanoDataSet::RawData = "raw";


PeanoDataSet::PeanoDataSet(const std::string& directory):
  _directory(directory) {
}


std::vector<std::string> PeanoDataSet::toString() {
	std::vector<std::string> lines;

	lines.push_back("begin dataset");

	//print the full data
/*
	for (auto p: _data) {
      std::string identifier = p->first;
      if (identifier==RawData) {
		lines.push_back("  include \"" + p->second + \"");
      }
      else {

      }
	}
*/

	//print the lower resolution versions
/*
	for(uint i = 0; i < resolutions->size(); i++) {
		int x = resolutions->at(i)[0];
		int y = resolutions->at(i)[1];
		int z = resolutions->at(i)[2];
		std::string file = resolutionData->at(i);
		lines.push_back("  include-resolution " + std::to_string(x) + " "
				+ std::to_string(y) + " " + std::to_string(z) + " " + file);
	}
*/


	lines.push_back("end dataset");

	return lines;
}


std::vector<visualisation::input::PeanoTextPatchFileReader*>* PeanoDataSet::createReaders( const std::string& identifier ) {
  std::vector<visualisation::input::PeanoTextPatchFileReader*>* readers = new std::vector<visualisation::input::PeanoTextPatchFileReader*>();
  const int maxSize = _data[ identifier ].size();

  #pragma omp parallel for
  for(uint i = 0; i < maxSize; i++) {
    visualisation::input::PeanoTextPatchFileReader* newReader = new visualisation::input::PeanoTextPatchFileReader();
    // @todo Das sollte jetzt aber irgendwie lazy sein. So landet ja alles im Speicher
    newReader->parse(_directory + _data[ identifier ].at(i));
    #pragma omp critical
	readers->push_back(newReader);
  }
  return readers;
}


PeanoDataSet::~PeanoDataSet() {
}
