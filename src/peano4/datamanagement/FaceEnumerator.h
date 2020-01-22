// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_DATAMANAGEMENT_FACE_ENUMERATOR_H_
#define _PEANO4_DATAMANAGEMENT_FACE_ENUMERATOR_H_


#include "peano4/utils/Globals.h"
#include "tarch/la/Vector.h"


namespace peano4 {
  namespace datamanagement {
    template <class Face>
    class FaceEnumerator;
  }
}


template <class Face>
class peano4::datamanagement::FaceEnumerator {
  private:
    Face* _faces[ TwoTimesD ];

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
    FaceEnumerator(const tarch::la::Vector<Dimensions,double>& x, const tarch::la::Vector<Dimensions,double>&  h):
      _x(x),
      _h(h) {
      #if PeanoDebug>0
      for (int i=0; i<TwoTimesD; i++) {
    	_faces[i] = nullptr;
      }
      #endif
    }


    /**
     * Face enumerator with standard ordering of faces within a consecutive
     * array.
     */
    FaceEnumerator(const tarch::la::Vector<Dimensions,double>  x, const tarch::la::Vector<Dimensions,double>  h, Face* firstFace):
      _x(x),
      _h(h) {
      for (int i=0; i<TwoTimesD; i++) {
      _faces[i] = firstFace+i;
      }
    }


    FaceEnumerator(const FaceEnumerator<Face>& copy ):
      _x(copy._x),
      _h(copy._h) {
      for (int i=0; i<TwoTimesD; i++) {
        _faces[i] = copy._faces[i];
      }
    }


    FaceEnumerator& operator=(const FaceEnumerator<Face>& copy ) {
      _x = copy._x;
      _h = copy._h;
      for (int i=0; i<TwoTimesD; i++) {
        _faces[i] = copy._faces[i];
      }
      return *this;
    }


	/**
	 * Usually is only used by the observers, i.e. users should not interact
	 * with this routine.
	 */
	void setPointer(int i, Face* value) {
	  assertion(i>=0);
	  assertion(i<TwoTimesD);
	  assertion(value!=nullptr);
	  _faces[i] = value;
	}

	Face& operator()(int i) const {
	  assertion(i>=0);
	  assertion(i<TwoTimesD);
	  return *(_faces[i]);
	}

	tarch::la::Vector<Dimensions,double> x(int i) const {
      tarch::la::Vector<Dimensions,double> result( _x );
      int normal = i % Dimensions;
	  result(normal) += i >= Dimensions ? _h(normal)/2.0 : -_h(normal)/2.0;
      return result;
	}

	tarch::la::Vector<Dimensions,double> normal(int i) const {
      tarch::la::Vector<Dimensions,double> result( 0.0 );
      int index = i % Dimensions;
      result(index) += i<Dimensions ? -_h(index) : _h(index);
      return result;
	}

	std::string toString() const {
	  return "(" + _x.toString() + "," + _h.toString() + ")";
	}
};


#endif
