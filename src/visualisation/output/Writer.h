#ifndef _VISUALISATION_OUTPUT_WRITER_H_
#define _VISUALISATION_OUTPUT_WRITER_H_


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
	virtual void writeFile(const PeanoMetaFile& metaFile, const std::string& selector) = 0;
};


#endif
