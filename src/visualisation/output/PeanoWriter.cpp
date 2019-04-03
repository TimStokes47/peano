#include <string>
#include <unordered_map>
#include <vector>

#include "../config.h"
#include "visualisation/input/PeanoTextPatchFileReader.h"

#include "tarch/Assertions.h"

#include "PeanoWriter.h"


tarch::logging::Log  visualisation::output::PeanoWriter::_log( "PeanoConverter" );
const std::string visualisation::output::PeanoWriter::_FileExtension( ".peano-patch-file" );
const std::string visualisation::output::PeanoWriter::_Header( "# \n# Peano patch file\n# Version 0.1\n# Written by Peano's writer tool\n#\nformat ASCII\n" ) ;


visualisation::output::PeanoWriter::PeanoWriter(const std::string&  directory, const std::string& outputFileWithoutExtension):
  _outputFileWithoutExtension(outputFileWithoutExtension),
  _directory(directory),
  _metaFile(directory + "/" + outputFileWithoutExtension+_FileExtension) {
  _metaFile << _Header << std::endl;
}


visualisation::output::PeanoWriter::~PeanoWriter() {
  _metaFile.close();
}


/*
void visualisation::output::PeanoWriter::writeFile(const visualisation::data::DataSet& data, const std::string& selector) {
  for( int timeStep = 0; timeStep<metaFile.getNumberOfDataSets(); timeStep++ ) {
    _metaFile << "begin dataset" << std::endl;

	for ( auto selector: metaFile.getStoredDataRepresentations(timeStep) ) {
	  std::vector<PeanoPatch*>  p = metaFile.getData( timeStep, selector );
      logInfo( "convertFile(...)", "process " << p.size() << " patches with selector " << selector );

      if (!p.empty()) {
    	  // @todo in the future, we might wanna split up the patches into subdirectories
        //std::string filename = _outputFileWithoutExtension + "-" + std::to_string(timeStep) + "-" + std::to_string(i);
    	// @todo Perhaps raw ito filename?
        std::string filename = _outputFileWithoutExtension + "-" + std::to_string(timeStep);

        #pragma omp critical
        {
          if (selector==PeanoMetaFile::RawData) {
            _metaFile << "  include \"" << filename << _FileExtension << "\"" << std::endl;
          }
          else {
           _metaFile << "  include \"" << selector << "\" \"" << filename << _FileExtension << "\"" << std::endl;
          }
        }
        writeFile(_directory + "/" + filename, p);
      }
	}

    _metaFile << "end dataset" << std::endl << std::endl;
  }
}


void visualisation::output::PeanoWriter::writeFile(const PeanoMetaFile&  metaFile, const std::string& selector) {
  for( int timeStep = 0; timeStep<metaFile.getNumberOfDataSets(); timeStep++ ) {
	std::vector<PeanoPatch*> p = metaFile.getData( timeStep, selector );

//    logInfo( "convertFile(...)", "time step consists of " << readers.size() << " data set(s)" );

    _metaFile << "begin dataset" << std::endl;

//    #pragma omp parallel for
    if (!p.empty()) {
//      std::string filename = _outputFileWithoutExtension + "-" + std::to_string(timeStep) + "-" + std::to_string(i);
      std::string filename = _outputFileWithoutExtension + "-" + std::to_string(timeStep);

//        #pragma omp critical
      {
        _metaFile << "  include \"" << filename << _FileExtension << "\"" << std::endl;
      }

      writeFile(_directory + "/" + filename, p);
    }

    _metaFile << "end dataset" << std::endl << std::endl;
  }
}
*/


//void visualisation::output::PeanoWriter::writeFile(const std::string& outputFileWithoutExtention, const std::vector<PeanoPatch*>& patches) {
void visualisation::output::PeanoWriter::writeFile(const visualisation::data::DataSet& data, const std::string& selector) {
	// @todo Hier faengt es an
/*
  std::ofstream  file( outputFileWithoutExtention + _FileExtension );
  logInfo( "writeFile(...)", "write to file " << outputFileWithoutExtention << _FileExtension );

  file << _Header << std::endl;

  file << "dimensions " << patches[0]->dimensions << std::endl;
  assertion( patches[0]->dimensions==2 or patches[0]->dimensions==3 );

  file << "patch-size " ;
  for (int i=0; i<patches[0]->dimensions; i++) {
	file << patches[0]->resolution[i] << " ";
    assertion( patches[0]->resolution[i]>=1 );
  }
  file << std::endl << std::endl;

  for (auto it : patches[0]->patchData) {
	PeanoPatchData& data = it.second;
    assertion( data.structure->unknowns>=1 );
	file << "begin " << (data.structure->type == Cell_Values?"cell":"vertex") << "-values";
	file << " \"" << data.structure->name << "\"\n";
	file << "  number-of-unknowns " << data.structure->unknowns << "\n";
	file << "end " << (data.structure->type == Cell_Values?"cell":"vertex") << "-values\n\n";
  }

  file << std::endl << std::endl;

  for (auto p: patches) {
	file << "begin patch" << std::endl;

	file << "  offset ";
    for (int d=0; d<p->dimensions; d++) {
	  file << p->offsets[d] << " ";
    }
    file << std::endl;

	file << "  size ";
    for (int d=0; d<p->dimensions; d++) {
	  file << p->sizes[d] << " ";
	  assertion( p->sizes[d]>=1 );
    }
    file << std::endl;

	for (auto it : p->patchData) {
  	  PeanoPatchData& data = it.second;
	  file << "  begin " << (data.structure->type == Cell_Values?"cell":"vertex") << "-values";
	  file << " \"" << data.structure->name << "\"\n";
	  file << "   ";
      for(int i=0; i<data.structure->totalValues; i++) {
		double value = data.values[i];
		if(value < 1e-12 && value > 0) {
          file << " 0";
		}
		else {
          file << " " << value;
		}
      }
      file << "\n  end " << (data.structure->type == Cell_Values?"cell":"vertex") << "-values\n";
	}

    file << "end patch" << std::endl << std::endl;
  }

  file.close();
*/
}
