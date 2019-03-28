#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

#include "../config.h"
#ifdef UseVTK
#include <vtkSmartPointer.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkMergeCells.h>
#endif


#include "output/VTUWriter.h"
#include "PeanoDataSet.h"
#include "PeanoMetaFile.h"
#include "input/PeanoTextPatchFileReader.h"
#include "input/PeanoTextMetaFileReader.h"


#include <experimental/filesystem> // or #include <filesystem>


const std::string OutputFormatPeano = "peano";
const std::string OutputFormatVTU   = "vtu";


void convertFile( std::string filename, const std::string& outputDirectory, const std::string& format ) {
  static tarch::logging::Log _log( "/" );
  std::string truncatedFile = filename;
  if ( truncatedFile.find_last_of(".")!=std::string::npos ) {
    truncatedFile.erase(truncatedFile.find_last_of(".") );
  }
  std::string outFile = outputDirectory + "/" + truncatedFile;
  logInfo( "convertFile(...)", "writing file " << outFile );

  visualisation::input::PeanoTextPatchFileReader reader;
  reader.parse( filename );
  visualisation::output::VTUWriter::writeFile( reader.patches, outFile );
}


void createDirectory( const std::string& directory ) {
  static tarch::logging::Log _log( "/" );
  if (
    !std::experimental::filesystem::is_directory(directory)
    ||
    !std::experimental::filesystem::exists(directory)
  ) {
	try {
      std::experimental::filesystem::create_directory(directory);
      logInfo( "createDirectory(...)", "created directory " << directory );
	}
	catch (std::exception exc) {
      logError( "createDirectory(...)", "failed to create directory " << directory );
      logError( "createDirectory(...)", "error message: " << exc.what() );
      exit(-1);
	}
  }
}


void convertTimeSeries( std::string filename, std::string outputDirectory, const std::string& format ) {
  static tarch::logging::Log _log( "/" );
  logInfo( "convertFile(...)", "read file " << filename );

  visualisation::input::PeanoTextMetaFileReader reader;
  reader.parse(filename);

  std::string outFileNamePrefix  = outputDirectory + "/" + filename.erase(filename.find_last_of(".") );
  std::string dataFileNamePrefix = outFileNamePrefix + "-full-resolution";
  std::string outFileName        = outFileNamePrefix + "-full-resolution.pvd";

  createDirectory( outputDirectory );
  createDirectory( dataFileNamePrefix );

  dataFileNamePrefix += "/data";

  std::ofstream pvdFile(outFileName);
  pvdFile << "<?xml version=\"1.0\"?>" << std::endl
		  << "<VTKFile type=\"Collection\" version=\"0.1\" >" << std::endl
		  << "<Collection>" << std::endl;

  int timeStepCounter = 0;
  std::vector<PeanoDataSet*>* dataSets = reader.file->getDataSets();
  logInfo( "convertFile(...)", "read " << dataSets->size() << " data sets (time steps)" );
  for( auto timeStep: *dataSets ) {
    // @todo There should indeed by an abstract PeanoReader superclass
    std::vector<  visualisation::input::PeanoTextPatchFileReader*>* readers = timeStep->createReaders( PeanoDataSet::RawData );

    logInfo( "convertFile(...)", "time step consists of " << readers->size() << " individual data sets" );

    #pragma omp parallel for
    for( int i=0; i<readers->size(); i++) {
      auto p = (*readers)[i];
      std::string outFile = dataFileNamePrefix + "-" + std::to_string(i) + "-" + std::to_string(timeStepCounter);

      #pragma omp critical
      logInfo( "convertFile(...)", "writing file " << outFile );

      if (!p->patches.empty()) {
        std::string filename =  visualisation::output::VTUWriter::writeFile( p->patches, outFile );

        // Strip output directory
        std::string toRemove = outputDirectory + "/";
        int pos = filename.find(toRemove);
        filename.erase(pos, toRemove.length());

        #pragma omp critical
        {
        pvdFile << "<DataSet timestep=\"" << timeStepCounter << "\" group=\"\" part=\"" << i << "\" "
      	        << " file=\"" << filename << "\" "
		        << "/>" << std::endl;
        }
      }
    }

    timeStepCounter++;
  }

  pvdFile << "</Collection>" << std::endl
		  << "</VTKFile>" << std::endl;
  pvdFile.close();
}


int main(int argc, char* argv[]) {
    std::cout << "Peano block file to vtk converter" << std::endl;
    std::cout << "(C) 2018 Dan Tuthill-Jones, Tobias Weinzierl" << std::endl << std::endl;
    bool validParams = true;

    if(argc < 5) {
      std::cerr << "too few arguments" << std::endl;
      validParams = false;
    }
    else {
      std::string mode   = argv[1];
      std::string format = argv[argc-1];
      if (mode.compare("convert-file")==0) {
    	std::string outputDirectory = argv[ argc-1 ];
    	std::cout << "write into directory " << outputDirectory << std::endl;
    	for (int i=2; i<argc-2; i++) {
    	  convertFile( argv[i], outputDirectory, format );
    	}
      }
      else if (mode.compare("convert-time-series")==0) {
    	std::string outputDirectory = argv[ argc-1 ];
    	std::cout << "write into directory " << outputDirectory << std::endl;
    	for (int i=2; i<argc-2; i++) {
    	  convertTimeSeries( argv[i], outputDirectory, format);
    	}
      }
      else {
        std::cerr << "unknown command. First argument has to be convert" << std::endl;
        validParams = false;
      }
    }

    if (!validParams) {
      std::cerr << std::endl << std::endl;
      std::cerr << "Usage:";
      std::cerr << "\t./executable convert-file InputFile1 [InputFile2 ...] OutputFolder Format" << std::endl;
      std::cerr << "\t./executable convert-set  InputFile1 [InputFile2 ...] OutputFolder Format" << std::endl;
      std::cerr << std::endl << std::endl;
      std::cerr << "Variants:" << std::endl;
      std::cerr << "\tconvert-file         Convert a single file" << std::endl;
      std::cerr << "\tconvert-set          Convert whole set of data comprising multiple time steps, resolutions, ..." << std::endl;
      std::cerr << std::endl << std::endl;
      std::cerr << "Options:" << std::endl;
      std::cerr << "\tFormat               Either " << OutputFormatPeano << " or " << OutputFormatVTU << std::endl;
      return -1;
    }
    else return 0;
}
