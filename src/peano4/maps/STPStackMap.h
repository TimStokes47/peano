// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_MAPS_STD_MAP_H_
#define _PEANO4_MAPS_STD_MAP_H_


#include "maps.h"
#include "peano4/grid/GridVertex.h"


#include <map>


namespace peano4 {
  namespace maps {
    template <typename T>
    class STDStackMap;

    typedef STDStackMap< peano4::stacks::GridVertexStack >   GridVertexStackMap;
  }
}


template <typename T>
class peano4::maps::STDStackMap {
  private:
	std::map< StackKey, T* >  _data;

    /**
     * Semaphore to protect _data.
     */
    tarch::multicore::BooleanSemaphore                      _semaphore;

    void createStack(int treeId, int stackId);
  public:
    ~STDStackMap();

    bool empty(int treeId, int stackId) const;

    /**
     * @return A stack of type T. Actually, it is a pointer and this routine is
     *   a lazy creation, i.e. might create the result upon demand.
     */
    T* get(int treeId, int stackId);
    T* get(const StackKey& key);

    std::string toString() const;

    /**
     * This one should be const, but I might need a semaphore, so I have to
     * omit the const qualifier.
     */
    std::set<StackKey>  getKeys();
};



template <typename T>
void peano4::maps::STDStackMap<T>::createStack(int treeId, int stackId) {
  tarch::multicore::Lock lock(_semaphore);
  if ( _data.count( StackKey(treeId,stackId) )==0 ) {
    _data.insert(
      std::pair< StackKey, T* >(
        StackKey(treeId,stackId),
		new T()
	  )
    );
  }
}


template <typename T>
bool peano4::maps::STDStackMap<T>::empty(int treeId, int stackId) const {
  return _data.count( StackKey(treeId,stackId) )==0
      or _data.at( StackKey(treeId,stackId) )->empty();
}


template <typename T>
T* peano4::maps::STDStackMap<T>::get(int treeId, int stackId) {
  createStack(treeId,stackId);

  tarch::multicore::Lock lock(_semaphore);
  return _data[StackKey(treeId,stackId)];
}


template <typename T>
T* peano4::maps::STDStackMap<T>::get(const StackKey& key) {
  createStack(key.first,key.second);

  tarch::multicore::Lock lock(_semaphore);
  return _data[key];
}


template <typename T>
std::string peano4::maps::STDStackMap<T>::toString() const {
  return "(" + std::to_string( _data.size() ) + ")";
}


template <typename T>
std::set<peano4::maps::StackKey>  peano4::maps::STDStackMap<T>::getKeys() {
  tarch::multicore::Lock lock(_semaphore);
  std::set<peano4::maps::StackKey> result;
  for (auto& p: _data) {
    result.insert(p.first);
  }
  return result;
}


template <typename T>
peano4::maps::STDStackMap<T>::~STDStackMap() {
  for (auto& p: _data) {
    delete p.second;
  }
}

#endif

