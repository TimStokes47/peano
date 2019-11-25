# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org
from .Helper import Overwrite
from .Helper import writeFile

import os


class Makefile(object):
  """ Represents the created makefile of a Peano4 project """
   
  def __init__(self):
    pass

  def parse_configure_outcome(self):
    pass

  def generate(self,overwrite,directory):
    filename = directory + "/Makefile";
    if writeFile(overwrite,True,filename):
      print( "write " + filename )
      d = {}
      d[ 'CXX' ] = "test"
      with open( os.path.realpath(__file__).replace( ".py", ".template" ), "r" ) as input:
        template = input.read()
      with open( filename, "w" ) as output:
        output.write( template.format(**d) )
   
