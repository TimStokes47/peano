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


#include "PeanoConverter.h"
#include "PeanoDataSet.h"
#include "PeanoMetaFile.h"
#include "input/PeanoTextPatchFileReader.h"
#include "input/PeanoTextMetaFileReader.h"


#include <experimental/filesystem> // or #include <filesystem>


void convertFile( std::string filename, const std::string& outputDirectory ) {
  static tarch::logging::Log _log( "/" );
  std::string truncatedFile = filename;
  if ( truncatedFile.find_last_of(".")!=std::string::npos ) {
    truncatedFile.erase(truncatedFile.find_last_of(".") );
  }
  std::string outFile = outputDirectory + "/" + truncatedFile;
  logInfo( "convertFile(...)", "writing file " << outFile );

  visualisation::input::PeanoTextPatchFileReader reader;
  reader.parse( filename );
  PeanoConverter::combineAndWriteToFile( reader.patches, outFile );
}


void convertTimeSeries( std::string filename, std::string outputDirectory ) {
  static tarch::logging::Log _log( "/" );
  logInfo( "convertFile(...)", "read file " << filename );

  visualisation::input::PeanoTextMetaFileReader reader;
  reader.parse(filename);

  std::string outFileNamePrefix  = outputDirectory + "/" + filename.erase(filename.find_last_of(".") );
  std::string outFileName        = outFileNamePrefix + "-full-resolution.pvd";
  std::string dataFileNamePrefix = outFileNamePrefix + "-full-resolution";
  if (
    !std::experimental::filesystem::is_directory(dataFileNamePrefix)
    ||
    !std::experimental::filesystem::exists(dataFileNamePrefix)
  ) {
	try {
      std::experimental::filesystem::create_directory(dataFileNamePrefix);
      logInfo( "convertFile(...)", "created directory " << dataFileNamePrefix );
	}
	catch (std::exception exc) {
      logError( "convertFile(...)", "failed to create directory " << dataFileNamePrefix );
      logError( "convertFile(...)", "error message: " << exc.what() );
	}
  }
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
    std::vector<  visualisation::input::PeanoTextPatchFileReader*>* readers = timeStep->createReadersForRawData();

    logInfo( "convertFile(...)", "time step consists of " << readers->size() << " individual data sets" );

    // @c++ parallel
    #pragma omp parallel for
    for( int i=0; i<readers->size(); i++) {
      auto p = (*readers)[i];
      std::string outFile = dataFileNamePrefix + "-" + std::to_string(i) + "-" + std::to_string(timeStepCounter);

      #pragma omp critical
      logInfo( "convertFile(...)", "writing file " << outFile );

      if (!p->patches.empty()) {
        std::string filename =  PeanoConverter::combineAndWriteToFile( p->patches, outFile );

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

    if(argc < 4) {
      std::cerr << "too few arguments" << std::endl;
      validParams = false;
    }
    else {
      std::string mode = argv[1];
      if (mode.compare("convert-file")==0) {
    	std::string outputDirectory = argv[ argc-1 ];
    	std::cout << "write into directory " << outputDirectory << std::endl;
    	for (int i=2; i<argc-1; i++) {
    	  convertFile( argv[i], outputDirectory );
    	}
      }
      else if (mode.compare("convert-time-series")==0) {
    	std::string outputDirectory = argv[ argc-1 ];
    	std::cout << "write into directory " << outputDirectory << std::endl;
    	for (int i=2; i<argc-1; i++) {
    	  convertTimeSeries( argv[i], outputDirectory );
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
      std::cerr << "\t./executable convert-file InputFile1 [InputFile2 ...] OutputFolder" << std::endl;
      std::cerr << "\t./executable convert-time-series InputFile1 [InputFile2 ...] OutputFolder" << std::endl;
      std::cerr << std::endl << std::endl;
      std::cerr << "Only convert will yield actual VTK files. All other operations create new " << std::endl
      		<< "internal data representations that then can be converted. " << std::endl << std::endl
                << "Please ensure that the OutputFolder exists prior to program invocation." << std::endl << std::endl
                << "convert-file only should be used for individual snapshot files, but not for files referring to/including other files" << std::endl << std::endl
                << "Please invoke the tool from the directory the actual input files are stored in, i.e. do not refer to files stored relative" << std::endl
                << "to the current directory." << std::endl << std::endl;
      return -1;
    }
    else return 0;
}
