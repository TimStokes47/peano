#include "PeanoPatch.h"

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <limits>

#include "PeanoPatchData.h"
#include "PeanoVariable.h"


PeanoPatch::PeanoPatch() {
  offsets    = nullptr;
  sizes      = nullptr;
  resolution = nullptr;
  dimensions = -1;
}


PeanoVariable* PeanoPatch::getStructure() {
	for (auto it : this->patchData) {
		return it.second->structure;
	}
	return nullptr;
}


bool PeanoPatch::hasMappings(){
	for (auto it : this->patchData) {
		return it.second->structure->mappings != -1;
	}
	return false;
}


double* PeanoPatch::getPositionMap(int x, int y, int z) {
	double* position = new double[3];
	return position;
}


double* PeanoPatch::getPositionVertex(int x, int y, int z) {
	double* position = new double[3];

	position[0] = offsets[0] + sizes[0]*x/resolution[0];
	position[1] = offsets[1] + sizes[1]*y/resolution[1];
	position[2] = offsets[2] + sizes[2]*z/resolution[2];

	return position;
}


double* PeanoPatch::getPositionCellCenter(int x, int y, int z) {
	double* position = new double[3];

	position[0] = offsets[0] + sizes[0]*(0.5+x)/resolution[0];
	position[1] = offsets[1] + sizes[1]*(0.5+y)/resolution[1];
	position[2] = offsets[2] + sizes[2]*(0.5+z)/resolution[2];

	return position;
}


int PeanoPatch::getIndexCellData(int x, int y, int z) {
	return z + y * resolution[2] + x*resolution[2]*resolution[1];
	//return x + y*(resolution[0]) + z*(resolution[0])*(resolution[1]);
}


int PeanoPatch::getIndexVertexData(int x, int y, int z) {
	return z + y * (resolution[2]+1) + x*(resolution[2]+1)*(resolution[1]+1);
	//return x + y*(resolution[0]) + z*(resolution[0])*(resolution[1]);
}


PeanoPatch::~PeanoPatch() {
	delete [] offsets;
	delete [] sizes;
	for (auto it : patchData) {
		delete it.second;
	}
}
