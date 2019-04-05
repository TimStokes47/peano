#ifndef _VISUALISATION_DATA_DATA_SET_H_
#define _VISUALISATION_DATA_DATA_SET_H_

#include <string>
#include <vector>
#include <map>

#include "PatchData.h"

#include "tarch/logging/Log.h"


#include "Variable.h"


namespace visualisation {
  namespace input {
    class PeanoTextPatchFileReader;
    class PeanoTextMetaFileReader;
  }
}


namespace visualisation {
  namespace data {
    class DataSet;
  }
}


/**
 * Wrapper around a set of patches. It basically describes a whole patch file.
 * If you have a meta file, each time step yields a set of data sets.
 */
class visualisation::data::DataSet {
  public:
	DataSet();

	void free();

	/**
	 * @return Set of readers tied to a particular model. These readers haven't
	 *         parsed their files yet, i.e. you have to call parse() manually.
	 */
    std::vector<PatchData>  getData( const Variable& selector ) const;

	/**
	 * @return All the selectors (data representations) stored for a time step
	 */
	std::vector<Variable>  getVariables() const;
	bool                   hasVariable( const std::string& name ) const;
	Variable               getVariable( const std::string& name ) const;

  private:
	friend class visualisation::input::PeanoTextPatchFileReader;
	friend class visualisation::input::PeanoTextMetaFileReader;

	static tarch::logging::Log  _log;

    std::map< Variable, std::vector<PatchData> >  data;
};

#endif
