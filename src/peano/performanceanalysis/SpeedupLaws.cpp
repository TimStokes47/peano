#include "peano/performanceanalysis/SpeedupLaws.h"



peano::performanceanalysis::SpeedupLaws::SpeedupLaws(int MaxEntries):
  _MaxEntries(MaxEntries),
  _f(0.5),
  _t1(0.0) {
}


void peano::performanceanalysis::SpeedupLaws::addMeasurement( int p, double t ) {
  assertion( p>0 );

  if (static_cast<int>(_p.size())==_MaxEntries) {
    _p.pop_back();
    _t.pop_back();
/*
    _p.erase( _p.begin() );
    _t.erase( _t.begin() );
*/
  }

  if ( _p.empty() ) {
    _f  = 0.5;
    _t1 = t;
  }

/*
  _p.push_back(p);
  _t.push_back(t);
*/
  _p.insert( _p.begin(), p );
  _t.insert( _t.begin(), t );
}


void peano::performanceanalysis::SpeedupLaws::relaxAmdahlsLaw() {
  assertion( _f>0.0  );
  // evtl. zu harsch
  assertion( _f<1.0 );
  assertion( _t1>0.0 );

//  if (!_p.empty()) {
  if (_p.size()>=2 and _p[0]>1) {
    const double tNormalisation =
      (1.0-1.0/_p[0]) * ( _f * _t1 + (1.0-_f) * _t1/_p[0] - _t[0] );
    const double fNormalisation =
      (1.0+(1.0-_f)/_f/_p[0]) * ( _f * _t1 + (1.0-_f) * _t1/_p[0] - _t[0] );

    double rhsT = 0.0;
    double rhsF = 0.0;

    assertion1(tNormalisation!=0.0, tNormalisation);
    assertion1(fNormalisation!=0.0, fNormalisation);

    for (int n=1; n<static_cast<int>(_p.size()); n++) {
      const double weight = std::pow(0.5,n-1.0);
      const double amdahlTerm = _f * _t1 + (1.0-_f) * _t1/_p[n] - _t[n];

      assertion3(weight>0.0,n,weight,amdahlTerm);

      rhsT += weight * amdahlTerm * ( _t1 - _t1/_p[n] );
      rhsF += weight * amdahlTerm * ( _f  + (1.0-_f)/_p[n]  );
    }

    const double omega = 0.2;
    _t1 = _t1 + omega * ( _t1 - rhsT / tNormalisation );
//    _f  = 0.5 * (_f +  rhsF / fNormalisation);
    _f  = _f  + omega * ( _f - rhsF / fNormalisation );

    assertion2( _f>0.0, _f, _t1  );
    // bullshit
    assertion2( _f<1.0, _f, _t1 );
    assertion2( _t1>0.0, _f, _t1 );
  }
}


double peano::performanceanalysis::SpeedupLaws::getSerialTime() const {
  return _t1;
}


double peano::performanceanalysis::SpeedupLaws::getSerialCodeFraction() const {
  return _f;
}
