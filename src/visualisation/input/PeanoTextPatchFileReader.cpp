#include <string>
#include <iostream>
#include <vector>
#include <fstream>


#include "Parser.h"
#include "PeanoTextPatchFileReader.h"


#include "tarch/Assertions.h"
#include "tarch/la/ScalarOperations.h"


tarch::logging::Log visualisation::input::PeanoTextPatchFileReader::_log( "visualisation::input::PeanoTextPatchFileReader" );


visualisation::input::PeanoTextPatchFileReader::PeanoTextPatchFileReader(const std::string &file):
  _dimensions(-1),
  _file(file) {
}


visualisation::input::PeanoTextPatchFileReader::~PeanoTextPatchFileReader() {
}


void visualisation::input::PeanoTextPatchFileReader::parse() {
  logInfo( "parse()", "reading file " << _file );

  std::ifstream ifs(_file);
  std::vector<std::string> lines;
  for (std::string line; std::getline(ifs, line); /**/ ) {
	lines.push_back(line);
  }
  ifs.close();

  if (lines.empty()) {
	logError( "parse()", "file is empty (or does not exist)" );
  }

  for(uint i = 0; i < lines.size(); i++) {
    std::string line = lines[i];

    std::vector< std::string > tokens = Parser::tokenise( line );

    if( tokens.empty()) { // or tokens[0]=="#" ) {
	}
	else if ( tokens[0]=="begin" and tokens[1]=="cell-values" ) { //define a cell variable
      std::string variableName = Parser::removeHyphens(tokens[2]);

	  std::vector<std::string> variableDeclarationLines;
      while ( lines[i].find( "end cell-values" )==std::string::npos ) {
        variableDeclarationLines.push_back( lines[i] );
        i++;
      }

      parseVariablesDeclaration( variableDeclarationLines, variableName, visualisation::data::PeanoDataType::Cell_Values );
    }
	else if ( tokens[0]=="begin" and tokens[1]=="vertex-values" ) { //define a vertex variable
      std::string variableName = Parser::removeHyphens(tokens[2]);

	  std::vector<std::string> variableDeclarationLines;
      while ( lines[i].find( "end vertex-values" )==std::string::npos ) {
        variableDeclarationLines.push_back( lines[i] );
        i++;
      }

      parseVariablesDeclaration( variableDeclarationLines, variableName, visualisation::data::PeanoDataType::Vertex_Values );
	}
    else if ( tokens[0]=="dimensions" ) {
	  _dimensions = std::stoi(tokens[1]);
	  logDebug( "parse()", "dimensions=" << _dimensions );
	}
    else if( tokens[0]=="begin" and tokens[1]=="patch" ) {
      logDebug( "parse()", "start to parse patch" );
	  std::vector<std::string> patchLines;
      while ( lines[i].find( "end patch" )==std::string::npos ) {
        patchLines.push_back( lines[i] );
        i++;
      }
      parsePatch( patchLines );
    }
    else {
  	  logDebug( "parse()", "ignore line " << line );
	}
  }
}


void visualisation::input::PeanoTextPatchFileReader::parseVariablesDeclaration( const std::vector<std::string>& description, const std::string&  variableName, visualisation::data::PeanoDataType type ) {
    //get mapping
    int numberOfUnknowns = -1;
    int numberOfDofs     = -1;
	for (auto p: description) {
      std::vector<std::string> tokens = Parser::tokenise( p );
	  if (tokens[0]=="number-of-unknowns") {
        numberOfUnknowns = std::stoi(tokens[1]);
	  }
	  if (tokens[0]=="number-of-dofs-per-axis") {
        numberOfDofs = std::stoi(tokens[1]);
	  }
	}
/*
    double* mapping = nullptr;
    int mappings = -1;
    std::string line2 = lines[i+2];
    if( line2.compare( "begin mapping")==0 ) {
      logDebug( "parse()", "start to parse mapping" );
      std::vector<std::string> patchLines;
      while ( lines[i].find( "end mapping" )==std::string::npos ) {
        patchLines.push_back( lines[i] );
        i++;
      }
      mapping = parseMapping( patchLines );
    }
    _data.push
*/

    // @todo This belongs into the variable definition
/*
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
*/

    logDebug( "parse()", "Added new variables " << variableName << " with " << numberOfDofs << " dof per axis (Cartesian layout) and " << numberOfUnknowns << " unknowns per dof" );

    _data.data.insert(
      std::pair<visualisation::data::Variable, std::vector<visualisation::data::PatchData>>(
        visualisation::data::Variable( variableName, numberOfDofs, numberOfUnknowns, type ),
		std::vector<visualisation::data::PatchData>()
	) );
}


