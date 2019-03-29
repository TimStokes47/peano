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
#include "output/PeanoWriter.h"
#include "PeanoMetaFile.h"
#include "input/PeanoTextPatchFileReader.h"
#include "input/PeanoTextMetaFileReader.h"


#include <experimental/filesystem> // or #include <filesystem>


const std::string OutputFormatPeano = "peano";
const std::string OutputFormatVTU   = "vtu";


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


void convertFile( std::string filename, const std::string& outputDirectory, const std::string& format ) {
  static tarch::logging::Log _log( "/" );

  createDirectory( outputDirectory );

  std::string truncatedFile = filename;
  if ( truncatedFile.find_last_of(".")!=std::string::npos ) {
    truncatedFile.erase(truncatedFile.find_last_of(".") );
  }
  std::string outFile = outputDirectory + "/" + truncatedFile;
  logInfo( "convertFile(...)", "writing file " << outFile );

  visualisation::input::PeanoTextPatchFileReader reader(filename);
  reader.parse();

  if (format==OutputFormatPeano) {
	  // @todo
//    visualisation::output::PeanoWriter::writeFile( reader.patches, outFile );
  }
  else if (format==OutputFormatVTU) {
    visualisation::output::VTUWriter::writeFile( outFile, reader.getPatches() );
  }
  else {
    logError( "convertFile(...)", "unknown output format " << format );
  }
}


void convertTimeSeries( std::string filename, std::string outputDirectory, const std::string& format ) {
  static tarch::logging::Log _log( "/" );
  logInfo( "convertFile(...)", "read file " << filename );

  createDirectory( outputDirectory );

  visualisation::input::MetaFileReader* reader = new visualisation::input::PeanoTextMetaFileReader(filename);
  reader->parse();

  std::string outFileNamePrefix  = filename.erase(filename.find_last_of(".") );
  visualisation::output::VTUWriter* writer = new visualisation::output::VTUWriter( outputDirectory, outFileNamePrefix );
  if (format==OutputFormatPeano) {
//    visualisation::output::PeanoWriter::writeFile( reader.patches, outFile );
  }
  else if (format==OutputFormatVTU) {
//    visualisation::output::VTUWriter::writeFile( reader.patches, outFile );
  }
  else {
    logError( "convertFile(...)", "unknown output format " << format );
  }

  writer->writeFile( *(reader->getFile()) );
  delete writer;
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
      std::string mode            = argv[1];
      std::string outputDirectory = argv[ argc-2 ];
      std::string format          = argv[ argc-1 ];
      if (mode.compare("convert-file")==0) {
    	for (int i=2; i<argc-2; i++) {
    	  convertFile( argv[i], outputDirectory, format );
    	}
      }
      else if (mode.compare("convert-time-series")==0) {
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
