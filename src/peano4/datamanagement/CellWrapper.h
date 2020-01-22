// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_DATAMANAGEMENT_CELL_ENUMERATOR_H_
#define _PEANO4_DATAMANAGEMENT_CELL_ENUMERATOR_H_


namespace peano4 {
  namespace datamanagement {
    template <class Cell>
    struct CellWrapper;
  }
}



/**
 * Cell enumerator is a likely misleading name.
 */
template <class Cell>
struct peano4::datamanagement::CellWrapper {
  private:
    Cell*   _cell;

    /**
     * Centre
     */
    const tarch::la::Vector<Dimensions,double>  _centre;

    const tarch::la::Vector<Dimensions,double>  _h;
  public:
    CellWrapper( const tarch::la::Vector<Dimensions,double>&  centre, const tarch::la::Vector<Dimensions,double>&  h, Cell* cell):
      _cell(cell),
      _centre(centre),
      _h(h) {

    }

    Cell& data() {
      return *_cell;
    }
};

#endif
