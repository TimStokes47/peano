# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
import os
import re
import jinja2

from copy import deepcopy

from .Helper import write_file

from .Overwrite import Overwrite

class Jinja2TemplateStringHeaderImplementationFilePair(object):
  """!
Represents a pair of C++ header and implementation files

Same idea as Jinja2TemplatedHeaderImplementationFilePair, but the two
templates are handed over as plain strings rather than as fully qualified
file names. This is convenient if the template content is assembled/built
dynamically in Python rather than read from a file on disk.
  """

  def __init__(self,
               headerfile_template,
               cppfile_template,
               classname,
               namespace,
               subdirectory,
               dictionary,
               default_overwrite=True,
               apply_iteratively=False):
    """
      headerfile_template and cppfile_template are strings holding the actual
      Jinja2 template content (not file names).

      classname is a string
      namespace is a (possibly empty) list of strings

      cppfile_template can be None
    """
    self.headerfile_template  = headerfile_template
    self.cppfile_template     = cppfile_template
    self.classname            = classname
    self.namespace            = namespace
    self.subdirectory         = subdirectory
    self.default_overwrite    = default_overwrite
    self.apply_iteratively    = apply_iteratively

    self.d = deepcopy(dictionary)
    self.d[ "CLASSNAME" ] = classname
    self.d[ "NAMESPACE" ] = []
    for i in namespace:
      self.d["NAMESPACE"].append(i)
    # Eliminate the prepending "::" that can lead to compatibility issues across the solvers
    self.d["FULL_QUALIFIED_NAMESPACE"] = "::".join(namespace)

  def __generate_file(self,overwrite,full_qualified_filename,template_string):
    if template_string!=None and write_file(overwrite,self.default_overwrite,full_qualified_filename):
      import inspect
      print( "{} written by {} (from inline template)".format(full_qualified_filename, os.path.basename(inspect.getfile(self.__class__))))

      templateEnv = jinja2.Environment(undefined=jinja2.DebugUndefined)
      template = templateEnv.from_string( template_string )

      if self.apply_iteratively:
        old_size                = 0
        rendered_text           = template.render(self.d)
        while "{{" in rendered_text and len(rendered_text)!=old_size:
          old_size      = len(rendered_text)
          template      = jinja2.Template(rendered_text, undefined=jinja2.DebugUndefined)
          rendered_text = template.render( self.d )

      with open( full_qualified_filename, "w" ) as output:
        output.write( template.render(self.d) )

  def generate(self,overwrite,directory):
    if not os.path.exists( directory + "/" + self.subdirectory ):
      os.mkdir(directory + "/" + self.subdirectory)

    header_filename = directory + "/" + self.subdirectory + "/" + self.classname + ".h"
    cpp_filename    = directory + "/" + self.subdirectory + "/" + self.classname + ".cpp"

    self.__generate_file(overwrite,header_filename,self.headerfile_template)
    self.__generate_file(overwrite,cpp_filename   ,self.cppfile_template)
