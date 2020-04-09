# This file is part of the ExaHyPE2 project. For conditions of distribution and 
# use, please see the copyright notice at www.peano-framework.org
import peano4
import dastgen2


def create_face_label():
  result = peano4.datamodel.DaStGen2( "Label" )
  result.data.add_attribute( dastgen2.attributes.Boolean("Boundary") )
  return result


