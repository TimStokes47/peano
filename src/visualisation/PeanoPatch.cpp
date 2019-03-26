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


bool PeanoPatch::saveToTextFile(std::string filename) {
	std::cout << "\nSaving patch to file " << filename << "\n";
	std::ofstream file;
	file.open(filename.c_str(), std::ofstream::out | std::ofstream::trunc);


	file << "#\n";
	file << "# Peano output file\n";
	file << "# Version 0.1\n";
	file << "#\n";
	file << "format ASCII\n";
	file << "dimensions " << dimensions << "\n";
	file << "patch-size " << resolution[0] << " " << resolution[1] << " " << resolution[2] << "\n\n";

	//metadata about the variables in this form
	//begin cell-values "Q"
	//  number-of-unknowns 23
	//end cell-values

	for (auto it : patchData) {
		PeanoPatchData* data = it.second;
		file << "begin " << (data->structure->type == Cell_Values?"cell":"vertex") << "-values";
		file << " \"" << data->structure->name << "\"\n";
		file << "  number-of-unknowns " << data->structure->unknowns << "\n";
		file << "end " << (data->structure->type == Cell_Values?"cell":"vertex") << "-values\n\n";
	}

	//output the actual patch
	file << "begin patch\n";
	file << "  offset " << offsets[0] << " " << offsets[1] << " " << offsets[2] << "\n";
	file << "  size " << sizes[0] << " " << sizes[1] << " " << sizes[2] << "\n";

	//output the variable values
	for (auto it : patchData) {
		PeanoPatchData* data = it.second;
		file << "  begin " << (data->structure->type == Cell_Values?"cell":"vertex") << "-values";
		file << " \"" << data->structure->name << "\"\n";
		file << "   ";
		for(int i = 0; i < data->structure->totalValues; i++) {
			//file << " " << data->values[i];

			double value = data->values[i];
			if(value < 0.000000000001 && value > 0) {
				file << " 0";
			} else {
				file << " " << value;
			}

		}
		file << "\n  end " << (data->structure->type == Cell_Values?"cell":"vertex") << "-values\n";
	}
	file << "end patch\n";

	file.close();

	return false;
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
