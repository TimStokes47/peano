#include "tarch/timing/GlidingAverageMeasurement.h"
#include "tarch/Assertions.h"
#include "tarch/la/Scalar.h"
#include "tarch/la/ScalarOperations.h"


#include <limits>
#include <cmath>
#include <sstream>

tarch::logging::Log tarch::timing::GlidingAverageMeasurement::_log( "tarch::timing::GlidingAverageMeasurement" );


tarch::timing::GlidingAverageMeasurement::GlidingAverageMeasurement(double accuracy, double weight, int maxEntries):
  _accuracy(accuracy),
  _weight(weight),
  _maxEntries(maxEntries),
  _isAccurateValue(false) {
  assertion1( accuracy>=0.0, accuracy );
  assertion1( weight<=1.0, weight );
  assertion1( maxEntries>=1, weight );
}


void tarch::timing::GlidingAverageMeasurement::erase() {
  _isAccurateValue      = false;
  _values.clear();
}


double tarch::timing::GlidingAverageMeasurement::getValue() const {
  if ( _values.empty() ) {
	return 0.0;
  }
  else {
    double totalWeight       = 0.0;
    double accumulatedWeight = 0.0;

    for (int i=static_cast<int>(_values.size())-1; i>=0; i--) {
      double currentWeight = std::pow(_weight, static_cast<int>(_values.size())-i-1 );
      accumulatedWeight += currentWeight;
      totalWeight       += currentWeight * _values[i];
    }
    return totalWeight / accumulatedWeight;
  }
}


double tarch::timing::GlidingAverageMeasurement::getStandardDeviation() const {
  if ( _values.empty() ) {
	return 0.0;
  }
  else {
    double totalWeight        = 0.0;
    double totalSquaredWeight = 0.0;
    double accumulatedWeight  = 0.0;

    for (int i=static_cast<int>(_values.size())-1; i>=0; i--) {
      double currentWeight = std::pow(_weight, static_cast<int>(_values.size())-i-1 );
      accumulatedWeight  += currentWeight;
      totalSquaredWeight += currentWeight * currentWeight * _values[i];
      totalWeight        += currentWeight * _values[i];
    }

    double result = ( totalSquaredWeight - totalWeight ) / accumulatedWeight;
    if (result>tarch::la::NUMERICAL_ZERO_DIFFERENCE) {
      return std::sqrt(result);
    }
    else {
      return 0.0;
    }
  }
}


bool tarch::timing::GlidingAverageMeasurement::isAccurateValue() const {
  return _isAccurateValue;
}


void tarch::timing::GlidingAverageMeasurement::setAccuracy(const double& value) {
  assertion( value>=0.0 );
  _accuracy        = value;
}


void tarch::timing::GlidingAverageMeasurement::increaseAccuracy( const double& factor ) {
  assertion(factor>0.0);
  _accuracy /= factor;
}


void tarch::timing::GlidingAverageMeasurement::setValue(const double& value) {
  if ( static_cast<int>(_values.size())<_maxEntries ) {
    _isAccurateValue = false;
    _values.push_back(value);
  }
  else {
    const double oldAverage = getValue();
    _isAccurateValue = false;

    for (int i=0; i<_maxEntries-1; i++) {
      _values[i] = _values[i+1];
    }

    _values[_maxEntries-1] = value;

    _isAccurateValue = std::abs(oldAverage - getValue()) < _accuracy;
  }
}


std::string tarch::timing::GlidingAverageMeasurement::toString() const {
  std::ostringstream msg;
  msg << "(" << getValue();
  for (auto p: _values) {
	msg << "," << p;
  }
  msg << ")";
  return msg.str();
}


double tarch::timing::GlidingAverageMeasurement::max() const {
  double result = -std::numeric_limits<double>::max();
  for (auto p: _values) {
	result = result > p ? result : p;
  }
  return result;
}


double tarch::timing::GlidingAverageMeasurement::min() const {
  double result = std::numeric_limits<double>::max();
  for (auto p: _values) {
	result = result < p ? result : p;
  }
  return result;
}


double tarch::timing::GlidingAverageMeasurement::getAccuracy() const {
  return _accuracy;
}

