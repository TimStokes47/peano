#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <limits>

#include "PatchData.h"

#include "tarch/la/ScalarOperations.h"


visualisation::data::PatchData::PatchData(int dimensions, double* offset_, double* size_, int pointsPerAxis, PeanoDataType type_) {
  for (int d=0; d<dimensions; d++) {
	offset[d]    = offset_[d];
	size[d]      = size_[d];
  }

  int points;
  if (type_ == PeanoDataType::Cell_Values) {
    points = tarch::la::aPowI(dimensions,pointsPerAxis);
  }
  else {
    points = tarch::la::aPowI(dimensions,pointsPerAxis+1);
  }

  data = new double[points];
}


void visualisation::data::PatchData::free() {
  delete[] data;
  data = nullptr;
}
