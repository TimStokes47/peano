// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#pragma once


#include <vector>

#include "peano4/utils/Globals.h"
#include "tarch/Assertions.h"

#include "VertexEnumerator.h"


namespace peano4 {
  namespace datamanagement {
    template <class Vertex>
    class VertexHeapDataEnumerator;
  }
}


/**
 * Enumerator over copies of the vertices of a cell
 *
 * Vertex counterpart of peano4::datamanagement::FaceHeapDataEnumerator: The
 * enumerator takes a peano4::datamanagement::VertexEnumerator, which holds
 * pointers to the 2^d vertices of a cell on Peano's stacks, and copies the
 * vertices. It owns these copies and offers the same access operator as the
 * VertexEnumerator, so code which runs after the traversal has left the cell,
 * e.g. a spawned task, can use it.
 *
 * The enumerator only refers to the actual vertex data if copying a vertex
 * is shallow, i.e. if the data is managed via smart pointers. Otherwise, the
 * enumerator works on deep copies, and writes are lost. See
 * peano4::datamanagement::FaceHeapDataEnumerator for details.
 */
template <class Vertex>
class peano4::datamanagement::VertexHeapDataEnumerator {
  private:
    std::vector<Vertex> _vertices;

  public:
    /**
     * Copy the 2^d vertices the enumerator points to.
     */
    VertexHeapDataEnumerator(const VertexEnumerator<Vertex>& vertices);

    /**
     * Access the copy of vertex i. Vertices are enumerated as in
     * peano4::datamanagement::VertexEnumerator.
     */
    Vertex& operator()(int i);

    const Vertex& operator()(int i) const;
};


#include "VertexHeapDataEnumerator.cpph"
