// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_DATAMANAGEMENT_VERTEX_MARKER_H_
#define _PEANO4_DATAMANAGEMENT_VERTEX_MARKER_H_


namespace peano4 {
  namespace datamanagement {
    struct VertexMarker;
  }

  namespace grid {
    class GridTraversalEvent;
  }
}



struct peano4::datamanagement::VertexMarker {
  VertexMarker(const peano4::grid::GridTraversalEvent&  event, int vertexNumber);
};

#endif
