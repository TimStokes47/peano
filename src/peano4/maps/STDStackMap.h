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
     * There are two versions of the routine: one for pushes, one for pops. I
     * distinguish the two, as the push variant might create new stacks lazily
     * whereas the pop knowns a priori that the requested stack does exist.
     *
     * <h2> Implementation details </h2>
     *
     * We nevertheless have to be careful with pops. In the vanilla version,
     * pops cannot create new stacks. But we use one global map for all stacks
     * of all spacetrees hosted by this rank. So concurrent pushes can create
     * new stacks on-the-fly. So we need some thread-saving mechanisms.
     *
     * @return A stack of type T. It is a pointer and this routine realises
     *   a lazy creation, i.e. might create the result upon demand.
     */
    T* getForPush(int treeId, int stackId);
    T* getForPush(const StackKey& key);

    /**
     * @see getForPush(int,int)
     *
     * @return A stack of type T.
     */
    T* getForPop(int treeId, int stackId);
    T* getForPop(const StackKey& key);

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
T* peano4::maps::STDStackMap<T>::getForPush(int treeId, int stackId) {
  return getForPush( StackKey(treeId,stackId) );
}


template <typename T>
T* peano4::maps::STDStackMap<T>::getForPush(const StackKey& key) {
  tarch::multicore::Lock lock(_semaphore);
  createStack(key);
  return _data[key];
}


template <typename T>
T* peano4::maps::STDStackMap<T>::getForPop(int treeId, int stackId) {
  return getForPop( StackKey(treeId,stackId) );
}


template <typename T>
T* peano4::maps::STDStackMap<T>::getForPop(const StackKey& key) {
  tarch::multicore::Lock lock(_semaphore);
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