/*
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
*/


void visualisation::input::PeanoTextPatchFileReader::parsePatch( const std::vector<std::string>& text ) {
  assertion( _dimensions==2 or _dimensions==3 );

  logDebug( "parsePatch(...)", "create patch" );
  logDebug( "parsePatch(...)", "parse " << text.size() << " lines" );

  std::vector<double> offset;
  std::vector<double> size;

  for (int i=0; i<text.size(); i++) {
    std::vector<std::string> tokens = Parser::tokenise( text[i] );

    if( tokens[0]=="offset" ) {
      logDebug( "parsePatch(...)", "set offset to " << text[i] );
      for(int j = 0; j < _dimensions; j++) {
    	offset.push_back( std::stod(tokens[j+1]) );
      }
	}
    else if ( tokens[0]=="size" ) {
      logDebug( "parsePatch(...)", "set sizes to " << text[i] );
      for(int j = 0; j < _dimensions; j++) {
    	size.push_back( std::stod(tokens[j+1]) );
      }
    }
    else if ( tokens[0]=="begin" and tokens[1]=="cell-values") {
      std::string variableName = Parser::removeHyphens(tokens[2]);

      i++;
	  std::vector<std::string> data;
      while ( text[i].find( "end cell-values" )==std::string::npos ) {
    	std::vector<std::string> tokens = Parser::tokenise( text[i] );
        data.insert( data.end(), tokens.begin(), tokens.end() );
        i++;
      }

      addDataToPatch(variableName,offset.data(),size.data(),data);
	}
    else if ( tokens[0]=="begin" and tokens[1]=="vertex-values" ) {
      std::string variableName = Parser::removeHyphens(tokens[2]);

      i++;
	  std::vector<std::string> data;
      while ( text[i].find( "end vertex-values" )==std::string::npos ) {
    	std::vector<std::string> tokens = Parser::tokenise( text[i] );
        data.insert( data.end(), tokens.begin(), tokens.end() );
        i++;
      }

      addDataToPatch(variableName,offset.data(),size.data(),data);
	}
  }
}


void visualisation::input::PeanoTextPatchFileReader::addDataToPatch( const std::string& variableName, double* offset, double* size, const std::vector< std::string >& textData ) {
  logDebug( "parsePatch(...)", "set data of variable " << variableName );

  const visualisation::data::Variable*  key = nullptr;
  for (auto& p: _data.data) {
 	if (p.first.name==variableName) {
  	  key = &(p.first);
  	}
  }

  if (key==nullptr) {
    logError( "parsePatch(...)", "no variables for " << variableName << " have been declared. Ignore data set");
    return;
  }

  const int expectedDataEntries = tarch::la::aPowI(_dimensions,key->dofsPerAxis);
  if ( textData.size()!=expectedDataEntries ) {
    logError( "parsePatch(...)", "expected " << expectedDataEntries << " data entries for variable " << variableName << " but got only " << textData.size() << ". Ignore data set");
    return;
  }

  visualisation::data::PatchData newEntry(_dimensions, offset, size, key->dofsPerAxis, key->type);
  for (int i=0; i<expectedDataEntries; i++) {
    newEntry.data[i] = std::stod(textData[i]);
  }

  _data.data[*key].push_back( newEntry );
}



visualisation::data::DataSet  visualisation::input::PeanoTextPatchFileReader::getData() const {
  return _data;
}
