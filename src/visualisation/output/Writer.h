#ifndef _VISUALISATION_OUTPUT_WRITER_H_
#define _VISUALISATION_OUTPUT_WRITER_H_


#include "visualisation/data/DataSet.h"

namespace visualisation {
  namespace output {
    class Writer;
  }
}


class visualisation::output::Writer {
  public:
	/**
	 * Our writers write only one variant at a time even though the Peano file
	 * format can hold multiple variants
	 */
	virtual void writeFile(const visualisation::data::DataSet& data, const std::string& selector) = 0;
};


#endif
