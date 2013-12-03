package org.peano.pdt.generators;

import org.peano.pdt.analysis.DepthFirstAdapter;
import org.peano.pdt.TranslationTable;

import org.peano.pdt.node.*;


public class DaStGenGenerator extends DepthFirstAdapter {
  private org.peano.pdt.generators.DirectoryGenerator _directoryGenerator;
  private org.peano.pdt.TranslationTable              _translationTable;

  final private boolean                                _QuietDaStGen = true;

  public DaStGenGenerator(
    org.peano.pdt.generators.DirectoryGenerator directoryGenerator,
    org.peano.pdt.TranslationTable              translationTable
  ) {
    _directoryGenerator = directoryGenerator;
    _translationTable   = translationTable;
  }
  

  private String[] getDaStGenArguments( String specificationFile ) {
    if (_QuietDaStGen) {
      String[] args = { "--plugin", "PeanoSnippetGenerator", "--naming",
          "PeanoNameTranslator", 
          "--include",
          _directoryGenerator.getProjectDirectoryAbsolute(), 
          "--quiet", "--pragmas", "--align", "--inline",
          specificationFile, 
          _directoryGenerator.getRecordsDirectory()
      };
            
      return args;
    }
    else {
      String[] args = { "--plugin", "PeanoSnippetGenerator", "--naming",
          "PeanoNameTranslator", 
          "--include",
          _directoryGenerator.getProjectDirectoryAbsolute(), 
          "--pragmas", "--align", "--inline",
          specificationFile, 
          _directoryGenerator.getRecordsDirectory()
      };

      return args;
    }
  }

  
  private void printArguments(String[] args) {
    System.err.print("used DaStGen arguments: ");
    for (String p : args) {
      System.err.print(p + " ");
    }
    System.err.println("");
  }

  
  public void inADescription(ADescription node) {
    String fileName                    = _directoryGenerator.getDaStGenDirectory()  
                                       + "/" 
                                       + TranslationTable.getRepositoryStateUnqualifiedName() 
                                       + ".def";
    String repositoryStateTemplateName = "DaStGenRepositoryState.template";

    _translationTable.convertTemplateFile(
      repositoryStateTemplateName,
      fileName, 
      true, true
    );

    printArguments(getDaStGenArguments(fileName));

    de.tum.in.dast.DaStGen.main( getDaStGenArguments(fileName) );
  }

  
  public void inAVertex(AVertex node) {
    String vertexFileName     = _directoryGenerator.getDaStGenDirectory()
                              + "/"
                              + TranslationTable.getVertexUnqualifiedName() 
                              + ".def";
    String vertexTemplateName = "DaStGenVertex.template";

    _translationTable.convertTemplateFile(
      vertexTemplateName,
      vertexFileName, 
      true, true
    );

    printArguments(getDaStGenArguments(vertexFileName));

    de.tum.in.dast.DaStGen.main( getDaStGenArguments(vertexFileName) );
  }

  
  public void inACell(ACell node) {
    String cellFileName     = _directoryGenerator.getDaStGenDirectory()
                            + "/"
                            + TranslationTable.getCellUnqualifiedName() 
                            + ".def";
    String cellTemplateName = "DaStGenCell.template";

    _translationTable.convertTemplateFile(
      cellTemplateName,
      cellFileName, 
      true, true
    );

    printArguments(getDaStGenArguments(cellFileName));

    de.tum.in.dast.DaStGen.main( getDaStGenArguments(cellFileName) );
  }


  public void inAState(AState node) {
    String stateFileName     = _directoryGenerator.getDaStGenDirectory()
                             + "/"
                             + TranslationTable.getStateUnqualifiedName() 
                             + ".def";
    String stateTemplateName = "DaStGenState.template";

    _translationTable.convertTemplateFile(
      stateTemplateName,
      stateFileName, 
      true, true
    );
    
    printArguments(getDaStGenArguments(stateFileName));

    de.tum.in.dast.DaStGen.main( getDaStGenArguments(stateFileName) );
  }     
}
