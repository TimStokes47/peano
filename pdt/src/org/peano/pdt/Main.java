// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org.
package org.peano.pdt;

import java.util.Arrays;
import java.util.Vector;

public class Main {
  public static void printUsage() {
    System.out.println("Peano Developement Toolkit (PDT)");
    System.out.println("(C) 2010 Tobias Weinzierl");
    System.out.println("");
    System.out.println("Usage: de.tum.peano.pdt.Main  peano-specification-file component-output-directory [template-directories]" );
    System.out.println("  peano-specification-file    Mandatory; specification file describing your project. " );
    System.out.println("  component-output-directory  Mandatory; typically the directory holding the spec file as well. All paths" );
    System.out.println("                              within the spec file are relative to the output directory." );
    System.out.println("  template-directories        Optional; points to the directories holding user-defined templates. Multiple directories are separated by colon" );
  }

  /**
   * @param args
   */
  public static void main(String[] args) {
    if (args.length < 2 || args.length > 3) {
      printUsage();
    } else {
      try {
        String inputFileName = args[0];
        String outputDirectoryName = args[1];
        String templateDirectoryName = args.length == 3 ? args[2] : null;

        java.io.File outputDirectory = new java.io.File(outputDirectoryName);
        if (!outputDirectory.isDirectory()) {
          System.err.println("output directory "
              + outputDirectory.getAbsolutePath() + " is no valid directory");
          System.exit(-1);
        }

        java.util.Vector<String> templateDirectory = new java.util.Vector<String>();
        if (templateDirectoryName != null) {
          System.out.println("parse template directory specification "
              + templateDirectoryName );
          templateDirectory = new Vector<String>( Arrays.asList(templateDirectoryName.split(":")) );
          
          for (String d: templateDirectory) {
            java.io.File checkFile = new java.io.File(d);
            if (checkFile.isDirectory()) {
              System.out.println("template directory "
                  + checkFile.getAbsolutePath()
                  + " added to user-defined template search path");
            }
            else {
              System.err.println("template directory "
                  + checkFile.getAbsolutePath()
                  + " is no valid directory");
              System.exit(-1);
            }
          }
        }

        System.out.print("read input file " + inputFileName);
        org.peano.pdt.parser.Parser parser = new org.peano.pdt.parser.Parser(
            new org.peano.pdt.lexer.Lexer(new java.io.PushbackReader(
                new java.io.FileReader(inputFileName))));
        org.peano.pdt.node.Start document = parser.parse();
        System.out.println(" ... ok");

        System.out.print("fill translation table");
        TranslationTable translationTable = new TranslationTable(
            templateDirectory);
        document.apply(translationTable);
        System.out.println(" ... ok");

        System.out.print("generate output directories");
        org.peano.pdt.generators.DirectoryGenerator dirGenerator = new org.peano.pdt.generators.DirectoryGenerator(
            outputDirectory, translationTable);
        document.apply(dirGenerator);
        System.out.println(" ... ok");

        System.out.print("run DaStGen");
        document.apply(new org.peano.pdt.generators.DaStGenGenerator(
          dirGenerator, translationTable
        ));
        System.out.println(" ... ok");
        
        System.out.print("generate blueprints for vertices, cells, states, repositories, and mappings");
        document.apply(new org.peano.pdt.generators.BluePrintGenerator(
          dirGenerator, translationTable
        ));
        System.out.println(" ... ok");
        
        System.out.print("generate repositories");
        document.apply(new org.peano.pdt.generators.RepositoryGenerator(
          dirGenerator, translationTable
        ));
        System.out.println(" ... ok");
        
         System.out.print("generate runners");
         document.apply(new org.peano.pdt.generators.RunnerGenerator(
           dirGenerator, translationTable
         ));
         System.out.println(" ... ok");
        
         System.out.print("generate adapters");
         document.apply(new org.peano.pdt.generators.AdapterGenerator(
           dirGenerator, translationTable
         ));
         System.out.println(" ... ok");
        
         System.out.print("generate makefiles");
         document.apply(new org.peano.pdt.generators.MakefileGenerator(
           dirGenerator, translationTable
         ));
         System.out.println(" ... ok");
        
         System.out.print("generate tests");
         document.apply(new org.peano.pdt.generators.TestCaseGenerator(
           dirGenerator, translationTable
         ));
         System.out.println(" ... ok");
      } catch (Exception e) {
        System.out.println(e.getClass() + " " + e.getMessage() + "\n");
        e.printStackTrace();
        System.out.println("Use option -h to display help message.");
      }
    }
  }
}
