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
    visualisation::output::PeanoWriter::writeFile( outFile, reader.getPatches() );
  }
  else if (format==OutputFormatVTU) {
    visualisation::output::VTUWriter::writeFile( outFile, reader.getPatches() );
  }
  else {
    logError( "convertFile(...)", "unknown output format " << format );
  }
}


void convertTimeSeries( std::string filename, std::string outputDirectory, const std::string& selector, const std::string& format ) {
  static tarch::logging::Log _log( "/" );
  logInfo( "convertFile(...)", "read file " << filename );

  createDirectory( outputDirectory );

  visualisation::input::MetaFileReader* reader = new visualisation::input::PeanoTextMetaFileReader(filename);
  reader->parse();

  std::string outFileNamePrefix  = filename.erase(filename.find_last_of(".") );
  visualisation::output::Writer* writer = nullptr;
  if (format==OutputFormatPeano) {
    writer = new visualisation::output::PeanoWriter( outputDirectory, outFileNamePrefix );
  }
  else if (format==OutputFormatVTU) {
	writer = new visualisation::output::VTUWriter( outputDirectory, outFileNamePrefix );
  }
  else {
    logError( "convertFile(...)", "unknown output format " << format );
  }

  writer->writeFile( *(reader->getFile()), selector );
  delete writer;
}


void extractFineGrid( std::string filename, std::string outputDirectory ) {
  static tarch::logging::Log _log( "/" );
  logInfo( "extractFineGrid(...)", "read file " << filename );

  createDirectory( outputDirectory );

  visualisation::input::MetaFileReader* reader = new visualisation::input::PeanoTextMetaFileReader(filename);
  reader->parse();

  std::string                        outFileNamePrefix  = filename.erase(filename.find_last_of(".") );
  visualisation::output::PeanoWriter writer( outputDirectory, outFileNamePrefix );
  writer.writeFile( *(reader->getFile()) );
}


int main(int argc, char* argv[]) {
    std::cout << "Peano block file to vtk converter" << std::endl;
    std::cout << "(C) 2018 Dan Tuthill-Jones, Tobias Weinzierl" << std::endl << std::endl;
    bool validParams = true;

    if(argc < 3) {
      std::cerr << "too few arguments" << std::endl;
      validParams = false;
    }
    else {
      std::string mode            = argv[1];
      if (mode.compare("convert-file")==0 and argc>=5) {
        std::string outputDirectory = argv[ argc-2 ];
        std::string format          = argv[ argc-1 ];
    	for (int i=2; i<argc-2; i++) {
    	  convertFile( argv[i], outputDirectory, format );
    	}
      }
      else if (mode.compare("convert-time-series")==0 and argc>=6) {
        std::string selector        = argv[ argc-3 ];
        std::string outputDirectory = argv[ argc-2 ];
        std::string format          = argv[ argc-1 ];
    	for (int i=2; i<argc-3; i++) {
    	  convertTimeSeries( argv[i], outputDirectory, selector, format);
    	}
      }
      else if (mode.compare("extract-fine-grid")==0 and argc>=4) {
        std::string outputDirectory = argv[ argc-1 ];
    	for (int i=2; i<argc-1; i++) {
    	  extractFineGrid( argv[i], outputDirectory );
    	}
      }
      else {
        std::cerr << "unknown command or invalid number of parameters for particular command" << std::endl;
        validParams = false;
      }
    }

    if (!validParams) {
      std::cerr << std::endl << std::endl;
      std::cerr << "Usage:";
      std::cerr << "\t./executable convert-file       InputFile1 [InputFile2 ...] OutputFolder Format" << std::endl;
      std::cerr << "\t./executable convert-set        InputFile1 [InputFile2 ...] Selector OutputFolder Format" << std::endl;
      std::cerr << "\t./executable extract-fine-grid  InputFile1 [InputFile2 ...] OutputFolder" << std::endl;
      std::cerr << std::endl << std::endl;
      std::cerr << "Variants:" << std::endl;
      std::cerr << "\tconvert-file         Convert a single file" << std::endl;
      std::cerr << "\tconvert-set          Convert whole set of data comprising multiple time steps, resolutions, ..." << std::endl;
      std::cerr << "\textract-fine-grid    Extract the fine grid per snapshot and store in a new patch file comprising both the raw data and the fine grid. The fine grid's selector is fine-grid" << std::endl;
      std::cerr << std::endl << std::endl;
      std::cerr << "Options:" << std::endl;
      std::cerr << "\tFormat               Either " << OutputFormatPeano << " or " << OutputFormatVTU << std::endl;
      std::cerr << "\tSelector             Either " << PeanoMetaFile::RawData << " or the identifier of a new data representations you created before" << std::endl;
      return -1;
    }
    else return 0;
}
