# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Helper import Overwrite
from .Helper import writeFile

import os
import re


class Observer(object):
  def __init__(self,name,namespace,directory):
    self.name      = name
    self.namespace = namespace
    self.directory = directory 
    
    self.d = dictionary
    self.d[ "OPEN_NAMESPACE" ]           = ""
    self.d[ "CLOSE_NAMESPACE" ]          = ""
    self.d[ "FULL_QUALIFIED_CLASSNAME" ] = ""
    for i in namespace:
      self.d[ "OPEN_NAMESPACE" ]           += "namespace " + i + "{\n"
      self.d[ "CLOSE_NAMESPACE" ]          += "}\n"
      self.d[ "FULL_QUALIFIED_CLASSNAME" ] += i + "::"
    self.d[ "CLASSNAME" ]                 = classname
    self.d[ "FULL_QUALIFIED_CLASSNAME" ] += classname
    self.d[ "INCLUDE_GUARD" ]             = self.d[ "FULL_QUALIFIED_CLASSNAME" ].replace( "::", "_" ).upper()
    pass

  
  def generate(self,overwrite,directory):
    pass
