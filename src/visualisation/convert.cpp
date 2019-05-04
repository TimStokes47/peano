#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

#include "../config.h"


#include "output/VTUWriter.h"
#include "output/PeanoWriter.h"

#include "input/PeanoTextPatchFileReader.h"

#include "filter/Copy.h"
#include "filter/Intersection.h"
#include "filter/SelectValue.h"
#include "filter/SeparateResolutions.h"


#include "tarch/Assertions.h"


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


std::string getFileNameWithoutExtensionAndWithoutPatch( std::string& file ) {
  static tarch::logging::Log _log( "/" );

  std::string result = file;
  logDebug( "getFileNameWithoutExtensionAndWithoutPatch(std::string)", "in=" << result );
  if ( result.find_last_of(".")!=std::string::npos ) {
    result = result.erase(result.find_last_of(".") );
  }
  if ( result.find_last_of("/")!=std::string::npos ) {
    result = result.erase(0,result.find_last_of("/")+1 );
  }
  logDebug( "getFileNameWithoutExtensionAndWithoutPatch(std::string)", "out=" << result );
  return result;
}


void convertFile( const std::string& outputDirectory, const std::string& truncatedFile, const std::string& format, visualisation::data::Variable variable, visualisation::data::DataSet& data ) {
  static tarch::logging::Log _log( "/" );

  visualisation::output::PeanoWriter::Writer* writer = nullptr;

  const std::string fileWithCorrectSelector = truncatedFile + "-" + variable.name;
  if (format==OutputFormatPeano) {
    writer = new visualisation::output::PeanoWriter( outputDirectory, fileWithCorrectSelector );
  }
  else if (format==OutputFormatVTU) {
    writer = new visualisation::output::VTUWriter( outputDirectory, fileWithCorrectSelector );
  }
  else {
    logError( "convertFile(...)", "unknown output format " << format );
    return;
  }

  writer->writeFile( variable, data.getData(variable) );

  delete writer;
}


void convertFile( std::string filename, const std::string& outputDirectory, const std::string& selector, const std::string& format ) {
  static tarch::logging::Log _log( "/" );

  createDirectory( outputDirectory );

  visualisation::input::PeanoTextPatchFileReader reader(filename);
  reader.parse();
  visualisation::data::DataSet data = reader.getData();

  std::string truncatedFile = getFileNameWithoutExtensionAndWithoutPatch( filename );
  logDebug( "convertFile(...)", "read " << filename << " and write into " << truncatedFile << " in directory " << outputDirectory );

  if (selector=="all") {
    for (auto variable: data.getVariables()) {
      convertFile(
        outputDirectory, truncatedFile, format,
        variable, data
      );
    }
  }
  else if (data.hasVariable(selector)) {
	visualisation::data::Variable variable = data.getVariable(selector);
	convertFile(
      outputDirectory, truncatedFile, format,
	  variable, data
	);
  }
  else {
    logError( "convertFile(...)", "data file does not contain any data set with name " << selector );
  }

  data.free();
}


enum class Filter {
  Copy,
  ExtractFineGrid,
  SelectValue,
  SeparateResolutions
};


std::string toString( Filter filter ) {
  switch (filter) {
    case Filter::Copy:
      return "copy";
    case Filter::ExtractFineGrid:
      return "extract-fine-grid";
    case Filter::SelectValue:
      return "select-value";
    case Filter::SeparateResolutions:
      return "separate-resolutions";
  }
  return "undef";
}


