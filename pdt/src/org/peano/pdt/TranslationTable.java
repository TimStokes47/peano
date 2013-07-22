package org.peano.pdt;


import java.io.FileNotFoundException;

import org.peano.pdt.analysis.DepthFirstAdapter;

import org.peano.pdt.node.*;


public class TranslationTable extends DepthFirstAdapter {

  private static final String MULTIPLE_REPLACEMENT_LINE_PREFIX = "__x";
  private static final String MULTIPLE_REPLACEMENT_LINE_PREFIX_WITH_COMMA = "__xc";

  private static final String _KEYWORD_COMPONENT_NAME       = "__COMPONENT_NAME__";

  private static final String _KEYWORD_INCLUDE_GUARD_PREFIX = "__INCLUDE_GUARD_PREFIX__";

  private static final String _KEYWORD_DOF_DASTGEN_DEFINITION_CELL = "__DOF_DASTGEN_DEFINITION_CELL__";
  private static final String _KEYWORD_DOF_DASTGEN_DEFINITION_STATE = "__DOF_DASTGEN_DEFINITION_STATE__";
  private static final String _KEYWORD_DOF_DASTGEN_DEFINITION_VERTEX = "__DOF_DASTGEN_DEFINITION_VERTEX__";
  
  private static final String _KEYWORD_NAMESPACE       = "__NAMESPACE__";
  private static final String _KEYWORD_OPEN_NAMESPACE  = "__OPEN_NAMESPACE__";
  private static final String _KEYWORD_CLOSE_NAMESPACE = "__CLOSE_NAMESPACE__";

  private static final String _KEYWORD_PROJECT_PATH       = "__PROJECT_PATH__";

  private static final String _KEYWORD_REPOSITORY_ACTION_ENUM = "__REPOSITORY_ACTION_ENUM__";

  private static final String _MULTIPLE_LINE_KEYWORD_FULL_QUALIFIED_ADAPTER_TYPE = "__FULL_QUALIFIED_ADAPTER_TYPE__";
  private static final String _MULTIPLE_LINE_KEYWORD_NONQUALIFIED_ADAPTER_TYPE = "__NONQUALIFIED_ADAPTER_TYPE__";

  private static final String _MULTIPLE_LINE_KEYWORD_READ_SCALAR_OPERATION = "__READ_SCALAR_OPERATION__";
  private static final String _MULTIPLE_LINE_KEYWORD_READ_VECTOR_OPERATION = "__READ_VECTOR_OPERATION__";
  private static final String _MULTIPLE_LINE_KEYWORD_READ_VECTOR_TWO_POW_D_OPERATION = "__READ_VECTOR_TWO_POW_D_OPERATION__";
  private static final String _MULTIPLE_LINE_KEYWORD_READ_STENCIL_OPERATION = "__READ_STENCIL_OPERATION__";
  private static final String _MULTIPLE_LINE_KEYWORD_READ_INTERGRID_OPERATOR_OPERATION = "__READ_INTERGRID_OPERATOR_OPERATION__";
  private static final String _MULTIPLE_LINE_KEYWORD_WRITE_SCALAR_OPERATION = "__WRITE_SCALAR_OPERATION__";
  private static final String _MULTIPLE_LINE_KEYWORD_WRITE_VECTOR_OPERATION = "__WRITE_VECTOR_OPERATION__";
  private static final String _MULTIPLE_LINE_KEYWORD_WRITE_VECTOR_TWO_POW_D_OPERATION = "__WRITE_VECTOR_TWO_POW_D_OPERATION__";
  private static final String _MULTIPLE_LINE_KEYWORD_WRITE_STENCIL_OPERATION = "__WRITE_STENCIL_OPERATION__";
  private static final String _MULTIPLE_LINE_KEYWORD_WRITE_INTERGRID_OPERATOR_OPERATION = "__WRITE_INTERGRID_OPERATOR_OPERATION__";

