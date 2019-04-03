#ifndef _VISUALISATION_OUTPUT_VTU_WRITER_H_
#define _VISUALISATION_OUTPUT_VTU_WRITER_H_

#include "Writer.h"

#include "../config.h"
#include "../data/PatchData.h"

#include "tarch/logging/Log.h"


#ifdef UseVTK
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#endif


#include <vector>


namespace visualisation {
  namespace output {
    class VTUWriter;
  }

  namespace input {
    /**
     * Forward declaration
     */
    class PeanoTextPatchFileReader;
  }
}


class visualisation::output::VTUWriter: public visualisation::output::Writer {
  public:
	VTUWriter(const std::string&  directory, const std::string& outputFileWithoutExtention);
	~VTUWriter();

	void writeFile(const visualisation::data::Variable& variable, const std::vector<visualisation::data::PatchData>& data) override;

	/**
	 * Meta file write
	 */
/*
	void writeFile(const PeanoMetaFile& metaFile, const std::string& selector) override;

	static void writeFile(const std::string& outputFileWithoutExtension, const std::vector<PeanoPatch*>& patches);
*/
  private:
	static tarch::logging::Log  _log;

	const std::string& _outputFileWithoutExtension;
	const std::string& _directory;
	std::ofstream      _pvdFile;

    #ifdef UseVTK
    /**
     * Maps a set of patches onto one unstructured grids.
     */
//    static vtkSmartPointer<vtkUnstructuredGrid> combine(const std::vector<PeanoPatch*>& patches);

	/**
	 * Used if the patch doesn't have any mapping and thus represents a regular grid.
	 */
//	static vtkSmartPointer<vtkImageData> toImageData(PeanoPatch *patch);

	/**
	 * Basic routine.
	 *
	 * Take an individual patch and convert it into an unstructured grid.
	 */
//	static vtkSmartPointer<vtkUnstructuredGrid> toUnstructuredGrid(PeanoPatch* patch);
    #endif

	/**
	 * Helper function that we use extensively when we map a patch within one
	 * cell onto an unstructured grid fragment.
	 */
	static int xyzToIndex(int x, int y, int z, int dimensions[3]);
};

#endif