void applyFilter( std::string filename, std::string outputDirectory, std::string selector, std::string filterName, std::string targetSelector) {
  static tarch::logging::Log _log( "/" );

  createDirectory( outputDirectory );

  std::string truncatedFile = getFileNameWithoutExtensionAndWithoutPatch( filename );
  logInfo( "convertFile(...)", "writing file " << truncatedFile );

  visualisation::input::PeanoTextPatchFileReader reader(filename);
  reader.parse();
  visualisation::data::DataSet data = reader.getData();

  visualisation::output::PeanoWriter writer( outputDirectory, truncatedFile );
  if (data.hasVariable(selector)) {
	visualisation::data::Variable variable = data.getVariable(selector);

    if (data.hasVariable(targetSelector)) {
      logError( "convertFile(...)", "file already contains data set with name " << targetSelector );
    }
    else {
      visualisation::filter::Filter* filter = nullptr;
      if (filterName==toString(Filter::Copy)) {
        filter = new visualisation::filter::Copy();
      }
      else if (filterName==toString(Filter::ExtractFineGrid)) {
        filter = new visualisation::filter::Intersection( visualisation::filter::Intersection::Strategy::KeepFinerGrid );
      }
      else if (filterName==toString(Filter::SeparateResolutions)) {
        filter = new visualisation::filter::SeparateResolutions();
      }
      else if (filterName.compare(toString(Filter::SelectValue))>0) {
    	std::string rangeToken = filterName.substr( filterName.find(':')+1 );
    	std::string fromToken  = rangeToken.substr( 0, rangeToken.find(':') );
    	std::string toToken    = rangeToken.substr( rangeToken.find(':')+1 );

    	logDebug( "convertFile(...)", "use range token " << rangeToken << " split into " << fromToken << " and " << toToken );
        filter = new visualisation::filter::SelectValue( std::stod(fromToken), std::stod(toToken) );
      }
      else {
        logError( "convertFile(...)", "unknown filter " << filterName );
      }

      if (filter!=nullptr) {
    	filter->apply( data, variable, targetSelector );
    	assertion1( data.hasVariable(targetSelector), targetSelector );
    	visualisation::data::Variable targetVariable = data.getVariable(targetSelector);
        writer.writeFile( data );
      }
    }
  }
  else {
    logError( "convertFile(...)", "data file does not contain any data set with name " << selector );
  }

  data.free();
}


int main(int argc, char* argv[]) {
    std::cout << "Peano block file to vtk converter" << std::endl;
    std::cout << "(C) 2018/2019 Dan Tuthill-Jones, Tobias Weinzierl" << std::endl << std::endl;
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
      else if (mode.compare("apply-filter")==0 and argc>=7) {
        std::string selector        = argv[ argc-4 ];
        std::string outputDirectory = argv[ argc-3 ];
        std::string filter          = argv[ argc-2 ];
        std::string targetSelector  = argv[ argc-1 ];
    	for (int i=2; i<argc-4; i++) {
    	  applyFilter( argv[i], outputDirectory, selector, filter, targetSelector);
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
      std::cerr << "\t./executable inspect       InputFile1 [InputFile2 ...] " << std::endl;
      std::cerr << "\t./executable convert-file  InputFile1 [InputFile2 ...] Selector OutputFolder Format" << std::endl;
      std::cerr << "\t./executable apply-filter  InputFile1 [InputFile2 ...] Selector OutputFolder Filter TargetSelector" << std::endl;

      std::cerr << std::endl << std::endl;
      std::cerr << "Variants:" << std::endl;
      std::cerr << "\tinspect              Inspect which data sets are stored within file (cmp Selector below)" << std::endl;
      std::cerr << "\tconvert-file         Convert a single file" << std::endl;
      std::cerr << "\tapply-filter         Take a data set from the input file (identified by Selector), apply a filter to" << std::endl;
      std::cerr << "\t                     this set and add the result to the file with a given TargetSelector name" << std::endl;

      std::cerr << std::endl << std::endl;
      std::cerr << "Options:" << std::endl;
      std::cerr << "\tFormat               Either " << OutputFormatPeano << " or " << OutputFormatVTU << std::endl;
      std::cerr << "\tSelector             Use inspect to see which data sets are stored within your patch file, i.e. which you can select. Pass all to convert all data sets (works only with convert-file)" << std::endl;

      std::cerr << std::endl << std::endl;
      std::cerr << "Filters:" << std::endl;
      std::cerr << "\t" << toString(Filter::Copy) << "                 Create 1:1 copy of dataset with different name (for debugging)" << std::endl;
      std::cerr << "\t" << toString(Filter::ExtractFineGrid) << "    Extract fine grid" << std::endl;
      std::cerr << "\t" << toString(Filter::SelectValue) << "         Extract grid patches that hold values of a certain range. Append :from:to to filter to specify range" << std::endl;
      std::cerr << "\t" << toString(Filter::SeparateResolutions) << " Splits up the tree mesh into its resolutions" << std::endl;

      std::cerr << std::endl << std::endl;
      std::cerr << "Output directory plus filename can be the same as the input file. In this case, the original file is overwritten/augmented with new data" << std::endl;
      return -1;
    }
    else return 0;
}
