// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_MAPS_STD_MAP_H_
#define _PEANO4_MAPS_STD_MAP_H_


#include "maps.h"


#include <map>


namespace peano4 {
  namespace maps {
    template <typename T>
    class STDStackMap;
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

    /**
     * This routine is not thread-safe, i.e. if you need it thread-safe then
     * you have to wrap it into  a semaphore manually.
     */
    void createStack(const StackKey& key);
  public:
    ~STDStackMap();

    bool empty(int treeId, int stackId) const;

    /**
     * Get the stack belonging to a tree.
     *
     * There are two versions of the routine: one is thread-safe, the other one
     * does not employ a dedicated semaphore. The one with a semaphore can
     * lazily create stacks; which is the reason why we need the semaphore:
     * everytime we lazily insert new stacks, we run risk that we break the
     * consistency of the underlying container. Originally, I worked with
     * thread-safe accessors only. This resulted in disappointing timings and
     * notably speedup. With a carelfull distinction, I got down the runtime
     * to two third of the original.
     *
     * @return A stack of type T. Actually, it is a pointer and this routine is
     *   a lazy creation, i.e. might create the result upon demand.
     */
    T* getThreadSafe(int treeId, int stackId);
    T* getThreadSafe(const StackKey& key);

    /**
     * @see getThreadSafe(int,int)
     */
    T* getNotThreadSafe(int treeId, int stackId) const;
    T* getNotThreadSafe(const StackKey& key) const;

    std::string toString() const;

    /**
     * This one should be const, but I might need a semaphore, so I have to
     * omit the const qualifier.
     */
    std::set<StackKey>  getKeys();
};



template <typename T>
void peano4::maps::STDStackMap<T>::createStack(const StackKey& key) {
  if ( _data.count( key )==0 ) {
    _data.insert(
      std::pair< StackKey, T* >(
        key,
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
T* peano4::maps::STDStackMap<T>::getThreadSafe(int treeId, int stackId) {
  return getThreadSafe( StackKey(treeId,stackId) );
}


template <typename T>
T* peano4::maps::STDStackMap<T>::getThreadSafe(const StackKey& key) {
  tarch::multicore::Lock lock(_semaphore);
  createStack(key);
  return _data[key];
}


template <typename T>
T* peano4::maps::STDStackMap<T>::getNotThreadSafe(int treeId, int stackId) const {
  return getNotThreadSafe( StackKey(treeId,stackId) );
}


template <typename T>
T* peano4::maps::STDStackMap<T>::getNotThreadSafe(const StackKey& key) const {
  assertion( _data.count(key)==1 );
  return _data.at( key );
}


template <typename T>
std::string peano4::maps::STDStackMap<T>::toString() const {
  std::string result = "(" + std::to_string( _data.size() );
  for (auto& p: _data) {
	result += ",";
	result += std::to_string(p.first.first) + "x" + std::to_string(p.first.second) + ":" + std::to_string(p.second->size());
  }
  result += ")";
  return result;
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

