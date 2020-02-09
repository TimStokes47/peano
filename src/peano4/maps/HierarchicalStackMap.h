// This file is part of the Peano project. For conditions of distribution and
// use, please see the copyright notice at www.peano-framework.org
#ifndef _PEANO4_MAPS_HIERARCHICAL_STACK_MAP_H_
#define _PEANO4_MAPS_HIERARCHICAL_STACK_MAP_H_


#include "maps.h"


#include <map>


namespace peano4 {
  namespace maps {
    template <typename T>
    class HierarchicalStackMap;
  }
}


/**
 *
 *
 * \section Software architecture
 *
 * There's one entry per tree. Each entry has a semaphore of its own. So if we
 * need a lock, we only lock once per tree.
 *
 *
 * \section Result
 *
 * With the standard stack map, I got the following data:

real    0m20.442s
user    1m9.988s
sys     0m1.161s

real    0m24.223s
user    1m13.845s
sys     0m1.438s

 *
 * With the new data structure, we get data alike
 *

real    0m14.495s
user    0m54.581s
sys     0m0.149s

 *
 *
 */
template <typename T>
class peano4::maps::HierarchicalStackMap {
  private:
	/**
	 * Data of one tree.
	 */
	struct TreeData {
      std::map< int, T* >                 _stackNumberToData;
      tarch::multicore::BooleanSemaphore  _semaphore;
	};

	std::map< int, TreeData* >            _data;

    /**
     * This routine is not thread-safe, i.e. if you need it thread-safe then
     * you have to wrap it into  a semaphore manually.
     */
    void createStack(const StackKey& key);
  public:
    ~HierarchicalStackMap();

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
    T* getForPush(int treeId, int stackId);
    T* getForPush(const StackKey& key);

    /**
     * @see getForPush(int,int)
     */
    T* getForPop(int treeId, int stackId) const;
    T* getForPop(const StackKey& key) const;

    std::string toString() const;

    /**
     * This one should be const, but I might need a semaphore, so I have to
     * omit the const qualifier.
     */
    std::set<StackKey>  getKeys();
};


template <typename T>
void peano4::maps::HierarchicalStackMap<T>::createStack(const StackKey& key) {
  if ( _data.at( key.first )->_stackNumberToData.count(key.second)==0 ) {
    _data[key.first]->_stackNumberToData.insert(
      std::pair< int, T* >(
        key.second,
		new T()
	  )
    );
  }
}


template <typename T>
bool peano4::maps::HierarchicalStackMap<T>::empty(int treeId, int stackId) const {
  return _data.count( treeId )==0
      or _data.at( treeId )->_stackNumberToData.count(stackId)==0
	  or _data.at( treeId )->_stackNumberToData.at(stackId)->empty();
}


template <typename T>
T* peano4::maps::HierarchicalStackMap<T>::getForPush(int treeId, int stackId) {
  return getForPush( StackKey(treeId,stackId) );
}


template <typename T>
T* peano4::maps::HierarchicalStackMap<T>::getForPush(const StackKey& key) {
  if (_data.count(key.first)==0) {
    _data.insert( std::pair< int, TreeData* >(
      key.first, new TreeData()
    ));
  }
  tarch::multicore::Lock lock(_data[key.first]->_semaphore);
  createStack(key);
  assertion( _data[key.first]->_stackNumberToData[key.second] != nullptr );
  return _data[key.first]->_stackNumberToData[key.second];
}


template <typename T>
T* peano4::maps::HierarchicalStackMap<T>::getForPop(int treeId, int stackId) const {
  return getForPop( StackKey(treeId,stackId) );
}


template <typename T>
T* peano4::maps::HierarchicalStackMap<T>::getForPop(const StackKey& key) const {
  assertion( _data.count(key.first)==1 );
  return _data.at(key.first)->_stackNumberToData.at(key.second);
}


template <typename T>
std::string peano4::maps::HierarchicalStackMap<T>::toString() const {
  std::string result = "(" + std::to_string( _data.size() );
/*
  for (auto& p: _data) {
	result += ",";
	result += std::to_string(p.first.first) + "x" + std::to_string(p.first.second) + ":" + std::to_string(p.second->size());
  }
*/
  result += ")";
  return result;
}


template <typename T>
std::set<peano4::maps::StackKey>  peano4::maps::HierarchicalStackMap<T>::getKeys() {
  static tarch::multicore::BooleanSemaphore getKeysSemaphore;
  tarch::multicore::Lock lock(getKeysSemaphore);

  std::set<peano4::maps::StackKey> result;
  for (auto& p: _data)
  for (auto& pp: p.second->_stackNumberToData) {
    result.insert( peano4::maps::StackKey(p.first,pp.first) );
  }

  return result;
}


template <typename T>
peano4::maps::HierarchicalStackMap<T>::~HierarchicalStackMap() {
  for (auto& p: _data) {
    delete p.second;
  }
}

#endif

