#ifndef PEANOCONVERTER_H_
#define PEANOCONVERTER_H_

#include "PeanoPatch.h"
#include "input/PeanoTextPatchFileReader.h"

#include "../config.h"
#ifdef UseVTK
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>
#endif


class PeanoConverter {
  public:
    #ifdef UseVTK
	/**
	 * Maps a set of patches onto one unstructured grids.
	 */
	static vtkSmartPointer<vtkUnstructuredGrid> combine(const std::vector<PeanoPatch*>& patches);
	static vtkSmartPointer<vtkUnstructuredGrid> combine(const std::vector<visualisation::input::PeanoTextPatchFileReader*>& readers);
    #endif

	/**
	 * See combine(). Don't pass in an extension. It is added automatically.
	 *
	 * @return File name used
	 */
	static std::string combineAndWriteToFile(const std::vector<PeanoPatch*>& patches, const std::string& outputFileWithoutExtention);

//	static PeanoPatch* subSample(std::vector<PeanoReader*> &readers, int x, int y, int z);
  private:
    #ifdef UseVTK
	/**
	 * Used if the patch doesn't have any mapping and thus represents a regular grid.
	 */
	static vtkSmartPointer<vtkImageData> toImageData(PeanoPatch *patch);

	/**
	 * Basic routine.
	 *
	 * Take an individual patch and convert it into an unstructured grid.
	 */
	static vtkSmartPointer<vtkUnstructuredGrid> toUnstructuredGrid(PeanoPatch* patch);
    #endif

	static int xyzToIndex(int x, int y, int z, int dimensions[3]);
	/*
	static std::vector<int> getOverlappingIndexes(PeanoPatch &patch, vtkSmartPointer<vtkImageData> image);
*/
};

#endif /* PEANOCONVERTER_H_ */
