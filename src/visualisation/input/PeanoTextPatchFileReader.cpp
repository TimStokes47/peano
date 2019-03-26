#include <string>
#include <iostream>
#include <vector>
#include <fstream>


#include "Parser.h"
#include "PeanoPatch.h"
#include "PeanoTextPatchFileReader.h"
#include "PeanoVariable.h"

tarch::logging::Log visualisation::input::PeanoTextPatchFileReader::_log( "visualisation::input::PeanoTextPatchFileReader" );


visualisation::input::PeanoTextPatchFileReader::PeanoTextPatchFileReader() {
}


std::string visualisation::input::PeanoTextPatchFileReader::removeHyphens( const std::string& value ) {
  std::string result = value;
  if (result.at(result.size()-1)=='"') {
    result = result.substr(0,result.size()-1);
  }
  if (result.at(0)=='"') {
    result = result.substr(1);
  }
  return result;
}


void visualisation::input::PeanoTextPatchFileReader::parse(const std::string &file) {
  patchSize = new int[3];

  logInfo( "PeanoTextPatchFileReader(string)", "Reading file " << file );

  std::ifstream ifs(file);
  std::vector<std::string> lines;
  for (std::string line; std::getline(ifs, line); /**/ ) {
	lines.push_back(line);
  }
  ifs.close();

  if (lines.empty()) {
	logError( "PeanoTextPatchFileReader(string)", "File is empty (or does not exist)" );
  }

  for(uint i = 0; i < lines.size(); i++) {
    std::string line = lines[i];

	logDebug( "PeanoTextPatchFileReader(string)", "Interpret line " << line);

    std::vector< std::string > tokens = Parser::tokenise( line );

    if( tokens.empty()) { // or tokens[0]=="#" ) {
	}
	else if ( tokens[0]=="begin" and tokens[1]=="cell-values" ) { //define a cell variable
      std::string variableName = removeHyphens(tokens[2]);

      // @todo Debug
  	  logInfo( "PeanoTextPatchFileReader(string)", "Added new variables " << variableName);

  	  i++;
  	  std::string nextLine = lines[i];
      std::vector<std::string> splitUnknowns = Parser::tokenise( nextLine );
      int unknowns = std::stoi(splitUnknowns[1]);

      //get mapping
      double* mapping = nullptr;
      int mappings = -1;
      std::string line2 = lines[i+2];
      if( line2.compare( "begin mapping")==0 ) {
    	  logError( "todo", "not implemented yet");
    	  /*
        std::string line3 = lines[i+3];
        boost::erase_all(line3, "end mapping");
		boost::trim(line3);
		std::vector<std::string> splitMappings;
		boost::split(splitMappings, line3, boost::is_any_of(" "));

		mappings = splitMappings.size();

				mapping = new double[mappings];
				for(uint j = 0; j < splitMappings.size(); j++) {
					mapping[j] = std::stod(splitMappings[j]);
				}
				i+= 4;
			}
*/
      }
      PeanoVariable* variable = new PeanoVariable(variableName, unknowns, Cell_Values, cells, mapping, mappings);
      variables.push_back(variable);
      i++;
    }
/*
	else if(boost::starts_with(line, "begin vertex-values")) {//define a vertex variable
			//get variable name
			std::vector<std::string> split;
			boost::split(split, line, boost::is_any_of(" "));
			std::string variableName = split[2];
			variableName = variableName.substr(1, variableName.size() -2);

			//get number of unknowns
			std::string line1 = lines[i+1];
			boost::trim(line1);
			std::vector<std::string> splitUnknowns;
			boost::split(splitUnknowns, line1, boost::is_any_of(" "));
			int unknowns = std::stoi(splitUnknowns[1]);

			//get mapping
			double* mapping = nullptr;
			int mappings = -1;
			std::string line2 = lines[i+2];
			boost::trim(line2);
			if(boost::starts_with(line2, "begin mapping")) {

				std::string line3 = lines[i+3];
				boost::erase_all(line3, "end mapping");
				boost::trim(line3);
				std::vector<std::string> splitMappings;
				boost::split(splitMappings, line3, boost::is_any_of(" "));

				mappings = splitMappings.size();

				mapping = new double[mappings];
				for(int j = 0; j < mappings; j++) {
					mapping[j] = std::stod(splitMappings[j]);
				}
				i+=4;
			}
			PeanoVariable* variable = new PeanoVariable(variableName, unknowns, Vertex_Values, vertices, mapping, mappings);
			variables.push_back(variable);
		}
		*/
    else if ( tokens[0]=="dimensions" ) {
	  dimensions = std::stoi(tokens[1]);
	  // @todo debug
	  logInfo( "PeanoTextPatchFileReader(string)", "dimensions=" << dimensions );
	}
    else if( tokens[0]=="patch-size" ) {
      for(int j = 1; j <= dimensions; j++) {
        patchSize[j-1] = std::stoi(tokens[j]);
      }

      //update number of cells and vertices
	  cells = 1;
      vertices = 1;
      for(int i = 0; i < dimensions; i++) {
        cells *= patchSize[i];
        vertices *= patchSize[i] +1;
      }
    }
    else if( tokens[0]=="begin" and tokens[1]=="patch" ) {
    	// @todo
      logInfo( "PeanoTextPatchFileReader(string)", "start to parse patch" );
      //build a list of lines for this patch
	  std::vector<std::string> patchLines;
      while ( lines[i].find( "end patch" )==std::string::npos ) {
    	logInfo( "PeanoTextPatchFileReader(string)", "backup " << lines[i] );
        patchLines.push_back( lines[i] );
        i++;
      }
      parsePatch( patchLines );
    }
    else {
	  // @todo debug
  	  logInfo( "PeanoTextPatchFileReader(string)", "ignore line " << line );
	}
  }
}


