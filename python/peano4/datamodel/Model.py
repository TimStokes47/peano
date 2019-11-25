# This file is part of the Peano project. For conditions of distribution and
# use, please see the copyright notice at www.peano-framework.org


class Model(object):
  def __init__(self):
    self.cell_data = []
    self.face_data = []
    self.vertex_data = []

  def add_cell(self,submodel):
    self.cell_data.append(submodel)

  def add_face(self,submodel):
    self.face_data.append(submodel)

  def add_vertex(self,submodel):
    self.vertex_data.append(submodel)

  def construct_output(self,output):
    for i in self.cell_data:
      i.generator.construct_output(output)
    for i in self.face_data:
      i.generator.construct_output(output)
    for i in self.vertex_data:
      i.generator.construct_output(output)
