// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_DATAMANAGEMENT_FACE_MARKER_H_
#define _PEANO4_DATAMANAGEMENT_FACE_MARKER_H_


#include "peano4/utils/Globals.h"
#include "peano4/grid/GridTraversalEvent.h"
#include "tarch/la/Vector.h"


namespace peano4 {
  namespace datamanagement {
    struct FaceMarker;
  }
}


struct peano4::datamanagement::FaceMarker {
  private:
	/**
	 * Centre of the underlying cell
	 */
    tarch::la::Vector<Dimensions,double>  _cellCentre;

	/**
	 * Size of the underlying cell
	 */
    tarch::la::Vector<Dimensions,double>  _h;

    int _select;
  public:
    FaceMarker(const peano4::grid::GridTraversalEvent& event);

    /**
     * Selects a face witin a cell, i.e. now the marker knows to which
     * face it corresponds. After that, the routine returns a this
     * reference.
     */
    FaceMarker& select(int face);

    tarch::la::Vector<Dimensions,double> x(int i) const;
    tarch::la::Vector<Dimensions,double> x() const;

    tarch::la::Vector<Dimensions,double> normal(int i) const;
    tarch::la::Vector<Dimensions,double> normal() const;

    tarch::la::Vector<Dimensions,double>  h() const;

    std::string toString() const;

    bool isRefined() const;
};


#endif
