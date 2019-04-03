#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

#include "../config.h"


#include "output/VTUWriter.h"
#include "output/PeanoWriter.h"
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


void inspect( std::string filename ) {
  static tarch::logging::Log _log( "/" );

  visualisation::input::PeanoTextPatchFileReader reader(filename);
  reader.parse();

  visualisation::data::DataSet data = reader.getData();

  for (auto p: data.getVariables()) {
	logInfo( "inspect", "variable " << p.name );
	logInfo( "inspect", "\ttype\t\t\t" << (p.type==visualisation::data::PeanoDataType::Cell_Values ? "cell values" : "vertex values" ));
	logInfo( "inspect", "\tdofs per axis\t\t" << p.dofsPerAxis );
	logInfo( "inspect", "\tunknowns per dof\t" << p.unknowns );
  }
}


void convertFile( std::string filename, const std::string& outputDirectory, const std::string& selector, const std::string& format ) {
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

  visualisation::output::PeanoWriter::Writer* writer = nullptr;
  if (format==OutputFormatPeano) {
    writer = new visualisation::output::PeanoWriter( outputDirectory, truncatedFile );
  }
  else if (format==OutputFormatVTU) {
    writer = new visualisation::output::VTUWriter( outputDirectory, truncatedFile );
  }
  else {
    logError( "convertFile(...)", "unknown output format " << format );
  }

  visualisation::data::DataSet data = reader.getData();
  if (data.hasVariable(selector)) {
	visualisation::data::Variable variable = data.getVariable(selector);
    writer->writeFile( variable, data.getData(variable) );
  }
  else {
    logError( "convertFile(...)", "data file does not contain any data set with name " << selector );
  }

  data.free();
  delete writer;
}


void convertTimeSeries( std::string filename, std::string outputDirectory, const std::string& selector, const std::string& format ) {
/*
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
*/
}


void extractFineGrid( std::string filename, std::string outputDirectory ) {
/*
  static tarch::logging::Log _log( "/" );
  logInfo( "extractFineGrid(...)", "read file " << filename );

  createDirectory( outputDirectory );

  visualisation::input::MetaFileReader* reader = new visualisation::input::PeanoTextMetaFileReader(filename);
  reader->parse();
//  visualisation::filter::Intersections::removeIntersectedCoarseGrainPatches(reader);

  std::string                        outFileNamePrefix  = filename.erase(filename.find_last_of(".") );
  visualisation::output::PeanoWriter writer( outputDirectory, outFileNamePrefix );
  writer.writeFile( *(reader->getFile()) );
*/
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
      if (mode.compare("inspect")==0 and argc>=3) {
    	for (int i=2; i<argc; i++) {
    	  inspect( argv[i] );
    	}
      }
      else if (mode.compare("convert-file")==0 and argc>=5) {
        std::string selector        = argv[ argc-3 ];
        std::string outputDirectory = argv[ argc-2 ];
        std::string format          = argv[ argc-1 ];
    	for (int i=2; i<argc-3; i++) {
    	  convertFile( argv[i], outputDirectory, selector, format );
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
      std::cerr << "\t./executable inspect            InputFile1 [InputFile2 ...] " << std::endl;
      std::cerr << "\t./executable convert-file       InputFile1 [InputFile2 ...] Selector OutputFolder Format" << std::endl;
      std::cerr << "\t./executable convert-set        InputFile1 [InputFile2 ...] Selector OutputFolder Format" << std::endl;
      std::cerr << "\t./executable extract-fine-grid  InputFile1 [InputFile2 ...] OutputFolder" << std::endl;
      std::cerr << std::endl << std::endl;
      std::cerr << "Variants:" << std::endl;
      std::cerr << "\tinspect              Inspect which data sets are stored within file (cmp Selector below)" << std::endl;
      std::cerr << "\tconvert-file         Convert a single file" << std::endl;
      std::cerr << "\tconvert-set          Convert whole set of data comprising multiple time steps, resolutions, ..." << std::endl;
      std::cerr << "\textract-fine-grid    Extract the fine grid per snapshot and store in a new patch file comprising both the raw data and the fine grid. The fine grid's selector is fine-grid" << std::endl;
      std::cerr << std::endl << std::endl;
      std::cerr << "Options:" << std::endl;
      std::cerr << "\tFormat               Either " << OutputFormatPeano << " or " << OutputFormatVTU << std::endl;
      std::cerr << "\tSelector             Use inspect to see which data sets are stored within your patch file, i.e. which you can select" << std::endl;
      return -1;
    }
    else return 0;
}
