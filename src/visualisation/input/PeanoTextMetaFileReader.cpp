#include "PeanoTextMetaFileReader.h"
#include "PeanoTextPatchFileReader.h"
#include "Parser.h"

#include <fstream>


tarch::logging::Log  visualisation::input::PeanoTextMetaFileReader::_log( "visualisation::input::PeanoTextMetaFileReader" );


/*
visualisation::input::PeanoTextMetaFileReader::PeanoTextMetaFileReader( const std::string& filename ):
  _filename(filename) {
}


visualisation::input::PeanoTextMetaFileReader::~PeanoTextMetaFileReader() {
  delete _file;
}


void visualisation::input::PeanoTextMetaFileReader::parse() {
  _file = new PeanoMetaFile();

  logInfo( "parse(...)", "parse meta file " << _filename );

  // read the file in to a vector of strings
  std::ifstream ifs(_filename);
  std::vector<std::string> lines;
  for (std::string line; std::getline(ifs, line);  ) {
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

  logInfo( "parse(...)", "meta file holds " << _file->_dataSets.size() << " entries (time stamps)" );
}


void visualisation::input::PeanoTextMetaFileReader::parseDataset(const std::vector<std::string> lines) {
  _file->_dataSets.push_back( PeanoMetaFile::PeanoDataSet() );
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

*/




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





/*
	}
	else if(tokens.size() == 2) {
	  std::string fileName = Parser::removeHyphens( tokens[1] );
      visualisation::input::PeanoTextPatchFileReader  reader(fileName);
      reader.parse();
      auto data = reader.getPatches();
	  _file->_dataSets.back().data[PeanoMetaFile::RawData].insert(
        _file->_dataSets.back().data[PeanoMetaFile::RawData].end(),
		data.begin(), data.end()
      );
	  // @todo remove
      logError( "parseDataset(...)", "inserted " << data.size() << " patch(es)" );
	}
  }
}



PeanoMetaFile* visualisation::input::PeanoTextMetaFileReader::getFile() {
	return _file;
}
*/
