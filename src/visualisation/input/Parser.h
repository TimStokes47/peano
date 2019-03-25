#ifndef _VISUALISATION_INPUT_PARSER_H_
#define _VISUALISATION_INPUT_PARSER_H_


namespace visualisation {
  namespace input {
    class Parser;
  }
}


#include <vector>
#include <string>


class visualisation::input::Parser {
  public:
	static std::vector<std::string> tokenise( const std::string& line );
};

#endif
