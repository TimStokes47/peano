#include <iostream>
#include <vector>
#include <string>
#include <limits>

#include "PeanoReader.h"
#include "PeanoConverter.h"
#include "PeanoMetaFile.h"
#include "PeanoDataSet.h"

#include <vtkSmartPointer.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkMergeCells.h>


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
      if (mode.compare("convert")==0) {
    	std::string outputDirectory = argv[ argc-1 ];
    	std::cout << "write into directory " << outputDirectory << std::endl;
    	for (int i=2; i<argc-1; i++) {
          std::string inputFile       = argv[i];
          PeanoReader reader( inputFile );

          vtkSmartPointer<vtkUnstructuredGrid> outputGrid = PeanoConverter::combine( reader.patches );

          vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
          std::string outFile = outputDirectory + "/" + inputFile.erase(inputFile.find_last_of(".") ) + "." + writer->GetDefaultFileExtension();
          std::cout << "writing file " << outFile << std::endl;
          writer->SetFileName(outFile.c_str());
          writer->AddInputDataObject( outputGrid );
          writer->Write();
    	}
      }
      else {
        std::cerr << "unknown command. First argument has to be convert" << std::endl;
        validParams = false;
      }
    }
/*
    if(argc == 6) {
        std::string command = argv[1];
        if(command.compare("subsample") == 0) {
            validParams = true;
            std::string input = argv[2];

        	int xSize = std::stoi(argv[3]);
        	int ySize = std::stoi(argv[4]);
        	int zSize = std::stoi(argv[5]);

            std::cout << "Reading initial file " << input << "...\n";
            PeanoMetaFile metaFile = PeanoMetaFile(input);

            #pragma omp parallel for
            for(int i = 0; i < metaFile.numberOfDataSets(); i++) {
                PeanoDataSet* dataset = metaFile.getDataSet(i);
                PeanoPatch* sample = dataset->createSubSample(xSize, ySize, zSize, true);
                delete sample;
            }

            //save the metadatafile
            metaFile.save();
            return 0;
        }
    } else if(argc == 4) {
        std::string command = argv[1];
        if(command.compare("convert") == 0) {
            validParams = true;
            std::string input = argv[2];
            std::string output = argv[3];

            std::cout << "Reading initial file " << input << "...\n";
            PeanoMetaFile metaFile = PeanoMetaFile(input);

            #pragma omp parallel for
            for(int i = 0; i < metaFile.numberOfDataSets(); i++) {
                PeanoDataSet* dataset = metaFile.getDataSet(i);
                std::vector<PeanoReader*>* readers = dataset->createReadersFull();
                vtkSmartPointer<vtkUnstructuredGrid> vtkGrid = PeanoConverter::combineImageData(readers);

                //save the patch as an XML file
                vtkSmartPointer<vtkXMLUnstructuredGridWriter> writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();

                std::string outFile = output + "/" + dataset->getSimpleName() + ".vtk";

                std::cout << "Writing to file " << outFile << "\n";
                writer->SetFileName(outFile.c_str());
                writer->AddInputDataObject(vtkGrid.GetPointer());
                writer->Write();
            }

            return 0;
        }
    }
*/

    if (!validParams) {
      std::cerr << std::endl << std::endl;
      std::cerr << "Usage:";
      std::cerr << "\t./executable convert InputFile1 [InputFile2 ...] OutputFolder\n";
//      std::cerr << "\t./PeanoStandalone subsample INPUT_FILE X_SIZE Y_SIZE Z_SIZE\n";
 //     std::cerr << "or:\n";
      std::cerr << std::endl << std::endl;
      std::cerr << "Only convert will yield actual VTK files. All other operations create new " << std::endl
      		  << "internal data representations that then can be converted. Please ensure that" << std::endl
                << "the OutputFolder exists prior to program invocation." << std::endl;
      return -1;
    }
    else return 0;
}
