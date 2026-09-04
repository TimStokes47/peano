#include "TaskNumber.h"

#include <sstream>

#include "tarch/Assertions.h"
#include "tarch/Enumerator.h"


const exaparty::TaskNumber exaparty::TaskNumber::NoOutDependencies{tarch::multicore::NoOutDependencies, TaskAssociation::TouchVertexFirstTime};


exaparty::TaskNumber::TaskNumber(int number0, TaskAssociation number1):
  _taskCounter(number0),
  _taskAssociation(number1) {
  if (_taskCounter == ::tarch::multicore::NoOutDependencies) {
    *this = NoOutDependencies;
  }
  else {
    assertion1(number0 >= 0, toString());
    assertion1(number1 != TaskAssociation::NumberOfEntries, toString());
    assertion1(number0 <= std::numeric_limits<int>::max()/ static_cast<int>(exaparty::TaskNumber::TaskAssociation::NumberOfEntries), toString());
  }
}


bool exaparty::TaskNumber::operator<(const TaskNumber& rhs) const {
  return _taskCounter < rhs._taskCounter
         or _taskCounter == rhs._taskCounter and _taskAssociation == TaskAssociation::TouchVertexFirstTime
              and rhs._taskAssociation != TaskAssociation::TouchVertexFirstTime
         or _taskCounter == rhs._taskCounter and _taskAssociation == TaskAssociation::TouchCellFirstTime
              and rhs._taskAssociation == TaskAssociation::TouchVertexLastTime;
}


bool exaparty::TaskNumber::equals(const TaskNumber& rhs) const {
  return _taskCounter == rhs._taskCounter and _taskAssociation == rhs._taskAssociation;
}


bool operator==(const exaparty::TaskNumber& lhs, const exaparty::TaskNumber& rhs) {
  return lhs.equals(rhs);
}


bool operator!=(const exaparty::TaskNumber& lhs, const exaparty::TaskNumber& rhs) {
  return not lhs.equals(rhs);
}


std::string exaparty::TaskNumber::toString() const {
  if (_taskCounter == tarch::multicore::NoOutDependencies) {
    return "no-dep";
  } else {
    std::string result = "(" + std::to_string(_taskCounter) + ",";
    switch (_taskAssociation) {
    case TaskAssociation::TouchVertexFirstTime:
      result += "touch-vertex-first-time(0)";
      break;
    case TaskAssociation::TouchCellFirstTime:
      result += "touch-cell-first-time(1)";
      break;
    case TaskAssociation::TouchVertexLastTime:
      result += "touch-vertex-last-time(2)";
      break;
    default:
      result += "<undef>";
      break;
    }
    result += ")";
    return result;
  }
}


tarch::multicore::TaskNumber exaparty::TaskNumber::flatten() const {
  assertionEquals(tarch::multicore::NoOutDependencies,tarch::Enumerator::NoNumber);
  if (_taskCounter == tarch::multicore::NoOutDependencies) {
    return tarch::multicore::NoOutDependencies;
  } else {
    return _taskCounter * static_cast<int>(exaparty::TaskNumber::TaskAssociation::NumberOfEntries)
           + static_cast<int>(_taskAssociation);
  }
}


std::string exaparty::toString(const TaskNumber numbers[TwoPowerD]) {
  std::set<TaskNumber> converted;
  for (int i=0; i<TwoPowerD; i++) {
    converted.insert( numbers[i] );
  }
  return toString(converted);
}


std::string exaparty::toString(const std::set<exaparty::TaskNumber>& numbers) {
  std::ostringstream msg;
  msg << "{";
  bool first = true;
  for (auto& p : numbers) {
    if (first) {
      first = false;
    } else {
      msg << ",";
    }
    msg << p.toString();
  }
  msg << "}";
  return msg.str();
}


int exaparty::flatten(const TaskNumber& number) { return number.flatten(); }


std::set<int> exaparty::flatten(const std::set<TaskNumber>& numbers) {
  std::set<int> result;
  for (auto& p : numbers) {
    if (p != exaparty::TaskNumber::NoOutDependencies) {
      result.insert(p.flatten());
    }
  }
  return result;
}


std::set<::exaparty::TaskNumber> exaparty::getDependenciesForTask(
  const ::exaparty::TaskNumber& task, PendingDependencies& pendingDependencies
) {
  std::set<::exaparty::TaskNumber> result;
  PendingDependencies::iterator  p = pendingDependencies.begin();
  while (p != pendingDependencies.end()) {
    if (p->second == task) {
      result.insert(p->first);
      p = pendingDependencies.erase(p);
    } else
      p++;
  }
  return result;
}
