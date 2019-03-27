#include "PeanoTextMetaFileReader.h"
#include "Parser.h"

#include <fstream>


tarch::logging::Log  visualisation::input::PeanoTextMetaFileReader::_log( "visualisation::input::PeanoTextMetaFileReader" );



void visualisation::input::PeanoTextMetaFileReader::parseDataset(const std::vector<std::string> lines) {
  PeanoDataSet* dataSet = new PeanoDataSet(file->directory);

  for(std::string line: lines) {
    if (line.empty()) continue;

	std::vector<std::string> tokens = Parser::tokenise(line);

	if( tokens[0]=="#" ) {
	}
	else if(tokens.size() == 5) {
	  int x = std::stoi(tokens[1]);
	  int y = std::stoi(tokens[2]);
      int z = std::stoi(tokens[3]);

      logError( "parseDataset(...)", "not supported yet")
/*
			std::string fileName = split[4];
//			boost::erase_all(fileName, "\"");

			std::vector<int> dimensions;
			dimensions.push_back(x);
			dimensions.push_back(y);
			dimensions.push_back(z);

			//std::vector<std::string> resolutionData;
			//std::vector<std::vector<int>> resolutions;

			resolutions->push_back(dimensions);
			resolutionData->push_back(fileName);
*/
	}
	else if(tokens.size() == 2) {
	  std::string fileName = tokens[1];
      dataSet->fullData->push_back( Parser::removeHyphens(fileName) );
	}
  }


  file->dataSets->push_back(dataSet);
}


void visualisation::input::PeanoTextMetaFileReader::parse(const std::string&  fileName) {
  file = new PeanoMetaFile();

  file->dataSets = new std::vector<PeanoDataSet*>();
  file->fileName = fileName;

  file->directory = Parser::getDirectory(fileName);

  logInfo( "parse(...)", "parse meta file " << fileName );

  // read the file in to a vector of strings
  std::ifstream ifs(fileName);
  std::vector<std::string> lines;
  for (std::string line; std::getline(ifs, line); /**/ ) {
	lines.push_back(line);
  }
  ifs.close();

  bool metadataFile = false;

  for(uint i = 0; i < lines.size(); i++) {
  	std::string line = lines[i];

	if (line.rfind("#",0)==0 ) {
		continue;
	}
	else if ( line.rfind("begin dataset",0)==0 ) {
		metadataFile = true;
		std::vector<std::string> dataSetLines;
		for(i++; i < lines.size(); i++) {
			std::string line2 = lines[i];
			if( line2.rfind("end dataset",0)==0 ) {
				break;
			} else {
				dataSetLines.push_back(line2);
			}
		}
		parseDataset( dataSetLines );
	}
  }

  //if the input was not a dataset then it was a peano file
  if(!metadataFile) {
    std::vector<std::string> dataLine;
	dataLine.push_back(fileName);
	parseDataset(dataLine);
  }

  logInfo( "parse(...)", "meta file holds " << file->dataSets->size() << " entries" );
}