  private static final String _MULTIPLE_LINE_KEYWORD_MAPPINGS = "__MAPPINGS__";
  private static final String _KEYWORD_IF_MAPPING_EXISTS_AND_PREDEFINED_MAPPING_EXISTS_INSERT_COMMA = "__IF_MAPPINGS_EXIST_AND_PREDEFINED_MAPPINGS_EXIST_INSERT_COMMA__";
  private static final String _MULTIPLE_LINE_KEYWORD_PREDEFINED_MAPPINGS = "__PREDEFINED_MAPPINGS__";

  private static final String _TEMPORARY_KEYWORD_TYPENAME = "__THIS_TYPENAME__";
  private static final String _TEMPORARY_KEYWORD_FULL_QUALIFIED_TYPENAME = "__THIS_QUALIFIED_TYPENAME__";
  
  private static final String _KEYWORD_NUMBER_OF_ADAPTERS_ = "__NUMBER_OF_ADAPTERS__";

  private java.io.File _templateDirectory;

  /**
   * All the mappings from keywords to concrete names.
   */
  private java.util.HashMap<String, String> _mapping;

  /**
   * All the mappings from keywords to several names.
   */
  private java.util.HashMap<String, java.util.Vector<String>> _multiMappings;

  /**
   * List of all the adapters associated to the project.
   */
  private java.util.Vector<String> _adapters;

