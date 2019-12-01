// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_DATAMANAGEMENT_VERTEX_ENUMERATOR_H_
#define _PEANO4_DATAMANAGEMENT_VERTEX_ENUMERATOR_H_


#include "peano4/utils/Globals.h"
#include "tarch/la/Vector.h"

#include <bitset>


namespace peano4 {
  namespace datamanagement {
    template <class Vertex>
    class VertexEnumerator;
  }
}


template <class Vertex>
class peano4::datamanagement::VertexEnumerator {
  private:
	Vertex* _vertices[ TwoPowerD ];

	/**
	 * Bottom left vertex of associated cell.
	 */
	tarch::la::Vector<Dimensions,double>  _x;

	tarch::la::Vector<Dimensions,double>  _h;
  public:
	/**
	 * Usually is only used by the observers, i.e. users should not interact
	 * with this routine.
	 */
	VertexEnumerator(const tarch::la::Vector<Dimensions,double>  x, const tarch::la::Vector<Dimensions,double>  h):
      _x(x),
	  _h(h) {
      #if PeanoDebug>0
	  for (int i=0; i<TwoTimesD; i++) {
		_vertices[i] = nullptr;
	  }
      #endif
	}


	VertexEnumerator(const VertexEnumerator<Vertex>& copy ):
      _x(copy._x),
      _h(copy._h) {
      for (int i=0; i<TwoTimesD; i++) {
        _vertices[i] = copy._vertices[i];
      }
    }


	VertexEnumerator& operator=(const VertexEnumerator<Vertex>& copy ) {
      _x = copy._x;
      _h = copy._h;
      for (int i=0; i<TwoTimesD; i++) {
        _vertices[i] = copy._vertices[i];
      }
      return *this;
    }


	/**
	 * Usually is only used by the observers, i.e. users should not interact
	 * with this routine.
	 */
	void setPointer(int i, Vertex* value) {
	  assertion(i>=0);
	  assertion(i<TwoPowerD);
	  assertion(value!=nullptr);
	  _vertices[i] = value;
	}

	Vertex& operator()(int i) const {
	  assertion(i>=0);
	  assertion(i<TwoPowerD);
	  return *(_vertices[i]);
	}

	tarch::la::Vector<Dimensions,double> x(int i) const {
      tarch::la::Vector<Dimensions,double> result( _x );
      std::bitset<Dimensions> myset(i);
      for (int d=0; d<Dimensions; d++) {
        result(d) += myset[d] * _h(d);
      }
      return result;
	}

	std::string toString() const {
	  return "(" + _x.toString() + "," + _h.toString() + ")";
	}
};


#endif
