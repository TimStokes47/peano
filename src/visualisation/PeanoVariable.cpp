#include "PeanoVariable.h"


PeanoVariable::PeanoVariable(std::string variableName, int unknowns, PeanoDataType type, int points, double mapping[], int mappings) {
	this->name = variableName;
	this->unknowns = unknowns;
	this->type = type;
	this->mapping = mapping;
	this->mappings = mappings;
	this->points = points;
	this->totalValues = points*unknowns;
}


PeanoVariable::~PeanoVariable() {
	delete [] mapping;
}