void visualisation::input::PeanoTextPatchFileReader::parsePatch( const std::vector<std::string>& text ) {
  PeanoPatch* data = new PeanoPatch();

  data->dimensions = dimensions;
  data->resolution = patchSize;

  logDebug( "parsePatch(...)", "create patch of dimension " << dimensions << " with " << variables.size() << " variable(s)" );

  //calculate the number of cells in the patch
  int cells = 1;
  for(int i = 0; i < dimensions; i++) {
    cells *= patchSize[i];
  }

  //initialize the data objects for each variable
  for(uint i = 0; i < variables.size(); i++) {
    PeanoPatchData* patchData = new PeanoPatchData(variables[i]);
    data->patchData[patchData->structure->name] = patchData;
    logDebug( "parsePatch(...)", "add entry for variable " << patchData->structure->name << " with " << variables[i]->totalValues << " value(s)" );
  }

  logDebug( "parsePatch(...)", "parse " << text.size() << " lines" );
  for (int i=0; i<text.size(); i++) {
    std::vector<std::string> tokens = Parser::tokenise( text[i] );

    if( tokens[0]=="offset" ) {
      logDebug( "parsePatch(...)", "set offset to " << text[i] );
      data->offsets = new double[dimensions];
      for(int j = 0; j < dimensions; j++) {
        data->offsets[j] = std::stod(tokens[j+1]);//the 0th element of split is "offset" so skip it
      }
	}
    else if ( tokens[0]=="size" ) {
      logInfo( "parsePatch(...)", "set sizes to " << text[i] );
      data->sizes = new double[dimensions];
      for(int j = 0; j < dimensions; j++) {
        data->sizes[j] = std::stod(tokens[j+1]);//the 0th element of split is "offset" so skip it
      }
    }
    else if ( tokens[0]=="begin" and (tokens[1]=="cell-values" or tokens[1]=="vertex-values") ) {
      std::string variableName = removeHyphens(tokens[2]); //variableName.substr(1, variableName.size() -2);
      logDebug( "parsePatch(...)", "set data of variable " << variableName );

      if ( data->patchData.count( variableName) == 0 ) {
    	logError( "parsePatch(...)", "No data set for variable " << variableName << " found" );
      }
      else {
        PeanoPatchData* variableData = data->patchData[variableName];

        i++;
        std::vector< std::string >   values = Parser::tokenise( text[i] );

        //convert the strings to doubles and add to the array
        for(int j = 0; j < variableData->structure->totalValues; j++) {
         variableData->values[j] = std::stod(values[j]);
        }
      }
	}
  }

  patches.push_back(data);
}


bool visualisation::input::PeanoTextPatchFileReader::isEmpty() {
  return variables.empty() and patches.empty();
}


visualisation::input::PeanoTextPatchFileReader::~PeanoTextPatchFileReader() {
  delete [] patchSize;

  for(uint i = 0; i < patches.size(); i++) {
    delete patches[i];
  }

  for(uint i = 0; i < variables.size(); i++) {
    delete variables[i];
  }
}
