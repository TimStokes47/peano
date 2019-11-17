#include "LoggerWithFilter.h"
#include "tarch/Assertions.h"


#include "tarch/mpi/Rank.h"


#include <sstream>
#include <iostream>


const int tarch::logging::LoggerWithFilter::FilterListEntry::AnyRank = -1;

const std::string tarch::logging::LoggerWithFilter::FilterListEntry::TargetAll   = "all";
const std::string tarch::logging::LoggerWithFilter::FilterListEntry::TargetInfo  = "info";
const std::string tarch::logging::LoggerWithFilter::FilterListEntry::TargetDebug = "debug";
const std::string tarch::logging::LoggerWithFilter::FilterListEntry::TargetTrace = "trace";


tarch::logging::LoggerWithFilter::FilterListEntry::FilterListEntry( const std::string& targetName, bool isBlackListEntry ):
  _targetName(targetName),
  _rank(AnyRank),
  _namespaceName(""),
  _isBlackEntry(isBlackListEntry) {
  assertion1( targetName==TargetInfo or targetName==TargetDebug or targetName==TargetAll or targetName==TargetTrace, targetName );
}


tarch::logging::LoggerWithFilter::FilterListEntry::FilterListEntry( const std::string& targetName, int rank, const std::string& className, bool isBlackListEntry ):
  _targetName(targetName),
  _rank(rank),
  _namespaceName(className),
  _isBlackEntry(isBlackListEntry) {
  assertion1( targetName==TargetInfo or targetName==TargetDebug or targetName==TargetAll or targetName==TargetTrace, targetName );
}


std::string tarch::logging::LoggerWithFilter::FilterListEntry::toString() const {
  std::ostringstream msg;
  msg << "(";
  msg << "target:" << _targetName << ",";
  msg << _namespaceName << ",";
  #ifdef Parallel
  msg << "rank:" << _rank;
  msg << ",";
  #endif
  if (_isBlackEntry) {
    msg << "blacklist-entry";
  }
  else {
    msg << "whitelist-entry";
  }
  msg << ")";
  return msg.str();
}


bool tarch::logging::LoggerWithFilter::FilterListEntry::operator<(const FilterListEntry& b) const {
  return _rank < b._rank || _namespaceName < b._namespaceName || _targetName < b._targetName;
}


bool tarch::logging::LoggerWithFilter::FilterListEntry::operator==(const FilterListEntry& b) const {
  return _rank == b._rank || _namespaceName == b._namespaceName || _targetName == b._targetName;
}


bool tarch::logging::LoggerWithFilter::FilterListEntry::operator!=(const FilterListEntry& b) const {
  return not operator==(b);
}


tarch::logging::LoggerWithFilter::LoggerWithFilter() {
  addFilterListEntry( FilterListEntry( FilterListEntry::TargetAll, false )  );
}


void tarch::logging::LoggerWithFilter::printFilterListToCout() const {
  if (_filterlist.empty()) {
    std::cout << "filter list is empty" << std::endl;
  }
  else {
    std::cout << "filter list is not empty and contains " << _filterlist.size() << " entries" << std::endl;
    for (FilterList::const_iterator p = _filterlist.begin(); p!=_filterlist.end(); p++ ) {
      std::cout <<  p->toString() << std::endl;
    }
  }
}


bool tarch::logging::LoggerWithFilter::writeDebug(const std::string& className) {
  #if PeanoDebug>=4
  return !filterOut(FilterListEntry::TargetDebug,className);
  #else
  return false;
  #endif
}


bool tarch::logging::LoggerWithFilter::writeInfo(const std::string& className) {
  return !filterOut(FilterListEntry::TargetInfo,className);
}


bool tarch::logging::LoggerWithFilter::writeTrace(const std::string& className) {
  #if PeanoDebug>=1
  const bool result = !filterOut(FilterListEntry::TargetTrace,className);
  return result;
  #else
  return false;
  #endif
}


bool tarch::logging::LoggerWithFilter::writeWarning(const std::string& className) {
  return true;
}


bool tarch::logging::LoggerWithFilter::writeError(const std::string& className) {
  return true;
}


void tarch::logging::LoggerWithFilter::addFilterListEntries( const FilterList&    entries) {
  for (FilterList::const_iterator p = entries.begin(); p!=entries.end(); p++ ) {
    addFilterListEntry(*p);
  }
}


void tarch::logging::LoggerWithFilter::clearFilterList() {
  _filterlist.clear();
}


bool tarch::logging::LoggerWithFilter::filterOut(
  const std::string& targetName,
  const std::string& className
) {
  bool result    = true;
  bool foundRule = false;
  int lengthActive = 0;
  for (FilterList::const_iterator p = _filterlist.begin(); p!=_filterlist.end(); p++ ) {
    int length = static_cast<int>(p->_namespaceName.size());
    if ( length >= lengthActive ) {
      if (
        ( targetName == p->_targetName or p->_targetName==FilterListEntry::TargetAll )
		and
        (className.find(p->_namespaceName,0)==0)
        #ifdef Parallel
        &&
        (p->_rank == FilterListEntry::AnyRank || p->_rank == tarch::mpi::Rank::getInstance().getRank())
        #endif
      ) {
        lengthActive = length;
        result       = p->_isBlackEntry;
        foundRule    = true;
      }
    }
  }

  if (!foundRule) {
    printFilterListToCout();
    std::cerr << "did not find filter rule for target \"" << targetName << "\" and class \"" << className << "\" on rank " << tarch::mpi::Rank::getInstance().getRank() << std::endl;
  }
  return result;
}


void tarch::logging::LoggerWithFilter::addFilterListEntry( const FilterListEntry& entry) {
  if (_filterlist.count(entry)!=0) {
    std::cerr << "tried to insert log filter " << entry.toString() << " multiple times" << std::endl;
    assertion(false);
  }
  else {
    _filterlist.insert( entry );
  }
}
