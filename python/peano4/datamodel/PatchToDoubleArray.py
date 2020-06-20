# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import peano4.output.TemplatedHeaderImplementationFilePair
import os

from .DoF import DoFAssociation


class PatchToDoubleArray():
  """
  
  Very simple converter which maps the patch 1:1 onto a double array. 
  By default, I leave all the MPI merges empty though obviously the 
  merges often are plain copying over along faces (see the discussion 
  on Finite Volumes in the guidebook). If you need any proper MPI 
  handling, use set_merge_method_definition() to inject it. Depending
  on how you use your patch (as cell or face or vertex data structure)
  the thing you pass has the respective semantics. The most popular
  default merge implementation is the one you find in the blockstructured
  toolbox.
  
  """
  def __init__(self,patch):
    self.data = patch
    self._merge_method_definitions = ""


  def set_merge_method_definition(self,implementation):
    self._merge_method_definitions = implementation
    
    
  def get_stack_container(self):
    return "peano4::stacks::STDVectorStack< " + self.data.get_full_qualified_type() + " >";

    
  def get_header_file_include(self):
    return "#include \"peano4/stacks/STDVectorStack.h\" \n \
            #include \"" + self.data.namespace[-1] + "/" + self.data.name + ".h\""

    
  def construct_output(self,output):
    d = { 
        "CARDINALITY_2D": str( int( self.data.no_of_unknowns*self.data.dim[0]*self.data.dim[1] )), 
        "CARDINALITY_3D": str( int( self.data.no_of_unknowns*self.data.dim[0]*self.data.dim[1]*self.data.dim[2] )),
        "MERGE_METHOD_DECLARATIONS": "",
        "MERGE_METHOD_DEFINITIONS": ""
      }

    if self.data.association==DoFAssociation.Vertex:
      d["MERGE_METHOD_DECLARATIONS"] = "void merge(const " + self.data.name + "& neighbour, const peano4::datamanagement::VertexMarker& marker);"
      d["MERGE_METHOD_DEFINITIONS"]  = "void " + self.data.get_full_qualified_type() + "::merge(const " + self.data.name + "& neighbour, const peano4::datamanagement::VertexMarker& marker) {\n" + self._merge_method_definitions + "\n}"
    elif self.data.association==DoFAssociation.Face:
      d["MERGE_METHOD_DECLARATIONS"] = "void merge(const " + self.data.name + "& neighbour, const peano4::datamanagement::FaceMarker& marker);"
      d["MERGE_METHOD_DEFINITIONS"]  = "void " + self.data.get_full_qualified_type() + "::merge(const " + self.data.name + "& neighbour, const peano4::datamanagement::FaceMarker& marker) {\n" + self._merge_method_definitions + "\n}"
    elif self.data.association==DoFAssociation.Cell:
      d["MERGE_METHOD_DECLARATIONS"] = ""


    """
      Pass in a version of output
    """
    output.makefile.add_cpp_file( self.data.namespace[-1] + "/" + self.data.name + ".cpp" )
    templatefile_prefix = os.path.realpath(__file__).replace( ".pyc", "" ).replace( ".py", "" )
    generated_files = peano4.output.TemplatedHeaderImplementationFilePair(
      templatefile_prefix+".h.template",
      templatefile_prefix+".cpp.template",
      self.data.name, 
      self.data.namespace,
      self.data.namespace[-1],
      d, 
      True)
    output.add(generated_files)

