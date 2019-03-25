#include "Parser.h"


#include <sstream>


std::vector<std::string> visualisation::input::Parser::tokenise( const std::string& line ) {
  std::vector<std::string> result;

  std::istringstream ss(line);
  std::string token;
  while (ss >> token) {
	result.push_back( token );
  }

  return result;
}

