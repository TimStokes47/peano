// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_DATAMANAGEMENT_CELL_ENUMERATOR_H_
#define _PEANO4_DATAMANAGEMENT_CELL_ENUMERATOR_H_


#include "tarch/la/Vector.h"
#include "peano4/utils/Globals.h"


namespace peano4 {
  namespace datamanagement {
    template <typename Cell>
    struct CellWrapper;
  }
}



/**
 * Cell enumerator is a likely misleading name.
 *
 * Cell = void means that no cell data is used. In such a case, we might still be interested in the meta data.
 */
template <class Cell>
struct peano4::datamanagement::CellWrapper {
  private:
    Cell*   _cell;

    /**
     * Centre
     */
    tarch::la::Vector<Dimensions,double>  _centre;

    tarch::la::Vector<Dimensions,double>  _h;

    bool _isRefined;
    bool _isAdjacentToTreeBoundary;

  public:
    CellWrapper(
      const tarch::la::Vector<Dimensions,double>&  centre,
	  const tarch::la::Vector<Dimensions,double>&  h,
	  Cell* cell
	):
      _cell(cell),
      _centre(centre),
      _h(h) {
    }


    CellWrapper(
      const tarch::la::Vector<Dimensions,double>&  centre,
	  const tarch::la::Vector<Dimensions,double>&  h,
	  const tarch::la::Vector<Dimensions,double>&  relativePositionToFather,
	  Cell* cell
	):
      _cell(cell),
      _centre(centre),
      _h(h) {
  	  for (int d=0; d<Dimensions; d++) {
        _centre(d) += (1.0-relativePositionToFather(d)) * _h(d);
  	  }

  	  _h = 3.0 * _h;
    }


    CellWrapper( const CellWrapper& copy ):
      _cell(copy.cell),
      _centre(copy.centre),
      _h(copy.h) {
    }


    CellWrapper& operator=( const CellWrapper& copy ) {
      _cell   = copy.cell;
      _centre = copy.centre;
      _h      = copy.h;
      return *this;
    }


    bool isRefined() const {
      return _isRefined;
    }


    tarch::la::Vector<Dimensions,double>  centre() const {
      return _centre;
    }


    tarch::la::Vector<Dimensions,double>  h() const {
      return _h;
    }


    Cell& data() {
      return *_cell;
    }
};

#endif
