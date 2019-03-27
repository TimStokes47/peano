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


void visualisation::input::PeanoTextPatchFileReader::parse(const std::string &file) {
  patchSize = new int[3];

  logInfo( "PeanoTextPatchFileReader(string)", "reading file " << file );

  std::ifstream ifs(file);
  std::vector<std::string> lines;
  for (std::string line; std::getline(ifs, line); /**/ ) {
	lines.push_back(line);
  }
  ifs.close();

  if (lines.empty()) {
	logError( "PeanoTextPatchFileReader(string)", "file is empty (or does not exist)" );
  }

  for(uint i = 0; i < lines.size(); i++) {
    std::string line = lines[i];

	logDebug( "PeanoTextPatchFileReader(string)", "Interpret line " << line);

    std::vector< std::string > tokens = Parser::tokenise( line );

    if( tokens.empty()) { // or tokens[0]=="#" ) {
	}
	else if ( tokens[0]=="begin" and tokens[1]=="cell-values" ) { //define a cell variable
      std::string variableName = Parser::removeHyphens(tokens[2]);

  	  logDebug( "PeanoTextPatchFileReader(string)", "Added new variables " << variableName);

  	  i++;
  	  std::string nextLine = lines[i];
      std::vector<std::string> splitUnknowns = Parser::tokenise( nextLine );
      int unknowns = std::stoi(splitUnknowns[1]);

      //get mapping
      double* mapping = nullptr;
      int mappings = -1;
      std::string line2 = lines[i+2];
      if( line2.compare( "begin mapping")==0 ) {
        logDebug( "PeanoTextPatchFileReader(string)", "start to parse mapping" );
        std::vector<std::string> patchLines;
        while ( lines[i].find( "end mapping" )==std::string::npos ) {
          patchLines.push_back( lines[i] );
          i++;
        }
        mapping = parseMapping( patchLines );
      }
      PeanoVariable* variable = new PeanoVariable(variableName, unknowns, Cell_Values, cells, mapping, mappings);
      variables.push_back(variable);
      i++;
    }
	else if ( tokens[0]=="begin" and tokens[1]=="vertex-values" ) { //define a vertex variable
      std::string variableName = Parser::removeHyphens(tokens[2]);

  	  logDebug( "PeanoTextPatchFileReader(string)", "Added new variables " << variableName);

  	  i++;
  	  std::string nextLine = lines[i];
      std::vector<std::string> splitUnknowns = Parser::tokenise( nextLine );
      int unknowns = std::stoi(splitUnknowns[1]);

      //get mapping
      double* mapping = nullptr;
      int mappings = -1;
      std::string line2 = lines[i+2];
      if( line2.compare( "begin mapping")==0 ) {
        logDebug( "PeanoTextPatchFileReader(string)", "start to parse mapping" );
        std::vector<std::string> patchLines;
        while ( lines[i].find( "end mapping" )==std::string::npos ) {
          patchLines.push_back( lines[i] );
          i++;
        }
        mapping = parseMapping( patchLines );
      }
      PeanoVariable* variable = new PeanoVariable(variableName, unknowns, Vertex_Values, vertices, mapping, mappings);
      variables.push_back(variable);
      i++;
	}
    else if ( tokens[0]=="dimensions" ) {
	  dimensions = std::stoi(tokens[1]);
	  logDebug( "PeanoTextPatchFileReader(string)", "dimensions=" << dimensions );
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
      logDebug( "PeanoTextPatchFileReader(string)", "start to parse patch" );
	  std::vector<std::string> patchLines;
      while ( lines[i].find( "end patch" )==std::string::npos ) {
        patchLines.push_back( lines[i] );
        i++;
      }
      parsePatch( patchLines );
    }
    else {
  	  logDebug( "PeanoTextPatchFileReader(string)", "ignore line " << line );
	}
  }
}


double* visualisation::input::PeanoTextPatchFileReader::parseMapping( const std::vector<std::string>& patchDescription ) {
  std::vector<double> data;
  for (auto p: patchDescription) {
    std::vector<std::string> tokens = Parser::tokenise(p);
    for (auto pp: tokens) {
      data.push_back( std::stod(pp) );
    }
  }
  double* result = new double[ data.size() ];
  for (int i=0; i<data.size(); i++) {
	result[i] = data[i];
  }
  return result;
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
      logDebug( "parsePatch(...)", "set sizes to " << text[i] );
      data->sizes = new double[dimensions];
      for(int j = 0; j < dimensions; j++) {
        data->sizes[j] = std::stod(tokens[j+1]);//the 0th element of split is "offset" so skip it
      }
    }
    else if ( tokens[0]=="begin" and (tokens[1]=="cell-values" or tokens[1]=="vertex-values") ) {
      std::string variableName = Parser::removeHyphens(tokens[2]); //variableName.substr(1, variableName.size() -2);
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
