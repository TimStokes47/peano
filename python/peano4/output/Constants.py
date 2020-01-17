# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Helper import Overwrite
from .Helper import writeFile

import os
import re


class Constants(object):
  """ Represents all the defines exported by a Project """
  default_overwrite = True
   
  def __init__(self,project):
    self.defines = []
    self.d = {}
    
    self.d[ "ADD_CONSTANTS" ]            = ""

    self.d[ "OPEN_NAMESPACE" ]           = ""
    self.d[ "CLOSE_NAMESPACE" ]          = ""
    self.d[ "INCLUDE_GUARD" ]            = "_"
    for i in project.namespace:
      self.d[ "OPEN_NAMESPACE" ]        += "namespace " + i + "{\n"
      self.d[ "CLOSE_NAMESPACE" ]       += "}\n"
      self.d[ "INCLUDE_GUARD" ]         += i + "_"

    self.d[ "INCLUDE_GUARD" ] = self.d[ "INCLUDE_GUARD" ].upper()
    
  def export_constant( self, value, name ):
    new_entry = "constexpr auto " + name + " = " + str(value) + ";"
    self.d[ "ADD_CONSTANTS" ] += "  " + new_entry + "\n"
    pass
    #self.constants.export_constant( value, name )    

  def generate(self, overwrite, directory):
    filename = directory + "/Constants.h";
    if writeFile(overwrite,self.default_overwrite,filename):
      print( "write " + filename )
      
      #for i in self.cppfiles:
      #  self.d[ 'SOURCES' ] += " "
      #  self.d[ 'SOURCES' ] += i
      
      # We first eliminate the precompiled variant, and then we get rid of the
      # postfix in the case where it is a source file
      with open( os.path.realpath(__file__).replace( ".pyc", ".h.template" ).replace( ".py", ".h.template" ), "r" ) as input:
        template = input.read()
      with open( filename, "w" ) as output:
        output.write( template.format(**self.d) )
   
