// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_DATAMANAGEMENT_FACE_MARKER_H_
#define _PEANO4_DATAMANAGEMENT_FACE_MARKER_H_


namespace peano4 {
  namespace datamanagement {
    struct FaceMarker;
  }

  namespace grid {
    class GridTraversalEvent;
  }
}



struct peano4::datamanagement::FaceMarker {
  FaceMarker(const peano4::grid::GridTraversalEvent&  event, int faceNumber);
};

#endif