  public TranslationTable(java.io.File templateDirectory) {
    _mapping = new java.util.HashMap<String, String>();
    _templateDirectory = templateDirectory;
    for (String p : _mapping.keySet()) {
      _mapping.put(p, "UNDEF");
    }
    _adapters = new java.util.Vector<String>();
    _multiMappings = new java.util.HashMap<String, java.util.Vector<String>>();

    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_READ_SCALAR_OPERATION,
        new java.util.Vector<String>());
    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_READ_VECTOR_OPERATION,
        new java.util.Vector<String>());
    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_READ_VECTOR_TWO_POW_D_OPERATION,
            new java.util.Vector<String>());
    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_READ_STENCIL_OPERATION,
        new java.util.Vector<String>());
    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_READ_INTERGRID_OPERATOR_OPERATION,
        new java.util.Vector<String>());
    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_WRITE_SCALAR_OPERATION,
        new java.util.Vector<String>());
    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_WRITE_VECTOR_OPERATION,
        new java.util.Vector<String>());
    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_WRITE_VECTOR_TWO_POW_D_OPERATION,
            new java.util.Vector<String>());
    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_WRITE_STENCIL_OPERATION,
        new java.util.Vector<String>());
    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_WRITE_INTERGRID_OPERATOR_OPERATION,
        new java.util.Vector<String>());
  }

  public void inAReadScalarOperation(AReadScalarOperation node) {
    _multiMappings.get(_MULTIPLE_LINE_KEYWORD_READ_SCALAR_OPERATION).add(
        node.getName().toString().trim());
  }

  public void inAReadVectorOperation(AReadVectorOperation node) {
    _multiMappings.get(_MULTIPLE_LINE_KEYWORD_READ_VECTOR_OPERATION).add(
        node.getName().toString().trim());
  }

  public void  inAReadVectorTwoPowDOperation(AReadVectorTwoPowDOperation node) {
    _multiMappings.get(_MULTIPLE_LINE_KEYWORD_READ_VECTOR_TWO_POW_D_OPERATION).add(
        node.getName().toString().trim());
  }

  public void inAReadStencilOperation(AReadStencilOperation node) {
    _multiMappings.get(_MULTIPLE_LINE_KEYWORD_READ_STENCIL_OPERATION).add(
        node.getName().toString().trim());
  }
  
  public void inAReadIntergridOperatorOperation(AReadIntergridOperatorOperation node) {
    _multiMappings.get(_MULTIPLE_LINE_KEYWORD_READ_INTERGRID_OPERATOR_OPERATION).add(
        node.getName().toString().trim());
  }

  
  public void inAWriteVectorOperation(AWriteVectorOperation node) {
    _multiMappings.get(_MULTIPLE_LINE_KEYWORD_WRITE_VECTOR_OPERATION).add(
        node.getName().toString().trim());
  }
  
  public void inAWriteVectorTwoPowDOperation(AWriteVectorTwoPowDOperation node) {
    _multiMappings.get(_MULTIPLE_LINE_KEYWORD_WRITE_VECTOR_TWO_POW_D_OPERATION).add(
        node.getName().toString().trim());
  }

  public void inAWriteScalarOperation(AWriteScalarOperation node) {
    _multiMappings.get(_MULTIPLE_LINE_KEYWORD_WRITE_SCALAR_OPERATION).add(
        node.getName().toString().trim());
  }

  public void inAWriteStencilOperation(AWriteStencilOperation node) {
    _multiMappings.get(_MULTIPLE_LINE_KEYWORD_WRITE_STENCIL_OPERATION).add(
        node.getName().toString().trim());
  }
  
  public void inAWriteIntergridOperatorOperation(AWriteIntergridOperatorOperation node) {
    _multiMappings.get(_MULTIPLE_LINE_KEYWORD_WRITE_INTERGRID_OPERATOR_OPERATION
        ).add(
        node.getName().toString().trim());
  }

  public void setThisTypenameToMappingTypename(String typename) {
    _mapping.put(_TEMPORARY_KEYWORD_TYPENAME, typename);
    _mapping.put(_TEMPORARY_KEYWORD_FULL_QUALIFIED_TYPENAME,
        _mapping.get(_KEYWORD_NAMESPACE) + "::mappings::" + typename);
  }
  

  public void setThisTypenameToAdapterTypename(
      String typename
  ) {
    _mapping.put(_TEMPORARY_KEYWORD_TYPENAME, typename);
    _mapping.put(_TEMPORARY_KEYWORD_FULL_QUALIFIED_TYPENAME,
        _mapping.get(_KEYWORD_NAMESPACE) + "::adapters::" + typename);
  }

  public void setThisTypenameToAdapterTypename(
      String typename,
      java.util.Vector<String> mappings,
      java.util.Vector<String> predefinedMappings
  ) {
    _mapping.put(_TEMPORARY_KEYWORD_TYPENAME, typename);
    _mapping.put(_TEMPORARY_KEYWORD_FULL_QUALIFIED_TYPENAME,
        _mapping.get(_KEYWORD_NAMESPACE) + "::adapters::" + typename);

    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_MAPPINGS, mappings);
    if (!mappings.isEmpty() && !predefinedMappings.isEmpty()) {
      _mapping.put(_KEYWORD_IF_MAPPING_EXISTS_AND_PREDEFINED_MAPPING_EXISTS_INSERT_COMMA, ",");
    }
    else {
      _mapping.put(_KEYWORD_IF_MAPPING_EXISTS_AND_PREDEFINED_MAPPING_EXISTS_INSERT_COMMA, "");
    }
      
    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_PREDEFINED_MAPPINGS, predefinedMappings);
  }

  private String getOpeningNamespace(String fullQualifiedName) {
    String namespace = fullQualifiedName;
    String result = "";
    int count = 0;
    while (namespace.length() != 0) {
      for (int i = 0; i < count; i++) {
        result += " ";
      }
      result += "namespace ";
      if (namespace.indexOf("::") != -1) {
        result += namespace.substring(0, namespace.indexOf("::"));
        namespace = namespace.substring(namespace.indexOf("::") + 2);
        count += 2;
        result += " { \n";
      } else {
        result += namespace;
        namespace = "";
        result += " {";
      }
    }
    return result;
  }

  private String getClosingNamespace(String fullQualifiedName) {
    String namespace = fullQualifiedName;
    String result = "";
    while (namespace.indexOf("::") != -1) {
      result += "}\n";
      namespace = namespace.substring(namespace.indexOf("::") + 2);
    }
    result += "}";
    return result;
  }
  
  public void inADescription(ADescription node) {
    String namespace = node.getNamespace().getText().trim();
    if (namespace.startsWith( "::" )) namespace = namespace.substring(2);
    _mapping.put(_KEYWORD_NAMESPACE, namespace );
    _mapping.put(
      _KEYWORD_INCLUDE_GUARD_PREFIX, 
      namespace.replaceAll("::", "_").toUpperCase()
    );
    _mapping.put(
        _KEYWORD_PROJECT_PATH, 
        namespace.replaceAll("::", "/")
      );
    
    _mapping.put(_KEYWORD_COMPONENT_NAME, node.getName().getText().trim());

    _mapping.put(_KEYWORD_OPEN_NAMESPACE,
        getOpeningNamespace(_mapping.get(_KEYWORD_NAMESPACE)));
    _mapping.put(_KEYWORD_CLOSE_NAMESPACE,
        getClosingNamespace(_mapping.get(_KEYWORD_NAMESPACE)));
  }

  public void inAAdapter(AAdapter node) {
    _adapters.add(node.getName().getText().trim());
  }

  public void inAVertex(AVertex node) {
    _mapping.put(_KEYWORD_DOF_DASTGEN_DEFINITION_VERTEX, node.getFilename().getText().trim());
  }

  public void inACell(ACell node) {
    _mapping.put(_KEYWORD_DOF_DASTGEN_DEFINITION_CELL, node.getFilename().getText().trim());
  }

  public void inAState(AState node) {
    _mapping.put(_KEYWORD_DOF_DASTGEN_DEFINITION_STATE, node.getFilename().getText().trim());
  }

  public void outStart(Start node) {
    String enumDeclaration = "enum Action { WriteCheckpoint, ReadCheckpoint, Terminate, RunOnAllNodes";
    for (String p : _adapters) {
      enumDeclaration += ",UseAdapter";
      enumDeclaration += p;
    }
    enumDeclaration += ",NumberOfAdapters";
    enumDeclaration += "}";

    _mapping.put(_KEYWORD_REPOSITORY_ACTION_ENUM, enumDeclaration);

    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_FULL_QUALIFIED_ADAPTER_TYPE,
        new java.util.Vector<String>());
    _multiMappings.put(_MULTIPLE_LINE_KEYWORD_NONQUALIFIED_ADAPTER_TYPE,
        new java.util.Vector<String>());

    for (String p : _adapters) {
      String typeName = p;
      String qualifiedTypeName = _mapping.get(_KEYWORD_NAMESPACE)
          + "::adapters::" + p;
      _multiMappings.get(_MULTIPLE_LINE_KEYWORD_NONQUALIFIED_ADAPTER_TYPE).add(
          typeName);
      _multiMappings.get(_MULTIPLE_LINE_KEYWORD_FULL_QUALIFIED_ADAPTER_TYPE)
          .add(qualifiedTypeName);
    }

    _mapping.put(_KEYWORD_NUMBER_OF_ADAPTERS_, java.lang.Integer.toString(_adapters.size()));
  }

  static public String getCellUnqualifiedName() {
    return "Cell";
  }

  static public String getStateUnqualifiedName() {
    return "State";
  }

  static public String getVertexUnqualifiedName() {
    return "Vertex";
  }

  static public String getRepositoryStateUnqualifiedName() {
    return "RepositoryState";
  }

  private String replaceKeywords(String line) {
    String text = line;
    for (String p : _mapping.keySet()) {
      text = text.replaceAll(p, _mapping.get(p));
    }
    return text;
  }

  public void copyTemplateFile(
    String    templateFile, 
    String    destinationFile,
    boolean   overwrite
  ) {
	destinationFile = destinationFile.replaceAll("\\\\", "/");
	if ((new java.io.File(destinationFile)).exists() && !overwrite) {
		System.out.print("\n " + destinationFile + " already exists)");
	} else {
      try {
		java.io.OutputStreamWriter writer = new java.io.OutputStreamWriter(
			new java.io.FileOutputStream(destinationFile));
		String text = new String();
   		java.io.BufferedReader reader = getBufferedReaderForStdTemplate(templateFile);

		while ((text = reader.readLine()) != null) {
		  writer.write(text);
          writer.write("\n");
        }
		writer.close();
		reader.close();
      } catch (Exception e) {
		System.out.println(e.getClass() + " " + e.getMessage() + "\n");
		e.printStackTrace();
	  }
	}
  }


  public void convertTemplateFile(
    String    templateFile, 
    String    destinationFile,
    java.util.ArrayList<String>  replaceAdditionalString,
    java.util.ArrayList<String>  withString,
    boolean   overwrite,
    boolean   extractTemplateFromJar
  ) {
  	destinationFile = destinationFile.replaceAll("\\\\", "/");
	
		if ((new java.io.File(destinationFile)).exists() && !overwrite) {
			System.out.print("\n  (blueprint of " + destinationFile
					+ " already exists)");
		} else {
			try {
				java.io.OutputStreamWriter writer = new java.io.OutputStreamWriter(
						new java.io.FileOutputStream(destinationFile));
				String text = new String();
			  	
				java.io.BufferedReader reader = null;
				if (extractTemplateFromJar) {
				  reader = getBufferedReaderForStdTemplate(templateFile);
				}
				else {
				  reader = getBufferedReaderForUserTemplate(templateFile);
				}

				while ((text = reader.readLine()) != null) {
					for (String p : _multiMappings.keySet()) {
					  boolean isPattern   = false;
					  boolean appendComma = false;
					  String  textToRemove = "";

					  if (text.contains(MULTIPLE_REPLACEMENT_LINE_PREFIX_WITH_COMMA + p )) {
	            isPattern     = true;
	            appendComma   = true;
	            textToRemove  = MULTIPLE_REPLACEMENT_LINE_PREFIX_WITH_COMMA + p;
	          }
					  else if (text.contains(MULTIPLE_REPLACEMENT_LINE_PREFIX + p)) {
					    isPattern = true;
              textToRemove  = MULTIPLE_REPLACEMENT_LINE_PREFIX + p;
					  }
					  
						if (isPattern) {
							String modifiedText = text.replaceAll(textToRemove, "");
							text = "";
							int numberOfOustandingReplacements = _multiMappings.get(p).size();
							for (String replacement : _multiMappings.get(p)) {
								text += modifiedText.replaceAll(p, replacement);
								numberOfOustandingReplacements--;
		            if (appendComma && numberOfOustandingReplacements>0) {
	                text += ", \n";
		            }
		            else {
	                text += "\n";
		            }
							}
						}
					}

					text = replaceKeywords(text);

					if (replaceAdditionalString!=null) {
						for (int i=0; i<replaceAdditionalString.size(); i++) {
							text = text.replaceAll(replaceAdditionalString.get(i), withString.get(i));
						}
					}

					writer.write(text);
					writer.write("\n");
				}

				writer.close();
				reader.close();
			} catch (Exception e) {
				System.out.println(e.getClass() + " " + e.getMessage() + "\n");
				e.printStackTrace();
			}
		}
  }

  private java.io.BufferedReader getBufferedReaderForStdTemplate(String templateFile) throws FileNotFoundException {
    templateFile = "/stdtemplates/" + templateFile;
    java.io.InputStream stream = Main.class.getResourceAsStream(templateFile);
    if (stream==null) {
      throw new FileNotFoundException(templateFile);
    }
   java.io.BufferedReader reader = new java.io.BufferedReader(
	    new java.io.InputStreamReader(stream)
	  );
	  return reader;
  }

  private java.io.BufferedReader getBufferedReaderForUserTemplate(String templateFile)
		throws FileNotFoundException {
  if (_templateDirectory!=null) {
    templateFile = _templateDirectory + "/" + templateFile;
  }
  else {
    System.err.println( "\nwarning: No user template directory passed, search for templates in current directory" );
  }
	templateFile = templateFile.replaceAll("\\\\", "/");
	java.io.BufferedReader reader = new java.io.BufferedReader(
			new java.io.FileReader(new java.io.File(templateFile)));
	return reader;
  }

  /**
   * Read a template file, replace all the keywords in it with the value, and
   * write the resulting file to destinationFile.
   * 
   * @param destinationFile
   * @param rootDirectory
   * @param templateFile
   */
  public void convertTemplateFile(
    String  templateFile, 
    String  destinationFile,
    boolean overwrite,
    boolean extractTemplateFromJar
  ) {
    convertTemplateFile(templateFile,destinationFile,null,null,overwrite,extractTemplateFromJar);
  }
}
