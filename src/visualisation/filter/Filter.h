#ifndef _VISUALISATION_FILTER_FILTER_H_
#define _VISUALISATION_FILTER_FILTER_H_


namespace visualisation {
  namespace filter {
    class Filter;
  }
}


namespace visualisation {
  namespace data {
    class Variable;
  }
}


class visualisation::filter::Filter {
  public:
	void apply( const Variable& inputVariable, const std::vector<visualisation::data::PatchData>&  inputData, std::string targetSelectorName ) = 0;
	std::vector<visualisation::data::PatchData> getData() const = 0;
	Variable getVariable() const = 0;
};

#endif
