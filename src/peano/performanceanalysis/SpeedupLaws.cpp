#include "peano/performanceanalysis/SpeedupLaws.h"
#include "tarch/la/Matrix.h"
#include "tarch/la/LUDecomposition.h"

#include <sstream>

tarch::logging::Log peano::performanceanalysis::SpeedupLaws::_log( "peano::performanceanalysis::SpeedupLaws" );

peano::performanceanalysis::SpeedupLaws::SpeedupLaws(double f):
  _f(f),
  _t_1(1.0),
  _s(0.0),
  _samples(0) {
}


void peano::performanceanalysis::SpeedupLaws::addMeasurement( int p, double t ) {
  assertion( p>0 );

  #if defined(PerformanceAnalysis)
  logInfo( "addMeasurement()", "p=" << p << ", t=" << t );
  #endif

  // If you want the algorithm just to erase the oldest measurement, simply
  // keep this entry to its initial value and do not modify it further.
  int measurementToBeDropped = Entries;

  // Find last=oldest p entry
  for (int i=0; i<Entries; i++) {
    if ( _p(i)==p ) {
      measurementToBeDropped = i;
    }
  }

  for (int i=measurementToBeDropped-1; i>=1; i--) {
    _p(i) = _p(i-1);
    _t(i) = _t(i-1);
  }

  _p(0) = p;
  _t(0) = t;

  if (_samples<Entries) {
    _t_1 = t;
  }
  _samples++;
}


void peano::performanceanalysis::SpeedupLaws::relaxAmdahlsLaw() {
  if (_samples>Entries) {
    const int NewtonIterations = Entries;
    for (int it=0; it<NewtonIterations; it++) {
      tarch::la::Matrix<2,Entries,double>  gradJ(0.0);
      tarch::la::Vector<Entries,double>    y(0.0);

      for (int n=0; n<static_cast<int>(_p.size()); n++) {
        double amdahlTerm = _f * _t_1 + (1.0-_f) * _t_1/_p(n) - _t(n);
        double dtdf       =      _t_1 -            _t_1/_p(n);
        double dtdt_1      = _f       + (1.0-_f)      /_p(n);

        gradJ(0,n) = std::pow(Weight,-n/2.0) * dtdf;
        gradJ(1,n) = std::pow(Weight,-n/2.0) * dtdt_1;
        y(n)       = std::pow(Weight,-n/2.0) * amdahlTerm;
      }

      tarch::la::Matrix<2,2,double> gradJgradJT;
      gradJgradJT = gradJ * tarch::la::transpose(gradJ);

      tarch::la::Vector<2,double>    rhs;
      rhs = -1.0 * (gradJ * y);

      if ( std::abs(det(gradJgradJT))>1e-5 ) {
        tarch::la::Vector<2,double>    shifts;
        shifts = tarch::la::invert( gradJgradJT ) * rhs;

        // Problem can be ill-posed
        if (
          !std::isnan(shifts(0)) &&
          !std::isnan(shifts(1))
        ) {
          _f   += shifts(0);
          _t_1 += shifts(1);
        }
      }

      // Problem can be ill-posed or too non-smooth, so we manually add
      // these constraints
      _f   = std::max(_f,MinF);
      _f   = std::min(_f,MaxF);
      _t_1 = std::max(_t_1,MinT1);
    }

    assertion4( _f>=0,    _f, _t_1, _p, _t );
    assertion4( _f<=1.0,  _f, _t_1, _p, _t );
    assertion4( _t_1>0.0, _f, _t_1, _p, _t );
  }

  #if defined(PerformanceAnalysis)
  logInfo( "relaxAmdahlsLaw()", "f=" << _f << ", t_1=" << _t_1 );
  #endif
}


void peano::performanceanalysis::SpeedupLaws::relaxAmdahlsLawWithThreadStartupCost() {
  if (_samples>Entries) {
    const int NewtonIterations = Entries;
    for (int it=0; it<NewtonIterations; it++) {
      tarch::la::Matrix<3,Entries,double>  gradJ(0.0);
      tarch::la::Vector<Entries,double>    y(0.0);

      for (int n=0; n<static_cast<int>(_p.size()); n++) {
        double amdahlTerm = _f * _t_1 + (1.0-_f) * _t_1/_p(n) + _s * ( _p(n)-1.0)  - _t(n);
        double dtdf       =      _t_1 -            _t_1/_p(n);
        double dtdt_1     = _f        + (1.0-_f)      /_p(n);
        double dtds       = _p(n)-1.0;

        gradJ(0,n) = std::pow(Weight,-n/2.0) * dtdf;
        gradJ(1,n) = std::pow(Weight,-n/2.0) * dtdt_1;
        gradJ(2,n) = std::pow(Weight,-n/2.0) * dtds;
        y(n)       = std::pow(Weight,-n/2.0) * amdahlTerm;
      }

      tarch::la::Matrix<3,3,double> gradJgradJT;
      gradJgradJT = gradJ * tarch::la::transpose(gradJ);

      tarch::la::Vector<3,double>    rhs;
      rhs = -1.0 * (gradJ * y);

      if ( std::abs(det(gradJgradJT))>1e-5 ) {
        tarch::la::Vector<3,double>    shifts;
        shifts = tarch::la::invert( gradJgradJT ) * rhs;

        // Problem can be ill-posed
        if (
          !std::isnan(shifts(0)) &&
          !std::isnan(shifts(1))
        ) {
          _f   += shifts(0);
          _t_1 += shifts(1);
          _s   += shifts(2);
        }
      }

      // Problem can be ill-posed or too non-smooth, so we manually add
      // these constraints
      _f   = std::max(   _f,MinF  );
      _f   = std::min(   _f,MaxF  );
      _t_1 = std::max( _t_1,MinT1 );
      _s   = std::max(   _s,MinS  );
    }

    assertion4( _f>=0,    _f, _t_1, _p, _t );
    assertion4( _f<=1.0,  _f, _t_1, _p, _t );
    assertion4( _t_1>0.0, _f, _t_1, _p, _t );
  }

  #if defined(PerformanceAnalysis)
  logInfo( "relaxAmdahlsLawWithThreadStartupCost()", "f=" << _f << ", t_1=" << _t_1 << ", s=" << _s );
  #endif
}


double peano::performanceanalysis::SpeedupLaws::getSerialTime() const {
  return _t_1;
}


double peano::performanceanalysis::SpeedupLaws::getSerialCodeFraction() const {
  return _f;
}


double peano::performanceanalysis::SpeedupLaws::getStartupCostPerThread() const {
  return _s;
}


std::string peano::performanceanalysis::SpeedupLaws::toString() const {
  std::ostringstream out;

  out << "(f="  << _f
      << ",t_1=" << _t_1
      << ",s="  << _samples
      << ",t="  << _t
      << ",p="  << _p
      << ")";

  return out.str();
}
